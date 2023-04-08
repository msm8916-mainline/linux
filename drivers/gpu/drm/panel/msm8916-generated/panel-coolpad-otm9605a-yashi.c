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

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

struct otm9605a_yashi_550 {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator *supply;
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct otm9605a_yashi_550 *to_otm9605a_yashi_550(struct drm_panel *panel)
{
	return container_of(panel, struct otm9605a_yashi_550, panel);
}

#define dsi_dcs_write_seq(dsi, seq...) do {				\
		static const u8 d[] = { seq };				\
		int ret;						\
		ret = mipi_dsi_dcs_write_buffer(dsi, d, ARRAY_SIZE(d));	\
		if (ret < 0)						\
			return ret;					\
	} while (0)

static void otm9605a_yashi_550_reset(struct otm9605a_yashi_550 *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	msleep(20);
}

static int otm9605a_yashi_550_on(struct otm9605a_yashi_550 *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xff, 0x96, 0x05, 0x01);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xff, 0x96, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0xc5);
	dsi_dcs_write_seq(dsi, 0xb0, 0x03);
	dsi_dcs_write_seq(dsi, 0x00, 0xa6);
	dsi_dcs_write_seq(dsi, 0xb3, 0x0b, 0x01);
	dsi_dcs_write_seq(dsi, 0x00, 0x89);
	dsi_dcs_write_seq(dsi, 0xc0, 0x01);
	dsi_dcs_write_seq(dsi, 0x00, 0xb4);
	dsi_dcs_write_seq(dsi, 0xc0, 0x50);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xc1, 0x36, 0x66);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xc1, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xc4, 0x9c);
	dsi_dcs_write_seq(dsi, 0x00, 0x91);
	dsi_dcs_write_seq(dsi, 0xc5, 0x79);
	dsi_dcs_write_seq(dsi, 0x00, 0xb1);
	dsi_dcs_write_seq(dsi, 0xc5, 0x28);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xc5, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xd0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04,
			  0x04, 0x04, 0x04, 0x04, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xe0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xf0);
	dsi_dcs_write_seq(dsi, 0xcb,
			  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			  0xff);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x01, 0x0d, 0x0f, 0x0b, 0x09, 0x05, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x02, 0x0e, 0x10);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x06, 0x10, 0x0e, 0x0a, 0x0c, 0x02, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x05, 0x0f, 0x0d);
	dsi_dcs_write_seq(dsi, 0x00, 0xd0);
	dsi_dcs_write_seq(dsi, 0xcc,
			  0x09, 0x0b, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			  0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x83, 0x01, 0x17, 0x82, 0x01, 0x17, 0x00, 0x0f, 0x00,
			  0x00, 0x0f, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x13, 0xbe, 0x17, 0x13, 0xbf, 0x17, 0xf0, 0x00, 0x00,
			  0xf0, 0x00, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x18, 0x01, 0x03, 0xbc, 0x00, 0x17, 0x00, 0x18, 0x00,
			  0x03, 0xbd, 0x00, 0x17, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x10, 0x00, 0x03, 0xbe, 0x00, 0x17, 0x00, 0x10, 0x01,
			  0x03, 0xbf, 0x00, 0x17, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x18, 0x01, 0x03, 0xc0, 0x00, 0x06, 0x12, 0x18, 0x00,
			  0x03, 0xc1, 0x00, 0x06, 0x12);
	dsi_dcs_write_seq(dsi, 0x00, 0xd0);
	dsi_dcs_write_seq(dsi, 0xce,
			  0x18, 0x03, 0x03, 0xc2, 0x00, 0x06, 0x12, 0x18, 0x02,
			  0x03, 0xc7, 0x00, 0x06, 0x12);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0xf0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xf0, 0x00,
			  0x00, 0x10, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x90);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0xf0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xf0, 0x00,
			  0x00, 0x10, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xa0);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0xf0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xf0, 0x00,
			  0x00, 0x10, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xb0);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0xf0, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0xf0, 0x00,
			  0x00, 0x10, 0x00, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0xc0);
	dsi_dcs_write_seq(dsi, 0xcf,
			  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00,
			  0x00);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xd8, 0x67, 0x67);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xd9, 0x17);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xe1,
			  0x02, 0x08, 0x0e, 0x0d, 0x06, 0x0f, 0x0c, 0x0b, 0x02,
			  0x06, 0x0b, 0x08, 0x0f, 0x12, 0x0d, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xe2,
			  0x02, 0x09, 0x0d, 0x0d, 0x06, 0x10, 0x0c, 0x0b, 0x02,
			  0x06, 0x0c, 0x08, 0x0f, 0x12, 0x0d, 0x05);
	dsi_dcs_write_seq(dsi, 0x00, 0xb2);
	dsi_dcs_write_seq(dsi, 0xc5, 0x01);
	dsi_dcs_write_seq(dsi, 0x00, 0xb1);
	dsi_dcs_write_seq(dsi, 0xc6, 0x06);
	dsi_dcs_write_seq(dsi, 0x00, 0xb4);
	dsi_dcs_write_seq(dsi, 0xc6, 0x10);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xec,
			  0x40, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
			  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
			  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
			  0x44, 0x44, 0x44, 0x44, 0x44, 0x04);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xed,
			  0x40, 0x34, 0x44, 0x44, 0x34, 0x44, 0x44, 0x34, 0x44,
			  0x44, 0x34, 0x44, 0x44, 0x34, 0x44, 0x44, 0x34, 0x44,
			  0x44, 0x43, 0x44, 0x44, 0x43, 0x44, 0x44, 0x43, 0x44,
			  0x44, 0x43, 0x44, 0x44, 0x43, 0x04);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xee,
			  0x40, 0x44, 0x34, 0x44, 0x44, 0x44, 0x44, 0x43, 0x44,
			  0x44, 0x44, 0x34, 0x44, 0x44, 0x44, 0x44, 0x34, 0x44,
			  0x44, 0x44, 0x44, 0x43, 0x44, 0x44, 0x44, 0x34, 0x44,
			  0x44, 0x44, 0x44, 0x43, 0x44, 0x04);
	dsi_dcs_write_seq(dsi, 0x00, 0x80);
	dsi_dcs_write_seq(dsi, 0xd6, 0x68);
	dsi_dcs_write_seq(dsi, 0x00, 0x87);
	dsi_dcs_write_seq(dsi, 0xc4, 0x40);
	dsi_dcs_write_seq(dsi, 0x00, 0x84);
	dsi_dcs_write_seq(dsi, 0xc4, 0x18);
	dsi_dcs_write_seq(dsi, 0x00, 0xd2);
	dsi_dcs_write_seq(dsi, 0xb0, 0x04);
	dsi_dcs_write_seq(dsi, 0x00, 0x00);
	dsi_dcs_write_seq(dsi, 0xff, 0xff, 0xff, 0xff);
	dsi_dcs_write_seq(dsi, 0x51, 0x00);
	dsi_dcs_write_seq(dsi, 0x53, 0x24);
	dsi_dcs_write_seq(dsi, 0x55, 0x01);

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
	msleep(20);

	return 0;
}

static int otm9605a_yashi_550_off(struct otm9605a_yashi_550 *ctx)
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

static int otm9605a_yashi_550_prepare(struct drm_panel *panel)
{
	struct otm9605a_yashi_550 *ctx = to_otm9605a_yashi_550(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_enable(ctx->supply);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulator: %d\n", ret);
		return ret;
	}

	otm9605a_yashi_550_reset(ctx);

	ret = otm9605a_yashi_550_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_disable(ctx->supply);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int otm9605a_yashi_550_unprepare(struct drm_panel *panel)
{
	struct otm9605a_yashi_550 *ctx = to_otm9605a_yashi_550(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = otm9605a_yashi_550_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_disable(ctx->supply);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode otm9605a_yashi_550_mode = {
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

static int otm9605a_yashi_550_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &otm9605a_yashi_550_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs otm9605a_yashi_550_panel_funcs = {
	.prepare = otm9605a_yashi_550_prepare,
	.unprepare = otm9605a_yashi_550_unprepare,
	.get_modes = otm9605a_yashi_550_get_modes,
};

static int otm9605a_yashi_550_bl_update_status(struct backlight_device *bl)
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
static int otm9605a_yashi_550_bl_get_brightness(struct backlight_device *bl)
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

static const struct backlight_ops otm9605a_yashi_550_bl_ops = {
	.update_status = otm9605a_yashi_550_bl_update_status,
	.get_brightness = otm9605a_yashi_550_bl_get_brightness,
};

static struct backlight_device *
otm9605a_yashi_550_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &otm9605a_yashi_550_bl_ops, &props);
}

static int otm9605a_yashi_550_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct otm9605a_yashi_550 *ctx;
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

	drm_panel_init(&ctx->panel, dev, &otm9605a_yashi_550_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);

	ctx->panel.backlight = otm9605a_yashi_550_create_backlight(dsi);
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

static void otm9605a_yashi_550_remove(struct mipi_dsi_device *dsi)
{
	struct otm9605a_yashi_550 *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id otm9605a_yashi_550_of_match[] = {
	{ .compatible = "coolpad,otm9605a-yashi" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, otm9605a_yashi_550_of_match);

static struct mipi_dsi_driver otm9605a_yashi_550_driver = {
	.probe = otm9605a_yashi_550_probe,
	.remove = otm9605a_yashi_550_remove,
	.driver = {
		.name = "panel-otm9605a-yashi-550",
		.of_match_table = otm9605a_yashi_550_of_match,
	},
};
module_mipi_dsi_driver(otm9605a_yashi_550_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for LCD_TYPE_OTM9605A_YASHI_QHD_550");
MODULE_LICENSE("GPL");
