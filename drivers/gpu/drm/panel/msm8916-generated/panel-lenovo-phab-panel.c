// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2023 FIXME
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

struct jdi {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct jdi *to_jdi(struct drm_panel *panel)
{
	return container_of(panel, struct jdi, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void jdi_reset(struct jdi *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
}

static int jdi_on(struct jdi *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	usleep_range(10000, 11000);
	dsi_dcs_write_seq(dsi, 0xb1, 0xe0, 0x21);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb2, 0x25);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb5, 0x05, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xba, 0x22, 0x26);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xbc, 0x0f, 0x01);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xbd, 0x01, 0x40, 0x08, 0x10);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xc8, 0x80);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xef, 0x00, 0x07, 0xff, 0xff, 0x40, 0x40);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x01);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb7, 0x00, 0x7c);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xce, 0x06);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xca, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb4, 0x14);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xbc, 0x78, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xbd, 0x78, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x02);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb0, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd1,
			  0x00, 0x00, 0x00, 0x20, 0x00, 0x47, 0x00, 0x60, 0x00,
			  0x77, 0x00, 0x9c, 0x00, 0xba, 0x00, 0xea);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd2,
			  0x01, 0x0f, 0x01, 0x48, 0x01, 0x75, 0x01, 0xc0, 0x01,
			  0xff, 0x02, 0x00, 0x02, 0x38, 0x02, 0x73);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd3,
			  0x02, 0x99, 0x02, 0xcc, 0x02, 0xf2, 0x03, 0x1f, 0x03,
			  0x2f, 0x03, 0x64, 0x03, 0x7e, 0x03, 0x9e);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd4, 0x03, 0xb9, 0x03, 0xc8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd5,
			  0x00, 0x8f, 0x00, 0x9d, 0x00, 0xb4, 0x00, 0xc2, 0x00,
			  0xd2, 0x00, 0xea, 0x00, 0xff, 0x01, 0x22);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd6,
			  0x01, 0x41, 0x01, 0x70, 0x01, 0x97, 0x01, 0xd7, 0x02,
			  0x11, 0x02, 0x12, 0x02, 0x46, 0x02, 0x7e);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd7,
			  0x02, 0xa0, 0x02, 0xd3, 0x02, 0xf2, 0x03, 0x22, 0x03,
			  0x40, 0x03, 0x68, 0x03, 0x7e, 0x03, 0x98);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd8, 0x03, 0xb9, 0x03, 0xc8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd9,
			  0x00, 0x00, 0x00, 0x20, 0x00, 0x47, 0x00, 0x60, 0x00,
			  0x77, 0x00, 0x9c, 0x00, 0xba, 0x00, 0xea);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xdd,
			  0x01, 0x0f, 0x01, 0x48, 0x01, 0x75, 0x01, 0xc0, 0x01,
			  0xff, 0x02, 0x00, 0x02, 0x38, 0x02, 0x73);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xde,
			  0x02, 0x99, 0x02, 0xcc, 0x02, 0xf2, 0x03, 0x1f, 0x03,
			  0x3f, 0x03, 0x64, 0x03, 0x7e, 0x03, 0x9e);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xdf, 0x03, 0xb9, 0x03, 0xc8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe0,
			  0x00, 0x00, 0x00, 0x14, 0x00, 0x2b, 0x00, 0x44, 0x00,
			  0x57, 0x00, 0x7c, 0x00, 0x9a, 0x00, 0xcc);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe1,
			  0x00, 0xf3, 0x01, 0x38, 0x01, 0x6d, 0x01, 0xc0, 0x01,
			  0xff, 0x02, 0x00, 0x02, 0x38, 0x02, 0x77);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe2,
			  0x02, 0xa1, 0x02, 0xda, 0x03, 0x02, 0x03, 0x2f, 0x03,
			  0x4f, 0x03, 0x74, 0x03, 0x8e, 0x03, 0xae);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe3, 0x03, 0xc9, 0x03, 0xd8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe4,
			  0x00, 0x8f, 0x00, 0x91, 0x00, 0x98, 0x00, 0xa6, 0x00,
			  0xb2, 0x00, 0xca, 0x00, 0xdf, 0x01, 0x04);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe5,
			  0x01, 0x25, 0x01, 0x60, 0x01, 0x8f, 0x01, 0xd7, 0x02,
			  0x11, 0x02, 0x12, 0x02, 0x46, 0x02, 0x82);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe6,
			  0x02, 0xa8, 0x02, 0xe1, 0x03, 0x02, 0x03, 0x32, 0x03,
			  0x50, 0x03, 0x78, 0x03, 0x8e, 0x03, 0xa8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe7, 0x03, 0xc9, 0x03, 0xd8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe8,
			  0x00, 0x00, 0x00, 0x14, 0x00, 0x2b, 0x00, 0x44, 0x00,
			  0x57, 0x00, 0x7c, 0x00, 0x9a, 0x00, 0xcc);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xe9,
			  0x00, 0xf3, 0x01, 0x38, 0x01, 0x6d, 0x01, 0xc0, 0x01,
			  0xff, 0x02, 0x00, 0x02, 0x38, 0x02, 0x77);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xea,
			  0x02, 0xa1, 0x02, 0xda, 0x03, 0x02, 0x03, 0x2f, 0x03,
			  0x4f, 0x03, 0x74, 0x03, 0x8e, 0x03, 0xae);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xeb, 0x03, 0xc9, 0x03, 0xd8);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x03);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb0, 0x00, 0x04, 0x00, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb1, 0x00, 0x00, 0x00, 0x04);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb2, 0x01, 0x00, 0x06, 0x04, 0x00, 0xf5, 0x42);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb3, 0x01, 0x00, 0x05, 0x04, 0x00, 0xf5, 0x42);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xba, 0x85, 0x03, 0x00, 0x04, 0x01, 0xf5, 0x42);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xbb, 0x85, 0x03, 0x00, 0x03, 0x01, 0xf5, 0x42);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x05);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xed, 0xb0);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x05);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb0, 0x03, 0x03, 0x00, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb1, 0x30, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb2, 0x03, 0x01, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb3, 0x82, 0x00, 0x81, 0x38);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb4, 0xd5, 0x75, 0x07, 0x57);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb6, 0x01, 0x00, 0xd5, 0x71, 0x07, 0x57);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd0, 0x03, 0x05, 0x02, 0x00, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xd1, 0x03, 0x05, 0x06, 0x00, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x06);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb0, 0x17, 0x17, 0x16, 0x16, 0x19);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb1, 0x19, 0x18, 0x18, 0x02, 0x3a);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb2, 0x3a, 0x3a, 0x3d, 0x3d, 0x3d);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb3, 0x03, 0x3d, 0x3d, 0x3d, 0x3d);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb4, 0x3d, 0x3d);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb5, 0x11, 0x11, 0x10, 0x10, 0x13);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb6, 0x13, 0x12, 0x12, 0x00, 0x3a);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb7, 0x3d, 0x3d, 0x3d, 0x3d, 0x3d);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb8, 0x01, 0x3d, 0x3d, 0x3d, 0x3d);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xb9, 0x3d, 0x3d);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xff, 0xaa, 0x55, 0xa5, 0x80);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0x6f, 0x09);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf7, 0x82);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0x6f, 0x0b);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xf7, 0xe0);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0x6f, 0x08);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0xfc, 0x00);
	usleep_range(1000, 2000);
	dsi_dcs_write_seq(dsi, 0x62, 0x01);
	usleep_range(1000, 2000);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(128);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(30);

	return 0;
}

static int jdi_off(struct jdi *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(121);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(100);

	return 0;
}

static int jdi_prepare(struct drm_panel *panel)
{
	struct jdi *ctx = to_jdi(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	jdi_reset(ctx);

	ret = jdi_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int jdi_unprepare(struct drm_panel *panel)
{
	struct jdi *ctx = to_jdi(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = jdi_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode jdi_mode = {
	.clock = (720 + 72 + 16 + 60) * (1280 + 8 + 4 + 4) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 72,
	.hsync_end = 720 + 72 + 16,
	.htotal = 720 + 72 + 16 + 60,
	.vdisplay = 1280,
	.vsync_start = 1280 + 8,
	.vsync_end = 1280 + 8 + 4,
	.vtotal = 1280 + 8 + 4 + 4,
	.width_mm = 61,
	.height_mm = 110,
};

static int jdi_get_modes(struct drm_panel *panel,
			 struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &jdi_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs jdi_panel_funcs = {
	.prepare = jdi_prepare,
	.unprepare = jdi_unprepare,
	.get_modes = jdi_get_modes,
};

static int jdi_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct jdi *ctx;
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
			  MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &jdi_panel_funcs,
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

static void jdi_remove(struct mipi_dsi_device *dsi)
{
	struct jdi *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id jdi_of_match[] = {
	{ .compatible = "lenovo,phab-panel" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, jdi_of_match);

static struct mipi_dsi_driver jdi_driver = {
	.probe = jdi_probe,
	.remove = jdi_remove,
	.driver = {
		.name = "panel-jdi",
		.of_match_table = jdi_of_match,
	},
};
module_mipi_dsi_driver(jdi_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for jdi 1080p video mode dsi panel");
MODULE_LICENSE("GPL");
