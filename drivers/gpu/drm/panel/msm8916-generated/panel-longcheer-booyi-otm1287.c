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

struct booyi_otm1287 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct booyi_otm1287 *to_booyi_otm1287(struct drm_panel *panel)
{
	return container_of(panel, struct booyi_otm1287, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void booyi_otm1287_reset(struct booyi_otm1287 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int booyi_otm1287_on(struct booyi_otm1287 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xff, 0x12, 0x87, 0x01);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xff, 0x12, 0x87);
	dsi_dcs_write_seq(dsi, 0x00, 0x92);
	dsi_dcs_write_seq(dsi, 0xff, 0x30);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xc0,
			  0x00, 0x64, 0x00, 0x0f, 0x11, 0x00, 0x64, 0x0f, 0x11);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xc0, 0x00, 0x5c, 0x00, 0x01, 0x00, 0x04);
	dsi_dcs_write_seq(dsi, 0x00, 0xa4);
	dsi_dcs_write_seq(dsi, 0xc0, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb3);
	dsi_dcs_write_seq(dsi, 0xc0, 0x00, 0x55);
	dsi_dcs_write_seq(dsi, 0x00, 0x81);
	dsi_dcs_write_seq(dsi, 0xc1, 0x55);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xf5, 0x02, 0x11, 0x02, 0x15);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xc5, 0x50);
	dsi_dcs_write_seq(dsi, 0x00, 0x94);
	dsi_dcs_write_seq(dsi, 0xc5, 0x66);
	dsi_dcs_write_seq(dsi, 0x00, 0xb2);
	dsi_dcs_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb6);
	dsi_dcs_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x94);
	dsi_dcs_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xd2);
	dsi_dcs_write_seq(dsi, 0xf5, 0x06, 0x15);
	dsi_dcs_write_seq(dsi, 0x00, 0xb4);
	dsi_dcs_write_seq(dsi, 0xc5, 0xcc);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xc4,
			  0x05, 0x10, 0x06, 0x02, 0x05, 0x15, 0x10, 0x05, 0x10,
			  0x07, 0x02, 0x05, 0x15, 0x10);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xc4, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x91);
	dsi_dcs_write_seq(dsi, 0xc5, 0x19, 0x52);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xd8, 0xbc, 0xbc);
	dsi_dcs_write_seq(dsi, 0x00, 0xb3);
	dsi_dcs_write_seq(dsi, 0xc5, 0x84);
	dsi_dcs_write_seq(dsi, 0x00, 0xbb);
	dsi_dcs_write_seq(dsi, 0xc5, 0x8a);
	dsi_dcs_write_seq(dsi, 0x00, 0x82);
	dsi_dcs_write_seq(dsi, 0xc4, 0x0a);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xe1,
			  0x05, 0x44, 0x54, 0x61, 0x72, 0x7f, 0x81, 0xa9, 0x98,
			  0xb0, 0x55, 0x41, 0x56, 0x38, 0x3a, 0x2e, 0x23, 0x19,
			  0x0c, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xe2,
			  0x05, 0x44, 0x54, 0x61, 0x72, 0x80, 0x80, 0xa9, 0x99,
			  0xb0, 0x54, 0x41, 0x56, 0x38, 0x3a, 0x2f, 0x23, 0x1a,
			  0x0d, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xd9, 0x71);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xd0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05,
			  0x05, 0x05, 0x05, 0x05, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xe0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x05, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0xf0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			  0xff, 0xff);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x09, 0x0b, 0x0d, 0x0f, 0x01, 0x03, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x2e, 0x2d, 0x0a, 0x0c,
			  0x0e, 0x10, 0x02, 0x04, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x2e, 0x2d);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x10, 0x0e, 0x0c, 0x0a, 0x04, 0x02, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x2d, 0x2e, 0x0f, 0x0d,
			  0x0b, 0x09, 0x03, 0x01, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xd0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x2d, 0x2e);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x8b, 0x03, 0x00, 0x8a, 0x03, 0x00, 0x89, 0x03, 0x00,
			  0x88, 0x03, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x38, 0x07, 0x84, 0xfc, 0x8b, 0x04, 0x00, 0x38, 0x06,
			  0x84, 0xfd, 0x8b, 0x04, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x38, 0x05, 0x84, 0xfe, 0x8b, 0x04, 0x00, 0x38, 0x04,
			  0x84, 0xff, 0x8b, 0x04, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x38, 0x03, 0x85, 0x00, 0x8b, 0x04, 0x00, 0x38, 0x02,
			  0x85, 0x01, 0x8b, 0x04, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xd0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x38, 0x01, 0x85, 0x02, 0x8b, 0x04, 0x00, 0x38, 0x00,
			  0x85, 0x03, 0x8b, 0x04, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x01, 0x01, 0x00,
			  0x02, 0x02);
	dsi_dcs_write_seq(dsi, 0x00, 0xb5);
	dsi_dcs_write_seq(dsi, 0xc5, 0x33, 0xf1, 0xff, 0x33, 0xf1, 0xff);
	dsi_dcs_write_seq(dsi, 0x00, 0xb1);
	dsi_dcs_write_seq(dsi, 0xc6, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x35, 0x01);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xff, 0xff, 0xff, 0xff);

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

	return 0;
}

static int booyi_otm1287_off(struct booyi_otm1287 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

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

static int booyi_otm1287_prepare(struct drm_panel *panel)
{
	struct booyi_otm1287 *ctx = to_booyi_otm1287(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	booyi_otm1287_reset(ctx);

	ret = booyi_otm1287_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int booyi_otm1287_unprepare(struct drm_panel *panel)
{
	struct booyi_otm1287 *ctx = to_booyi_otm1287(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = booyi_otm1287_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode booyi_otm1287_mode = {
	.clock = (720 + 90 + 10 + 90) * (1280 + 20 + 4 + 16) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 90,
	.hsync_end = 720 + 90 + 10,
	.htotal = 720 + 90 + 10 + 90,
	.vdisplay = 1280,
	.vsync_start = 1280 + 20,
	.vsync_end = 1280 + 20 + 4,
	.vtotal = 1280 + 20 + 4 + 16,
	.width_mm = 62,
	.height_mm = 110,
};

static int booyi_otm1287_get_modes(struct drm_panel *panel,
				   struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &booyi_otm1287_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs booyi_otm1287_panel_funcs = {
	.prepare = booyi_otm1287_prepare,
	.unprepare = booyi_otm1287_unprepare,
	.get_modes = booyi_otm1287_get_modes,
};

static int booyi_otm1287_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct booyi_otm1287 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supply = devm_regulator_get(dev, "power");
	if (IS_ERR(ctx->supply))
		return dev_err_probe(dev, PTR_ERR(ctx->supply),
				     "Failed to get power regulator\n");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			  MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &booyi_otm1287_panel_funcs,
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

static void booyi_otm1287_remove(struct mipi_dsi_device *dsi)
{
	struct booyi_otm1287 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id booyi_otm1287_of_match[] = {
	{ .compatible = "longcheer,booyi-otm1287" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, booyi_otm1287_of_match);

static struct mipi_dsi_driver booyi_otm1287_driver = {
	.probe = booyi_otm1287_probe,
	.remove = booyi_otm1287_remove,
	.driver = {
		.name = "panel-booyi-otm1287",
		.of_match_table = booyi_otm1287_of_match,
	},
};
module_mipi_dsi_driver(booyi_otm1287_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for booyi OTM1287 720p video mode dsi panel");
MODULE_LICENSE("GPL");
