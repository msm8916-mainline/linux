// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct s6e3fa2_ams549eh01 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[3];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct s6e3fa2_ams549eh01 *to_s6e3fa2_ams549eh01(struct drm_panel *panel)
{
	return container_of(panel, struct s6e3fa2_ams549eh01, panel);
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

static void s6e3fa2_ams549eh01_reset(struct s6e3fa2_ams549eh01 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int s6e3fa2_ams549eh01_on(struct s6e3fa2_ams549eh01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(20);

	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xca,
			      0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x80,
			      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
			      0x00);
	dsi_generic_write_seq(dsi, 0xb2, 0x00, 0x0e, 0x00, 0x0e);
	dsi_generic_write_seq(dsi, 0xb6, 0x8c, 0x0b);
	dsi_generic_write_seq(dsi, 0xf7, 0x03);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xb0, 0x05);
	dsi_generic_write_seq(dsi, 0xb8, 0x19);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0x35, 0x00);
	dsi_generic_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xfc, 0x5a, 0x5a);
	dsi_generic_write_seq(dsi, 0xfd, 0xb8);
	dsi_generic_write_seq(dsi, 0xb0, 0x14);
	dsi_generic_write_seq(dsi, 0xd7, 0x75);
	dsi_generic_write_seq(dsi, 0xb0, 0x20);
	dsi_generic_write_seq(dsi, 0xd7, 0x00);
	dsi_generic_write_seq(dsi, 0xfe, 0x80);
	dsi_generic_write_seq(dsi, 0xfe, 0x00);
	msleep(120);
	dsi_generic_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_generic_write_seq(dsi, 0xfc, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_MEMORY_START);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_MEMORY_CONTINUE);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int s6e3fa2_ams549eh01_off(struct s6e3fa2_ams549eh01 *ctx)
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
	usleep_range(10000, 11000);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(150);

	return 0;
}

static int s6e3fa2_ams549eh01_prepare(struct drm_panel *panel)
{
	struct s6e3fa2_ams549eh01 *ctx = to_s6e3fa2_ams549eh01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	s6e3fa2_ams549eh01_reset(ctx);

	ret = s6e3fa2_ams549eh01_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int s6e3fa2_ams549eh01_unprepare(struct drm_panel *panel)
{
	struct s6e3fa2_ams549eh01 *ctx = to_s6e3fa2_ams549eh01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = s6e3fa2_ams549eh01_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode s6e3fa2_ams549eh01_mode = {
	.clock = (1080 + 160 + 12 + 36) * (1920 + 12 + 2 + 3) * 50 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 160,
	.hsync_end = 1080 + 160 + 12,
	.htotal = 1080 + 160 + 12 + 36,
	.vdisplay = 1920,
	.vsync_start = 1920 + 12,
	.vsync_end = 1920 + 12 + 2,
	.vtotal = 1920 + 12 + 2 + 3,
	.width_mm = 69,
	.height_mm = 122,
};

static int s6e3fa2_ams549eh01_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &s6e3fa2_ams549eh01_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs s6e3fa2_ams549eh01_panel_funcs = {
	.prepare = s6e3fa2_ams549eh01_prepare,
	.unprepare = s6e3fa2_ams549eh01_unprepare,
	.get_modes = s6e3fa2_ams549eh01_get_modes,
};

static int s6e3fa2_ams549eh01_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct s6e3fa2_ams549eh01 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vdd3";
	ctx->supplies[1].supply = "vddr";
	ctx->supplies[2].supply = "vcc";
	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(ctx->supplies),
				      ctx->supplies);
	if (ret < 0)
		return dev_err_probe(dev, ret, "Failed to get regulators\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO_BURST;

	drm_panel_init(&ctx->panel, dev, &s6e3fa2_ams549eh01_panel_funcs,
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

static void s6e3fa2_ams549eh01_remove(struct mipi_dsi_device *dsi)
{
	struct s6e3fa2_ams549eh01 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id s6e3fa2_ams549eh01_of_match[] = {
	{ .compatible = "samsung,s6e3fa2-ams549eh01" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, s6e3fa2_ams549eh01_of_match);

static struct mipi_dsi_driver s6e3fa2_ams549eh01_driver = {
	.probe = s6e3fa2_ams549eh01_probe,
	.remove = s6e3fa2_ams549eh01_remove,
	.driver = {
		.name = "panel-s6e3fa2-ams549eh01",
		.of_match_table = s6e3fa2_ams549eh01_of_match,
	},
};
module_mipi_dsi_driver(s6e3fa2_ams549eh01_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for ss_dsi_panel_S6E3FA2_AMS549EH01_FHD");
MODULE_LICENSE("GPL");
