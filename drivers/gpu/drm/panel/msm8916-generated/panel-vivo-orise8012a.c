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

struct orise8012a {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data orise8012a_supplies[] = {
	{ .supply = "vdd" },
	{ .supply = "vddio" },
};

static inline struct orise8012a *to_orise8012a(struct drm_panel *panel)
{
	return container_of_const(panel, struct orise8012a, panel);
}

static void orise8012a_reset(struct orise8012a *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int orise8012a_on(struct orise8012a *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x80, 0x12, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0x80, 0x12);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x81);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0x57);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1, 0x25, 0x55);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa1);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x1c, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb3, 0x18);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0x10, 0x05, 0x15);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb4);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0x55);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x81);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4, 0x83, 0xf2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x8b);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4, 0x24);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x50);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x82);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0xf0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x02, 0x79, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x94);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x33, 0x33, 0x36);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x40);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xba);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf5, 0x01, 0x11);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xc2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xf5, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x82, 0x01, 0x1a, 0x81, 0x01, 0x1a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x13, 0x55, 0x1a, 0x13, 0x56, 0x1a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x18, 0x04, 0x03, 0x56, 0x00, 0x1a, 0x00,
				     0x18, 0x03, 0x03, 0x57, 0x00, 0x1a, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x18, 0x06, 0x03, 0x58, 0x00, 0x1a, 0x00,
				     0x18, 0x05, 0x03, 0x59, 0x00, 0x1a, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x18, 0x06, 0x03, 0x58, 0x00, 0x12, 0x1a,
				     0x18, 0x05, 0x03, 0x59, 0x00, 0x12, 0x1a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xce,
				     0x18, 0x04, 0x03, 0x59, 0x00, 0x12, 0x1a,
				     0x18, 0x03, 0x03, 0x5a, 0x00, 0x12, 0x1a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcf,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
				     0x84, 0x00, 0x0a, 0x0a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				     0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				     0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x15, 0x00, 0x15, 0x15, 0x15, 0x15, 0x15,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				     0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xd5);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				     0x00, 0x00, 0x15);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xe0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0x15, 0x15, 0x15, 0x15, 0x00, 0x15);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xf0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcb,
				     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
				     0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x01, 0x00, 0x0b, 0x09, 0x0f, 0x0d, 0x05,
				     0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x9a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x00, 0x00, 0x00, 0x00, 0x06, 0x0e, 0x10,
				     0x0a, 0x0c, 0x00, 0x02);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x06, 0x00, 0x0c, 0x0a, 0x0e, 0x10, 0x02,
				     0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xca);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xcc,
				     0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x0d,
				     0x09, 0x0b, 0x00, 0x05);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xd8, 0x97, 0x97);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe1,
				     0x04, 0x12, 0x1b, 0x26, 0x35, 0x41, 0x44,
				     0x70, 0x63, 0x7f, 0x82, 0x6a, 0x79, 0x51,
				     0x4d, 0x3f, 0x2f, 0x20, 0x1a, 0x0d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xe2,
				     0x04, 0x12, 0x1c, 0x26, 0x35, 0x42, 0x44,
				     0x70, 0x63, 0x7f, 0x81, 0x69, 0x79, 0x51,
				     0x4d, 0x3f, 0x2f, 0x20, 0x1a, 0x0d);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xec,
				     0x40, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
				     0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
				     0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
				     0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
				     0x44, 0x44, 0x44, 0x44, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xed,
				     0x40, 0x43, 0x43, 0x34, 0x34, 0x44, 0x43,
				     0x34, 0x34, 0x44, 0x43, 0x43, 0x34, 0x34,
				     0x44, 0x43, 0x34, 0x34, 0x44, 0x43, 0x43,
				     0x34, 0x34, 0x44, 0x43, 0x43, 0x34, 0x44,
				     0x43, 0x43, 0x34, 0x34, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xee,
				     0x40, 0x34, 0x44, 0x34, 0x44, 0x44, 0x43,
				     0x44, 0x34, 0x44, 0x34, 0x44, 0x44, 0x43,
				     0x44, 0x43, 0x44, 0x34, 0x44, 0x34, 0x44,
				     0x44, 0x43, 0x44, 0x34, 0x44, 0x34, 0x44,
				     0x44, 0x43, 0x44, 0x43, 0x04);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x83);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1, 0x50);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4, 0x30);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x8a);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4, 0x40);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x92);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc4, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xb1);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xb0, 0xb2);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x91);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc1, 0x08);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xa4);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc5, 0x55, 0x55);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xaa);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0xaa);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xc0, 0xc0);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0xff, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0xff, 0xff, 0xff, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x99, 0x35, 0x45);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x35);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x99, 0x00, 0x00);

	return dsi_ctx.accum_err;
}

static int orise8012a_off(struct orise8012a *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x99, 0x35, 0x45);
	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 50);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x99, 0x00, 0x00);

	return dsi_ctx.accum_err;
}

static int orise8012a_prepare(struct drm_panel *panel)
{
	struct orise8012a *ctx = to_orise8012a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(orise8012a_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	orise8012a_reset(ctx);

	ret = orise8012a_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(orise8012a_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int orise8012a_unprepare(struct drm_panel *panel)
{
	struct orise8012a *ctx = to_orise8012a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = orise8012a_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(orise8012a_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode orise8012a_mode = {
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

static int orise8012a_get_modes(struct drm_panel *panel,
				struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &orise8012a_mode);
}

static const struct drm_panel_funcs orise8012a_panel_funcs = {
	.prepare = orise8012a_prepare,
	.unprepare = orise8012a_unprepare,
	.get_modes = orise8012a_get_modes,
};

static int orise8012a_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct orise8012a *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct orise8012a, panel,
				   &orise8012a_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(orise8012a_supplies),
					    orise8012a_supplies,
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

static const struct of_device_id orise8012a_of_match[] = {
	{ .compatible = "vivo,orise8012a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, orise8012a_of_match);

static struct mipi_dsi_driver orise8012a_driver = {
	.probe = orise8012a_probe,
	.driver = {
		.name = "panel-vivo-orise8012a",
		.of_match_table = orise8012a_of_match,
	},
};
module_mipi_dsi_driver(orise8012a_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for orise8012a fwvga command mode dsi panel");
MODULE_LICENSE("GPL");
