// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct boe_otm8019a_5p0 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct boe_otm8019a_5p0 *to_boe_otm8019a_5p0(struct drm_panel *panel)
{
	return container_of(panel, struct boe_otm8019a_5p0, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void boe_otm8019a_5p0_reset(struct boe_otm8019a_5p0 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(120);
}

static int boe_otm8019a_5p0_on(struct boe_otm8019a_5p0 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0x80, 0x19, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xff, 0x80, 0x19);
	dsi_generic_write_seq(dsi, 0x00, 0x03);
	dsi_generic_write_seq(dsi, 0xff, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xb3, 0x02);
	dsi_generic_write_seq(dsi, 0x00, 0x92);
	dsi_generic_write_seq(dsi, 0xb3, 0x45);
	dsi_generic_write_seq(dsi, 0x00, 0xa2);
	dsi_generic_write_seq(dsi, 0xc0, 0x00, 0x1b);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc0,
			      0x00, 0x58, 0x00, 0x14, 0x16, 0x00, 0x58, 0x14,
			      0x16);
	dsi_generic_write_seq(dsi, 0x00, 0xb4);
	dsi_generic_write_seq(dsi, 0xc0, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb5);
	dsi_generic_write_seq(dsi, 0xc0, 0x18);
	dsi_generic_write_seq(dsi, 0x00, 0x81);
	dsi_generic_write_seq(dsi, 0xc4, 0x04);
	dsi_generic_write_seq(dsi, 0x00, 0x8a);
	dsi_generic_write_seq(dsi, 0xc4, 0x40);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc1, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xc0, 0x00, 0x15, 0x00, 0x00, 0x00, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc4, 0x30);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xc1, 0xe8);
	dsi_generic_write_seq(dsi, 0x00, 0x98);
	dsi_generic_write_seq(dsi, 0xc0, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa9);
	dsi_generic_write_seq(dsi, 0xc0, 0x0a);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xc1, 0x20, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xe1);
	dsi_generic_write_seq(dsi, 0xc0, 0x40, 0x30);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xb6, 0xb4);
	dsi_generic_write_seq(dsi, 0x00, 0x87);
	dsi_generic_write_seq(dsi, 0xc4, 0x18);
	dsi_generic_write_seq(dsi, 0x00, 0x89);
	dsi_generic_write_seq(dsi, 0xc4, 0x08);
	dsi_generic_write_seq(dsi, 0x00, 0x82);
	dsi_generic_write_seq(dsi, 0xc5, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xc5, 0x4e, 0x29, 0x00, 0x7b, 0x44);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xd8, 0x5f);
	dsi_generic_write_seq(dsi, 0x00, 0x01);
	dsi_generic_write_seq(dsi, 0xd8, 0x5f);
	dsi_generic_write_seq(dsi, 0x00, 0x81);
	dsi_generic_write_seq(dsi, 0xc1, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0xa1);
	dsi_generic_write_seq(dsi, 0xc1, 0x08);
	dsi_generic_write_seq(dsi, 0x00, 0xb1);
	dsi_generic_write_seq(dsi, 0xc5, 0x29);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe1,
			      0x1c, 0x2a, 0x34, 0x40, 0x4d, 0x5c, 0x5c, 0x82,
			      0x73, 0x8b, 0x7a, 0x65, 0x77, 0x55, 0x52, 0x41,
			      0x34, 0x29, 0x23, 0x1f);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe2,
			      0x1c, 0x2a, 0x34, 0x40, 0x4d, 0x5c, 0x5c, 0x82,
			      0x73, 0x8b, 0x7a, 0x65, 0x77, 0x55, 0x52, 0x41,
			      0x34, 0x29, 0x23, 0x1f);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xce,
			      0x86, 0x01, 0x00, 0x85, 0x01, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xce,
			      0x18, 0x05, 0x83, 0x5a, 0x86, 0x04, 0x00, 0x18,
			      0x04, 0x83, 0x5b, 0x86, 0x04, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xce,
			      0x18, 0x03, 0x83, 0x5c, 0x86, 0x04, 0x00, 0x18,
			      0x02, 0x83, 0x5d, 0x86, 0x04, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcf,
			      0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x01, 0x01,
			      0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcf, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xe0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
			      0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xf0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			      0xff, 0xff);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x26, 0x09, 0x0b, 0x01, 0x25, 0x00, 0x00,
			      0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x02, 0x0c,
			      0x0a, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x25, 0x0a, 0x0c, 0x02, 0x26, 0x00, 0x00,
			      0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x01, 0x0b,
			      0x09, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xca, 0x09);
	dsi_generic_write_seq(dsi, 0x00, 0xb3);
	dsi_generic_write_seq(dsi, 0xca, 0x50);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xfb, 0x00);
	msleep(50);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0xff, 0xff, 0xff);

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0x0000);
	if (ret < 0) {
		dev_err(dev, "Failed to set display brightness: %d\n", ret);
		return ret;
	}

	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x24);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x01);
	dsi_dcs_write_seq(dsi, MIPI_DCS_SET_CABC_MIN_BRIGHTNESS, 0x28);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(20);

	return 0;
}

static int boe_otm8019a_5p0_off(struct boe_otm8019a_5p0 *ctx)
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
	msleep(20);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int boe_otm8019a_5p0_prepare(struct drm_panel *panel)
{
	struct boe_otm8019a_5p0 *ctx = to_boe_otm8019a_5p0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	boe_otm8019a_5p0_reset(ctx);

	ret = boe_otm8019a_5p0_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int boe_otm8019a_5p0_unprepare(struct drm_panel *panel)
{
	struct boe_otm8019a_5p0 *ctx = to_boe_otm8019a_5p0(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = boe_otm8019a_5p0_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode boe_otm8019a_5p0_mode = {
	.clock = (480 + 92 + 12 + 88) * (854 + 18 + 4 + 18) * 60 / 1000,
	.hdisplay = 480,
	.hsync_start = 480 + 92,
	.hsync_end = 480 + 92 + 12,
	.htotal = 480 + 92 + 12 + 88,
	.vdisplay = 854,
	.vsync_start = 854 + 18,
	.vsync_end = 854 + 18 + 4,
	.vtotal = 854 + 18 + 4 + 18,
	.width_mm = 62,
	.height_mm = 110,
};

static int boe_otm8019a_5p0_get_modes(struct drm_panel *panel,
				      struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &boe_otm8019a_5p0_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs boe_otm8019a_5p0_panel_funcs = {
	.prepare = boe_otm8019a_5p0_prepare,
	.unprepare = boe_otm8019a_5p0_unprepare,
	.get_modes = boe_otm8019a_5p0_get_modes,
};

static int boe_otm8019a_5p0_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

// TODO: Check if /sys/class/backlight/.../actual_brightness actually returns
// correct values. If not, remove this function.
static int boe_otm8019a_5p0_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return brightness & 0xff;
}

static const struct backlight_ops boe_otm8019a_5p0_bl_ops = {
	.update_status = boe_otm8019a_5p0_bl_update_status,
	.get_brightness = boe_otm8019a_5p0_bl_get_brightness,
};

static struct backlight_device *
boe_otm8019a_5p0_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 250,
		.max_brightness = 250,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &boe_otm8019a_5p0_bl_ops, &props);
}

static int boe_otm8019a_5p0_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct boe_otm8019a_5p0 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(ctx->supply))
		return dev_err_probe(dev, PTR_ERR(ctx->supply),
				     "Failed to get power regulator\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &boe_otm8019a_5p0_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = boe_otm8019a_5p0_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static void boe_otm8019a_5p0_remove(struct mipi_dsi_device *dsi)
{
	struct boe_otm8019a_5p0 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id boe_otm8019a_5p0_of_match[] = {
	{ .compatible = "huawei,boe-otm8019a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, boe_otm8019a_5p0_of_match);

static struct mipi_dsi_driver boe_otm8019a_5p0_driver = {
	.probe = boe_otm8019a_5p0_probe,
	.remove = boe_otm8019a_5p0_remove,
	.driver = {
		.name = "panel-boe-otm8019a-5p0",
		.of_match_table = boe_otm8019a_5p0_of_match,
	},
};
module_mipi_dsi_driver(boe_otm8019a_5p0_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for BOE_OTM8019A_5P0_FWVGA_VIDEO");
MODULE_LICENSE("GPL");
