// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct ili9881_qimei {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct ili9881_qimei *to_ili9881_qimei(struct drm_panel *panel)
{
	return container_of(panel, struct ili9881_qimei, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void ili9881_qimei_reset(struct ili9881_qimei *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(30);
}

static int ili9881_qimei_on(struct ili9881_qimei *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xff, 0x98, 0x81, 0x07);
	dsi_dcs_write_seq(dsi, 0x03, 0x21);
	dsi_dcs_write_seq(dsi, 0x04, 0x06);
	dsi_dcs_write_seq(dsi, 0x05, 0x00);
	dsi_dcs_write_seq(dsi, 0x06, 0x00);
	dsi_dcs_write_seq(dsi, 0x07, 0x00);
	dsi_dcs_write_seq(dsi, 0x08, 0x04);
	dsi_dcs_write_seq(dsi, 0x09, 0x00);
	dsi_dcs_write_seq(dsi, 0x0a, 0x04);
	dsi_dcs_write_seq(dsi, 0x0b, 0x03);
	dsi_dcs_write_seq(dsi, 0x0c, 0x03);
	dsi_dcs_write_seq(dsi, 0x0d, 0x00);
	dsi_dcs_write_seq(dsi, 0x0e, 0x00);
	dsi_dcs_write_seq(dsi, 0x0f, 0x00);
	dsi_dcs_write_seq(dsi, 0x10, 0x40);
	dsi_dcs_write_seq(dsi, 0x11, 0x04);
	dsi_dcs_write_seq(dsi, 0x12, 0x0b);
	dsi_dcs_write_seq(dsi, 0x13, 0x00);
	dsi_dcs_write_seq(dsi, 0x14, 0x00);
	dsi_dcs_write_seq(dsi, 0x15, 0x00);
	dsi_dcs_write_seq(dsi, 0x16, 0x04);
	dsi_dcs_write_seq(dsi, 0x17, 0x04);
	dsi_dcs_write_seq(dsi, 0x18, 0x00);
	dsi_dcs_write_seq(dsi, 0x19, 0x00);
	dsi_dcs_write_seq(dsi, 0x1a, 0x00);
	dsi_dcs_write_seq(dsi, 0x1b, 0x54);
	dsi_dcs_write_seq(dsi, 0x1c, 0xbb);
	dsi_dcs_write_seq(dsi, 0x1d, 0x3c);
	dsi_dcs_write_seq(dsi, 0x1e, 0x04);
	dsi_dcs_write_seq(dsi, 0x1f, 0x00);
	dsi_dcs_write_seq(dsi, 0x20, 0x00);
	dsi_dcs_write_seq(dsi, 0x21, 0x00);
	dsi_dcs_write_seq(dsi, 0x22, 0x00);
	dsi_dcs_write_seq(dsi, 0x23, 0xc0);
	dsi_dcs_write_seq(dsi, 0x24, 0x50);
	dsi_dcs_write_seq(dsi, 0x25, 0x00);
	dsi_dcs_write_seq(dsi, 0x26, 0x08);
	dsi_dcs_write_seq(dsi, 0x27, 0x05);
	dsi_dcs_write_seq(dsi, 0x30, 0x01);
	dsi_dcs_write_seq(dsi, 0x31, 0x23);
	dsi_dcs_write_seq(dsi, 0x32, 0x45);
	dsi_dcs_write_seq(dsi, 0x33, 0x67);
	dsi_dcs_write_seq(dsi, 0x34, 0x89);
	dsi_dcs_write_seq(dsi, 0x35, 0xab);
	dsi_dcs_write_seq(dsi, 0x36, 0x01);
	dsi_dcs_write_seq(dsi, 0x37, 0x23);
	dsi_dcs_write_seq(dsi, 0x38, 0x45);
	dsi_dcs_write_seq(dsi, 0x39, 0x67);
	dsi_dcs_write_seq(dsi, 0x3a, 0x89);
	dsi_dcs_write_seq(dsi, 0x3b, 0xab);
	dsi_dcs_write_seq(dsi, 0x3c, 0xcd);
	dsi_dcs_write_seq(dsi, 0x3d, 0xef);
	dsi_dcs_write_seq(dsi, 0x50, 0x01);
	dsi_dcs_write_seq(dsi, 0x51, 0x14);
	dsi_dcs_write_seq(dsi, 0x52, 0x15);
	dsi_dcs_write_seq(dsi, 0x53, 0x0c);
	dsi_dcs_write_seq(dsi, 0x54, 0x0d);
	dsi_dcs_write_seq(dsi, 0x55, 0x0e);
	dsi_dcs_write_seq(dsi, 0x56, 0x0f);
	dsi_dcs_write_seq(dsi, 0x57, 0x10);
	dsi_dcs_write_seq(dsi, 0x58, 0x11);
	dsi_dcs_write_seq(dsi, 0x59, 0x08);
	dsi_dcs_write_seq(dsi, 0x5a, 0x02);
	dsi_dcs_write_seq(dsi, 0x5b, 0x0a);
	dsi_dcs_write_seq(dsi, 0x5c, 0x02);
	dsi_dcs_write_seq(dsi, 0x5d, 0x02);
	dsi_dcs_write_seq(dsi, 0x5e, 0x02);
	dsi_dcs_write_seq(dsi, 0x5f, 0x02);
	dsi_dcs_write_seq(dsi, 0x60, 0x02);
	dsi_dcs_write_seq(dsi, 0x61, 0x02);
	dsi_dcs_write_seq(dsi, 0x62, 0x02);
	dsi_dcs_write_seq(dsi, 0x63, 0x02);
	dsi_dcs_write_seq(dsi, 0x64, 0x06);
	dsi_dcs_write_seq(dsi, 0x65, 0x02);
	dsi_dcs_write_seq(dsi, 0x66, 0x02);
	dsi_dcs_write_seq(dsi, 0x67, 0x14);
	dsi_dcs_write_seq(dsi, 0x68, 0x15);
	dsi_dcs_write_seq(dsi, 0x69, 0x11);
	dsi_dcs_write_seq(dsi, 0x6a, 0x10);
	dsi_dcs_write_seq(dsi, 0x6b, 0x0f);
	dsi_dcs_write_seq(dsi, 0x6c, 0x0e);
	dsi_dcs_write_seq(dsi, 0x6d, 0x0d);
	dsi_dcs_write_seq(dsi, 0x6e, 0x0c);
	dsi_dcs_write_seq(dsi, 0x6f, 0x06);
	dsi_dcs_write_seq(dsi, 0x70, 0x02);
	dsi_dcs_write_seq(dsi, 0x71, 0x0a);
	dsi_dcs_write_seq(dsi, 0x72, 0x02);
	dsi_dcs_write_seq(dsi, 0x73, 0x02);
	dsi_dcs_write_seq(dsi, 0x74, 0x02);
	dsi_dcs_write_seq(dsi, 0x75, 0x02);
	dsi_dcs_write_seq(dsi, 0x76, 0x02);
	dsi_dcs_write_seq(dsi, 0x77, 0x02);
	dsi_dcs_write_seq(dsi, 0x78, 0x02);
	dsi_dcs_write_seq(dsi, 0x79, 0x02);
	dsi_dcs_write_seq(dsi, 0x7a, 0x08);
	dsi_dcs_write_seq(dsi, 0x7b, 0x02);
	dsi_dcs_write_seq(dsi, 0x7c, 0x02);
	dsi_dcs_write_seq(dsi, 0xff, 0x98, 0x81, 0x08);
	dsi_dcs_write_seq(dsi, 0x1f, 0x75);
	dsi_dcs_write_seq(dsi, 0xab, 0x74);
	dsi_dcs_write_seq(dsi, 0x5d, 0x4c);
	dsi_dcs_write_seq(dsi, 0x8a, 0xc0);
	dsi_dcs_write_seq(dsi, 0xe4, 0xe0);
	dsi_dcs_write_seq(dsi, 0x76, 0xd4);
	dsi_dcs_write_seq(dsi, 0x78, 0x02);
	dsi_dcs_write_seq(dsi, 0x74, 0x3b);
	dsi_dcs_write_seq(dsi, 0x8e, 0x15);
	dsi_dcs_write_seq(dsi, 0x40, 0x01);
	dsi_dcs_write_seq(dsi, 0x7b, 0x04);
	dsi_dcs_write_seq(dsi, 0x72, 0x25);
	dsi_dcs_write_seq(dsi, 0x87, 0x3a);
	dsi_dcs_write_seq(dsi, 0x6c, 0x05);
	dsi_dcs_write_seq(dsi, 0x49, 0x10);
	dsi_dcs_write_seq(dsi, 0x2d, 0x80);
	dsi_dcs_write_seq(dsi, 0x32, 0x05);
	dsi_dcs_write_seq(dsi, 0x3c, 0x05);
	dsi_dcs_write_seq(dsi, 0xff, 0x98, 0x81, 0x01);
	dsi_dcs_write_seq(dsi, 0x41, 0x32);
	dsi_dcs_write_seq(dsi, 0x42, 0x07);
	dsi_dcs_write_seq(dsi, 0x35, 0x07);
	dsi_dcs_write_seq(dsi, 0x22, 0x0a);
	dsi_dcs_write_seq(dsi, 0x31, 0x00);
	dsi_dcs_write_seq(dsi, 0x53, 0x81);
	dsi_dcs_write_seq(dsi, 0x55, 0x88);
	dsi_dcs_write_seq(dsi, 0x50, 0xa3);
	dsi_dcs_write_seq(dsi, 0x51, 0xa3);
	dsi_dcs_write_seq(dsi, 0x60, 0x30);
	dsi_dcs_write_seq(dsi, 0xa0, 0x00);
	dsi_dcs_write_seq(dsi, 0xa1, 0x20);
	dsi_dcs_write_seq(dsi, 0xa2, 0x2c);
	dsi_dcs_write_seq(dsi, 0xa3, 0x18);
	dsi_dcs_write_seq(dsi, 0xa4, 0x1b);
	dsi_dcs_write_seq(dsi, 0xa5, 0x2e);
	dsi_dcs_write_seq(dsi, 0xa6, 0x20);
	dsi_dcs_write_seq(dsi, 0xa7, 0x21);
	dsi_dcs_write_seq(dsi, 0xa8, 0x88);
	dsi_dcs_write_seq(dsi, 0xa9, 0x1f);
	dsi_dcs_write_seq(dsi, 0xaa, 0x2a);
	dsi_dcs_write_seq(dsi, 0xab, 0x6d);
	dsi_dcs_write_seq(dsi, 0xac, 0x1b);
	dsi_dcs_write_seq(dsi, 0xad, 0x1a);
	dsi_dcs_write_seq(dsi, 0xae, 0x4f);
	dsi_dcs_write_seq(dsi, 0xaf, 0x24);
	dsi_dcs_write_seq(dsi, 0xb0, 0x27);
	dsi_dcs_write_seq(dsi, 0xb1, 0x43);
	dsi_dcs_write_seq(dsi, 0xb2, 0x56);
	dsi_dcs_write_seq(dsi, 0xb3, 0x3f);
	dsi_dcs_write_seq(dsi, 0xc0, 0x00);
	dsi_dcs_write_seq(dsi, 0xc1, 0x22);
	dsi_dcs_write_seq(dsi, 0xc2, 0x32);
	dsi_dcs_write_seq(dsi, 0xc3, 0x12);
	dsi_dcs_write_seq(dsi, 0xc4, 0x14);
	dsi_dcs_write_seq(dsi, 0xc5, 0x27);
	dsi_dcs_write_seq(dsi, 0xc6, 0x1b);
	dsi_dcs_write_seq(dsi, 0xc7, 0x1d);
	dsi_dcs_write_seq(dsi, 0xc8, 0x8d);
	dsi_dcs_write_seq(dsi, 0xc9, 0x1a);
	dsi_dcs_write_seq(dsi, 0xca, 0x26);
	dsi_dcs_write_seq(dsi, 0xcb, 0x7a);
	dsi_dcs_write_seq(dsi, 0xcc, 0x19);
	dsi_dcs_write_seq(dsi, 0xcd, 0x18);
	dsi_dcs_write_seq(dsi, 0xce, 0x4c);
	dsi_dcs_write_seq(dsi, 0xcf, 0x20);
	dsi_dcs_write_seq(dsi, 0xd0, 0x28);
	dsi_dcs_write_seq(dsi, 0xd1, 0x51);
	dsi_dcs_write_seq(dsi, 0xd2, 0x5c);
	dsi_dcs_write_seq(dsi, 0xd3, 0x3f);
	dsi_dcs_write_seq(dsi, 0xff, 0x98, 0x81, 0x00);

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

static int ili9881_qimei_off(struct ili9881_qimei *ctx)
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
	msleep(140);

	return 0;
}

static int ili9881_qimei_prepare(struct drm_panel *panel)
{
	struct ili9881_qimei *ctx = to_ili9881_qimei(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	ili9881_qimei_reset(ctx);

	ret = ili9881_qimei_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int ili9881_qimei_unprepare(struct drm_panel *panel)
{
	struct ili9881_qimei *ctx = to_ili9881_qimei(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = ili9881_qimei_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode ili9881_qimei_mode = {
	.clock = (720 + 168 + 24 + 240) * (1280 + 12 + 4 + 20) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 168,
	.hsync_end = 720 + 168 + 24,
	.htotal = 720 + 168 + 24 + 240,
	.vdisplay = 1280,
	.vsync_start = 1280 + 12,
	.vsync_end = 1280 + 12 + 4,
	.vtotal = 1280 + 12 + 4 + 20,
	.width_mm = 59,
	.height_mm = 104,
};

static int ili9881_qimei_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &ili9881_qimei_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs ili9881_qimei_panel_funcs = {
	.prepare = ili9881_qimei_prepare,
	.unprepare = ili9881_qimei_unprepare,
	.get_modes = ili9881_qimei_get_modes,
};

static int ili9881_qimei_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct ili9881_qimei *ctx;
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
			  MIPI_DSI_CLOCK_NON_CONTINUOUS |
			  MIPI_DSI_MODE_VIDEO_NO_HFP |
			  MIPI_DSI_MODE_VIDEO_NO_HSA |
			  MIPI_DSI_MODE_VIDEO_NO_HBP;

	drm_panel_init(&ctx->panel, dev, &ili9881_qimei_panel_funcs,
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

static void ili9881_qimei_remove(struct mipi_dsi_device *dsi)
{
	struct ili9881_qimei *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id ili9881_qimei_of_match[] = {
	{ .compatible = "wingtech,qimei-ili9881" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ili9881_qimei_of_match);

static struct mipi_dsi_driver ili9881_qimei_driver = {
	.probe = ili9881_qimei_probe,
	.remove = ili9881_qimei_remove,
	.driver = {
		.name = "panel-ili9881-qimei",
		.of_match_table = ili9881_qimei_of_match,
	},
};
module_mipi_dsi_driver(ili9881_qimei_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for ili9881_qimei_HD720p_video_Yassy");
MODULE_LICENSE("GPL");
