// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>
#include <video/mipi_display.h>

struct s6e88a0_ams427ap24 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct backlight_device *backlight;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;

	bool prepared;
	bool enabled;
};

static inline struct s6e88a0_ams427ap24 *to_s6e88a0_ams427ap24(struct drm_panel *panel)
{
	return container_of(panel, struct s6e88a0_ams427ap24, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void s6e88a0_ams427ap24_reset(struct s6e88a0_ams427ap24 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(18000, 19000);
}

static int s6e88a0_ams427ap24_on(struct s6e88a0_ams427ap24 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xfc, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xb0, 0x11);
	dsi_dcs_write_seq(dsi, 0xfd, 0x11);
	dsi_dcs_write_seq(dsi, 0xb0, 0x13);
	dsi_dcs_write_seq(dsi, 0xfd, 0x18);
	dsi_dcs_write_seq(dsi, 0xb0, 0x02);
	dsi_dcs_write_seq(dsi, 0xb8, 0x30);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(20);

	dsi_dcs_write_seq(dsi, 0xf1, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xcc, 0x4c);
	dsi_dcs_write_seq(dsi, 0xf2, 0x03, 0x0d);
	dsi_dcs_write_seq(dsi, 0xf1, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, 0xca,
			  0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
			  0x80, 0x80, 0x80, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xb2, 0x40, 0x08, 0x20, 0x00, 0x08);
	dsi_dcs_write_seq(dsi, 0xb6, 0x28, 0x0b);
	dsi_dcs_write_seq(dsi, 0xf7, 0x03);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x00);
	dsi_dcs_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, 0xfc, 0xa5, 0xa5);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int s6e88a0_ams427ap24_off(struct s6e88a0_ams427ap24 *ctx)
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
	msleep(120);

	return 0;
}

static int s6e88a0_ams427ap24_prepare(struct drm_panel *panel)
{
	struct s6e88a0_ams427ap24 *ctx = to_s6e88a0_ams427ap24(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	s6e88a0_ams427ap24_reset(ctx);

	ret = s6e88a0_ams427ap24_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 0);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int s6e88a0_ams427ap24_unprepare(struct drm_panel *panel)
{
	struct s6e88a0_ams427ap24 *ctx = to_s6e88a0_ams427ap24(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = s6e88a0_ams427ap24_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static int s6e88a0_ams427ap24_enable(struct drm_panel *panel)
{
	struct s6e88a0_ams427ap24 *ctx = to_s6e88a0_ams427ap24(panel);
	int ret;

	if (ctx->enabled)
		return 0;

	ret = backlight_enable(ctx->backlight);
	if (ret < 0) {
		dev_err(&ctx->dsi->dev, "Failed to enable backlight: %d\n", ret);
		return ret;
	}

	ctx->enabled = true;
	return 0;
}

static int s6e88a0_ams427ap24_disable(struct drm_panel *panel)
{
	struct s6e88a0_ams427ap24 *ctx = to_s6e88a0_ams427ap24(panel);
	int ret;

	if (!ctx->enabled)
		return 0;

	ret = backlight_disable(ctx->backlight);
	if (ret < 0) {
		dev_err(&ctx->dsi->dev, "Failed to disable backlight: %d\n", ret);
		return ret;
	}

	ctx->enabled = false;
	return 0;
}

static const struct drm_display_mode s6e88a0_ams427ap24_mode = {
	.clock = (540 + 94 + 4 + 18) * (960 + 12 + 1 + 3) * 60 / 1000,
	.hdisplay = 540,
	.hsync_start = 540 + 94,
	.hsync_end = 540 + 94 + 4,
	.htotal = 540 + 94 + 4 + 18,
	.vdisplay = 960,
	.vsync_start = 960 + 12,
	.vsync_end = 960 + 12 + 1,
	.vtotal = 960 + 12 + 1 + 3,
	.vrefresh = 60,
	.width_mm = 55,
	.height_mm = 95,
};

static int s6e88a0_ams427ap24_get_modes(struct drm_panel *panel)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(panel->drm, &s6e88a0_ams427ap24_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	panel->connector->display_info.width_mm = mode->width_mm;
	panel->connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(panel->connector, mode);

	return 1;
}

static const struct drm_panel_funcs s6e88a0_ams427ap24_panel_funcs = {
	.disable = s6e88a0_ams427ap24_disable,
	.unprepare = s6e88a0_ams427ap24_unprepare,
	.prepare = s6e88a0_ams427ap24_prepare,
	.enable = s6e88a0_ams427ap24_enable,
	.get_modes = s6e88a0_ams427ap24_get_modes,
};

static int s6e88a0_ams427ap24_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = bl->props.brightness;

	if (bl->props.power != FB_BLANK_UNBLANK ||
	    bl->props.fb_blank != FB_BLANK_UNBLANK ||
	    bl->props.state & (BL_CORE_SUSPENDED | BL_CORE_FBBLANK))
		brightness = 0;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	/* TODO */
	dev_err(&dsi->dev, "Brightness not implemented: %d\n", brightness);

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static int s6e88a0_ams427ap24_bl_get_brightness(struct backlight_device *bl)
{
	return bl->props.brightness; /* TODO */
}

static const struct backlight_ops s6e88a0_ams427ap24_bl_ops = {
	.update_status = s6e88a0_ams427ap24_bl_update_status,
	.get_brightness = s6e88a0_ams427ap24_bl_get_brightness,
};

static struct backlight_device *
s6e88a0_ams427ap24_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &s6e88a0_ams427ap24_bl_ops, &props);
}

static int s6e88a0_ams427ap24_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct s6e88a0_ams427ap24 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vdd3";
	ctx->supplies[1].supply = "vci";
	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(ctx->supplies),
				      ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to get regulators: %d\n", ret);
		return ret;
	}

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset_gpio)) {
		ret = PTR_ERR(ctx->reset_gpio);
		dev_err(dev, "Failed to get reset-gpios: %d\n", ret);
		return ret;
	}

	ctx->backlight = s6e88a0_ams427ap24_create_backlight(dsi);
	if (IS_ERR(ctx->backlight)) {
		ret = PTR_ERR(ctx->backlight);
		dev_err(dev, "Failed to create backlight: %d\n", ret);
		return ret;
	}

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_EOT_PACKET;

	drm_panel_init(&ctx->panel, dev, &s6e88a0_ams427ap24_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ret = drm_panel_add(&ctx->panel);
	if (ret < 0) {
		dev_err(dev, "Failed to add panel: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		return ret;
	}

	return 0;
}

static int s6e88a0_ams427ap24_remove(struct mipi_dsi_device *dsi)
{
	struct s6e88a0_ams427ap24 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id s6e88a0_ams427ap24_of_match[] = {
	{ .compatible = "samsung,s6e88a0-ams427ap24" },
	{ }
};
MODULE_DEVICE_TABLE(of, s6e88a0_ams427ap24_of_match);

static struct mipi_dsi_driver s6e88a0_ams427ap24_driver = {
	.probe = s6e88a0_ams427ap24_probe,
	.remove = s6e88a0_ams427ap24_remove,
	.driver = {
		.name = "panel-s6e88a0-ams427ap24",
		.of_match_table = s6e88a0_ams427ap24_of_match,
	},
};
module_mipi_dsi_driver(s6e88a0_ams427ap24_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>");
MODULE_DESCRIPTION("DRM driver for DRM driver for Samsung S6E88A0 AMS427AP24 DSI video mode panel");
MODULE_LICENSE("GPL v2");
