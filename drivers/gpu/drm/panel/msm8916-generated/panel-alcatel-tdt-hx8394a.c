// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2021 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct hx8394a_tdt {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct hx8394a_tdt *to_hx8394a_tdt(struct drm_panel *panel)
{
	return container_of(panel, struct hx8394a_tdt, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void hx8394a_tdt_reset(struct hx8394a_tdt *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int hx8394a_tdt_on(struct hx8394a_tdt *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xb9, 0xff, 0x83, 0x94);
	dsi_dcs_write_seq(dsi, 0xba, 0x13);
	dsi_dcs_write_seq(dsi, 0xb1,
			  0x7c, 0x00, 0x07, 0x8a, 0x01, 0x11, 0x11, 0x34, 0x39,
			  0x3f, 0x3f, 0x47, 0x12, 0x01, 0xe6, 0xe2);
	dsi_dcs_write_seq(dsi, 0xb4,
			  0x80, 0x06, 0x32, 0x10, 0x03, 0x32, 0x15, 0x08, 0x32,
			  0x10, 0x08, 0x33, 0x04, 0x43, 0x05, 0x37, 0x04, 0x3f,
			  0x06, 0x61, 0x61, 0x06);
	dsi_dcs_write_seq(dsi, 0xb2, 0x00, 0xc8, 0x08, 0x04, 0x00, 0x22);
	dsi_dcs_write_seq(dsi, 0xd5,
			  0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x01, 0x00, 0xcc,
			  0x00, 0x00, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
			  0x88, 0x88, 0x88, 0x88, 0x01, 0x67, 0x45, 0x23, 0x01,
			  0x23, 0x88, 0x88, 0x88, 0x88);
	dsi_dcs_write_seq(dsi, 0xc7, 0x00, 0x10, 0x00, 0x10);
	dsi_dcs_write_seq(dsi, 0xbf, 0x06, 0x02, 0x10, 0x04);
	dsi_dcs_write_seq(dsi, 0xcc, 0x09);
	dsi_dcs_write_seq(dsi, 0xe0,
			  0x00, 0x04, 0x06, 0x2b, 0x33, 0x3f, 0x14, 0x34, 0x0a,
			  0x0e, 0x0d, 0x10, 0x13, 0x11, 0x12, 0x10, 0x17, 0x00,
			  0x04, 0x06, 0x2b, 0x33, 0x3f, 0x14, 0x34, 0x0a, 0x0e,
			  0x0d, 0x10, 0x13, 0x11, 0x12, 0x10, 0x17, 0x0d, 0x17,
			  0x07, 0x11, 0x0d, 0x17, 0x07, 0x11);
	dsi_dcs_write_seq(dsi, 0xc1,
			  0x01, 0x00, 0x07, 0x0e, 0x15, 0x1d, 0x25, 0x2d, 0x34,
			  0x3c, 0x42, 0x49, 0x51, 0x58, 0x5f, 0x67, 0x6f, 0x77,
			  0x80, 0x87, 0x8f, 0x98, 0x9f, 0xa7, 0xaf, 0xb7, 0xc1,
			  0xcb, 0xd3, 0xdd, 0xe6, 0xef, 0xf6, 0xff, 0x16, 0x25,
			  0x7c, 0x62, 0xca, 0x3a, 0xc2, 0x1f, 0xc0, 0x00, 0x07,
			  0x0e, 0x15, 0x1d, 0x25, 0x2d, 0x34, 0x3c, 0x42, 0x49,
			  0x51, 0x58, 0x5f, 0x67, 0x6f, 0x77, 0x80, 0x87, 0x8f,
			  0x98, 0x9f, 0xa7, 0xaf, 0xb7, 0xc1, 0xcb, 0xd3, 0xdd,
			  0xe6, 0xef, 0xf6, 0xff, 0x16, 0x25, 0x7c, 0x62, 0xca,
			  0x3a, 0xc2, 0x1f, 0xc0, 0x00, 0x07, 0x0e, 0x15, 0x1d,
			  0x25, 0x2d, 0x34, 0x3c, 0x42, 0x49, 0x51, 0x58, 0x5f,
			  0x67, 0x6f, 0x77, 0x80, 0x87, 0x8f, 0x98, 0x9f, 0xa7,
			  0xaf, 0xb7, 0xc1, 0xcb, 0xd3, 0xdd, 0xe6, 0xef, 0xf6,
			  0xff, 0x16, 0x25, 0x7c, 0x62, 0xca, 0x3a, 0xc2, 0x1f,
			  0xc0);
	dsi_dcs_write_seq(dsi, 0xb6, 0x00);
	dsi_dcs_write_seq(dsi, 0xd4, 0x32);

	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0) {
		dev_err(dev, "Failed to set tear on: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(121);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(25);

	return 0;
}

static int hx8394a_tdt_off(struct hx8394a_tdt *ctx)
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
	usleep_range(2000, 3000);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(121);

	return 0;
}

static int hx8394a_tdt_prepare(struct drm_panel *panel)
{
	struct hx8394a_tdt *ctx = to_hx8394a_tdt(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	hx8394a_tdt_reset(ctx);

	ret = hx8394a_tdt_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int hx8394a_tdt_unprepare(struct drm_panel *panel)
{
	struct hx8394a_tdt *ctx = to_hx8394a_tdt(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = hx8394a_tdt_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode hx8394a_tdt_mode = {
	.clock = (720 + 92 + 60 + 60) * (1280 + 7 + 2 + 10) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 92,
	.hsync_end = 720 + 92 + 60,
	.htotal = 720 + 92 + 60 + 60,
	.vdisplay = 1280,
	.vsync_start = 1280 + 7,
	.vsync_end = 1280 + 7 + 2,
	.vtotal = 1280 + 7 + 2 + 10,
	.width_mm = 62,
	.height_mm = 110,
};

static int hx8394a_tdt_get_modes(struct drm_panel *panel,
				 struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &hx8394a_tdt_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs hx8394a_tdt_panel_funcs = {
	.prepare = hx8394a_tdt_prepare,
	.unprepare = hx8394a_tdt_unprepare,
	.get_modes = hx8394a_tdt_get_modes,
};

static int hx8394a_tdt_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct hx8394a_tdt *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &hx8394a_tdt_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static int hx8394a_tdt_remove(struct mipi_dsi_device *dsi)
{
	struct hx8394a_tdt *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id hx8394a_tdt_of_match[] = {
	{ .compatible = "mdss,hx8394a-tdt" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hx8394a_tdt_of_match);

static struct mipi_dsi_driver hx8394a_tdt_driver = {
	.probe = hx8394a_tdt_probe,
	.remove = hx8394a_tdt_remove,
	.driver = {
		.name = "panel-hx8394a-tdt",
		.of_match_table = hx8394a_tdt_of_match,
	},
};
module_mipi_dsi_driver(hx8394a_tdt_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for hx8394a alto5 vdf tdt");
MODULE_LICENSE("GPL v2");
