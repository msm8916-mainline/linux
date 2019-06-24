// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2018 Michael Srba
 */
#include <linux/backlight.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drmP.h>
#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>

#include <video/mipi_display.h>

struct s6e88a0_panel {
	struct drm_panel base;
	struct mipi_dsi_device *dsi;

	struct gpio_desc *extra_power_gpio;
	struct gpio_desc *reset_gpio;
	struct backlight_device *backlight;

	bool prepared;
	bool enabled;

	const struct drm_display_mode *mode;
};

static inline struct s6e88a0_panel *to_s6e88a0_panel(struct drm_panel *panel)
{
	return container_of(panel, struct s6e88a0_panel, base);
}

static int s6e88a0_panel_init(struct s6e88a0_panel *s6e88a0)
{
	struct mipi_dsi_device *dsi = s6e88a0->dsi;
	
	int ret;

	ret = mipi_dsi_dcs_soft_reset(dsi);
	if (ret < 0)
		return ret;

	usleep_range(10000, 20000);

	return 0;
}

static int s6e88a0_panel_on(struct s6e88a0_panel *s6e88a0)
{
	struct mipi_dsi_device *dsi = s6e88a0->dsi;
	struct device *dev = &s6e88a0->dsi->dev;
	int ret;

	msleep(5);

	ret = mipi_dsi_dcs_write(dsi, 0xf0, (u8[]){ 0x5a, 0x5a }, 2);
	if (ret < 0) {
		dev_err(dev, "failed on dsc command `f0 5a 5a` ('tesk key on') : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, 0xcc, (u8[]){ 0x4c }, 1);
	if (ret < 0) {
		dev_err(dev, "failed on dsc command `0xcc 0x4c` (whatever it is) : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_EXIT_SLEEP_MODE, (u8[]){ 0x00 }, 1);
	//ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "failed to exit sleep mode : %d\n", ret);
		return ret;
	}
	
	msleep(120);
	
	ret = mipi_dsi_dcs_write(dsi, 0xca, (u8[]){ 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x80,
		                                        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
		                                        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
		                                        0x80, 0x80, 0x80, 0x6b, 0x68, 0x71, 0x00, 0x00,
		                                        0x00 }, 33);
	if (ret < 0) {
		dev_err(dev, "failed on setting 'brightness (and) gama' : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, 0xb2, (u8[]){ 0x40, 0x0a, 0x17, 0x00, 0x0a }, 5);
	if (ret < 0) {
		dev_err(dev, "failed on dsc command `b2 40 0a 17 00 0a` ('aid set') : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, 0xb6, (u8[]){ 0x2c, 0x0b }, 2);
	if (ret < 0) {
		dev_err(dev, "failed on dsc command `b6 2c 0b` ('elvss') : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, MIPI_DCS_WRITE_POWER_SAVE, (u8[]){ 0x00 }, 1);
	if (ret < 0) {
		dev_err(dev, "failed on MIPI_DCS_WRITE_POWER_SAVE command : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, 0xf7, (u8[]){ 0x03 }, 1);
	if (ret < 0) {
		dev_err(dev, "failed on dsc command `f7 03` ('gamma update') : %d\n", ret);
		return ret;
	}
	
	ret = mipi_dsi_dcs_write(dsi, 0xf0, (u8[]){ 0xa5, 0xa5 }, 2);
	if (ret < 0) {
		dev_err(dev, "failed on dsc command `f0 a5 a5` ('tesk key off') : %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "failed to set display on: %d\n", ret);
		return ret;
	}
	
	return 0;
}

static void s6e88a0_panel_off(struct s6e88a0_panel *s6e88a0)
{
	struct mipi_dsi_device *dsi = s6e88a0->dsi;
	struct device *dev = &s6e88a0->dsi->dev;

	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0)
		dev_err(dev, "failed to set display off: %d\n", ret);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0)
		dev_err(dev, "failed to enter sleep mode: %d\n", ret);

	msleep(100);
}

static int s6e88a0_panel_disable(struct drm_panel *panel)
{
	struct s6e88a0_panel *s6e88a0 = to_s6e88a0_panel(panel);

	if (!s6e88a0->enabled)
		return 0;

	backlight_disable(s6e88a0->backlight);

	s6e88a0->enabled = false;

	return 0;
}

static int s6e88a0_panel_unprepare(struct drm_panel *panel)
{
	struct s6e88a0_panel *s6e88a0 = to_s6e88a0_panel(panel);

	if (!s6e88a0->prepared)
		return 0;

	s6e88a0_panel_off(s6e88a0);

	gpiod_set_value(s6e88a0->extra_power_gpio, 0);

	gpiod_set_value(s6e88a0->reset_gpio, 0);

	s6e88a0->prepared = false;

	return 0;
}

static int s6e88a0_panel_prepare(struct drm_panel *panel)
{
	struct s6e88a0_panel *s6e88a0 = to_s6e88a0_panel(panel);
	struct device *dev = &s6e88a0->dsi->dev;
	int ret;

	if (s6e88a0->prepared)
		return 0;
	
	gpiod_set_value(s6e88a0->extra_power_gpio, 0); //just a try, maybe it was already on and needs a power cycle?

	gpiod_set_value(s6e88a0->reset_gpio, 0); // -||-


	msleep(20);

	
	gpiod_set_value(s6e88a0->reset_gpio, 1);
	msleep(5);
	gpiod_set_value(s6e88a0->reset_gpio, 0);
	msleep(1);
	gpiod_set_value(s6e88a0->reset_gpio, 1);
	msleep(10);

	msleep(100);

	gpiod_set_value(s6e88a0->extra_power_gpio, 1);
	usleep_range(10, 20);

	ret = s6e88a0_panel_init(s6e88a0);
	if (ret < 0) {
		dev_err(dev, "failed to init panel: %d\n", ret);
		goto poweroff;
	}

	ret = s6e88a0_panel_on(s6e88a0);
	if (ret < 0) {
		dev_err(dev, "failed to set panel on: %d\n", ret);
		goto poweroff;
	}

	s6e88a0->prepared = true;

	return 0;

poweroff:

	gpiod_set_value(s6e88a0->extra_power_gpio, 0);

	gpiod_set_value(s6e88a0->reset_gpio, 0);

	return ret;
}

static int s6e88a0_panel_enable(struct drm_panel *panel)
{
	struct s6e88a0_panel *s6e88a0 = to_s6e88a0_panel(panel);

	if (s6e88a0->enabled)
		return 0;

	backlight_enable(s6e88a0->backlight);

	s6e88a0->enabled = true;

	return 0;
}

static const struct drm_display_mode default_mode = {
		.clock = 38494,
		.hdisplay = 540,
		.hsync_start = 540 + 88,
		.hsync_end = 540 + 88 + 4,
		.htotal = 540 + 88 + 4 + 20,
		.vdisplay = 960,
		.vsync_start = 960 + 14,
		.vsync_end = 960 + 14 + 2,
		.vtotal = 960 + 14 + 2 + 8,
		.vrefresh = 60,
		.flags = 0,
};

static int s6e88a0_panel_get_modes(struct drm_panel *panel)
{
	struct drm_display_mode *mode;
	struct s6e88a0_panel *s6e88a0 = to_s6e88a0_panel(panel);
	struct device *dev = &s6e88a0->dsi->dev;

	mode = drm_mode_duplicate(panel->drm, &default_mode);
	if (!mode) {
		dev_err(dev, "failed to add mode %ux%ux@%u\n",
			default_mode.hdisplay, default_mode.vdisplay,
			default_mode.vrefresh);
		return -ENOMEM;
	}

	drm_mode_set_name(mode);

	drm_mode_probed_add(panel->connector, mode);

	panel->connector->display_info.width_mm = 56;
	panel->connector->display_info.height_mm = 100;

	return 1;
}

static int dsi_dcs_bl_get_brightness(struct backlight_device *bl) //probably needs custom stuff (steal from s6e8aa0)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	int ret;
	u16 brightness = bl->props.brightness;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return brightness & 0xff;
	
	return 0;
}

static int dsi_dcs_bl_update_status(struct backlight_device *bl) //probably needs custom stuff (steal from s6e8aa0)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, bl->props.brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops dsi_bl_ops = {
	.update_status = dsi_dcs_bl_update_status,
	.get_brightness = dsi_dcs_bl_get_brightness,
};

static struct backlight_device *
drm_panel_create_dsi_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct backlight_properties props;

	memset(&props, 0, sizeof(props));
	props.type = BACKLIGHT_RAW;
	props.brightness = 255;
	props.max_brightness = 255;

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &dsi_bl_ops, &props);
}

static const struct drm_panel_funcs s6e88a0_panel_funcs = {
	.disable = s6e88a0_panel_disable,
	.unprepare = s6e88a0_panel_unprepare,
	.prepare = s6e88a0_panel_prepare,
	.enable = s6e88a0_panel_enable,
	.get_modes = s6e88a0_panel_get_modes,
};

static const struct of_device_id s6e88a0_of_match[] = {
	{ .compatible = "samsung,s6e88a0-ams452ef01", },
	{ }
};
MODULE_DEVICE_TABLE(of, s6e88a0_of_match);

static int s6e88a0_panel_add(struct s6e88a0_panel *s6e88a0)
{
	struct device *dev = &s6e88a0->dsi->dev;
	int ret;

	s6e88a0->mode = &default_mode;

	s6e88a0->extra_power_gpio = devm_gpiod_get(dev, "extra-power", GPIOD_OUT_HIGH);
	if (IS_ERR(s6e88a0->extra_power_gpio)) {
		ret = PTR_ERR(s6e88a0->extra_power_gpio);
		dev_err(dev, "cannot get extra-power-gpio %d\n", ret);
		return ret;
	}

	s6e88a0->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(s6e88a0->reset_gpio)) {
		ret = PTR_ERR(s6e88a0->reset_gpio);
		dev_err(dev, "cannot get reset-gpios %d\n", ret);
		return ret;
	}

	s6e88a0->backlight = drm_panel_create_dsi_backlight(s6e88a0->dsi);
	if (IS_ERR(s6e88a0->backlight)) {
		ret = PTR_ERR(s6e88a0->backlight);
		dev_err(dev, "failed to register backlight %d\n", ret);
		return ret;
	}

	drm_panel_init(&s6e88a0->base);
	s6e88a0->base.funcs = &s6e88a0_panel_funcs;
	s6e88a0->base.dev = &s6e88a0->dsi->dev;

	ret = drm_panel_add(&s6e88a0->base);

	return ret;
}

static void s6e88a0_panel_del(struct s6e88a0_panel *s6e88a0)
{
	if (s6e88a0->base.dev)
		drm_panel_remove(&s6e88a0->base);
}

static int s6e88a0_panel_probe(struct mipi_dsi_device *dsi)
{
	struct s6e88a0_panel *s6e88a0;
	int ret;

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags =  MIPI_DSI_MODE_VIDEO/* | MIPI_DSI_CLOCK_NON_CONTINUOUS*/;

	s6e88a0 = devm_kzalloc(&dsi->dev, sizeof(*s6e88a0), GFP_KERNEL);
	if (!s6e88a0)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, s6e88a0);

	s6e88a0->dsi = dsi;

	ret = s6e88a0_panel_add(s6e88a0);
	if (ret < 0)
		return ret;

	return mipi_dsi_attach(dsi);
}

static int s6e88a0_panel_remove(struct mipi_dsi_device *dsi)
{
	struct s6e88a0_panel *s6e88a0 = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = s6e88a0_panel_disable(&s6e88a0->base);
	if (ret < 0)
		dev_err(&dsi->dev, "failed to disable panel: %d\n", ret);

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "failed to detach from DSI host: %d\n",
			ret);

	s6e88a0_panel_del(s6e88a0);

	return 0;
}

static void s6e88a0_panel_shutdown(struct mipi_dsi_device *dsi)
{
	struct s6e88a0_panel *s6e88a0 = mipi_dsi_get_drvdata(dsi);

	s6e88a0_panel_disable(&s6e88a0->base);
}

static struct mipi_dsi_driver s6e88a0_panel_driver = {
	.driver = {
		.name = "panel-s6e88a0-ams452ef01",
		.of_match_table = s6e88a0_of_match,
	},
	.probe = s6e88a0_panel_probe,
	.remove = s6e88a0_panel_remove,
	.shutdown = s6e88a0_panel_shutdown,
};
module_mipi_dsi_driver(s6e88a0_panel_driver);

MODULE_AUTHOR("Michael Srba <Michael.Srba@seznam.cz>");
MODULE_DESCRIPTION("s6e88a0 ams452ef01 qHD");
MODULE_LICENSE("GPL v2");

