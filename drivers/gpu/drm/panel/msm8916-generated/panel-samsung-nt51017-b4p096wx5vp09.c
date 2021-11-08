// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2022 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct nt51017 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	bool prepared;
};

static inline struct nt51017 *to_nt51017(struct drm_panel *panel)
{
	return container_of(panel, struct nt51017, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static int nt51017_on(struct nt51017 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;

	dsi_dcs_write_seq(dsi, 0x83, 0x96);
	dsi_dcs_write_seq(dsi, 0x84, 0x69);
	dsi_dcs_write_seq(dsi, 0x92, 0x19);
	dsi_dcs_write_seq(dsi, 0x95, 0x00);
	dsi_dcs_write_seq(dsi, 0x83, 0x00);
	dsi_dcs_write_seq(dsi, 0x84, 0x00);
	dsi_dcs_write_seq(dsi, 0x90, 0x77);
	dsi_dcs_write_seq(dsi, 0x94, 0xff);
	dsi_dcs_write_seq(dsi, 0x96, 0xff);
	dsi_dcs_write_seq(dsi, 0x91, 0xfd);
	dsi_dcs_write_seq(dsi, 0x90, 0x77);

	return 0;
}

static int nt51017_off(struct nt51017 *ctx)
{
	return 0;
}

static int nt51017_prepare(struct drm_panel *panel)
{
	struct nt51017 *ctx = to_nt51017(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	msleep(30);

	ret = nt51017_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int nt51017_unprepare(struct drm_panel *panel)
{
	struct nt51017 *ctx = to_nt51017(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = nt51017_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode nt51017_mode = {
	.clock = (800 + 152 + 8 + 128) * (1280 + 18 + 1 + 23) * 60 / 1000,
	.hdisplay = 800,
	.hsync_start = 800 + 152,
	.hsync_end = 800 + 152 + 8,
	.htotal = 800 + 152 + 8 + 128,
	.vdisplay = 1280,
	.vsync_start = 1280 + 18,
	.vsync_end = 1280 + 18 + 1,
	.vtotal = 1280 + 18 + 1 + 23,
	.width_mm = 129,
	.height_mm = 206,
};

static int nt51017_get_modes(struct drm_panel *panel,
			     struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &nt51017_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs nt51017_panel_funcs = {
	.prepare = nt51017_prepare,
	.unprepare = nt51017_unprepare,
	.get_modes = nt51017_get_modes,
};

static int nt51017_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct nt51017 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supply = devm_regulator_get(dev, "lcd");
	if (IS_ERR(ctx->supply))
		return dev_err_probe(dev, PTR_ERR(ctx->supply),
				     "Failed to get lcd regulator\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_NO_EOT_PACKET;

	drm_panel_init(&ctx->panel, dev, &nt51017_panel_funcs,
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

static int nt51017_remove(struct mipi_dsi_device *dsi)
{
	struct nt51017 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id nt51017_of_match[] = {
	{ .compatible = "samsung,nt51017-b4p096wx5vp09" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, nt51017_of_match);

static struct mipi_dsi_driver nt51017_driver = {
	.probe = nt51017_probe,
	.remove = nt51017_remove,
	.driver = {
		.name = "panel-nt51017",
		.of_match_table = nt51017_of_match,
	},
};
module_mipi_dsi_driver(nt51017_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for NT51017 wxga video mode dsi panel");
MODULE_LICENSE("GPL v2");
