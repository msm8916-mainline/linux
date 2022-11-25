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

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct tianma_nt35596_5p5xa {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct tianma_nt35596_5p5xa *to_tianma_nt35596_5p5xa(struct drm_panel *panel)
{
	return container_of(panel, struct tianma_nt35596_5p5xa, panel);
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

static void tianma_nt35596_5p5xa_reset(struct tianma_nt35596_5p5xa *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(15000, 16000);
}

static int tianma_nt35596_5p5xa_on(struct tianma_nt35596_5p5xa *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_generic_write_seq(dsi, 0xff, 0xee);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x18, 0x40);
	dsi_generic_write_seq(dsi, 0x18, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0x01);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x5c, 0x82);
	dsi_generic_write_seq(dsi, 0x5d, 0x82);
	dsi_generic_write_seq(dsi, 0x5e, 0x02);
	dsi_generic_write_seq(dsi, 0x5f, 0x02);
	dsi_generic_write_seq(dsi, 0x60, 0x0a);
	dsi_generic_write_seq(dsi, 0xff, 0x03);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x00, 0x50);
	dsi_generic_write_seq(dsi, 0x01, 0x50);
	dsi_generic_write_seq(dsi, 0x02, 0x50);
	dsi_generic_write_seq(dsi, 0x03, 0x50);
	dsi_generic_write_seq(dsi, 0x04, 0x50);
	dsi_generic_write_seq(dsi, 0x05, 0x50);
	dsi_generic_write_seq(dsi, 0x06, 0x50);
	dsi_generic_write_seq(dsi, 0x07, 0x50);
	dsi_generic_write_seq(dsi, 0x08, 0x50);
	dsi_generic_write_seq(dsi, 0x09, 0x46);
	dsi_generic_write_seq(dsi, 0x0a, 0x3c);
	dsi_generic_write_seq(dsi, 0x0b, 0x32);
	dsi_generic_write_seq(dsi, 0x0c, 0x28);
	dsi_generic_write_seq(dsi, 0x0d, 0x20);
	dsi_generic_write_seq(dsi, 0x0e, 0x18);
	dsi_generic_write_seq(dsi, 0x0f, 0x10);
	dsi_generic_write_seq(dsi, 0x10, 0x00);
	dsi_generic_write_seq(dsi, 0x11, 0x00);
	dsi_generic_write_seq(dsi, 0x12, 0x00);
	dsi_generic_write_seq(dsi, 0x13, 0x00);
	dsi_generic_write_seq(dsi, 0x32, 0x00);
	dsi_generic_write_seq(dsi, 0x33, 0x00);
	dsi_generic_write_seq(dsi, 0x34, 0x00);
	dsi_generic_write_seq(dsi, 0x35, 0x02);
	dsi_generic_write_seq(dsi, 0x36, 0x06);
	dsi_generic_write_seq(dsi, 0x37, 0x0d);
	dsi_generic_write_seq(dsi, 0x38, 0x0d);
	dsi_generic_write_seq(dsi, 0x39, 0x10);
	dsi_generic_write_seq(dsi, 0x3a, 0x10);
	dsi_generic_write_seq(dsi, 0x3b, 0x0e);
	dsi_generic_write_seq(dsi, 0x3f, 0x10);
	dsi_generic_write_seq(dsi, 0x40, 0x12);
	dsi_generic_write_seq(dsi, 0x41, 0x14);
	dsi_generic_write_seq(dsi, 0x42, 0x14);
	dsi_generic_write_seq(dsi, 0x43, 0x12);
	dsi_generic_write_seq(dsi, 0x44, 0x10);
	dsi_generic_write_seq(dsi, 0x45, 0x0e);
	dsi_generic_write_seq(dsi, 0x46, 0x0c);
	dsi_generic_write_seq(dsi, 0x47, 0x0a);
	dsi_generic_write_seq(dsi, 0x48, 0x08);
	dsi_generic_write_seq(dsi, 0x49, 0x06);
	dsi_generic_write_seq(dsi, 0x4a, 0x04);
	dsi_generic_write_seq(dsi, 0x4b, 0x02);
	dsi_generic_write_seq(dsi, 0x4c, 0x00);
	dsi_generic_write_seq(dsi, 0x4e, 0x05);
	dsi_generic_write_seq(dsi, 0x1a, 0x00);
	dsi_generic_write_seq(dsi, 0x53, 0x77);
	dsi_generic_write_seq(dsi, 0x54, 0x00);
	dsi_generic_write_seq(dsi, 0x55, 0x77);
	dsi_generic_write_seq(dsi, 0x5b, 0x00);
	dsi_generic_write_seq(dsi, 0x63, 0x00);
	dsi_generic_write_seq(dsi, 0xff, 0x04);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x13, 0xf3);
	dsi_generic_write_seq(dsi, 0x14, 0xf2);
	dsi_generic_write_seq(dsi, 0x15, 0xf1);
	dsi_generic_write_seq(dsi, 0x16, 0xf0);
	dsi_generic_write_seq(dsi, 0x21, 0xff);
	dsi_generic_write_seq(dsi, 0x22, 0xf4);
	dsi_generic_write_seq(dsi, 0x23, 0xe9);
	dsi_generic_write_seq(dsi, 0x24, 0xde);
	dsi_generic_write_seq(dsi, 0x25, 0xd3);
	dsi_generic_write_seq(dsi, 0x26, 0xc8);
	dsi_generic_write_seq(dsi, 0x27, 0xbd);
	dsi_generic_write_seq(dsi, 0x28, 0xb2);
	dsi_generic_write_seq(dsi, 0x29, 0xa7);
	dsi_generic_write_seq(dsi, 0x2a, 0x9c);
	dsi_generic_write_seq(dsi, 0xff, 0x04);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x07, 0x20);
	dsi_generic_write_seq(dsi, 0x08, 0x06);
	dsi_generic_write_seq(dsi, 0xff, 0x05);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x23, 0x5c);
	dsi_generic_write_seq(dsi, 0x35, 0x5f);
	dsi_generic_write_seq(dsi, 0x7a, 0x00);
	dsi_generic_write_seq(dsi, 0x7e, 0x04);
	dsi_generic_write_seq(dsi, 0x7f, 0x1a);
	dsi_generic_write_seq(dsi, 0x81, 0x03);
	dsi_generic_write_seq(dsi, 0x82, 0x02);
	dsi_generic_write_seq(dsi, 0x84, 0x03);
	dsi_generic_write_seq(dsi, 0x92, 0x63);
	dsi_generic_write_seq(dsi, 0xc5, 0x31);
	dsi_generic_write_seq(dsi, 0xff, 0x00);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);
	dsi_generic_write_seq(dsi, 0x35, 0x00);
	dsi_dcs_write_seq(dsi, 0x53, 0x24);
	dsi_dcs_write_seq(dsi, 0x55, 0x01);
	dsi_dcs_write_seq(dsi, 0x5e, 0x28);
	dsi_dcs_write_seq(dsi, 0x51, 0x00);
	dsi_dcs_write_seq(dsi, 0xd3, 0x04);
	dsi_dcs_write_seq(dsi, 0xd4, 0x08);

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
	msleep(40);

	return 0;
}

static int tianma_nt35596_5p5xa_off(struct tianma_nt35596_5p5xa *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	dsi_generic_write_seq(dsi, 0xff, 0x00);
	dsi_generic_write_seq(dsi, 0xfb, 0x01);

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(40);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int tianma_nt35596_5p5xa_prepare(struct drm_panel *panel)
{
	struct tianma_nt35596_5p5xa *ctx = to_tianma_nt35596_5p5xa(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	tianma_nt35596_5p5xa_reset(ctx);

	ret = tianma_nt35596_5p5xa_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int tianma_nt35596_5p5xa_unprepare(struct drm_panel *panel)
{
	struct tianma_nt35596_5p5xa *ctx = to_tianma_nt35596_5p5xa(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = tianma_nt35596_5p5xa_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode tianma_nt35596_5p5xa_mode = {
	.clock = (1080 + 184 + 8 + 20) * (1920 + 8 + 2 + 2) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 184,
	.hsync_end = 1080 + 184 + 8,
	.htotal = 1080 + 184 + 8 + 20,
	.vdisplay = 1920,
	.vsync_start = 1920 + 8,
	.vsync_end = 1920 + 8 + 2,
	.vtotal = 1920 + 8 + 2 + 2,
	.width_mm = 68,
	.height_mm = 121,
};

static int tianma_nt35596_5p5xa_get_modes(struct drm_panel *panel,
					  struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &tianma_nt35596_5p5xa_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs tianma_nt35596_5p5xa_panel_funcs = {
	.prepare = tianma_nt35596_5p5xa_prepare,
	.unprepare = tianma_nt35596_5p5xa_unprepare,
	.get_modes = tianma_nt35596_5p5xa_get_modes,
};

static int tianma_nt35596_5p5xa_bl_update_status(struct backlight_device *bl)
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

// TODO: Check if /sys/class/backlight/.../actual_brightness actually returns
// correct values. If not, remove this function.
static int tianma_nt35596_5p5xa_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return brightness & 0xff;
}

static const struct backlight_ops tianma_nt35596_5p5xa_bl_ops = {
	.update_status = tianma_nt35596_5p5xa_bl_update_status,
	.get_brightness = tianma_nt35596_5p5xa_bl_get_brightness,
};

static struct backlight_device *
tianma_nt35596_5p5xa_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &tianma_nt35596_5p5xa_bl_ops, &props);
}

static int tianma_nt35596_5p5xa_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct tianma_nt35596_5p5xa *ctx;
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

	drm_panel_init(&ctx->panel, dev, &tianma_nt35596_5p5xa_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = tianma_nt35596_5p5xa_create_backlight(dsi);
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

static void tianma_nt35596_5p5xa_remove(struct mipi_dsi_device *dsi)
{
	struct tianma_nt35596_5p5xa *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id tianma_nt35596_5p5xa_of_match[] = {
	{ .compatible = "huawei,kiwi-tianma-nt35596" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tianma_nt35596_5p5xa_of_match);

static struct mipi_dsi_driver tianma_nt35596_5p5xa_driver = {
	.probe = tianma_nt35596_5p5xa_probe,
	.remove = tianma_nt35596_5p5xa_remove,
	.driver = {
		.name = "panel-tianma-nt35596-5p5xa",
		.of_match_table = tianma_nt35596_5p5xa_of_match,
	},
};
module_mipi_dsi_driver(tianma_nt35596_5p5xa_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for TIANMA_NT35596_5P5_1080PXA_VIDEO");
MODULE_LICENSE("GPL");
