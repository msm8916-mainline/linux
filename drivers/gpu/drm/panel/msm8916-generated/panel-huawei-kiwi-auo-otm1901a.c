// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025 FIXME
// Generated with linux-mdss-dsi-panel-driver-generator from vendor device tree:
//   Copyright (c) 2013, The Linux Foundation. All rights reserved. (FIXME)

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_probe_helper.h>

struct auo_otm1901a_5p5xa {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data *supplies;
	struct gpio_desc *reset_gpio;
};

static const struct regulator_bulk_data auo_otm1901a_5p5xa_supplies[] = {
	{ .supply = "vsp" },
	{ .supply = "vsn" },
};

static inline
struct auo_otm1901a_5p5xa *to_auo_otm1901a_5p5xa(struct drm_panel *panel)
{
	return container_of(panel, struct auo_otm1901a_5p5xa, panel);
}

static void auo_otm1901a_5p5xa_reset(struct auo_otm1901a_5p5xa *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(15000, 16000);
}

static int auo_otm1901a_5p5xa_on(struct auo_otm1901a_5p5xa *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff,
					 0x19, 0x01, 0x01, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0x19, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x1c, 0x33);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc1, 0xe8);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xa7);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc1, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0,
					 0x00, 0x2f, 0x00, 0x00, 0x00, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0,
					 0x00, 0x2f, 0x00, 0x00, 0x00, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x9a);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0, 0x1e);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xac);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0, 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xdc);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0, 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x81);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xa5, 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x82);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc4, 0xf0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x92);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe9, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf3, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x93);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc5, 0x1e);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x95);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc5, 0x32);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x97);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc5, 0x19);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x99);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc5, 0x2d);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x9b);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc5, 0x44, 0x40);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xd8, 0x1f, 0x1f);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0, 0xcc);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xbc);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x84);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc4, 0x22);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc4, 0x38);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x94);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc1, 0x84);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x98);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc1, 0x74);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xcd);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf5, 0x19);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xdb);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf5, 0x19);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xf5);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc1, 0x40);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb9);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0, 0x11);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x8d);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xf5, 0x20);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0,
					 0x00, 0x86, 0x00, 0x0a, 0x0a, 0x00,
					 0x86, 0x0a, 0x0a, 0x00, 0x86, 0x00,
					 0x0a, 0x0a);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xf0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc3,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0,
					 0x00, 0x00, 0x03, 0x00, 0x00, 0x1e,
					 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc0,
					 0x00, 0x00, 0x03, 0x00, 0x00, 0x1e,
					 0x06);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc2,
					 0x82, 0x00, 0x3d, 0x40);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc2,
					 0x01, 0x00, 0x45, 0x43, 0x01, 0x01,
					 0x45, 0x43);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc3,
					 0x84, 0x04, 0x01, 0x00, 0x02, 0x87,
					 0x83, 0x04, 0x01, 0x00, 0x02, 0x87);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc,
					 0x09, 0x0d, 0x11, 0x12, 0x13, 0x14,
					 0x15, 0x16, 0x17, 0x18, 0x0e, 0x28,
					 0x28, 0x28, 0x28);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc,
					 0x0d, 0x09, 0x12, 0x11, 0x13, 0x14,
					 0x15, 0x16, 0x17, 0x18, 0x0e, 0x28,
					 0x28, 0x28, 0x28);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc,
					 0x1d, 0x1e, 0x1f, 0x19, 0x1a, 0x1b,
					 0x1c, 0x20, 0x21, 0x22, 0x23, 0x24,
					 0x25, 0x26, 0x27);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc,
					 0x01, 0x02, 0x03, 0x05, 0x06, 0x07,
					 0x04, 0x08);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc,
					 0xff, 0x00, 0x30, 0xc0, 0x0f, 0x30,
					 0x00, 0x00, 0x33, 0x03, 0x00, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xde);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcc, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
					 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x30, 0x00, 0xff, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x15, 0x15, 0x05, 0xf5, 0x01, 0x01,
					 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
					 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x00, 0x01, 0xfd, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xc0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xd0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xe0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
					 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xf0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcb,
					 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
					 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x80);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcd,
					 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
					 0x3f, 0x3f, 0x01, 0x12, 0x11, 0x03,
					 0x04, 0x0b, 0x17);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcd,
					 0x3d, 0x02, 0x3d, 0x25, 0x25, 0x25,
					 0x1f, 0x20, 0x21, 0x25, 0x25);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xa0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcd,
					 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
					 0x3f, 0x3f, 0x01, 0x12, 0x11, 0x03,
					 0x04, 0x0b, 0x17);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xcd,
					 0x17, 0x02, 0x3d, 0x25, 0x25, 0x25,
					 0x1f, 0x20, 0x21, 0x25, 0x25);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xd4);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc3,
					 0x01, 0x01, 0x01, 0x01);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xf2);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc1, 0x80, 0x0f, 0x0f);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xf7);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc3,
					 0x03, 0x1b, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb0);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xca, 0x04);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb1);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xca, 0x04);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0xb3);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xca, 0x48);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xb0, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe1,
					 0x1f, 0x2e, 0x33, 0x3c, 0x42, 0x49,
					 0x54, 0x64, 0x6c, 0x7d, 0x88, 0x90,
					 0x6b, 0x67, 0x63, 0x54, 0x43, 0x33,
					 0x27, 0x21, 0x18, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe2,
					 0x1f, 0x2e, 0x33, 0x3c, 0x42, 0x49,
					 0x54, 0x64, 0x6c, 0x7d, 0x88, 0x90,
					 0x6b, 0x67, 0x63, 0x54, 0x43, 0x33,
					 0x27, 0x21, 0x18, 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe3,
					 0x37, 0x3e, 0x41, 0x47, 0x4d, 0x52,
					 0x5b, 0x68, 0x70, 0x81, 0x8a, 0x91,
					 0x6a, 0x66, 0x62, 0x54, 0x43, 0x33,
					 0x29, 0x23, 0x1b, 0x13, 0x0c, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe4,
					 0x37, 0x3e, 0x41, 0x47, 0x4d, 0x52,
					 0x5b, 0x68, 0x70, 0x81, 0x8a, 0x91,
					 0x6a, 0x66, 0x62, 0x54, 0x43, 0x33,
					 0x29, 0x23, 0x1b, 0x13, 0x0c, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe5,
					 0x1f, 0x2a, 0x2f, 0x38, 0x40, 0x46,
					 0x51, 0x62, 0x6b, 0x7e, 0x88, 0x90,
					 0x6b, 0x67, 0x64, 0x56, 0x45, 0x35,
					 0x2b, 0x25, 0x1e, 0x0f, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xe6,
					 0x1f, 0x2a, 0x2f, 0x38, 0x40, 0x46,
					 0x51, 0x62, 0x6b, 0x7e, 0x88, 0x90,
					 0x6b, 0x67, 0x64, 0x56, 0x45, 0x35,
					 0x2b, 0x25, 0x1e, 0x0f, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x90);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xca,
					 0xcc, 0xff, 0xa8, 0xff, 0x80, 0xff,
					 0x00, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x10);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0xb0, 0xab, 0xbc, 0xbb, 0xb9, 0xcb,
					 0x88, 0x88, 0x88, 0x78, 0x77, 0x66,
					 0x66, 0x66, 0x66, 0x66, 0x66, 0x66);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x11);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0xb0, 0xaa, 0xac, 0xbb, 0xa9, 0xcb,
					 0x88, 0x88, 0x88, 0x78, 0x77, 0x77,
					 0x67, 0x66, 0x66, 0x66, 0x66, 0x66);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x12);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0xb0, 0xb9, 0x9b, 0xac, 0xa9, 0xbb,
					 0x88, 0x88, 0x88, 0x78, 0x77, 0x77,
					 0x77, 0x77, 0x66, 0x66, 0x66, 0x66);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x13);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0xa0, 0xaa, 0xaa, 0xbb, 0x98, 0xbb,
					 0x88, 0x88, 0x88, 0x78, 0x77, 0x77,
					 0x77, 0x77, 0x77, 0x67, 0x66, 0x66);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x14);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0xa0, 0x9a, 0xaa, 0xba, 0xa8, 0xba,
					 0x88, 0x88, 0x88, 0x78, 0x77, 0x77,
					 0x77, 0x77, 0x77, 0x77, 0x67, 0x66);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x15);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0xa0, 0x99, 0xaa, 0xaa, 0x99, 0xaa,
					 0x88, 0x88, 0x88, 0x78, 0x77, 0x77,
					 0x77, 0x77, 0x77, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x16);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x90, 0x9a, 0x9a, 0xaa, 0x98, 0xaa,
					 0x88, 0x88, 0x88, 0x88, 0x78, 0x77,
					 0x77, 0x77, 0x77, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x17);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x90, 0x99, 0x9a, 0xa9, 0x99, 0xa9,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x78,
					 0x77, 0x77, 0x77, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x18);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x90, 0x99, 0x99, 0xa9, 0x98, 0xa9,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x78, 0x77, 0x77, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x19);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x90, 0x98, 0xa9, 0xa8, 0x88, 0xa9,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x78, 0x77, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x1a);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x90, 0x98, 0x99, 0xa8, 0x88, 0x99,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x88, 0x78, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x1b);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x90, 0x88, 0x99, 0x98, 0x89, 0xa8,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x88, 0x88, 0x77, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x1c);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x80, 0x98, 0xa8, 0x97, 0x89, 0x98,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x88, 0x88, 0x88, 0x77, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x1d);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x80, 0x88, 0x89, 0x98, 0x88, 0x89,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x77);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x1e);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x80, 0x88, 0x98, 0x88, 0x89, 0x88,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x78);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x1f);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc7,
					 0x80, 0x88, 0x88, 0x88, 0x88, 0x98,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
					 0x88, 0x88, 0x88, 0x88, 0x88, 0x88);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xc6, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0x00, 0x00);
	mipi_dsi_generic_write_seq_multi(&dsi_ctx, 0xff, 0xff, 0xff, 0xff);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x51, 0x00);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x53, 0x24);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x55, 0x01);
	mipi_dsi_dcs_write_seq_multi(&dsi_ctx, 0x5e, 0x28);
	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 20);

	return dsi_ctx.accum_err;
}

static int auo_otm1901a_5p5xa_off(struct auo_otm1901a_5p5xa *ctx)
{
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = ctx->dsi };

	ctx->dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 40);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);

	return dsi_ctx.accum_err;
}

static int auo_otm1901a_5p5xa_prepare(struct drm_panel *panel)
{
	struct auo_otm1901a_5p5xa *ctx = to_auo_otm1901a_5p5xa(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(auo_otm1901a_5p5xa_supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	auo_otm1901a_5p5xa_reset(ctx);

	ret = auo_otm1901a_5p5xa_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(auo_otm1901a_5p5xa_supplies), ctx->supplies);
		return ret;
	}

	return 0;
}

static int auo_otm1901a_5p5xa_unprepare(struct drm_panel *panel)
{
	struct auo_otm1901a_5p5xa *ctx = to_auo_otm1901a_5p5xa(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	ret = auo_otm1901a_5p5xa_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(auo_otm1901a_5p5xa_supplies), ctx->supplies);

	return 0;
}

static const struct drm_display_mode auo_otm1901a_5p5xa_mode = {
	.clock = (1080 + 104 + 20 + 54) * (1920 + 14 + 2 + 46) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 104,
	.hsync_end = 1080 + 104 + 20,
	.htotal = 1080 + 104 + 20 + 54,
	.vdisplay = 1920,
	.vsync_start = 1920 + 14,
	.vsync_end = 1920 + 14 + 2,
	.vtotal = 1920 + 14 + 2 + 46,
	.width_mm = 68,
	.height_mm = 121,
	.type = DRM_MODE_TYPE_DRIVER,
};

static int auo_otm1901a_5p5xa_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	return drm_connector_helper_get_modes_fixed(connector, &auo_otm1901a_5p5xa_mode);
}

static const struct drm_panel_funcs auo_otm1901a_5p5xa_panel_funcs = {
	.prepare = auo_otm1901a_5p5xa_prepare,
	.unprepare = auo_otm1901a_5p5xa_unprepare,
	.get_modes = auo_otm1901a_5p5xa_get_modes,
};

static int auo_otm1901a_5p5xa_bl_update_status(struct backlight_device *bl)
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
static int auo_otm1901a_5p5xa_bl_get_brightness(struct backlight_device *bl)
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

static const struct backlight_ops auo_otm1901a_5p5xa_bl_ops = {
	.update_status = auo_otm1901a_5p5xa_bl_update_status,
	.get_brightness = auo_otm1901a_5p5xa_bl_get_brightness,
};

static struct backlight_device *
auo_otm1901a_5p5xa_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &auo_otm1901a_5p5xa_bl_ops, &props);
}

static int auo_otm1901a_5p5xa_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct auo_otm1901a_5p5xa *ctx;
	int ret;

	ctx = devm_drm_panel_alloc(dev, struct auo_otm1901a_5p5xa, panel,
				   &auo_otm1901a_5p5xa_panel_funcs,
				   DRM_MODE_CONNECTOR_DSI);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ret = devm_regulator_bulk_get_const(dev,
					    ARRAY_SIZE(auo_otm1901a_5p5xa_supplies),
					    auo_otm1901a_5p5xa_supplies,
					    &ctx->supplies);
	if (ret < 0)
		return ret;

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

	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = auo_otm1901a_5p5xa_create_backlight(dsi);
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

static void auo_otm1901a_5p5xa_remove(struct mipi_dsi_device *dsi)
{
	struct auo_otm1901a_5p5xa *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id auo_otm1901a_5p5xa_of_match[] = {
	{ .compatible = "huawei,kiwi-auo-otm1901a" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, auo_otm1901a_5p5xa_of_match);

static struct mipi_dsi_driver auo_otm1901a_5p5xa_driver = {
	.probe = auo_otm1901a_5p5xa_probe,
	.remove = auo_otm1901a_5p5xa_remove,
	.driver = {
		.name = "panel-huawei-kiwi-auo-otm1901a",
		.of_match_table = auo_otm1901a_5p5xa_of_match,
	},
};
module_mipi_dsi_driver(auo_otm1901a_5p5xa_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for AUO_OTM1901A_5P5_1080PXA_VIDEO");
MODULE_LICENSE("GPL");
