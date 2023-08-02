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

struct hx8394d_800p {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
};

static inline struct hx8394d_800p *to_hx8394d_800p(struct drm_panel *panel)
{
	return container_of(panel, struct hx8394d_800p, panel);
}

static void hx8394d_800p_reset(struct hx8394d_800p *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(25);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(250);
}

static int hx8394d_800p_on(struct hx8394d_800p *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq(dsi, 0xb9, 0xff, 0x83, 0x94);
	mipi_dsi_dcs_write_seq(dsi, 0xba, 0x33, 0x83);
	mipi_dsi_dcs_write_seq(dsi, 0xb1,
			       0x6c, 0x15, 0x15, 0x24, 0xe4, 0x11, 0xf1, 0x80,
			       0xe4, 0x97, 0x23, 0x80, 0xc0, 0xd2, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0xb2,
			       0x00, 0x64, 0x10, 0x07, 0x20, 0x1c, 0x08, 0x08,
			       0x1c, 0x4d, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb4,
			       0x00, 0xff, 0x03, 0x64, 0x03, 0x64, 0x03, 0x64,
			       0x01, 0x74, 0x01, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0xd3,
			       0x00, 0x06, 0x00, 0x40, 0x1a, 0x08, 0x00, 0x32,
			       0x10, 0x07, 0x00, 0x07, 0x54, 0x15, 0x0f, 0x05,
			       0x04, 0x02, 0x12, 0x10, 0x05, 0x07, 0x33, 0x33,
			       0x0b, 0x0b, 0x37, 0x10, 0x07, 0x07, 0x08, 0x00,
			       0x00, 0x00, 0x0a, 0x00, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xd5,
			       0x19, 0x19, 0x18, 0x18, 0x1a, 0x1a, 0x1b, 0x1b,
			       0x04, 0x05, 0x06, 0x07, 0x00, 0x01, 0x02, 0x03,
			       0x20, 0x21, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			       0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x22, 0x23,
			       0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			       0x18, 0x18, 0x18, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0xd6,
			       0x18, 0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1b,
			       0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04,
			       0x23, 0x22, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			       0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x21, 0x20,
			       0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			       0x18, 0x18, 0x18, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0xe0,
			       0x00, 0x06, 0x0c, 0x31, 0x34, 0x3f, 0x1d, 0x41,
			       0x06, 0x0a, 0x0c, 0x17, 0x0f, 0x12, 0x15, 0x13,
			       0x14, 0x07, 0x12, 0x15, 0x16, 0x00, 0x06, 0x0c,
			       0x31, 0x34, 0x3f, 0x1d, 0x41, 0x06, 0x0a, 0x0c,
			       0x17, 0x0f, 0x12, 0x15, 0x13, 0x14, 0x07, 0x12,
			       0x15, 0x16);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0x2a, 0x2a);
	mipi_dsi_dcs_write_seq(dsi, 0xcc, 0x09);
	mipi_dsi_dcs_write_seq(dsi, 0xd2, 0x55);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x30, 0x14);
	mipi_dsi_dcs_write_seq(dsi, 0xbc, 0x07);
	mipi_dsi_dcs_write_seq(dsi, 0xbf, 0x41, 0x0e, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc7, 0x00, 0xc0, 0x00, 0xc0);
	mipi_dsi_dcs_write_seq(dsi, 0xdf, 0x8e);

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
	msleep(200);

	return 0;
}

static int hx8394d_800p_off(struct hx8394d_800p *ctx)
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

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}

	return 0;
}

static int hx8394d_800p_prepare(struct drm_panel *panel)
{
	struct hx8394d_800p *ctx = to_hx8394d_800p(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	hx8394d_800p_reset(ctx);

	ret = hx8394d_800p_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	return 0;
}

static int hx8394d_800p_unprepare(struct drm_panel *panel)
{
	struct hx8394d_800p *ctx = to_hx8394d_800p(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = hx8394d_800p_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	return 0;
}

static const struct drm_display_mode hx8394d_800p_mode = {
	.clock = (800 + 90 + 20 + 30) * (1280 + 9 + 2 + 16) * 60 / 1000,
	.hdisplay = 800,
	.hsync_start = 800 + 90,
	.hsync_end = 800 + 90 + 20,
	.htotal = 800 + 90 + 20 + 30,
	.vdisplay = 1280,
	.vsync_start = 1280 + 9,
	.vsync_end = 1280 + 9 + 2,
	.vtotal = 1280 + 9 + 2 + 16,
	.width_mm = 0,
	.height_mm = 0,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int hx8394d_800p_get_modes(struct drm_panel *panel,
				  struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &hx8394d_800p_mode);
}

static const struct drm_panel_funcs hx8394d_800p_panel_funcs = {
	.prepare = hx8394d_800p_prepare,
	.unprepare = hx8394d_800p_unprepare,
	.get_modes = hx8394d_800p_get_modes,
};

static int hx8394d_800p_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct hx8394d_800p *ctx;
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
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &hx8394d_800p_panel_funcs,
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

static void hx8394d_800p_remove(struct mipi_dsi_device *dsi)
{
	struct hx8394d_800p *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id hx8394d_800p_of_match[] = {
	{ .compatible = "gplus,fl8005a-hx8394d" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hx8394d_800p_of_match);

static struct mipi_dsi_driver hx8394d_800p_driver = {
	.probe = hx8394d_800p_probe,
	.remove = hx8394d_800p_remove,
	.driver = {
		.name = "panel-gplus-fl8005a-hx8394d",
		.of_match_table = hx8394d_800p_of_match,
	},
};
module_mipi_dsi_driver(hx8394d_800p_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for hx8394d 800p video mode dsi panel");
MODULE_LICENSE("GPL");
