// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2018-2019, Michael Srba

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>
#include <video/mipi_display.h>

struct s6e88a0_ams452ef01 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct backlight_device *backlight;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;

	bool prepared;
	bool enabled;
};

static inline struct s6e88a0_ams452ef01 *to_s6e88a0_ams452ef01(struct drm_panel *panel)
{
	return container_of(panel, struct s6e88a0_ams452ef01, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void s6e88a0_ams452ef01_reset(struct s6e88a0_ams452ef01 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
}

static int s6e88a0_ams452ef01_on(struct s6e88a0_ams452ef01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xf0, 0x5a, 0x5a); // tesk key on
	dsi_dcs_write_seq(dsi, 0xcc, 0x4c);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	// setup default brightness/gama
	dsi_dcs_write_seq(dsi, 0xca,
			  0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			  0x6b, 0x68, 0x71, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xb2, 0x40, 0x0a, 0x17, 0x00, 0x0a); // aid set
	dsi_dcs_write_seq(dsi, 0xb6, 0x2c, 0x0b); // elvss
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x00);
	dsi_dcs_write_seq(dsi, 0xf7, 0x03); // gamma update
	dsi_dcs_write_seq(dsi, 0xf0, 0xa5, 0xa5); // tesk key off

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int s6e88a0_ams452ef01_off(struct s6e88a0_ams452ef01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(35);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int s6e88a0_ams452ef01_prepare(struct drm_panel *panel)
{
	struct s6e88a0_ams452ef01 *ctx = to_s6e88a0_ams452ef01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	s6e88a0_ams452ef01_reset(ctx);

	ret = s6e88a0_ams452ef01_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 0);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int s6e88a0_ams452ef01_unprepare(struct drm_panel *panel)
{
	struct s6e88a0_ams452ef01 *ctx = to_s6e88a0_ams452ef01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = s6e88a0_ams452ef01_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static int s6e88a0_ams452ef01_enable(struct drm_panel *panel)
{
	struct s6e88a0_ams452ef01 *ctx = to_s6e88a0_ams452ef01(panel);
	int ret;

	if (ctx->enabled)
		return 0;

	ret = backlight_enable(ctx->backlight);
	if (ret < 0) {
		dev_err(&ctx->dsi->dev, "Failed to enable backlight: %d\n", ret);
		return ret;
	}

	ctx->enabled = true;
	return 0;
}

static int s6e88a0_ams452ef01_disable(struct drm_panel *panel)
{
	struct s6e88a0_ams452ef01 *ctx = to_s6e88a0_ams452ef01(panel);
	int ret;

	if (!ctx->enabled)
		return 0;

	ret = backlight_disable(ctx->backlight);
	if (ret < 0) {
		dev_err(&ctx->dsi->dev, "Failed to disable backlight: %d\n", ret);
		return ret;
	}

	ctx->enabled = false;
	return 0;
}

static const struct drm_display_mode s6e88a0_ams452ef01_mode = {
	.clock = (540 + 88 + 4 + 20) * (960 + 14 + 2 + 8) * 60 / 1000,
	.hdisplay = 540,
	.hsync_start = 540 + 88,
	.hsync_end = 540 + 88 + 4,
	.htotal = 540 + 88 + 4 + 20,
	.vdisplay = 960,
	.vsync_start = 960 + 14,
	.vsync_end = 960 + 14 + 2,
	.vtotal = 960 + 14 + 2 + 8,
	.vrefresh = 60,
	.width_mm = 56,
	.height_mm = 100,
};

static int s6e88a0_ams452ef01_get_modes(struct drm_panel *panel)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(panel->drm, &s6e88a0_ams452ef01_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	panel->connector->display_info.width_mm = mode->width_mm;
	panel->connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(panel->connector, mode);

	return 1;
}

static const struct drm_panel_funcs s6e88a0_ams452ef01_panel_funcs = {
	.disable = s6e88a0_ams452ef01_disable,
	.unprepare = s6e88a0_ams452ef01_unprepare,
	.prepare = s6e88a0_ams452ef01_prepare,
	.enable = s6e88a0_ams452ef01_enable,
	.get_modes = s6e88a0_ams452ef01_get_modes,
};

static int dsi_dcs_bl_get_brightness(struct backlight_device *bl)
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
}

static int dsi_dcs_bl_update_status(struct backlight_device *bl)
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
s6e88a0_ams452ef01_create_backlight(struct mipi_dsi_device *dsi)
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

static int s6e88a0_ams452ef01_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct s6e88a0_ams452ef01 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vdd3";
	ctx->supplies[1].supply = "vci";
	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(ctx->supplies),
				      ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to get regulators: %d\n", ret);
		return ret;
	}

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset_gpio)) {
		ret = PTR_ERR(ctx->reset_gpio);
		dev_err(dev, "Failed to get reset-gpios: %d\n", ret);
		return ret;
	}

	ctx->backlight = s6e88a0_ams452ef01_create_backlight(dsi);
	if (IS_ERR(ctx->backlight)) {
		ret = PTR_ERR(ctx->backlight);
		dev_err(dev, "Failed to create backlight: %d\n", ret);
		return ret;
	}

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST;

	drm_panel_init(&ctx->panel, dev, &s6e88a0_ams452ef01_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ret = drm_panel_add(&ctx->panel);
	if (ret < 0) {
		dev_err(dev, "Failed to add panel: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static int s6e88a0_ams452ef01_remove(struct mipi_dsi_device *dsi)
{
	struct s6e88a0_ams452ef01 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id s6e88a0_ams452ef01_of_match[] = {
	{ .compatible = "samsung,s6e88a0-ams452ef01" },
	{ }
};
MODULE_DEVICE_TABLE(of, s6e88a0_ams452ef01_of_match);

static struct mipi_dsi_driver s6e88a0_ams452ef01_driver = {
	.probe = s6e88a0_ams452ef01_probe,
	.remove = s6e88a0_ams452ef01_remove,
	.driver = {
		.name = "panel-s6e88a0-ams452ef01",
		.of_match_table = s6e88a0_ams452ef01_of_match,
	},
};
module_mipi_dsi_driver(s6e88a0_ams452ef01_driver);

MODULE_AUTHOR("Michael Srba <Michael.Srba@seznam.cz>");
MODULE_DESCRIPTION("MIPI-DSI based Panel Driver for AMS452EF01 AMOLED LCD with a S6E88A0 controller");
MODULE_LICENSE("GPL v2");
