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

struct truly_otm1288a {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct truly_otm1288a *to_truly_otm1288a(struct drm_panel *panel)
{
	return container_of(panel, struct truly_otm1288a, panel);
}

#define dsi_generic_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_generic_write(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void truly_otm1288a_reset(struct truly_otm1288a *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int truly_otm1288a_on(struct truly_otm1288a *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0x12, 0x88, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xff, 0x12, 0x88);
	/* keep the TE signal for ESD (unneeded for video mode I guess?)
	 * dsi_generic_write_seq(dsi, 0x00, 0xa0);
	 * dsi_generic_write_seq(dsi, 0xf6, 0x02); */
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc0,
			      0x00, 0x64, 0x00, 0x10, 0x10, 0x00, 0x64, 0x10,
			      0x10);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xc0, 0x00, 0x00, 0x00, 0x14, 0x00, 0x1b);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x1c, 0x32); /* 0x00 -> 0x32 for video mode */
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xb0, 0x4d);
	dsi_generic_write_seq(dsi, 0x00, 0xa3);
	dsi_generic_write_seq(dsi, 0xc0, 0x25);
	dsi_generic_write_seq(dsi, 0x00, 0xb3);
	dsi_generic_write_seq(dsi, 0xc0, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc4, 0x43);
	dsi_generic_write_seq(dsi, 0x00, 0xa2);
	dsi_generic_write_seq(dsi, 0xc1, 0x41);
	dsi_generic_write_seq(dsi, 0x00, 0xb6);
	dsi_generic_write_seq(dsi, 0xb0, 0x18);
	dsi_generic_write_seq(dsi, 0x00, 0x92);
	dsi_generic_write_seq(dsi, 0xff, 0x30, 0x02);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xc5, 0x53);
	dsi_generic_write_seq(dsi, 0x00, 0x93);
	dsi_generic_write_seq(dsi, 0xc5, 0x66);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xc4,
			      0x05, 0x10, 0x06, 0x02, 0x05, 0x15, 0x10, 0x05,
			      0x10, 0x07, 0x02, 0x05, 0x15, 0x10);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xc4, 0x00, 0x00, 0x03);
	dsi_generic_write_seq(dsi, 0x00, 0x91);
	dsi_generic_write_seq(dsi, 0xc5, 0x3d, 0xa6);
	dsi_generic_write_seq(dsi, 0x00, 0xc2);
	dsi_generic_write_seq(dsi, 0xc5, 0xb1);
	dsi_generic_write_seq(dsi, 0x00, 0xe1);
	dsi_generic_write_seq(dsi, 0xf5, 0x55);
	dsi_generic_write_seq(dsi, 0x00, 0xc2);
	dsi_generic_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc4);
	dsi_generic_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc6);
	dsi_generic_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xf3);
	dsi_generic_write_seq(dsi, 0xcf, 0x34);
	dsi_generic_write_seq(dsi, 0x00, 0x83);
	dsi_generic_write_seq(dsi, 0xc5, 0x30);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xd0, 0x40);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xd1, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
			      0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x05,
			      0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05);
	dsi_generic_write_seq(dsi, 0x00, 0xe0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x05, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x05, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xf0);
	dsi_generic_write_seq(dsi, 0xcb,
			      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			      0xff, 0xff, 0xff);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x29, 0x2a, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14,
			      0x06, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x29,
			      0x2a, 0x09, 0x0b, 0x0d, 0x0f, 0x11, 0x13);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x01, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x29, 0x2a, 0x13, 0x11, 0x0f, 0x0d, 0x0b, 0x09,
			      0x01, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x29,
			      0x2a, 0x14, 0x12, 0x10, 0x0e, 0x0c, 0x0a);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xcc,
			      0x02, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x06, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xce,
			      0x87, 0x05, 0x10, 0x86, 0x05, 0x10, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xce,
			      0x54, 0xff, 0x10, 0x55, 0x00, 0x10, 0x55, 0x03,
			      0x10, 0x55, 0x04, 0x10, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xa0);
	dsi_generic_write_seq(dsi, 0xce,
			      0x58, 0x05, 0x04, 0xff, 0x00, 0x10, 0x00, 0x58,
			      0x04, 0x05, 0x00, 0x00, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xb0);
	dsi_generic_write_seq(dsi, 0xce,
			      0x58, 0x03, 0x05, 0x01, 0x00, 0x10, 0x00, 0x58,
			      0x02, 0x05, 0x02, 0x00, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xce,
			      0x58, 0x01, 0x05, 0x03, 0x00, 0x10, 0x00, 0x58,
			      0x00, 0x05, 0x04, 0x00, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xd0);
	dsi_generic_write_seq(dsi, 0xce,
			      0x50, 0x00, 0x05, 0x05, 0x00, 0x10, 0x00, 0x50,
			      0x01, 0x05, 0x06, 0x00, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xcf,
			      0x50, 0x02, 0x05, 0x07, 0x00, 0x10, 0x00, 0x50,
			      0x03, 0x05, 0x08, 0x00, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x90);
	dsi_generic_write_seq(dsi, 0xcf,
			      0x50, 0x04, 0x05, 0x09, 0x00, 0x10, 0x00, 0x50,
			      0x05, 0x05, 0x0a, 0x00, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc0);
	dsi_generic_write_seq(dsi, 0xcf,
			      0x3d, 0x20, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00,
			      0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x80);
	dsi_generic_write_seq(dsi, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0x92);
	dsi_generic_write_seq(dsi, 0xf5, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0x00, 0xc8);
	dsi_generic_write_seq(dsi, 0xf5, 0x0b, 0x15);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xd8, 0xb6, 0xb6);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe1,
			      0x02, 0x27, 0x35, 0x45, 0x54, 0x62, 0x64, 0x8c,
			      0x7b, 0x93, 0x72, 0x5d, 0x6e, 0x48, 0x45, 0x3a,
			      0x2d, 0x27, 0x21, 0x1f);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xe2,
			      0x02, 0x27, 0x35, 0x45, 0x54, 0x62, 0x64, 0x8c,
			      0x7b, 0x93, 0x72, 0x5d, 0x6e, 0x48, 0x45, 0x3a,
			      0x2d, 0x27, 0x21, 0x1f);
	dsi_generic_write_seq(dsi, 0x00, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0xff, 0xff, 0xff);

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

static int truly_otm1288a_off(struct truly_otm1288a *ctx)
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

static int truly_otm1288a_prepare(struct drm_panel *panel)
{
	struct truly_otm1288a *ctx = to_truly_otm1288a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	truly_otm1288a_reset(ctx);

	ret = truly_otm1288a_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int truly_otm1288a_unprepare(struct drm_panel *panel)
{
	struct truly_otm1288a *ctx = to_truly_otm1288a(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = truly_otm1288a_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode truly_otm1288a_mode = {
	.clock = (720 + 40 + 12 + 30) * (1280 + 40 + 2 + 11) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 40,
	.hsync_end = 720 + 40 + 12,
	.htotal = 720 + 40 + 12 + 30,
	.vdisplay = 1280,
	.vsync_start = 1280 + 40,
	.vsync_end = 1280 + 40 + 2,
	.vtotal = 1280 + 40 + 2 + 11,
	.width_mm = 62,
	.height_mm = 111,
};

static int truly_otm1288a_get_modes(struct drm_panel *panel,
				    struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &truly_otm1288a_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs truly_otm1288a_panel_funcs = {
	.prepare = truly_otm1288a_prepare,
	.unprepare = truly_otm1288a_unprepare,
	.get_modes = truly_otm1288a_get_modes,
};

static int truly_otm1288a_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct truly_otm1288a *ctx;
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
			  MIPI_DSI_CLOCK_NON_CONTINUOUS | MIPI_DSI_MODE_LPM;

	drm_panel_init(&ctx->panel, dev, &truly_otm1288a_panel_funcs,
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

static int truly_otm1288a_remove(struct mipi_dsi_device *dsi)
{
	struct truly_otm1288a *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id truly_otm1288a_of_match[] = {
	{ .compatible = "longcheer,truly-otm1288a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, truly_otm1288a_of_match);

static struct mipi_dsi_driver truly_otm1288a_driver = {
	.probe = truly_otm1288a_probe,
	.remove = truly_otm1288a_remove,
	.driver = {
		.name = "panel-truly-otm1288a",
		.of_match_table = truly_otm1288a_of_match,
	},
};
module_mipi_dsi_driver(truly_otm1288a_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for truly OTM1288A command mode dsi panel");
MODULE_LICENSE("GPL v2");
