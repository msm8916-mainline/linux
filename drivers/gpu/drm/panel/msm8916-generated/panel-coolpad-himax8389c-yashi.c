// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2023 FIXME
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

struct himax8389c_yashi_550 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct himax8389c_yashi_550 *to_himax8389c_yashi_550(struct drm_panel *panel)
{
	return container_of(panel, struct himax8389c_yashi_550, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void himax8389c_yashi_550_reset(struct himax8389c_yashi_550 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int himax8389c_yashi_550_on(struct himax8389c_yashi_550 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0xb9, 0xff, 0x83, 0x89);
	dsi_dcs_write_seq(dsi, 0xba, 0x41, 0x93);
	dsi_dcs_write_seq(dsi, 0xb1,
			  0x5f, 0x13, 0x13, 0x2f, 0x4f, 0x50, 0xd0, 0xec, 0x9a,
			  0x80, 0xa0, 0xa0, 0xf8, 0x22, 0x21, 0x12);
	dsi_dcs_write_seq(dsi, 0xb2,
			  0x45, 0x50, 0x05, 0x07, 0xa0, 0x38, 0x11, 0x62, 0x5d,
			  0x09);
	dsi_dcs_write_seq(dsi, 0xb7, 0x00, 0x00, 0x59);
	dsi_dcs_write_seq(dsi, 0xb4,
			  0x26, 0x66, 0x26, 0x66, 0x70, 0x15, 0x10, 0x72, 0x30,
			  0x72, 0xb0, 0x00, 0xff);
	dsi_dcs_write_seq(dsi, 0xd3,
			  0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x02, 0x32, 0x10,
			  0x04, 0x00, 0x04, 0x03, 0xc6, 0x03, 0xc6, 0x00, 0x00,
			  0x00, 0x00, 0x17, 0x11, 0x05, 0x05, 0x13, 0x02, 0x02,
			  0x33, 0x05, 0x08, 0x00, 0x00, 0x0a, 0x00, 0x01, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0xd5,
			  0x18, 0x18, 0x18, 0x18, 0x21, 0x20, 0x10, 0x13, 0x12,
			  0x11, 0x00, 0x03, 0x02, 0x01, 0x25, 0x24, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x00);
	dsi_dcs_write_seq(dsi, 0xd6,
			  0x18, 0x18, 0x18, 0x18, 0x24, 0x25, 0x13, 0x10, 0x11,
			  0x12, 0x01, 0x02, 0x03, 0x00, 0x20, 0x21, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
			  0x18, 0x18);
	dsi_dcs_write_seq(dsi, 0xe0,
			  0x00, 0x00, 0x03, 0x2b, 0x31, 0x3f, 0x0a, 0x38, 0x02,
			  0x06, 0x0a, 0x15, 0x0d, 0x10, 0x12, 0x10, 0x12, 0x06,
			  0x11, 0x13, 0x17, 0x00, 0x00, 0x03, 0x2b, 0x31, 0x3f,
			  0x0a, 0x38, 0x02, 0x06, 0x0a, 0x15, 0x0d, 0x10, 0x12,
			  0x10, 0x12, 0x06, 0x11, 0x13, 0x17);
	dsi_dcs_write_seq(dsi, 0xbd, 0x00);
	dsi_dcs_write_seq(dsi, 0xc1,
			  0x01, 0x00, 0x06, 0x0e, 0x16, 0x1f, 0x27, 0x32, 0x3b,
			  0x42, 0x49, 0x4f, 0x57, 0x5d, 0x64, 0x6c, 0x74, 0x7c,
			  0x83, 0x8b, 0x92, 0x9a, 0xa2, 0xa9, 0xb2, 0xb9, 0xc1,
			  0xc9, 0xd1, 0xda, 0xe3, 0xea, 0xf2, 0xfb, 0x4c, 0x09,
			  0x51, 0x46, 0x6e, 0x27, 0x92, 0x9c, 0x80);
	dsi_dcs_write_seq(dsi, 0xbd, 0x01);
	dsi_dcs_write_seq(dsi, 0xc1,
			  0x00, 0x05, 0x0c, 0x14, 0x1c, 0x24, 0x2c, 0x36, 0x3d,
			  0x43, 0x49, 0x50, 0x56, 0x5c, 0x63, 0x6a, 0x71, 0x78,
			  0x7f, 0x86, 0x8c, 0x94, 0x9b, 0xa2, 0xa9, 0xb0, 0xb7,
			  0xbe, 0xc5, 0xcd, 0xd4, 0xdd, 0xe4, 0x58, 0x9a, 0x54,
			  0xb4, 0x2a, 0x32, 0xef, 0x7a, 0x80);
	dsi_dcs_write_seq(dsi, 0xbd, 0x02);
	dsi_dcs_write_seq(dsi, 0xc1,
			  0x00, 0x05, 0x0c, 0x14, 0x1d, 0x25, 0x2e, 0x37, 0x3e,
			  0x45, 0x4b, 0x52, 0x58, 0x5f, 0x65, 0x6d, 0x74, 0x7b,
			  0x82, 0x89, 0x90, 0x98, 0x9f, 0xa6, 0xae, 0xb5, 0xbc,
			  0xc4, 0xcc, 0xd3, 0xdb, 0xe4, 0xea, 0x59, 0xa8, 0x51,
			  0xad, 0x91, 0x7b, 0xf5, 0xf8, 0xc0);
	dsi_dcs_write_seq(dsi, 0xb6, 0x58, 0x58);
	dsi_dcs_write_seq(dsi, 0xcc, 0x02);
	dsi_dcs_write_seq(dsi, 0xd2, 0x33);
	dsi_dcs_write_seq(dsi, 0xc0, 0x43, 0x17);

	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VBLANK);
	if (ret < 0) {
		dev_err(dev, "Failed to set tear on: %d\n", ret);
		return ret;
	}

	dsi_dcs_write_seq(dsi, 0xc9, 0x1f, 0x00, 0x0e);
	dsi_dcs_write_seq(dsi, 0xe4, 0x13);
	dsi_dcs_write_seq(dsi, 0xbc, 0x04);

	ret = mipi_dsi_dcs_set_display_brightness(dsi, 0x0000);
	if (ret < 0) {
		dev_err(dev, "Failed to set display brightness: %d\n", ret);
		return ret;
	}

	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_CONTROL_DISPLAY, 0x24);
	dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x01);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	dsi_dcs_write_seq(dsi, 0xca,
			  0x24, 0x24, 0x24, 0x23, 0x23, 0x23, 0x22, 0x20, 0x20);
	usleep_range(1000, 2000);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(20);

	return 0;
}

static int himax8389c_yashi_550_off(struct himax8389c_yashi_550 *ctx)
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

static int himax8389c_yashi_550_prepare(struct drm_panel *panel)
{
	struct himax8389c_yashi_550 *ctx = to_himax8389c_yashi_550(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	himax8389c_yashi_550_reset(ctx);

	ret = himax8389c_yashi_550_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int himax8389c_yashi_550_unprepare(struct drm_panel *panel)
{
	struct himax8389c_yashi_550 *ctx = to_himax8389c_yashi_550(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = himax8389c_yashi_550_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode himax8389c_yashi_550_mode = {
	.clock = (540 + 52 + 4 + 48) * (960 + 18 + 2 + 14) * 60 / 1000,
	.hdisplay = 540,
	.hsync_start = 540 + 52,
	.hsync_end = 540 + 52 + 4,
	.htotal = 540 + 52 + 4 + 48,
	.vdisplay = 960,
	.vsync_start = 960 + 18,
	.vsync_end = 960 + 18 + 2,
	.vtotal = 960 + 18 + 2 + 14,
	.width_mm = 68,
	.height_mm = 120,
};

static int himax8389c_yashi_550_get_modes(struct drm_panel *panel,
					  struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &himax8389c_yashi_550_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs himax8389c_yashi_550_panel_funcs = {
	.prepare = himax8389c_yashi_550_prepare,
	.unprepare = himax8389c_yashi_550_unprepare,
	.get_modes = himax8389c_yashi_550_get_modes,
};

static int himax8389c_yashi_550_bl_update_status(struct backlight_device *bl)
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
static int himax8389c_yashi_550_bl_get_brightness(struct backlight_device *bl)
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

static const struct backlight_ops himax8389c_yashi_550_bl_ops = {
	.update_status = himax8389c_yashi_550_bl_update_status,
	.get_brightness = himax8389c_yashi_550_bl_get_brightness,
};

static struct backlight_device *
himax8389c_yashi_550_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &himax8389c_yashi_550_bl_ops, &props);
}

static int himax8389c_yashi_550_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct himax8389c_yashi_550 *ctx;
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

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_NO_EOT_PACKET |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &himax8389c_yashi_550_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = himax8389c_yashi_550_create_backlight(dsi);
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

static void himax8389c_yashi_550_remove(struct mipi_dsi_device *dsi)
{
	struct himax8389c_yashi_550 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id himax8389c_yashi_550_of_match[] = {
	{ .compatible = "coolpad,himax8389c-yashi" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, himax8389c_yashi_550_of_match);

static struct mipi_dsi_driver himax8389c_yashi_550_driver = {
	.probe = himax8389c_yashi_550_probe,
	.remove = himax8389c_yashi_550_remove,
	.driver = {
		.name = "panel-himax8389c-yashi-550",
		.of_match_table = himax8389c_yashi_550_of_match,
	},
};
module_mipi_dsi_driver(himax8389c_yashi_550_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for LCD_TYPE_HIMAX8389C_YASHI_QHD_550");
MODULE_LICENSE("GPL");
