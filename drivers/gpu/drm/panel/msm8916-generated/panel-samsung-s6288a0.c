// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2024 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.

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
#include <drm/drm_probe_helper.h>

//From downstream dt property "samsung,panel-aid-cmds-list-350"
#define AID_MIN 8
#define AID_MAX 785
#define MAX_BRIGHTNESS (AID_MAX - AID_MIN)

struct samsung {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline struct samsung *to_samsung(struct drm_panel *panel)
{
	return container_of(panel, struct samsung, panel);
}

static void samsung_reset(struct samsung *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(1000, 2000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(10000, 11000);
}

static int samsung_send_dcs_aid(struct mipi_dsi_device *dsi, u16 brightness)
{
	u8 payload[5] = { 0x40, 0x08, 0x20, 0, 0 };
	int ret;
	u16 aid;

	//Calculate AID value from brightness level
	if (brightness > MAX_BRIGHTNESS)
		brightness = MAX_BRIGHTNESS;
	aid = (MAX_BRIGHTNESS - brightness) + AID_MIN;
	payload[3] = (aid >> 8) & 0xff;
	payload[4] = aid & 0xff;

	//Set AID
	ret = mipi_dsi_dcs_write(dsi, 0xb2, payload, sizeof(payload));
	if (ret < 0) {
		dev_err(&dsi->dev, "Failed to set AID: %d\n", ret);
		return ret;
	}

	return 0;
}

static int samsung_on(struct samsung *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	//Tesk key ON - Enable level 1 control commands
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x5a, 0x5a);
	//Enable level 2 control commands
	mipi_dsi_dcs_write_seq(dsi, 0xfc, 0x5a, 0x5a);

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	//AVDD Setting
	mipi_dsi_dcs_write_seq(dsi, 0xb8, 0x38, 0x0b, 0x30);

	//Brightness gamma
	mipi_dsi_dcs_write_seq(dsi, 0xca,
				0x01, 0x00, 0x01, 0x00, 0x01, 0x00, // V255 RGB
				0x80, 0x80, 0x80,                   // V203 RGB
				0x80, 0x80, 0x80,                   // V151 RGB
				0x80, 0x80, 0x80,                   // V87  RGB
				0x80, 0x80, 0x80,                   // V51  RGB
				0x80, 0x80, 0x80,                   // V35  RGB
				0x80, 0x80, 0x80,                   // V23  RGB
				0x80, 0x80, 0x80,                   // V11  RGB
				0x80, 0x80, 0x80,                   // V3   RGB
				0x00, 0x00, 0x00);                  // VT   RGB

	//Set AID
	// We need the actual backlight value even while blank, hence why the
	// value is read directly instead of via backlight_get_brightness()
	samsung_send_dcs_aid(dsi, ctx->panel.backlight->props.brightness);

	//Set ELVSS condition
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0x28, 0x0b);

	//Set ACL
	mipi_dsi_dcs_write_seq(dsi, MIPI_DCS_WRITE_POWER_SAVE, 0x00);

	//Update gamma, LTPS(AID)
	mipi_dsi_dcs_write_seq(dsi, 0xf7, 0x03);

	//Disable level 2 control commands
	mipi_dsi_dcs_write_seq(dsi, 0xfc, 0xa5, 0xa5);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}

	return 0;
}

static int samsung_off(struct samsung *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	//Tesk key OFF - Disable level 1 control commands
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0xa5, 0xa5);

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
	msleep(120);

	return 0;
}

static int samsung_prepare(struct drm_panel *panel)
{
	struct samsung *ctx = to_samsung(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	samsung_reset(ctx);

	ret = samsung_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int samsung_unprepare(struct drm_panel *panel)
{
	struct samsung *ctx = to_samsung(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = samsung_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode samsung_mode = {
	.clock = (480 + 90 + 2 + 50) * (800 + 13 + 1 + 2) * 60 / 1000,
	.hdisplay = 480,
	.hsync_start = 480 + 90,
	.hsync_end = 480 + 90 + 2,
	.htotal = 480 + 90 + 2 + 50,
	.vdisplay = 800,
	.vsync_start = 800 + 13,
	.vsync_end = 800 + 13 + 1,
	.vtotal = 800 + 13 + 1 + 2,
	.width_mm = 56,
	.height_mm = 94,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int samsung_get_modes(struct drm_panel *panel,
			     struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &samsung_mode);
}

static const struct drm_panel_funcs samsung_panel_funcs = {
	.prepare = samsung_prepare,
	.unprepare = samsung_unprepare,
	.get_modes = samsung_get_modes,
};

static int samsung_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	int ret;
	u16 brightness = backlight_get_brightness(bl);

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	//Set AID
	ret = samsung_send_dcs_aid(dsi, brightness);
	if (ret != 0)
		return ret;

	//Update gamma, LTPS(AID)
	mipi_dsi_dcs_write_seq(dsi, 0xf7, 0x03);

	//TODO: downstream driver also updates ACL and ELVSS based on brightness
	// check out dsi-panel-samsung-wvga-video.dtsi

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops samsung_bl_ops = {
	.update_status = samsung_bl_update_status,
};

static struct backlight_device *
samsung_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = MAX_BRIGHTNESS,
		.max_brightness = MAX_BRIGHTNESS,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &samsung_bl_ops, &props);
}

static int samsung_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct samsung *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->supplies[0].supply = "vddio";
	ctx->supplies[1].supply = "vdd";
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

	dsi->lanes = 2;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST |
			  MIPI_DSI_MODE_NO_EOT_PACKET | MIPI_DSI_MODE_VIDEO_NO_HFP;

	drm_panel_init(&ctx->panel, dev, &samsung_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = samsung_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void samsung_remove(struct mipi_dsi_device *dsi)
{
	struct samsung *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id samsung_of_match[] = {
	{ .compatible = "samsung,s6288a0" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, samsung_of_match);

static struct mipi_dsi_driver samsung_driver = {
	.probe = samsung_probe,
	.remove = samsung_remove,
	.driver = {
		.name = "panel-samsung",
		.of_match_table = samsung_of_match,
	},
};
module_mipi_dsi_driver(samsung_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for Samsung WVGA video mode dsi panel");
MODULE_LICENSE("GPL");
