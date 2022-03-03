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

struct hx8394d_auo {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct hx8394d_auo *to_hx8394d_auo(struct drm_panel *panel)
{
	return container_of(panel, struct hx8394d_auo, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void hx8394d_auo_reset(struct hx8394d_auo *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(50);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(120);
}

static int hx8394d_auo_on(struct hx8394d_auo *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xb9, 0xff, 0x83, 0x94);
	dsi_dcs_write_seq(dsi, 0xba,
			  0x73, 0x83, 0xa0, 0x65, 0xb2, 0x3f, 0x4f, 0x40, 0x50,
			  0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xb1,
			  0x6c, 0x12, 0x12, 0x34, 0x04, 0x11, 0xf1, 0x80, 0x9f,
			  0xa0, 0x23, 0x80, 0xc0, 0xd2, 0x58);
	dsi_dcs_write_seq(dsi, 0xb2,
			  0x00, 0x64, 0x05, 0x07, 0x22, 0x1c, 0x08, 0x08, 0x1c,
			  0x4d, 0x00, 0x03, 0x20, 0x46, 0x48);
	dsi_dcs_write_seq(dsi, 0xb4,
			  0x00, 0xff, 0x03, 0x5a, 0x03, 0x5a, 0x03, 0x5a, 0x01,
			  0x70, 0x01, 0x70);
	dsi_dcs_write_seq(dsi, 0xbc, 0x07);
	dsi_dcs_write_seq(dsi, 0xe0,
			  0x12, 0x2d, 0x36, 0x30, 0x33, 0x39, 0x39, 0x4a, 0x09,
			  0x0c, 0x0e, 0x18, 0x0f, 0x12, 0x15, 0x13, 0x14, 0x06,
			  0x10, 0x10, 0x18, 0x12, 0x2d, 0x36, 0x30, 0x33, 0x39,
			  0x39, 0x4a, 0x09, 0x0c, 0x0e, 0x18, 0x0f, 0x12, 0x15,
			  0x13, 0x14, 0x0a, 0x16, 0x18, 0x18);
	dsi_dcs_write_seq(dsi, 0xd3,
			  0x00, 0x06, 0x00, 0x01, 0x01, 0x10, 0x00, 0x32, 0x10,
			  0x00, 0x00, 0x00, 0x32, 0x15, 0x04, 0x35, 0x04, 0x32,
			  0x15, 0x14, 0x05, 0x14, 0x37, 0x33, 0x00, 0x00, 0x37,
			  0x00, 0x07, 0x37);
	dsi_dcs_write_seq(dsi, 0xd5,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x25, 0x24, 0x27, 0x26,
			  0x18, 0x18, 0x47, 0x46, 0x43, 0x42, 0x41, 0x40, 0x45,
			  0x44, 0x05, 0x04, 0x01, 0x00, 0x07, 0x06, 0x03, 0x02,
			  0x21, 0x20, 0x23, 0x22, 0x18, 0x18, 0x18, 0x18);
	dsi_dcs_write_seq(dsi, 0xd6,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x22, 0x23, 0x20, 0x21,
			  0x18, 0x18, 0x00, 0x01, 0x04, 0x05, 0x06, 0x07, 0x02,
			  0x03, 0x02, 0x03, 0x06, 0x07, 0x00, 0x01, 0x04, 0x05,
			  0x26, 0x27, 0x24, 0x25, 0x18, 0x18, 0x18, 0x18);
	dsi_dcs_write_seq(dsi, 0xbd, 0x01);
	dsi_dcs_write_seq(dsi, 0xd8,
			  0xaa, 0xab, 0xef, 0xff, 0xff, 0xf0, 0xaa, 0xab, 0xef,
			  0xff, 0xff, 0xf0, 0xaa, 0xab, 0xef, 0xff, 0xff, 0xf0,
			  0xaa, 0xab, 0xef, 0xff, 0xff, 0xf0, 0xaa, 0xab, 0xef,
			  0xff, 0xff, 0xf0, 0xaa, 0xab, 0xef, 0xff, 0xff, 0xf0);
	dsi_dcs_write_seq(dsi, 0xbd, 0x02);
	dsi_dcs_write_seq(dsi, 0xd8,
			  0xaa, 0xab, 0xef, 0xff, 0xff, 0xf0, 0xaa, 0xab, 0xef,
			  0xff, 0xff, 0xf0);
	dsi_dcs_write_seq(dsi, 0xcc, 0x01);
	dsi_dcs_write_seq(dsi, 0xb6, 0x62, 0x62);
	dsi_dcs_write_seq(dsi, 0xc0, 0x30, 0x14);
	dsi_dcs_write_seq(dsi, 0xc7, 0x00, 0xc0, 0x40, 0xc0);
	dsi_dcs_write_seq(dsi, MIPI_DCS_SET_ADDRESS_MODE, 0x02);

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
	msleep(120);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	usleep_range(10000, 11000);

	return 0;
}

static int hx8394d_auo_off(struct hx8394d_auo *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int hx8394d_auo_prepare(struct drm_panel *panel)
{
	struct hx8394d_auo *ctx = to_hx8394d_auo(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	hx8394d_auo_reset(ctx);

	ret = hx8394d_auo_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int hx8394d_auo_unprepare(struct drm_panel *panel)
{
	struct hx8394d_auo *ctx = to_hx8394d_auo(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = hx8394d_auo_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode hx8394d_auo_mode = {
	.clock = (720 + 52 + 24 + 100) * (1280 + 9 + 2 + 5) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 52,
	.hsync_end = 720 + 52 + 24,
	.htotal = 720 + 52 + 24 + 100,
	.vdisplay = 1280,
	.vsync_start = 1280 + 9,
	.vsync_end = 1280 + 9 + 2,
	.vtotal = 1280 + 9 + 2 + 5,
	.width_mm = 59,
	.height_mm = 104,
};

static int hx8394d_auo_get_modes(struct drm_panel *panel,
				 struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &hx8394d_auo_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs hx8394d_auo_panel_funcs = {
	.prepare = hx8394d_auo_prepare,
	.unprepare = hx8394d_auo_unprepare,
	.get_modes = hx8394d_auo_get_modes,
};

static int hx8394d_auo_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct hx8394d_auo *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vsp";
	ctx->supplies[1].supply = "vsn";
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
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &hx8394d_auo_panel_funcs,
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

static int hx8394d_auo_remove(struct mipi_dsi_device *dsi)
{
	struct hx8394d_auo *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id hx8394d_auo_of_match[] = {
	{ .compatible = "alcatel,auo-hx8394d" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hx8394d_auo_of_match);

static struct mipi_dsi_driver hx8394d_auo_driver = {
	.probe = hx8394d_auo_probe,
	.remove = hx8394d_auo_remove,
	.driver = {
		.name = "panel-hx8394d-auo",
		.of_match_table = hx8394d_auo_of_match,
	},
};
module_mipi_dsi_driver(hx8394d_auo_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for hx8394d auo 720p video mode dsi panel");
MODULE_LICENSE("GPL v2");
