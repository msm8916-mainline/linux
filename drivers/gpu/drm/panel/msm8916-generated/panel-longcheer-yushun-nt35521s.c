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

struct yushun_nt35521s {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct yushun_nt35521s *to_yushun_nt35521s(struct drm_panel *panel)
{
	return container_of(panel, struct yushun_nt35521s, panel);
}

static void yushun_nt35521s_reset(struct yushun_nt35521s *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int yushun_nt35521s_on(struct yushun_nt35521s *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_generic_write_seq(dsi, 0xff, 0xaa, 0x55, 0xa5, 0x80);
	mipi_dsi_generic_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xb1, 0x68, 0x21);
	mipi_dsi_generic_write_seq(dsi, 0xb6, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0x6f, 0x02);
	mipi_dsi_generic_write_seq(dsi, 0xb8, 0x08);
	mipi_dsi_generic_write_seq(dsi, 0xbb, 0x11, 0x11);
	mipi_dsi_generic_write_seq(dsi, 0xbc, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xbd, 0x01, 0xa3, 0x10, 0x10, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0xc8, 0x83);
	mipi_dsi_generic_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0xb3, 0x28, 0x28);
	mipi_dsi_generic_write_seq(dsi, 0xb4, 0x19, 0x19);
	mipi_dsi_generic_write_seq(dsi, 0xb5, 0x04, 0x04);
	mipi_dsi_generic_write_seq(dsi, 0xb9, 0x35, 0x35);
	mipi_dsi_generic_write_seq(dsi, 0xba, 0x25, 0x25);
	mipi_dsi_generic_write_seq(dsi, 0xbc, 0x70, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xbd, 0x70, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xbe, 0x3e);
	mipi_dsi_generic_write_seq(dsi, 0xc0, 0x0c);
	mipi_dsi_generic_write_seq(dsi, 0xca, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x02);
	mipi_dsi_generic_write_seq(dsi, 0xee, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0xb0,
				   0x00, 0x00, 0x00, 0x23, 0x00, 0x54, 0x00,
				   0x75, 0x00, 0x91, 0x00, 0xbd, 0x00, 0xe1,
				   0x01, 0x18);
	mipi_dsi_generic_write_seq(dsi, 0xb1,
				   0x01, 0x43, 0x01, 0x88, 0x01, 0xbe, 0x02,
				   0x13, 0x02, 0x57, 0x02, 0x58, 0x02, 0x95,
				   0x02, 0xd6);
	mipi_dsi_generic_write_seq(dsi, 0xb2,
				   0x02, 0xfe, 0x03, 0x32, 0x03, 0x55, 0x03,
				   0x82, 0x03, 0x9e, 0x03, 0xc1, 0x03, 0xd5,
				   0x03, 0xeb);
	mipi_dsi_generic_write_seq(dsi, 0xb3, 0x03, 0xfb, 0x03, 0xff);
	mipi_dsi_generic_write_seq(dsi, 0xef, 0x0f, 0x0f, 0x10, 0x10);
	mipi_dsi_generic_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb0, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xb1, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xb2, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xb3, 0x2e, 0x09);
	mipi_dsi_generic_write_seq(dsi, 0xb4, 0x0b, 0x23);
	mipi_dsi_generic_write_seq(dsi, 0xb5, 0x1d, 0x1f);
	mipi_dsi_generic_write_seq(dsi, 0xb6, 0x11, 0x17);
	mipi_dsi_generic_write_seq(dsi, 0xb7, 0x13, 0x19);
	mipi_dsi_generic_write_seq(dsi, 0xb8, 0x01, 0x03);
	mipi_dsi_generic_write_seq(dsi, 0xb9, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xba, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xbb, 0x02, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xbc, 0x18, 0x12);
	mipi_dsi_generic_write_seq(dsi, 0xbd, 0x16, 0x10);
	mipi_dsi_generic_write_seq(dsi, 0xbe, 0x1e, 0x1c);
	mipi_dsi_generic_write_seq(dsi, 0xbf, 0x22, 0x0a);
	mipi_dsi_generic_write_seq(dsi, 0xc0, 0x08, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xc1, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xc2, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xc3, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xe5, 0x25, 0x24);
	mipi_dsi_generic_write_seq(dsi, 0xc4, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xc5, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xc6, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xc7, 0x2e, 0x02);
	mipi_dsi_generic_write_seq(dsi, 0xc8, 0x00, 0x24);
	mipi_dsi_generic_write_seq(dsi, 0xc9, 0x1e, 0x1c);
	mipi_dsi_generic_write_seq(dsi, 0xca, 0x18, 0x12);
	mipi_dsi_generic_write_seq(dsi, 0xcb, 0x16, 0x10);
	mipi_dsi_generic_write_seq(dsi, 0xcc, 0x0a, 0x08);
	mipi_dsi_generic_write_seq(dsi, 0xcd, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xce, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xcf, 0x09, 0x0b);
	mipi_dsi_generic_write_seq(dsi, 0xd0, 0x11, 0x17);
	mipi_dsi_generic_write_seq(dsi, 0xd1, 0x13, 0x19);
	mipi_dsi_generic_write_seq(dsi, 0xd2, 0x1d, 0x1f);
	mipi_dsi_generic_write_seq(dsi, 0xd3, 0x25, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0xd4, 0x03, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xd5, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xd6, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xd7, 0x2e, 0x2e);
	mipi_dsi_generic_write_seq(dsi, 0xe6, 0x22, 0x23);
	mipi_dsi_generic_write_seq(dsi, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xd9, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xe7, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x03);
	mipi_dsi_generic_write_seq(dsi, 0xb0, 0x20, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xb1, 0x20, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xb2, 0x04, 0x00, 0x52, 0x01, 0x51);
	mipi_dsi_generic_write_seq(dsi, 0xb3, 0x04, 0x00, 0x52, 0x01, 0x51);
	mipi_dsi_generic_write_seq(dsi, 0xb6, 0x04, 0x00, 0x52, 0x01, 0x51);
	mipi_dsi_generic_write_seq(dsi, 0xb7, 0x04, 0x00, 0x52, 0x01, 0x51);
	mipi_dsi_generic_write_seq(dsi, 0xba, 0x44, 0x00, 0x60, 0x01, 0x72);
	mipi_dsi_generic_write_seq(dsi, 0xbb, 0x44, 0x00, 0x60, 0x01, 0x72);
	mipi_dsi_generic_write_seq(dsi, 0xbc, 0x53, 0x00, 0x03, 0x00, 0x48);
	mipi_dsi_generic_write_seq(dsi, 0xbd, 0x53, 0x00, 0x03, 0x00, 0x48);
	mipi_dsi_generic_write_seq(dsi, 0xc4, 0x40);
	mipi_dsi_generic_write_seq(dsi, 0xc5, 0x40);
	mipi_dsi_generic_write_seq(dsi, 0xf0, 0x55, 0xaa, 0x52, 0x08, 0x05);
	mipi_dsi_generic_write_seq(dsi, 0xb0, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb1, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb2, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb3, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb4, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb5, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb6, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb7, 0x17, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xb8, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xb9, 0x00, 0x03);
	mipi_dsi_generic_write_seq(dsi, 0xba, 0x00, 0x03);
	mipi_dsi_generic_write_seq(dsi, 0xbb, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xbc, 0x00, 0x01);
	mipi_dsi_generic_write_seq(dsi, 0xbd, 0x0f, 0x03, 0x03, 0x00, 0x03);
	mipi_dsi_generic_write_seq(dsi, 0xe5, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xe6, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xe7, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xe8, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xe9, 0x0a);
	mipi_dsi_generic_write_seq(dsi, 0xea, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xeb, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xec, 0x06);
	mipi_dsi_generic_write_seq(dsi, 0xc0, 0x07);
	mipi_dsi_generic_write_seq(dsi, 0xc1, 0x05);
	mipi_dsi_generic_write_seq(dsi, 0xc4, 0x82);
	mipi_dsi_generic_write_seq(dsi, 0xc5, 0x80);
	mipi_dsi_generic_write_seq(dsi, 0xc8, 0x03, 0x20);
	mipi_dsi_generic_write_seq(dsi, 0xc9, 0x01, 0x21);
	mipi_dsi_generic_write_seq(dsi, 0xca, 0x03, 0x20);
	mipi_dsi_generic_write_seq(dsi, 0xcb, 0x07, 0x20);
	mipi_dsi_generic_write_seq(dsi, 0xd1,
				   0x03, 0x05, 0x00, 0x05, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xd2,
				   0x03, 0x05, 0x00, 0x03, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xd3,
				   0x03, 0x05, 0x04, 0x05, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xd4,
				   0x03, 0x05, 0x04, 0x03, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq(dsi, 0xed, 0x30);

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

static int yushun_nt35521s_off(struct yushun_nt35521s *ctx)
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

static int yushun_nt35521s_prepare(struct drm_panel *panel)
{
	struct yushun_nt35521s *ctx = to_yushun_nt35521s(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	yushun_nt35521s_reset(ctx);

	ret = yushun_nt35521s_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int yushun_nt35521s_unprepare(struct drm_panel *panel)
{
	struct yushun_nt35521s *ctx = to_yushun_nt35521s(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = yushun_nt35521s_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode yushun_nt35521s_mode = {
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
	.height_mm = 111,
};

static int yushun_nt35521s_get_modes(struct drm_panel *panel,
				     struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &yushun_nt35521s_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs yushun_nt35521s_panel_funcs = {
	.prepare = yushun_nt35521s_prepare,
	.unprepare = yushun_nt35521s_unprepare,
	.get_modes = yushun_nt35521s_get_modes,
};

static int yushun_nt35521s_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct yushun_nt35521s *ctx;
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

	drm_panel_init(&ctx->panel, dev, &yushun_nt35521s_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

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

static void yushun_nt35521s_remove(struct mipi_dsi_device *dsi)
{
	struct yushun_nt35521s *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id yushun_nt35521s_of_match[] = {
	{ .compatible = "longcheer,yushun-nt35521s" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, yushun_nt35521s_of_match);

static struct mipi_dsi_driver yushun_nt35521s_driver = {
	.probe = yushun_nt35521s_probe,
	.remove = yushun_nt35521s_remove,
	.driver = {
		.name = "panel-yushun-nt35521s",
		.of_match_table = yushun_nt35521s_of_match,
	},
};
module_mipi_dsi_driver(yushun_nt35521s_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for yushun NT35521S 720p video mode dsi panel");
MODULE_LICENSE("GPL");
