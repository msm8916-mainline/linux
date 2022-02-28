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

struct s6d7aa0_ltl101at01 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct s6d7aa0_ltl101at01 *to_s6d7aa0_ltl101at01(struct drm_panel *panel)
{
	return container_of(panel, struct s6d7aa0_ltl101at01, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void s6d7aa0_ltl101at01_reset(struct s6d7aa0_ltl101at01 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int s6d7aa0_ltl101at01_on(struct s6d7aa0_ltl101at01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xf1, 0x5a, 0x5a);
	dsi_dcs_write_seq(dsi, 0xfc, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, 0xd0, 0x00, 0x10);
	dsi_dcs_write_seq(dsi, 0xc3, 0x40, 0x00, 0x08);
	dsi_dcs_write_seq(dsi, 0xbc, 0x01, 0x4e, 0x0b);
	dsi_dcs_write_seq(dsi, 0xfd, 0x16, 0x10, 0x11, 0x23, 0x09);
	dsi_dcs_write_seq(dsi, 0xfe, 0x00, 0x02, 0x03, 0x21, 0x80, 0x68);
	dsi_dcs_write_seq(dsi, 0xb3, 0x51);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x24);
	dsi_dcs_write_seq(dsi, 0xf2, 0x02, 0x08, 0x08);
	usleep_range(10000, 11000);
	dsi_dcs_write_seq(dsi, 0xc0, 0x80, 0x80, 0x30);
	dsi_dcs_write_seq(dsi, 0xcd,
			  0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e,
			  0x2e, 0x2e, 0x2e, 0x2e);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xc1, 0x03);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	dsi_dcs_write_seq(dsi, 0xf0, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, 0xf1, 0xa5, 0xa5);
	dsi_dcs_write_seq(dsi, 0xfc, 0x5a, 0x5a);

	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0) {
		dev_err(dev, "Failed to set tear on: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	usleep_range(5000, 6000);

	return 0;
}

static int s6d7aa0_ltl101at01_off(struct s6d7aa0_ltl101at01 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0x22, 0x00);

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(64);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	usleep_range(10000, 11000);

	return 0;
}

static int s6d7aa0_ltl101at01_prepare(struct drm_panel *panel)
{
	struct s6d7aa0_ltl101at01 *ctx = to_s6d7aa0_ltl101at01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	s6d7aa0_ltl101at01_reset(ctx);

	ret = s6d7aa0_ltl101at01_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int s6d7aa0_ltl101at01_unprepare(struct drm_panel *panel)
{
	struct s6d7aa0_ltl101at01 *ctx = to_s6d7aa0_ltl101at01(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = s6d7aa0_ltl101at01_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode s6d7aa0_ltl101at01_mode = {
	.clock = (768 + 96 + 16 + 184) * (1024 + 8 + 2 + 6) * 60 / 1000,
	.hdisplay = 768,
	.hsync_start = 768 + 96,
	.hsync_end = 768 + 96 + 16,
	.htotal = 768 + 96 + 16 + 184,
	.vdisplay = 1024,
	.vsync_start = 1024 + 8,
	.vsync_end = 1024 + 8 + 2,
	.vtotal = 1024 + 8 + 2 + 6,
	.width_mm = 148,
	.height_mm = 197,
};

static int s6d7aa0_ltl101at01_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &s6d7aa0_ltl101at01_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs s6d7aa0_ltl101at01_panel_funcs = {
	.prepare = s6d7aa0_ltl101at01_prepare,
	.unprepare = s6d7aa0_ltl101at01_unprepare,
	.get_modes = s6d7aa0_ltl101at01_get_modes,
};

static int s6d7aa0_ltl101at01_bl_update_status(struct backlight_device *bl)
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
static int s6d7aa0_ltl101at01_bl_get_brightness(struct backlight_device *bl)
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

static const struct backlight_ops s6d7aa0_ltl101at01_bl_ops = {
	.update_status = s6d7aa0_ltl101at01_bl_update_status,
	.get_brightness = s6d7aa0_ltl101at01_bl_get_brightness,
};

static struct backlight_device *
s6d7aa0_ltl101at01_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &s6d7aa0_ltl101at01_bl_ops, &props);
}

static int s6d7aa0_ltl101at01_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct s6d7aa0_ltl101at01 *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vmipi";
	ctx->supplies[1].supply = "5p4v";
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

	drm_panel_init(&ctx->panel, dev, &s6d7aa0_ltl101at01_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = s6d7aa0_ltl101at01_create_backlight(dsi);
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

static int s6d7aa0_ltl101at01_remove(struct mipi_dsi_device *dsi)
{
	struct s6d7aa0_ltl101at01 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id s6d7aa0_ltl101at01_of_match[] = {
	{ .compatible = "samsung,s6d7aa0-ltl101at01" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, s6d7aa0_ltl101at01_of_match);

static struct mipi_dsi_driver s6d7aa0_ltl101at01_driver = {
	.probe = s6d7aa0_ltl101at01_probe,
	.remove = s6d7aa0_ltl101at01_remove,
	.driver = {
		.name = "panel-s6d7aa0-ltl101at01",
		.of_match_table = s6d7aa0_ltl101at01_of_match,
	},
};
module_mipi_dsi_driver(s6d7aa0_ltl101at01_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for S6D7AA0 xga video mode dsi panel");
MODULE_LICENSE("GPL v2");
