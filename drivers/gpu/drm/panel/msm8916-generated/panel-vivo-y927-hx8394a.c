// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct hx8394a {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
};

static inline struct hx8394a *to_hx8394a(struct drm_panel *panel)
{
	return container_of_const(panel, struct hx8394a, panel);
}

static void hx8394a_reset(struct hx8394a *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int hx8394a_on(struct hx8394a *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb9, 0xff, 0x83, 0x94);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbc, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xba,
				     0x13, 0x82, 0x00, 0x16, 0xc5, 0x40);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb1,
				     0x01, 0x00, 0x07, 0x86, 0x01, 0x11, 0x11,
				     0x2a, 0x30, 0x3f, 0x3f, 0x47, 0x12, 0x01,
				     0xe6, 0xe2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2,
				     0x00, 0xc8, 0x08, 0x04, 0x00, 0x22);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd5,
				     0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x01,
				     0x00, 0xcc, 0x00, 0x00, 0x00, 0x88, 0x88,
				     0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
				     0x88, 0x01, 0x67, 0x45, 0x23, 0x01, 0x23,
				     0x88, 0x88, 0x88, 0x88);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb4,
				     0x80, 0x06, 0x32, 0x10, 0x03, 0x32, 0x15,
				     0x08, 0x32, 0x10, 0x08, 0x33, 0x04, 0x43,
				     0x05, 0x37, 0x04, 0x3f, 0x06, 0x61, 0x61,
				     0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb6, 0xfa);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe0,
				     0x02, 0x07, 0x07, 0x2b, 0x33, 0x3f, 0x0f,
				     0x32, 0x04, 0x0a, 0x0d, 0x11, 0x13, 0x11,
				     0x13, 0x10, 0x17, 0x02, 0x07, 0x07, 0x2b,
				     0x33, 0x3f, 0x0f, 0x32, 0x04, 0x0a, 0x0d,
				     0x11, 0x13, 0x11, 0x13, 0x10, 0x17, 0x07,
				     0x15, 0x07, 0x11, 0x07, 0x15, 0x07, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1,
				     0x01, 0x00, 0x0e, 0x15, 0x1e, 0x27, 0x30,
				     0x39, 0x41, 0x48, 0x4f, 0x57, 0x5e, 0x66,
				     0x6e, 0x77, 0x7f, 0x87, 0x8e, 0x96, 0x9e,
				     0xa5, 0xae, 0xb5, 0xbc, 0xc3, 0xcc, 0xd4,
				     0xda, 0xe1, 0xe7, 0xef, 0xf7, 0xff, 0x31,
				     0x25, 0x67, 0x5e, 0x1e, 0xd6, 0x10, 0x21,
				     0xc0, 0x00, 0x0e, 0x14, 0x1c, 0x25, 0x2d,
				     0x36, 0x3d, 0x44, 0x4b, 0x52, 0x59, 0x60,
				     0x67, 0x71, 0x78, 0x7f, 0x86, 0x8e, 0x95,
				     0x9c, 0xa3, 0xab, 0xb1, 0xb8, 0xbf, 0xc7,
				     0xcc, 0xd4, 0xdc, 0xe1, 0xe7, 0xee, 0x12,
				     0x33, 0xdb, 0xb1, 0x95, 0xa6, 0xe3, 0x13,
				     0xc0, 0x00, 0x0e, 0x14, 0x1d, 0x26, 0x2f,
				     0x38, 0x40, 0x47, 0x4d, 0x55, 0x5d, 0x64,
				     0x6b, 0x75, 0x7d, 0x84, 0x8b, 0x93, 0x9b,
				     0xa2, 0xa9, 0xb2, 0xb8, 0xc0, 0xc8, 0xce,
				     0xd5, 0xdc, 0xe2, 0xea, 0xf1, 0xf9, 0x2e,
				     0xa4, 0x38, 0x78, 0x6d, 0xb7, 0x2d, 0xb1,
				     0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc, 0x09);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0x0c, 0x17);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc7, 0x00, 0x10, 0x00, 0x10);
	mipi_dsi_dcs_set_tear_scanline_multi(&dsi_ctx, 0x0280);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x35);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbf, 0x06, 0x02, 0x10, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb9, 0x00);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 150);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);

	return dsi_ctx.accum_err;
}

static int hx8394a_off(struct hx8394a *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 50);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int hx8394a_prepare(struct drm_panel *panel)
{
	struct hx8394a *ctx = to_hx8394a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	hx8394a_reset(ctx);

	ret = hx8394a_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	return 0;
}

static int hx8394a_unprepare(struct drm_panel *panel)
{
	struct hx8394a *ctx = to_hx8394a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = hx8394a_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	return 0;
}

static const struct drm_display_mode hx8394a_mode = {
	.clock = (720 + 144 + 10 + 204) * (1280 + 6 + 1 + 9) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 144,
	.hsync_end = 720 + 144 + 10,
	.htotal = 720 + 144 + 10 + 204,
	.vdisplay = 1280,
	.vsync_start = 1280 + 6,
	.vsync_end = 1280 + 6 + 1,
	.vtotal = 1280 + 6 + 1 + 9,
	.width_mm = 59,
	.height_mm = 104,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int hx8394a_get_modes(struct drm_panel *panel,
			     struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &hx8394a_mode);
}

static const struct drm_panel_funcs hx8394a_panel_funcs = {
	.prepare = hx8394a_prepare,
	.unprepare = hx8394a_unprepare,
	.get_modes = hx8394a_get_modes,
};

static int hx8394a_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct hx8394a *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct hx8394a, panel,
				   &hx8394a_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS |
			  MIPI_DSI_MODE_VIDEO_NO_HBP;

	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void hx8394a_remove(struct mipi_dsi_device *dsi)
{
	struct hx8394a *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id hx8394a_of_match[] = {
	{ .compatible = "vivo,y927-hx8394a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hx8394a_of_match);

static struct mipi_dsi_driver hx8394a_driver = {
	.probe = hx8394a_probe,
	.remove = hx8394a_remove,
	.driver = {
		.name = "panel-hx8394a",
		.of_match_table = hx8394a_of_match,
	},
};
module_mipi_dsi_driver(hx8394a_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for hx8394a 720p video mode dsi panel");
MODULE_LICENSE("GPL");
