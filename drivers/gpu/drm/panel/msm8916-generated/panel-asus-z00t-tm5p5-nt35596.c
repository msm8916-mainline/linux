// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct tm5p5_nt35596 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *backlight_gpio;
};

static inline struct tm5p5_nt35596 *to_tm5p5_nt35596(struct drm_panel *panel)
{
	return container_of_const(panel, struct tm5p5_nt35596, panel);
}

static void tm5p5_nt35596_reset(struct tm5p5_nt35596 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(15000, 16000);
}

static int tm5p5_nt35596_on(struct tm5p5_nt35596 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x05);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc5, 0x31);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x04);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x01, 0x84);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x05, 0x25);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x06, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x07, 0x20);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x08, 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x09, 0x08);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0a, 0x10);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0b, 0x10);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0c, 0x10);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0d, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0e, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x0f, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x10, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x11, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x12, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x17, 0xf3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x18, 0xc0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x19, 0xc0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1a, 0xc0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1b, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1c, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1d, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1e, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1f, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x20, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xfb, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x35, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xd3, 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xd4, 0x04);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x51, 0x0d);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x53, 0x24);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x5e, 0x0d);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x11, 0x00);
	mipi_dsi_msleep(&dsi_ctx, 100);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x29, 0x00);

	return dsi_ctx.accum_err;
}

static int tm5p5_nt35596_off(struct tm5p5_nt35596 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 60);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	// WARNING: Ignoring weird NULL_PACKET
	mipi_dsi_msleep(&dsi_ctx, 72);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x4f, 0x01);

	return dsi_ctx.accum_err;
}

static int tm5p5_nt35596_prepare(struct drm_panel *panel)
{
	struct tm5p5_nt35596 *ctx = to_tm5p5_nt35596(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	tm5p5_nt35596_reset(ctx);

	ret = tm5p5_nt35596_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	return 0;
}

static int tm5p5_nt35596_unprepare(struct drm_panel *panel)
{
	struct tm5p5_nt35596 *ctx = to_tm5p5_nt35596(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = tm5p5_nt35596_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	return 0;
}

static const struct drm_display_mode tm5p5_nt35596_mode = {
	.clock = (1080 + 100 + 8 + 16) * (1920 + 4 + 2 + 4) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 100,
	.hsync_end = 1080 + 100 + 8,
	.htotal = 1080 + 100 + 8 + 16,
	.vdisplay = 1920,
	.vsync_start = 1920 + 4,
	.vsync_end = 1920 + 4 + 2,
	.vtotal = 1920 + 4 + 2 + 4,
	.width_mm = 68,
	.height_mm = 121,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int tm5p5_nt35596_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &tm5p5_nt35596_mode);
}

static const struct drm_panel_funcs tm5p5_nt35596_panel_funcs = {
	.prepare = tm5p5_nt35596_prepare,
	.unprepare = tm5p5_nt35596_unprepare,
	.get_modes = tm5p5_nt35596_get_modes,
};

static int tm5p5_nt35596_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	struct tm5p5_nt35596 *ctx = mipi_dsi_get_drvdata(dsi);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	gpiod_set_value_cansleep(ctx->backlight_gpio, !!brightness);

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops tm5p5_nt35596_bl_ops = {
	.update_status = tm5p5_nt35596_bl_update_status,
};

static struct backlight_device *
tm5p5_nt35596_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &tm5p5_nt35596_bl_ops, &props);
}

static int tm5p5_nt35596_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tm5p5_nt35596 *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct tm5p5_nt35596, panel,
				   &tm5p5_nt35596_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ctx->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(ctx->supply))
		return dev_err_probe(dev, PTR_ERR(ctx->supply),
				     "Failed to get power regulator\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->backlight_gpio = devm_gpiod_get(dev, "backlight", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->backlight_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->backlight_gpio),
				     "Failed to get backlight-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = tm5p5_nt35596_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void tm5p5_nt35596_remove(struct mipi_dsi_device *dsi)
{
	struct tm5p5_nt35596 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id tm5p5_nt35596_of_match[] = {
	{ .compatible = "asus,z00t-tm5p5-nt35596" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tm5p5_nt35596_of_match);

static struct mipi_dsi_driver tm5p5_nt35596_driver = {
	.probe = tm5p5_nt35596_probe,
	.remove = tm5p5_nt35596_remove,
	.driver = {
		.name = "panel-asus-z00t-tm5p5-nt35596",
		.of_match_table = tm5p5_nt35596_of_match,
	},
};
module_mipi_dsi_driver(tm5p5_nt35596_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for tm5p5 nt35596 1080p video mode dsi panel");
MODULE_LICENSE("GPL");
