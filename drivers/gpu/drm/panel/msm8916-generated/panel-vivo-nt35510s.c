// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2026 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/device-id/of.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct nt35510s {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data nt35510s_supplies[] = {
	{ .supply = "vdd" },
	{ .supply = "vddio" },
};

static inline struct nt35510s *to_nt35510s(struct drm_panel *panel)
{
	return container_of_const(panel, struct nt35510s, panel);
}

static void nt35510s_reset(struct nt35510s *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int nt35510s_on(struct nt35510s *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0xaa, 0x55, 0xa5, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6f, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf2, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x6f, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf7, 0x28);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0xaa, 0x55, 0xa5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf0,
				     0x55, 0xaa, 0x52, 0x08, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb6, 0x34, 0x44, 0x44);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0x00, 0x0f, 0x0f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb7, 0x24, 0x34, 0x34);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb1, 0x00, 0x0f, 0x0f);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb8, 0x05, 0x34, 0x34);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbf, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb2, 0x03, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb9, 0x24, 0x34, 0x34);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x05, 0x05, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xba, 0x14, 0x14, 0x14);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb5, 0x0c, 0x08, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbc, 0x00, 0x60, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbd, 0x00, 0x60, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbe, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd1,
				     0x00, 0x69, 0x00, 0x8b, 0x00, 0xb2, 0x00,
				     0xd5, 0x00, 0xe7, 0x01, 0x07, 0x01, 0x20,
				     0x01, 0x4f, 0x01, 0x77, 0x01, 0xac, 0x01,
				     0xd8, 0x02, 0x20, 0x02, 0x5a, 0x02, 0x5c,
				     0x02, 0x90, 0x02, 0xc8, 0x02, 0xeb, 0x03,
				     0x17, 0x03, 0x32, 0x03, 0x79, 0x03, 0x9c,
				     0x03, 0xc0, 0x03, 0xd2, 0x03, 0xe4, 0x03,
				     0xf6, 0x03, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd2,
				     0x01, 0x5c, 0x01, 0x61, 0x01, 0x69, 0x01,
				     0x73, 0x01, 0x79, 0x01, 0x86, 0x01, 0x91,
				     0x01, 0xa9, 0x01, 0xc1, 0x01, 0xe5, 0x02,
				     0x05, 0x02, 0x3e, 0x02, 0x70, 0x02, 0x72,
				     0x02, 0xa1, 0x02, 0xd8, 0x02, 0xfb, 0x03,
				     0x2a, 0x03, 0x49, 0x03, 0x88, 0x03, 0xa7,
				     0x03, 0xc7, 0x03, 0xd7, 0x03, 0xe7, 0x03,
				     0xf7, 0x03, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd3,
				     0x01, 0x30, 0x01, 0x36, 0x01, 0x40, 0x01,
				     0x4c, 0x01, 0x54, 0x01, 0x63, 0x01, 0x70,
				     0x01, 0x8c, 0x01, 0xa8, 0x01, 0xd1, 0x01,
				     0xf5, 0x02, 0x34, 0x02, 0x69, 0x02, 0x6b,
				     0x02, 0x9c, 0x02, 0xd5, 0x02, 0xfa, 0x03,
				     0x2e, 0x03, 0x56, 0x03, 0x90, 0x03, 0xad,
				     0x03, 0xca, 0x03, 0xd9, 0x03, 0xe8, 0x03,
				     0xf7, 0x03, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd4,
				     0x00, 0x69, 0x00, 0x8b, 0x00, 0xb2, 0x00,
				     0xd5, 0x00, 0xe7, 0x01, 0x07, 0x01, 0x20,
				     0x01, 0x4f, 0x01, 0x77, 0x01, 0xac, 0x01,
				     0xd8, 0x02, 0x20, 0x02, 0x5a, 0x02, 0x5c,
				     0x02, 0x90, 0x02, 0xc8, 0x02, 0xeb, 0x03,
				     0x17, 0x03, 0x32, 0x03, 0x79, 0x03, 0x9c,
				     0x03, 0xc0, 0x03, 0xd2, 0x03, 0xe4, 0x03,
				     0xf6, 0x03, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd5,
				     0x01, 0x5c, 0x01, 0x61, 0x01, 0x69, 0x01,
				     0x73, 0x01, 0x79, 0x01, 0x86, 0x01, 0x91,
				     0x01, 0xa9, 0x01, 0xc1, 0x01, 0xe5, 0x02,
				     0x05, 0x02, 0x3e, 0x02, 0x70, 0x02, 0x72,
				     0x02, 0xa1, 0x02, 0xd8, 0x02, 0xfb, 0x03,
				     0x2a, 0x03, 0x49, 0x03, 0x88, 0x03, 0xa7,
				     0x03, 0xc7, 0x03, 0xd7, 0x03, 0xe7, 0x03,
				     0xf7, 0x03, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd6,
				     0x01, 0x30, 0x01, 0x36, 0x01, 0x40, 0x01,
				     0x4c, 0x01, 0x54, 0x01, 0x63, 0x01, 0x70,
				     0x01, 0x8c, 0x01, 0xa8, 0x01, 0xd1, 0x01,
				     0xf5, 0x02, 0x34, 0x02, 0x69, 0x02, 0x6b,
				     0x02, 0x9c, 0x02, 0xd5, 0x02, 0xfa, 0x03,
				     0x2e, 0x03, 0x56, 0x03, 0x90, 0x03, 0xad,
				     0x03, 0xca, 0x03, 0xd9, 0x03, 0xe8, 0x03,
				     0xf7, 0x03, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf0,
				     0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb1, 0xfc, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb5, 0x6b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb6, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb7, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb8, 0x01, 0x07, 0x07, 0x07);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xbc, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc9,
				     0xc0, 0x02, 0x50, 0x90, 0x50);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);

	return dsi_ctx.accum_err;
}

static int nt35510s_off(struct nt35510s *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 50);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int nt35510s_prepare(struct drm_panel *panel)
{
	struct nt35510s *ctx = to_nt35510s(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(nt35510s_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	nt35510s_reset(ctx);

	ret = nt35510s_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(nt35510s_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int nt35510s_unprepare(struct drm_panel *panel)
{
	struct nt35510s *ctx = to_nt35510s(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = nt35510s_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(nt35510s_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode nt35510s_mode = {
	.clock = (480 + 76 + 70 + 76) * (854 + 6 + 5 + 27) * 60 / 1000,
	.hdisplay = 480,
	.hsync_start = 480 + 76,
	.hsync_end = 480 + 76 + 70,
	.htotal = 480 + 76 + 70 + 76,
	.vdisplay = 854,
	.vsync_start = 854 + 6,
	.vsync_end = 854 + 6 + 5,
	.vtotal = 854 + 6 + 5 + 27,
	.width_mm = 0,
	.height_mm = 0,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int nt35510s_get_modes(struct drm_panel *panel,
			      struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &nt35510s_mode);
}

static const struct drm_panel_funcs nt35510s_panel_funcs = {
	.prepare = nt35510s_prepare,
	.unprepare = nt35510s_unprepare,
	.get_modes = nt35510s_get_modes,
};

static int nt35510s_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct nt35510s *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct nt35510s, panel,
				   &nt35510s_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(nt35510s_supplies),
					    nt35510s_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

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
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	ret = devm_drm_panel_add(dev, &ctx->panel);
	if (ret)
		return ret;

	return devm_mipi_dsi_attach(dev, dsi);
}

static const struct of_device_id nt35510s_of_match[] = {
	{ .compatible = "vivo,nt35510s" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, nt35510s_of_match);

static struct mipi_dsi_driver nt35510s_driver = {
	.probe = nt35510s_probe,
	.driver = {
		.name = "panel-vivo-nt35510s",
		.of_match_table = nt35510s_of_match,
	},
};
module_mipi_dsi_driver(nt35510s_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for nt35510s fwvga command mode dsi panel");
MODULE_LICENSE("GPL");
