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

struct r61308 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct r61308 *to_r61308(struct drm_panel *panel)
{
	return container_of(panel, struct r61308, panel);
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

static void r61308_reset(struct r61308 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(30);
}

static int r61308_on(struct r61308 *ctx)
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
	msleep(120);

	dsi_dcs_write_seq(dsi, MIPI_DCS_SET_ADDRESS_MODE, 0x00);

	ret = mipi_dsi_dcs_set_pixel_format(dsi, 0x07);
	if (ret < 0) {
		dev_err(dev, "Failed to set pixel format: %d\n", ret);
		return ret;
	}

	dsi_generic_write_seq(dsi, 0xb0, 0x04);
	dsi_generic_write_seq(dsi, 0xc1,
			      0x50, 0x02, 0x22, 0x00, 0x00, 0xed, 0x11);
	dsi_generic_write_seq(dsi, 0xc8,
			      0x1a, 0x24, 0x29, 0x2d, 0x32, 0x37, 0x14, 0x13,
			      0x10, 0x0c, 0x0a, 0x06, 0x1a, 0x24, 0x28, 0x2d,
			      0x32, 0x37, 0x14, 0x13, 0x10, 0x0c, 0x0a, 0x06);
	dsi_generic_write_seq(dsi, 0xcb,
			      0x10, 0x20, 0x40, 0x80, 0xa0, 0xc0, 0xd0, 0xe0);
	dsi_generic_write_seq(dsi, 0xcc, 0xc8, 0xd8, 0xff);
	dsi_generic_write_seq(dsi, 0xcd,
			      0x1c, 0x1e, 0x1e, 0x1d, 0x1c, 0x1e, 0x1e);
	dsi_generic_write_seq(dsi, 0xce,
			      0x1e, 0x1e, 0x1e, 0x1d, 0x1d, 0x1e, 0x1e);
	dsi_generic_write_seq(dsi, 0xcf,
			      0x1e, 0x1f, 0x20, 0x20, 0x20, 0x20, 0x21);
	dsi_generic_write_seq(dsi, 0xb0, 0x03);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	usleep_range(10000, 11000);

	return 0;
}

static int r61308_off(struct r61308 *ctx)
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

static int r61308_prepare(struct drm_panel *panel)
{
	struct r61308 *ctx = to_r61308(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	r61308_reset(ctx);

	ret = r61308_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int r61308_unprepare(struct drm_panel *panel)
{
	struct r61308 *ctx = to_r61308(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = r61308_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode r61308_mode = {
	.clock = (720 + 120 + 2 + 50) * (1280 + 6 + 1 + 14) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 120,
	.hsync_end = 720 + 120 + 2,
	.htotal = 720 + 120 + 2 + 50,
	.vdisplay = 1280,
	.vsync_start = 1280 + 6,
	.vsync_end = 1280 + 6 + 1,
	.vtotal = 1280 + 6 + 1 + 14,
	.width_mm = 58,
	.height_mm = 103,
};

static int r61308_get_modes(struct drm_panel *panel,
			    struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &r61308_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs r61308_panel_funcs = {
	.prepare = r61308_prepare,
	.unprepare = r61308_unprepare,
	.get_modes = r61308_get_modes,
};

static int r61308_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct r61308 *ctx;
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

	dsi->lanes = 3;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &r61308_panel_funcs,
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

static int r61308_remove(struct mipi_dsi_device *dsi)
{
	struct r61308 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id r61308_of_match[] = {
	{ .compatible = "wingtech,auo-r61308" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, r61308_of_match);

static struct mipi_dsi_driver r61308_driver = {
	.probe = r61308_probe,
	.remove = r61308_remove,
	.driver = {
		.name = "panel-r61308",
		.of_match_table = r61308_of_match,
	},
};
module_mipi_dsi_driver(r61308_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for r61308_HD720p_video_AUO3.5");
MODULE_LICENSE("GPL v2");
