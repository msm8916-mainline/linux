// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct sc7798a_bv045wvm {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct sc7798a_bv045wvm *to_sc7798a_bv045wvm(struct drm_panel *panel)
{
	return container_of(panel, struct sc7798a_bv045wvm, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void sc7798a_bv045wvm_reset(struct sc7798a_bv045wvm *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(150);
}

static int sc7798a_bv045wvm_on(struct sc7798a_bv045wvm *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xb9, 0xf1, 0x08, 0x00);
	dsi_dcs_write_seq(dsi, 0xb1, 0x31, 0x0c, 0x0c, 0xa7, 0x33, 0x01, 0xb7);
	dsi_dcs_write_seq(dsi, 0xc6, 0x00, 0x00, 0xfd);
	dsi_dcs_write_seq(dsi, 0xe3, 0x03, 0x03, 0x03, 0x03);
	dsi_dcs_write_seq(dsi, 0xb8, 0x06, 0x22);
	usleep_range(10000, 11000);
	dsi_dcs_write_seq(dsi, 0xba,
			  0x31, 0x00, 0x44, 0x25, 0x91, 0x0a, 0x00, 0x00, 0xc2,
			  0x34, 0x00, 0x00, 0x04, 0x02, 0x1d, 0xb9, 0xee, 0x40);
	dsi_dcs_write_seq(dsi, 0xb3,
			  0x00, 0x00, 0x00, 0x00, 0x0c, 0x0a, 0x25, 0x20);
	dsi_dcs_write_seq(dsi, 0xb4, 0x00);
	dsi_dcs_write_seq(dsi, 0xcc, 0x0e);
	dsi_dcs_write_seq(dsi, 0xbc, 0x67);
	dsi_dcs_write_seq(dsi, 0xb2, 0x03);
	dsi_dcs_write_seq(dsi, 0xc0, 0x73, 0x50, 0x00, 0x08, 0x70);
	dsi_dcs_write_seq(dsi, 0xe9,
			  0x00, 0x00, 0x06, 0x00, 0x00, 0x6e, 0x29, 0x12, 0x30,
			  0x00, 0x48, 0x08, 0x6e, 0x29, 0x47, 0x03, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x89, 0x98, 0x33,
			  0x11, 0x77, 0x55, 0x13, 0x00, 0x00, 0x89, 0x98, 0x22,
			  0x00, 0x66, 0x44, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xea,
			  0x00, 0x00, 0x98, 0x98, 0x44, 0x66, 0x00, 0x22, 0x20,
			  0x00, 0x00, 0x98, 0x98, 0x55, 0x77, 0x11, 0x33, 0x31,
			  0x30, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xe0,
			  0x01, 0x09, 0x0d, 0x2e, 0x36, 0x3f, 0x10, 0x31, 0x07,
			  0x0d, 0x11, 0x15, 0x13, 0x16, 0x15, 0x32, 0x38, 0x01,
			  0x05, 0x05, 0x30, 0x35, 0x3f, 0x0e, 0x30, 0x04, 0x09,
			  0x0a, 0x0d, 0x12, 0x0d, 0x0f, 0x2c, 0x35);
	dsi_dcs_write_seq(dsi, 0xc7, 0xc0);
	dsi_dcs_write_seq(dsi, 0xc8, 0x11, 0x01);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x24);

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

static int sc7798a_bv045wvm_off(struct sc7798a_bv045wvm *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0x28, 0x00);
	msleep(70);
	dsi_dcs_write_seq(dsi, 0x10, 0x00);
	msleep(70);

	return 0;
}

static int sc7798a_bv045wvm_prepare(struct drm_panel *panel)
{
	struct sc7798a_bv045wvm *ctx = to_sc7798a_bv045wvm(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	sc7798a_bv045wvm_reset(ctx);

	ret = sc7798a_bv045wvm_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int sc7798a_bv045wvm_unprepare(struct drm_panel *panel)
{
	struct sc7798a_bv045wvm *ctx = to_sc7798a_bv045wvm(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = sc7798a_bv045wvm_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode sc7798a_bv045wvm_mode = {
	.clock = (480 + 140 + 4 + 132) * (800 + 10 + 4 + 12) * 60 / 1000,
	.hdisplay = 480,
	.hsync_start = 480 + 140,
	.hsync_end = 480 + 140 + 4,
	.htotal = 480 + 140 + 4 + 132,
	.vdisplay = 800,
	.vsync_start = 800 + 10,
	.vsync_end = 800 + 10 + 4,
	.vtotal = 800 + 10 + 4 + 12,
	.width_mm = 62,
	.height_mm = 106,
};

static int sc7798a_bv045wvm_get_modes(struct drm_panel *panel,
				      struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &sc7798a_bv045wvm_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs sc7798a_bv045wvm_panel_funcs = {
	.prepare = sc7798a_bv045wvm_prepare,
	.unprepare = sc7798a_bv045wvm_unprepare,
	.get_modes = sc7798a_bv045wvm_get_modes,
};

static int sc7798a_bv045wvm_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops sc7798a_bv045wvm_bl_ops = {
	.update_status = sc7798a_bv045wvm_bl_update_status,
};

static struct backlight_device *
sc7798a_bv045wvm_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &sc7798a_bv045wvm_bl_ops, &props);
}

static int sc7798a_bv045wvm_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct sc7798a_bv045wvm *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vddio";
	ctx->supplies[1].supply = "vdd";
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

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &sc7798a_bv045wvm_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = sc7798a_bv045wvm_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static void sc7798a_bv045wvm_remove(struct mipi_dsi_device *dsi)
{
	struct sc7798a_bv045wvm *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id sc7798a_bv045wvm_of_match[] = {
	{ .compatible = "samsung,sc7798a-bv045wvm" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sc7798a_bv045wvm_of_match);

static struct mipi_dsi_driver sc7798a_bv045wvm_driver = {
	.probe = sc7798a_bv045wvm_probe,
	.remove = sc7798a_bv045wvm_remove,
	.driver = {
		.name = "panel-sc7798a-bv045wvm",
		.of_match_table = sc7798a_bv045wvm_of_match,
	},
};
module_mipi_dsi_driver(sc7798a_bv045wvm_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for SC7798A wvga video mode dsi panel");
MODULE_LICENSE("GPL");
