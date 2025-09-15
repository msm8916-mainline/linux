// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct ea8061_id400418 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data ea8061_id400418_supplies[] = {
	{ .supply = "vdd3" },
	{ .supply = "vci" },
};

static inline
struct ea8061_id400418 *to_ea8061_id400418(struct drm_panel *panel)
{
	return container_of(panel, struct ea8061_id400418, panel);
}

static void ea8061_id400418_reset(struct ea8061_id400418 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int ea8061_id400418_on(struct ea8061_id400418 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf0, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf7, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4,
				     0x54, 0xb3, 0x54, 0xb3, 0x64, 0x9a, 0x64,
				     0x9a, 0x00, 0x00, 0x0b, 0xfa, 0x00, 0x0b,
				     0xfa, 0x00, 0x00, 0x09, 0x09, 0x09, 0x36,
				     0x68, 0xab, 0x00, 0x00, 0x08, 0x02, 0x05,
				     0x00, 0x0c, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf7, 0xa5, 0xa5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_SET_ADDRESS_MODE, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x00, 0x30, 0x00, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb4, 0x33, 0x07, 0x00);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf7, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xca,
				     0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80,
				     0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
				     0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
				     0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
				     0x80, 0x80, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x00, 0x30, 0x00, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf7, 0xa5, 0xa5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2,
				     0x0f, 0xb4, 0xa0, 0x13, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf1, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd4, 0x38, 0x00, 0x48);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf1, 0xa5, 0xa5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, MIPI_DCS_WRITE_POWER_SAVE, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x06);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2, 0x19);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf1, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd4, 0x48);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf1, 0xa5, 0xa5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2, 0x0f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf1, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xfc, 0x5a, 0x5a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd7, 0x0a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x0a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf1, 0xa5, 0xa5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xfc, 0xa5, 0xa5);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 34);

	return dsi_ctx.accum_err;
}

static int ea8061_id400418_off(struct ea8061_id400418 *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 34);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 150);

	return dsi_ctx.accum_err;
}

static int ea8061_id400418_prepare(struct drm_panel *panel)
{
	struct ea8061_id400418 *ctx = to_ea8061_id400418(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(ea8061_id400418_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	ea8061_id400418_reset(ctx);

	ret = ea8061_id400418_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ea8061_id400418_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int ea8061_id400418_unprepare(struct drm_panel *panel)
{
	struct ea8061_id400418 *ctx = to_ea8061_id400418(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = ea8061_id400418_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ea8061_id400418_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode ea8061_id400418_mode = {
	.clock = (720 + 114 + 96 + 114) * (1280 + 13 + 2 + 5) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 114,
	.hsync_end = 720 + 114 + 96,
	.htotal = 720 + 114 + 96 + 114,
	.vdisplay = 1280,
	.vsync_start = 1280 + 13,
	.vsync_end = 1280 + 13 + 2,
	.vtotal = 1280 + 13 + 2 + 5,
	.width_mm = 68,
	.height_mm = 122,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int ea8061_id400418_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &ea8061_id400418_mode);
}

static const struct drm_panel_funcs ea8061_id400418_panel_funcs = {
	.prepare = ea8061_id400418_prepare,
	.unprepare = ea8061_id400418_unprepare,
	.get_modes = ea8061_id400418_get_modes,
};

static int ea8061_id400418_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct ea8061_id400418 *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct ea8061_id400418, panel,
				   &ea8061_id400418_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(ea8061_id400418_supplies),
					    ea8061_id400418_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_NO_EOT_PACKET;

	ctx->panel.prepare_prev_first = true;

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void ea8061_id400418_remove(struct mipi_dsi_device *dsi)
{
	struct ea8061_id400418 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id ea8061_id400418_of_match[] = {
	{ .compatible = "samsung,ea8061-ams549bu19-id400418" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ea8061_id400418_of_match);

static struct mipi_dsi_driver ea8061_id400418_driver = {
	.probe = ea8061_id400418_probe,
	.remove = ea8061_id400418_remove,
	.driver = {
		.name = "panel-samsung-ea8061-ams549bu19-id400418",
		.of_match_table = ea8061_id400418_of_match,
	},
};
module_mipi_dsi_driver(ea8061_id400418_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for Samsung EA8061_ID400418 HD video mode panel");
MODULE_LICENSE("GPL");
