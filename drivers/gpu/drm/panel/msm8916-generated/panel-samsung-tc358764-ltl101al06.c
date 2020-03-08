// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct tc358764_ltl101a106 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	struct clk *pwm_clk;
	bool prepared;
};

static inline
struct tc358764_ltl101a106 *to_tc358764_ltl101a106(struct drm_panel *panel)
{
	return container_of(panel, struct tc358764_ltl101a106, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

/* FIXME: Fix panel initialization */

static void tc358764_ltl101a106_reset(struct tc358764_ltl101a106 *ctx)
{
#if 0
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
#endif
}

static int tc358764_ltl101a106_on(struct tc358764_ltl101a106 *ctx)
{
#if 0
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_generic_write_seq(dsi, 0x3c, 0x01, 0x06, 0x00, 0x05, 0x00);
	dsi_generic_write_seq(dsi, 0x14, 0x01, 0x04, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x64, 0x01, 0x04, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x68, 0x01, 0x04, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x6c, 0x01, 0x04, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x70, 0x01, 0x04, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x34, 0x01, 0x1f, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x10, 0x02, 0x1f, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x04, 0x01, 0x01, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x04, 0x02, 0x01, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x50, 0x04, 0x20, 0x01, 0xf0, 0x03);
	dsi_generic_write_seq(dsi, 0x54, 0x04, 0x02, 0x00, 0x30, 0x00);
	dsi_generic_write_seq(dsi, 0x58, 0x04, 0x20, 0x03, 0x30, 0x00);
	dsi_generic_write_seq(dsi, 0x5c, 0x04, 0x02, 0x00, 0x40, 0x00);
	dsi_generic_write_seq(dsi, 0x60, 0x04, 0x00, 0x05, 0x20, 0x00);
	dsi_generic_write_seq(dsi, 0x64, 0x04, 0x01, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xa0, 0x04, 0x06, 0x80, 0x44, 0x00);
	usleep_range(2000, 3000);
	dsi_generic_write_seq(dsi, 0xa0, 0x04, 0x06, 0x80, 0x04, 0x00);
	usleep_range(2000, 3000);
	dsi_generic_write_seq(dsi, 0x04, 0x05, 0x04, 0x00, 0x00, 0x00);
	usleep_range(2000, 3000);
	dsi_generic_write_seq(dsi, 0x80, 0x04, 0x00, 0x01, 0x02, 0x03);
	dsi_generic_write_seq(dsi, 0x84, 0x04, 0x04, 0x07, 0x05, 0x08);
	dsi_generic_write_seq(dsi, 0x88, 0x04, 0x09, 0x0a, 0x0e, 0x0f);
	dsi_generic_write_seq(dsi, 0x8c, 0x04, 0x0b, 0x0c, 0x0d, 0x10);
	dsi_generic_write_seq(dsi, 0x90, 0x04, 0x16, 0x17, 0x11, 0x12);
	dsi_generic_write_seq(dsi, 0x94, 0x04, 0x13, 0x14, 0x15, 0x1b);
	dsi_generic_write_seq(dsi, 0x98, 0x04, 0x18, 0x19, 0x1a, 0x06);
	dsi_generic_write_seq(dsi, 0x9c, 0x04, 0x01, 0x00, 0x00, 0x00);
#endif

	return 0;
}

static int tc358764_ltl101a106_off(struct tc358764_ltl101a106 *ctx)
{
#if 0
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	dsi_generic_write_seq(dsi, 0xa0, 0x04, 0x01, 0x00, 0x00, 0x00);
	usleep_range(1000, 2000);
	dsi_generic_write_seq(dsi, 0x9c, 0x04, 0x00, 0x00, 0x00, 0x00);
#endif

	return 0;
}

static int tc358764_ltl101a106_prepare(struct drm_panel *panel)
{
	struct tc358764_ltl101a106 *ctx = to_tc358764_ltl101a106(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	tc358764_ltl101a106_reset(ctx);

	ret = tc358764_ltl101a106_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ret = clk_prepare_enable(ctx->pwm_clk);
	if (ret < 0) {
		dev_err(dev, "Failed to enable pwm clock: %d\n", ret);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int tc358764_ltl101a106_unprepare(struct drm_panel *panel)
{
	struct tc358764_ltl101a106 *ctx = to_tc358764_ltl101a106(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	clk_disable_unprepare(ctx->pwm_clk);

	ret = tc358764_ltl101a106_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode tc358764_ltl101a106_mode = {
	.clock = (1280 + 52 + 4 + 48) * (800 + 32 + 6 + 64) * 60 / 1000,
	.hdisplay = 1280,
	.hsync_start = 1280 + 52,
	.hsync_end = 1280 + 52 + 4,
	.htotal = 1280 + 52 + 4 + 48,
	.vdisplay = 800,
	.vsync_start = 800 + 32,
	.vsync_end = 800 + 32 + 6,
	.vtotal = 800 + 32 + 6 + 64,
	.width_mm = 228,
	.height_mm = 149,
};

static int tc358764_ltl101a106_get_modes(struct drm_panel *panel,
					 struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &tc358764_ltl101a106_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs tc358764_ltl101a106_panel_funcs = {
	.prepare = tc358764_ltl101a106_prepare,
	.unprepare = tc358764_ltl101a106_unprepare,
	.get_modes = tc358764_ltl101a106_get_modes,
};

static int tc358764_ltl101a106_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tc358764_ltl101a106 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "lcd";
	ctx->supplies[1].supply = "lvds";
	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(ctx->supplies),
				      ctx->supplies);
	if (ret < 0)
		return dev_err_probe(dev, ret, "Failed to get regulators\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_ASIS);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->pwm_clk = devm_clk_get(dev, "pwm");
	if (IS_ERR(ctx->pwm_clk))
		return dev_err_probe(dev, PTR_ERR(ctx->pwm_clk),
				     "Failed to get pwm clock\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			  MIPI_DSI_MODE_NO_EOT_PACKET;

	drm_panel_init(&ctx->panel, dev, &tc358764_ltl101a106_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static int tc358764_ltl101a106_remove(struct mipi_dsi_device *dsi)
{
	struct tc358764_ltl101a106 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id tc358764_ltl101a106_of_match[] = {
	{ .compatible = "samsung,tc358764-ltl101al06" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tc358764_ltl101a106_of_match);

static struct mipi_dsi_driver tc358764_ltl101a106_driver = {
	.probe = tc358764_ltl101a106_probe,
	.remove = tc358764_ltl101a106_remove,
	.driver = {
		.name = "panel-tc358764-ltl101a106",
		.of_match_table = tc358764_ltl101a106_of_match,
	},
};
module_mipi_dsi_driver(tc358764_ltl101a106_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for ss_dsi_panel_TC358764_LTL101A106_WXGA");
MODULE_LICENSE("GPL v2");
