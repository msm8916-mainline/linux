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

struct ea8061v_ams497ee01 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct ea8061v_ams497ee01 *to_ea8061v_ams497ee01(struct drm_panel *panel)
{
	return container_of(panel, struct ea8061v_ams497ee01, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void ea8061v_ams497ee01_reset(struct ea8061v_ams497ee01 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int ea8061v_ams497ee01_on(struct ea8061v_ams497ee01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xf1, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xb8, 0x19, 0x10);
	dsi_dcs_write_seq(dsi, 0xba, 0x57);
	dsi_dcs_write_seq(dsi, 0xfc, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xb0, 0x0b);
	dsi_dcs_write_seq(dsi, 0xd2, 0x00, 0x85);
	dsi_dcs_write_seq(dsi, 0xcb, 0x70);
	dsi_dcs_write_seq(dsi, 0xfc, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, 0xca,
			  0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xb2, 0x00, 0x00, 0x00, 0x0a);
	dsi_dcs_write_seq(dsi, 0xb6, 0x5c, 0x8a);
	dsi_dcs_write_seq(dsi, 0xf7, 0x01);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	dsi_dcs_write_seq(dsi, 0xf1, 0xa5, 0xa5);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int ea8061v_ams497ee01_off(struct ea8061v_ams497ee01 *ctx)
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
	msleep(35);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(100);

	return 0;
}

static int ea8061v_ams497ee01_prepare(struct drm_panel *panel)
{
	struct ea8061v_ams497ee01 *ctx = to_ea8061v_ams497ee01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	ea8061v_ams497ee01_reset(ctx);

	ret = ea8061v_ams497ee01_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int ea8061v_ams497ee01_unprepare(struct drm_panel *panel)
{
	struct ea8061v_ams497ee01 *ctx = to_ea8061v_ams497ee01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = ea8061v_ams497ee01_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode ea8061v_ams497ee01_mode = {
	.clock = (720 + 80 + 96 + 128) * (1280 + 14 + 2 + 8) * 60 / 1000,
	.hdisplay = 720,
	.hsync_start = 720 + 80,
	.hsync_end = 720 + 80 + 96,
	.htotal = 720 + 80 + 96 + 128,
	.vdisplay = 1280,
	.vsync_start = 1280 + 14,
	.vsync_end = 1280 + 14 + 2,
	.vtotal = 1280 + 14 + 2 + 8,
	.width_mm = 62,
	.height_mm = 110,
};

static int ea8061v_ams497ee01_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &ea8061v_ams497ee01_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs ea8061v_ams497ee01_panel_funcs = {
	.prepare = ea8061v_ams497ee01_prepare,
	.unprepare = ea8061v_ams497ee01_unprepare,
	.get_modes = ea8061v_ams497ee01_get_modes,
};

static int ea8061v_ams497ee01_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct ea8061v_ams497ee01 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vdd3";
	ctx->supplies[1].supply = "vci";
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
			  MIPI_DSI_MODE_NO_EOT_PACKET;

	drm_panel_init(&ctx->panel, dev, &ea8061v_ams497ee01_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}

	return 0;
}

static int ea8061v_ams497ee01_remove(struct mipi_dsi_device *dsi)
{
	struct ea8061v_ams497ee01 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id ea8061v_ams497ee01_of_match[] = {
	{ .compatible = "samsung,ea8061v-ams497ee01" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ea8061v_ams497ee01_of_match);

static struct mipi_dsi_driver ea8061v_ams497ee01_driver = {
	.probe = ea8061v_ams497ee01_probe,
	.remove = ea8061v_ams497ee01_remove,
	.driver = {
		.name = "panel-ea8061v-ams497ee01",
		.of_match_table = ea8061v_ams497ee01_of_match,
	},
};
module_mipi_dsi_driver(ea8061v_ams497ee01_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for ss_dsi_panel_EA8061V_AMS497EE01_HD");
MODULE_LICENSE("GPL v2");
