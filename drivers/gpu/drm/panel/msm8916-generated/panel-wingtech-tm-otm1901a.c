// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
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
#include <drm/drm_probe_helper.h>

struct otm1901a_tm {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
};

static inline struct otm1901a_tm *to_otm1901a_tm(struct drm_panel *panel)
{
	return container_of(panel, struct otm1901a_tm, panel);
}

static void otm1901a_tm_reset(struct otm1901a_tm *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int otm1901a_tm_on(struct otm1901a_tm *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_generic_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xff, 0x19, 0x01, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_generic_write_seq(dsi, 0xff, 0x19, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_generic_write_seq(dsi, 0xb0, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x1c, 0x33);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xcd);
	mipi_dsi_generic_write_seq(dsi, 0xf5, 0x19);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xc1);
	mipi_dsi_generic_write_seq(dsi, 0xf5, 0x15);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xc3);
	mipi_dsi_generic_write_seq(dsi, 0xf5, 0x15);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xdb);
	mipi_dsi_generic_write_seq(dsi, 0xf5, 0x19);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x8d);
	mipi_dsi_generic_write_seq(dsi, 0xf5, 0x20);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xf5);
	mipi_dsi_generic_write_seq(dsi, 0xc1, 0x40);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xa2);
	mipi_dsi_generic_write_seq(dsi, 0xc1, 0x30);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xb9);
	mipi_dsi_generic_write_seq(dsi, 0xc0, 0x11);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xa0);
	mipi_dsi_generic_write_seq(dsi, 0xcb,
				   0xfd, 0xfe, 0xfd, 0xfd, 0xfd, 0xfd);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xe0);
	mipi_dsi_generic_write_seq(dsi, 0xcb,
				   0x33, 0x33, 0x33, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xf0);
	mipi_dsi_generic_write_seq(dsi, 0xcb,
				   0x31, 0x31, 0x31, 0x01, 0x01, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x82);
	mipi_dsi_generic_write_seq(dsi, 0xd6, 0x02);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xa0);
	mipi_dsi_generic_write_seq(dsi, 0xd6,
				   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xb0);
	mipi_dsi_generic_write_seq(dsi, 0xd6,
				   0x00, 0x00, 0x00, 0x33, 0xcd, 0x33, 0x33,
				   0x33, 0x80, 0x33, 0x33, 0x33);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xc0);
	mipi_dsi_generic_write_seq(dsi, 0xd6,
				   0x44, 0x00, 0x00, 0x22, 0x89, 0x22, 0x22,
				   0x22, 0x55, 0x22, 0x22, 0x22);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xd0);
	mipi_dsi_generic_write_seq(dsi, 0xd6,
				   0x44, 0x00, 0x00, 0x11, 0x44, 0x11, 0x11,
				   0x11, 0x2b, 0x11, 0x11, 0x11);
	mipi_dsi_generic_write_seq(dsi, 0x81, 0x80);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x80);
	mipi_dsi_generic_write_seq(dsi, 0xd8,
				   0x00, 0x00, 0x00, 0x01, 0x02, 0x02, 0x03,
				   0x03, 0x02, 0x02, 0x81, 0x84);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x90);
	mipi_dsi_generic_write_seq(dsi, 0xd8,
				   0x00, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
				   0x04, 0x05, 0x06, 0x0e, 0x15);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xa0);
	mipi_dsi_generic_write_seq(dsi, 0xd8,
				   0x01, 0x02, 0x04, 0x05, 0x08, 0x0b, 0x0e,
				   0x10, 0x11, 0x4c, 0x66, 0x7f);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xb0);
	mipi_dsi_generic_write_seq(dsi, 0xd8,
				   0x00, 0x80, 0x80, 0x81, 0x81, 0x82, 0x83,
				   0x84, 0x84, 0x8e, 0x93, 0x97);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xc0);
	mipi_dsi_generic_write_seq(dsi, 0xd8,
				   0x81, 0x82, 0x82, 0x83, 0x85, 0x87, 0x89,
				   0x8b, 0x8e, 0x95, 0x9b, 0xa0);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xd0);
	mipi_dsi_generic_write_seq(dsi, 0xd8,
				   0x01, 0x03, 0x04, 0x05, 0x08, 0x0a, 0x0d,
				   0x0f, 0x12, 0x25, 0x34, 0x42);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0xc0);
	mipi_dsi_generic_write_seq(dsi, 0xd7, 0x82, 0x80, 0xc0, 0xff);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x35, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x51, 0xff);
	mipi_dsi_generic_write_seq(dsi, 0x53, 0x2c);
	mipi_dsi_generic_write_seq(dsi, 0x55, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0x84, 0xc6);
	mipi_dsi_generic_write_seq(dsi, 0x81, 0x83);
	mipi_dsi_generic_write_seq(dsi, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xfb, 0x01);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(150);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(50);

	return 0;
}

static int otm1901a_tm_off(struct otm1901a_tm *ctx)
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
	msleep(50);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int otm1901a_tm_prepare(struct drm_panel *panel)
{
	struct otm1901a_tm *ctx = to_otm1901a_tm(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	otm1901a_tm_reset(ctx);

	ret = otm1901a_tm_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int otm1901a_tm_unprepare(struct drm_panel *panel)
{
	struct otm1901a_tm *ctx = to_otm1901a_tm(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = otm1901a_tm_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode otm1901a_tm_mode = {
	.clock = (1080 + 60 + 12 + 60) * (1920 + 80 + 4 + 30) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 60,
	.hsync_end = 1080 + 60 + 12,
	.htotal = 1080 + 60 + 12 + 60,
	.vdisplay = 1920,
	.vsync_start = 1920 + 80,
	.vsync_end = 1920 + 80 + 4,
	.vtotal = 1920 + 80 + 4 + 30,
	.width_mm = 64,
	.height_mm = 117,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int otm1901a_tm_get_modes(struct drm_panel *panel,
				 struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &otm1901a_tm_mode);
}

static const struct drm_panel_funcs otm1901a_tm_panel_funcs = {
	.prepare = otm1901a_tm_prepare,
	.unprepare = otm1901a_tm_unprepare,
	.get_modes = otm1901a_tm_get_modes,
};

static int otm1901a_tm_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct otm1901a_tm *ctx;
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

	drm_panel_init(&ctx->panel, dev, &otm1901a_tm_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
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

static void otm1901a_tm_remove(struct mipi_dsi_device *dsi)
{
	struct otm1901a_tm *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id otm1901a_tm_of_match[] = {
	{ .compatible = "wingtech,tm-otm1901a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, otm1901a_tm_of_match);

static struct mipi_dsi_driver otm1901a_tm_driver = {
	.probe = otm1901a_tm_probe,
	.remove = otm1901a_tm_remove,
	.driver = {
		.name = "panel-wingtech-tm-otm1901a",
		.of_match_table = otm1901a_tm_of_match,
	},
};
module_mipi_dsi_driver(otm1901a_tm_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for otm1901a_1080p_video_TianMa");
MODULE_LICENSE("GPL");
