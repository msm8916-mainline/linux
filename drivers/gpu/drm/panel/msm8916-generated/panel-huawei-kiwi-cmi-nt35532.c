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

struct cmi_nt35532_5p5xa {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct regulator_bulk_data supplies[2];
	struct gpio_desc *reset_gpio;
	bool prepared;
};

static inline
struct cmi_nt35532_5p5xa *to_cmi_nt35532_5p5xa(struct drm_panel *panel)
{
	return container_of(panel, struct cmi_nt35532_5p5xa, panel);
}

static void cmi_nt35532_5p5xa_reset(struct cmi_nt35532_5p5xa *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(5000, 6000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(15000, 16000);
}

static int cmi_nt35532_5p5xa_on(struct cmi_nt35532_5p5xa *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x01, 0x55);
	mipi_dsi_dcs_write_seq(dsi, 0x02, 0x59);
	mipi_dsi_dcs_write_seq(dsi, 0x04, 0x0c);
	mipi_dsi_dcs_write_seq(dsi, 0x05, 0x3b);
	mipi_dsi_dcs_write_seq(dsi, 0x06, 0x6e);
	mipi_dsi_dcs_write_seq(dsi, 0x07, 0xc6);
	mipi_dsi_dcs_write_seq(dsi, 0x0d, 0xc5);
	mipi_dsi_dcs_write_seq(dsi, 0x0e, 0xc5);
	mipi_dsi_dcs_write_seq(dsi, 0x0f, 0xe0);
	mipi_dsi_dcs_write_seq(dsi, 0x10, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0x12, 0x5a);
	mipi_dsi_dcs_write_seq(dsi, 0x16, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x17, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x68, 0x13);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x75, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x76, 0xea);
	mipi_dsi_dcs_write_seq(dsi, 0x77, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x78, 0xee);
	mipi_dsi_dcs_write_seq(dsi, 0x79, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7a, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0x7b, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7c, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0x7d, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7e, 0x2f);
	mipi_dsi_dcs_write_seq(dsi, 0x7f, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x80, 0x3e);
	mipi_dsi_dcs_write_seq(dsi, 0x81, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x82, 0x4c);
	mipi_dsi_dcs_write_seq(dsi, 0x83, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x84, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0x85, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x86, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0x87, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x88, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0x89, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x8a, 0xa8);
	mipi_dsi_dcs_write_seq(dsi, 0x8b, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x8c, 0xd9);
	mipi_dsi_dcs_write_seq(dsi, 0x8d, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x8e, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x8f, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x90, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0x91, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x92, 0x72);
	mipi_dsi_dcs_write_seq(dsi, 0x93, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x94, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0x95, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x96, 0xa0);
	mipi_dsi_dcs_write_seq(dsi, 0x97, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x98, 0xcc);
	mipi_dsi_dcs_write_seq(dsi, 0x99, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x9a, 0xeb);
	mipi_dsi_dcs_write_seq(dsi, 0x9b, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x9c, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0x9d, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x9e, 0x34);
	mipi_dsi_dcs_write_seq(dsi, 0x9f, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa0, 0x5e);
	mipi_dsi_dcs_write_seq(dsi, 0xa2, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa3, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0xa4, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa5, 0x73);
	mipi_dsi_dcs_write_seq(dsi, 0xa6, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa7, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xa9, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xaa, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0xab, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xac, 0xa4);
	mipi_dsi_dcs_write_seq(dsi, 0xad, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xae, 0xbb);
	mipi_dsi_dcs_write_seq(dsi, 0xaf, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0xcd);
	mipi_dsi_dcs_write_seq(dsi, 0xb1, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xb2, 0xcf);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb4, 0x76);
	mipi_dsi_dcs_write_seq(dsi, 0xb5, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0xda);
	mipi_dsi_dcs_write_seq(dsi, 0xb7, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xb8, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xb9, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xba, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0xbb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xbc, 0x2f);
	mipi_dsi_dcs_write_seq(dsi, 0xbd, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xbe, 0x3e);
	mipi_dsi_dcs_write_seq(dsi, 0xbf, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x4c);
	mipi_dsi_dcs_write_seq(dsi, 0xc1, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc2, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0xc3, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc4, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0xc5, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc6, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0xc7, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc8, 0xa8);
	mipi_dsi_dcs_write_seq(dsi, 0xc9, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0xd9);
	mipi_dsi_dcs_write_seq(dsi, 0xcb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xcc, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0xcd, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xce, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0xcf, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd0, 0x72);
	mipi_dsi_dcs_write_seq(dsi, 0xd1, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd2, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0xd3, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd4, 0xa0);
	mipi_dsi_dcs_write_seq(dsi, 0xd5, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd6, 0xcc);
	mipi_dsi_dcs_write_seq(dsi, 0xd7, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd8, 0xeb);
	mipi_dsi_dcs_write_seq(dsi, 0xd9, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xda, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0xdb, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xdc, 0x34);
	mipi_dsi_dcs_write_seq(dsi, 0xdd, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xde, 0x5e);
	mipi_dsi_dcs_write_seq(dsi, 0xdf, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe0, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0xe1, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe2, 0x73);
	mipi_dsi_dcs_write_seq(dsi, 0xe3, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe4, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xe5, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe6, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0xe7, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe8, 0xa4);
	mipi_dsi_dcs_write_seq(dsi, 0xe9, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xea, 0xbb);
	mipi_dsi_dcs_write_seq(dsi, 0xeb, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xec, 0xcd);
	mipi_dsi_dcs_write_seq(dsi, 0xed, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xee, 0xcf);
	mipi_dsi_dcs_write_seq(dsi, 0xef, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0xea);
	mipi_dsi_dcs_write_seq(dsi, 0xf1, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xf2, 0xee);
	mipi_dsi_dcs_write_seq(dsi, 0xf3, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xf4, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xf5, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xf6, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0xf7, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xf8, 0x2f);
	mipi_dsi_dcs_write_seq(dsi, 0xf9, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xfa, 0x3e);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x01, 0x4c);
	mipi_dsi_dcs_write_seq(dsi, 0x02, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x03, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0x04, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x05, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0x06, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x07, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0x08, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x09, 0xa8);
	mipi_dsi_dcs_write_seq(dsi, 0x0a, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x0b, 0xd9);
	mipi_dsi_dcs_write_seq(dsi, 0x0c, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x0d, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x0e, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x0f, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0x10, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x72);
	mipi_dsi_dcs_write_seq(dsi, 0x12, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x13, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0x14, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x15, 0xa0);
	mipi_dsi_dcs_write_seq(dsi, 0x16, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x17, 0xcc);
	mipi_dsi_dcs_write_seq(dsi, 0x18, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x19, 0xeb);
	mipi_dsi_dcs_write_seq(dsi, 0x1a, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x1b, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0x1c, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x1d, 0x34);
	mipi_dsi_dcs_write_seq(dsi, 0x1e, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x1f, 0x5e);
	mipi_dsi_dcs_write_seq(dsi, 0x20, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x21, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0x22, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x23, 0x73);
	mipi_dsi_dcs_write_seq(dsi, 0x24, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x25, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0x26, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x27, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0x28, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0xa4);
	mipi_dsi_dcs_write_seq(dsi, 0x2a, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x2b, 0xbb);
	mipi_dsi_dcs_write_seq(dsi, 0x2d, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x2f, 0xcd);
	mipi_dsi_dcs_write_seq(dsi, 0x30, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x31, 0xcf);
	mipi_dsi_dcs_write_seq(dsi, 0x32, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x33, 0x76);
	mipi_dsi_dcs_write_seq(dsi, 0x34, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x35, 0xda);
	mipi_dsi_dcs_write_seq(dsi, 0x36, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x37, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0x38, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x39, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0x3a, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x3b, 0x2f);
	mipi_dsi_dcs_write_seq(dsi, 0x3d, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x3f, 0x3e);
	mipi_dsi_dcs_write_seq(dsi, 0x40, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x41, 0x4c);
	mipi_dsi_dcs_write_seq(dsi, 0x42, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x43, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0x44, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x45, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0x46, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x47, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0x48, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x49, 0xa8);
	mipi_dsi_dcs_write_seq(dsi, 0x4a, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x4b, 0xd9);
	mipi_dsi_dcs_write_seq(dsi, 0x4c, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x4d, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x4e, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x4f, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0x50, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x51, 0x72);
	mipi_dsi_dcs_write_seq(dsi, 0x52, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x53, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0x54, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x55, 0xa0);
	mipi_dsi_dcs_write_seq(dsi, 0x56, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x58, 0xcc);
	mipi_dsi_dcs_write_seq(dsi, 0x59, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x5a, 0xeb);
	mipi_dsi_dcs_write_seq(dsi, 0x5b, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x5c, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0x5d, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x5e, 0x34);
	mipi_dsi_dcs_write_seq(dsi, 0x5f, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x60, 0x5e);
	mipi_dsi_dcs_write_seq(dsi, 0x61, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x62, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0x63, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x64, 0x73);
	mipi_dsi_dcs_write_seq(dsi, 0x65, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x66, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0x67, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x68, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0x69, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x6a, 0xa4);
	mipi_dsi_dcs_write_seq(dsi, 0x6b, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x6c, 0xbb);
	mipi_dsi_dcs_write_seq(dsi, 0x6d, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x6e, 0xcd);
	mipi_dsi_dcs_write_seq(dsi, 0x6f, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x70, 0xcf);
	mipi_dsi_dcs_write_seq(dsi, 0x71, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x72, 0xea);
	mipi_dsi_dcs_write_seq(dsi, 0x73, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x74, 0xee);
	mipi_dsi_dcs_write_seq(dsi, 0x75, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x76, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0x77, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x78, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0x79, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7a, 0x2f);
	mipi_dsi_dcs_write_seq(dsi, 0x7b, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7c, 0x3e);
	mipi_dsi_dcs_write_seq(dsi, 0x7d, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7e, 0x4c);
	mipi_dsi_dcs_write_seq(dsi, 0x7f, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x80, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0x81, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x82, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0x83, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x84, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0x85, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x86, 0xa8);
	mipi_dsi_dcs_write_seq(dsi, 0x87, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x88, 0xd9);
	mipi_dsi_dcs_write_seq(dsi, 0x89, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x8a, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x8b, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x8c, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0x8d, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x8e, 0x72);
	mipi_dsi_dcs_write_seq(dsi, 0x8f, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x90, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0x91, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x92, 0xa0);
	mipi_dsi_dcs_write_seq(dsi, 0x93, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x94, 0xcc);
	mipi_dsi_dcs_write_seq(dsi, 0x95, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0x96, 0xeb);
	mipi_dsi_dcs_write_seq(dsi, 0x97, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x98, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0x99, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x9a, 0x34);
	mipi_dsi_dcs_write_seq(dsi, 0x9b, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x9c, 0x5e);
	mipi_dsi_dcs_write_seq(dsi, 0x9d, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x9e, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0x9f, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa0, 0x73);
	mipi_dsi_dcs_write_seq(dsi, 0xa2, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa3, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xa4, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa5, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0xa6, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xa7, 0xa4);
	mipi_dsi_dcs_write_seq(dsi, 0xa9, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xaa, 0xbb);
	mipi_dsi_dcs_write_seq(dsi, 0xab, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xac, 0xcd);
	mipi_dsi_dcs_write_seq(dsi, 0xad, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xae, 0xcf);
	mipi_dsi_dcs_write_seq(dsi, 0xaf, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x76);
	mipi_dsi_dcs_write_seq(dsi, 0xb1, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb2, 0xda);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xb4, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xb5, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0xb7, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xb8, 0x2f);
	mipi_dsi_dcs_write_seq(dsi, 0xb9, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xba, 0x3e);
	mipi_dsi_dcs_write_seq(dsi, 0xbb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xbc, 0x4c);
	mipi_dsi_dcs_write_seq(dsi, 0xbd, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xbe, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0xbf, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0xc1, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc2, 0x89);
	mipi_dsi_dcs_write_seq(dsi, 0xc3, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc4, 0xa8);
	mipi_dsi_dcs_write_seq(dsi, 0xc5, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc6, 0xd9);
	mipi_dsi_dcs_write_seq(dsi, 0xc7, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xc8, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0xc9, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0xcb, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xcc, 0x72);
	mipi_dsi_dcs_write_seq(dsi, 0xcd, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xce, 0x74);
	mipi_dsi_dcs_write_seq(dsi, 0xcf, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd0, 0xa0);
	mipi_dsi_dcs_write_seq(dsi, 0xd1, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd2, 0xcc);
	mipi_dsi_dcs_write_seq(dsi, 0xd3, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xd4, 0xeb);
	mipi_dsi_dcs_write_seq(dsi, 0xd5, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xd6, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0xd7, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xd8, 0x34);
	mipi_dsi_dcs_write_seq(dsi, 0xd9, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xda, 0x5e);
	mipi_dsi_dcs_write_seq(dsi, 0xdb, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xdc, 0x66);
	mipi_dsi_dcs_write_seq(dsi, 0xdd, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xde, 0x73);
	mipi_dsi_dcs_write_seq(dsi, 0xdf, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe0, 0x82);
	mipi_dsi_dcs_write_seq(dsi, 0xe1, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe2, 0x91);
	mipi_dsi_dcs_write_seq(dsi, 0xe3, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe4, 0xa4);
	mipi_dsi_dcs_write_seq(dsi, 0xe5, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe6, 0xbb);
	mipi_dsi_dcs_write_seq(dsi, 0xe7, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xe8, 0xcd);
	mipi_dsi_dcs_write_seq(dsi, 0xe9, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0xea, 0xcf);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x04);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x13, 0xf3);
	mipi_dsi_dcs_write_seq(dsi, 0x14, 0xf2);
	mipi_dsi_dcs_write_seq(dsi, 0x15, 0xf1);
	mipi_dsi_dcs_write_seq(dsi, 0x16, 0xf0);
	mipi_dsi_dcs_write_seq(dsi, 0x21, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x22, 0xf4);
	mipi_dsi_dcs_write_seq(dsi, 0x23, 0xe9);
	mipi_dsi_dcs_write_seq(dsi, 0x24, 0xde);
	mipi_dsi_dcs_write_seq(dsi, 0x25, 0xd3);
	mipi_dsi_dcs_write_seq(dsi, 0x26, 0xc8);
	mipi_dsi_dcs_write_seq(dsi, 0x27, 0xbd);
	mipi_dsi_dcs_write_seq(dsi, 0x28, 0xb2);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0xa7);
	mipi_dsi_dcs_write_seq(dsi, 0x2a, 0x9c);
	mipi_dsi_dcs_write_seq(dsi, 0x07, 0x20);
	mipi_dsi_dcs_write_seq(dsi, 0x08, 0x07);
	mipi_dsi_dcs_write_seq(dsi, 0x46, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x00, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x01, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x02, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x03, 0x06);
	mipi_dsi_dcs_write_seq(dsi, 0x04, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x05, 0x16);
	mipi_dsi_dcs_write_seq(dsi, 0x06, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x07, 0x1a);
	mipi_dsi_dcs_write_seq(dsi, 0x08, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0x09, 0x1e);
	mipi_dsi_dcs_write_seq(dsi, 0x0a, 0x20);
	mipi_dsi_dcs_write_seq(dsi, 0x0b, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x0c, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x0d, 0x26);
	mipi_dsi_dcs_write_seq(dsi, 0x0e, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0x0f, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0x10, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x11, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x12, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x13, 0x0e);
	mipi_dsi_dcs_write_seq(dsi, 0x14, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x15, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x16, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x17, 0x07);
	mipi_dsi_dcs_write_seq(dsi, 0x18, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x19, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0x1a, 0x19);
	mipi_dsi_dcs_write_seq(dsi, 0x1b, 0x1b);
	mipi_dsi_dcs_write_seq(dsi, 0x1c, 0x1d);
	mipi_dsi_dcs_write_seq(dsi, 0x1d, 0x1f);
	mipi_dsi_dcs_write_seq(dsi, 0x1e, 0x21);
	mipi_dsi_dcs_write_seq(dsi, 0x1f, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x20, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x21, 0x26);
	mipi_dsi_dcs_write_seq(dsi, 0x22, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0x23, 0x09);
	mipi_dsi_dcs_write_seq(dsi, 0x24, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x25, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x26, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x27, 0x0f);
	mipi_dsi_dcs_write_seq(dsi, 0x28, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x29, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x2a, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x2b, 0x09);
	mipi_dsi_dcs_write_seq(dsi, 0x2d, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x2f, 0x19);
	mipi_dsi_dcs_write_seq(dsi, 0x30, 0x17);
	mipi_dsi_dcs_write_seq(dsi, 0x31, 0x21);
	mipi_dsi_dcs_write_seq(dsi, 0x32, 0x1f);
	mipi_dsi_dcs_write_seq(dsi, 0x33, 0x1d);
	mipi_dsi_dcs_write_seq(dsi, 0x34, 0x1b);
	mipi_dsi_dcs_write_seq(dsi, 0x35, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x36, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x37, 0x26);
	mipi_dsi_dcs_write_seq(dsi, 0x38, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0x39, 0x07);
	mipi_dsi_dcs_write_seq(dsi, 0x3a, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x3b, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x3d, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x3f, 0x0f);
	mipi_dsi_dcs_write_seq(dsi, 0x40, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x41, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x42, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x43, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0x44, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x45, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x46, 0x16);
	mipi_dsi_dcs_write_seq(dsi, 0x47, 0x20);
	mipi_dsi_dcs_write_seq(dsi, 0x48, 0x1e);
	mipi_dsi_dcs_write_seq(dsi, 0x49, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0x4a, 0x1a);
	mipi_dsi_dcs_write_seq(dsi, 0x4b, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x4c, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0x4d, 0x26);
	mipi_dsi_dcs_write_seq(dsi, 0x4e, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0x4f, 0x06);
	mipi_dsi_dcs_write_seq(dsi, 0x50, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x51, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x52, 0x38);
	mipi_dsi_dcs_write_seq(dsi, 0x53, 0x0e);
	mipi_dsi_dcs_write_seq(dsi, 0x54, 0x07);
	mipi_dsi_dcs_write_seq(dsi, 0x55, 0x19);
	mipi_dsi_dcs_write_seq(dsi, 0x59, 0x24);
	mipi_dsi_dcs_write_seq(dsi, 0x5b, 0x4f);
	mipi_dsi_dcs_write_seq(dsi, 0x5c, 0x2c);
	mipi_dsi_dcs_write_seq(dsi, 0x5d, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x5e, 0x22);
	mipi_dsi_dcs_write_seq(dsi, 0x62, 0x21);
	mipi_dsi_dcs_write_seq(dsi, 0x63, 0x69);
	mipi_dsi_dcs_write_seq(dsi, 0x64, 0x12);
	mipi_dsi_dcs_write_seq(dsi, 0x66, 0x57);
	mipi_dsi_dcs_write_seq(dsi, 0x67, 0x11);
	mipi_dsi_dcs_write_seq(dsi, 0x68, 0x2b);
	mipi_dsi_dcs_write_seq(dsi, 0x69, 0x12);
	mipi_dsi_dcs_write_seq(dsi, 0x6a, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0x6b, 0x29);
	mipi_dsi_dcs_write_seq(dsi, 0x6c, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0x6d, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0x6f, 0x3c);
	mipi_dsi_dcs_write_seq(dsi, 0x70, 0x03);
	mipi_dsi_dcs_write_seq(dsi, 0x72, 0x22);
	mipi_dsi_dcs_write_seq(dsi, 0x73, 0x22);
	mipi_dsi_dcs_write_seq(dsi, 0x7d, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0x7e, 0xaa);
	mipi_dsi_dcs_write_seq(dsi, 0x7f, 0xaa);
	mipi_dsi_dcs_write_seq(dsi, 0x80, 0xaa);
	mipi_dsi_dcs_write_seq(dsi, 0x81, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x85, 0x3f);
	mipi_dsi_dcs_write_seq(dsi, 0x86, 0x3f);
	mipi_dsi_dcs_write_seq(dsi, 0xa2, 0xb0);
	mipi_dsi_dcs_write_seq(dsi, 0xbd, 0xa6);
	mipi_dsi_dcs_write_seq(dsi, 0xbe, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0xbf, 0x12);
	mipi_dsi_dcs_write_seq(dsi, 0xc8, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc9, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xcb, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xcc, 0x09);
	mipi_dsi_dcs_write_seq(dsi, 0xce, 0x18);
	mipi_dsi_dcs_write_seq(dsi, 0xcf, 0x88);
	mipi_dsi_dcs_write_seq(dsi, 0xd0, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xd1, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xd2, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xd3, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xd4, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0xd6, 0x22);
	mipi_dsi_dcs_write_seq(dsi, 0x90, 0x7b);
	mipi_dsi_dcs_write_seq(dsi, 0x91, 0x10);
	mipi_dsi_dcs_write_seq(dsi, 0x92, 0x10);
	mipi_dsi_dcs_write_seq(dsi, 0x98, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x99, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x9f, 0x0f);
	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);
	mipi_dsi_dcs_write_seq(dsi, 0xba, 0xc3);
	mipi_dsi_dcs_write_seq(dsi, 0xd3, 0x10);
	mipi_dsi_dcs_write_seq(dsi, 0xd4, 0x1a);
	mipi_dsi_dcs_write_seq(dsi, 0xd5, 0x14);
	mipi_dsi_dcs_write_seq(dsi, 0xd6, 0x14);
	mipi_dsi_dcs_write_seq(dsi, 0x5e, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0x51, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x53, 0x24);
	mipi_dsi_dcs_write_seq(dsi, 0x55, 0x01);

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

static int cmi_nt35532_5p5xa_off(struct cmi_nt35532_5p5xa *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	mipi_dsi_dcs_write_seq(dsi, 0xff, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xfb, 0x01);

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}
	msleep(20);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}
	msleep(120);

	return 0;
}

static int cmi_nt35532_5p5xa_prepare(struct drm_panel *panel)
{
	struct cmi_nt35532_5p5xa *ctx = to_cmi_nt35532_5p5xa(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
		dev_err(dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	cmi_nt35532_5p5xa_reset(ctx);

	ret = cmi_nt35532_5p5xa_on(ctx);
	if (ret < 0) {
		dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 1);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	ctx->prepared = true;
	return 0;
}

static int cmi_nt35532_5p5xa_unprepare(struct drm_panel *panel)
{
	struct cmi_nt35532_5p5xa *ctx = to_cmi_nt35532_5p5xa(panel);
	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (!ctx->prepared)
		return 0;

	ret = cmi_nt35532_5p5xa_off(ctx);
	if (ret < 0)
		dev_err(dev, "Failed to un-initialize panel: %d\n", ret);

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode cmi_nt35532_5p5xa_mode = {
	.clock = (1080 + 94 + 12 + 85) * (1920 + 26 + 8 + 8) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 94,
	.hsync_end = 1080 + 94 + 12,
	.htotal = 1080 + 94 + 12 + 85,
	.vdisplay = 1920,
	.vsync_start = 1920 + 26,
	.vsync_end = 1920 + 26 + 8,
	.vtotal = 1920 + 26 + 8 + 8,
	.width_mm = 68,
	.height_mm = 121,
};

static int cmi_nt35532_5p5xa_get_modes(struct drm_panel *panel,
				       struct drm_connector *connector)
{
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &cmi_nt35532_5p5xa_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs cmi_nt35532_5p5xa_panel_funcs = {
	.prepare = cmi_nt35532_5p5xa_prepare,
	.unprepare = cmi_nt35532_5p5xa_unprepare,
	.get_modes = cmi_nt35532_5p5xa_get_modes,
};

static int cmi_nt35532_5p5xa_bl_update_status(struct backlight_device *bl)
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
static int cmi_nt35532_5p5xa_bl_get_brightness(struct backlight_device *bl)
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

static const struct backlight_ops cmi_nt35532_5p5xa_bl_ops = {
	.update_status = cmi_nt35532_5p5xa_bl_update_status,
	.get_brightness = cmi_nt35532_5p5xa_bl_get_brightness,
};

static struct backlight_device *
cmi_nt35532_5p5xa_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &cmi_nt35532_5p5xa_bl_ops, &props);
}

static int cmi_nt35532_5p5xa_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct cmi_nt35532_5p5xa *ctx;
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

	drm_panel_init(&ctx->panel, dev, &cmi_nt35532_5p5xa_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = cmi_nt35532_5p5xa_create_backlight(dsi);
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

static void cmi_nt35532_5p5xa_remove(struct mipi_dsi_device *dsi)
{
	struct cmi_nt35532_5p5xa *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id cmi_nt35532_5p5xa_of_match[] = {
	{ .compatible = "huawei,kiwi-cmi-nt35532" }, // FIXME
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, cmi_nt35532_5p5xa_of_match);

static struct mipi_dsi_driver cmi_nt35532_5p5xa_driver = {
	.probe = cmi_nt35532_5p5xa_probe,
	.remove = cmi_nt35532_5p5xa_remove,
	.driver = {
		.name = "panel-cmi-nt35532-5p5xa",
		.of_match_table = cmi_nt35532_5p5xa_of_match,
	},
};
module_mipi_dsi_driver(cmi_nt35532_5p5xa_driver);

MODULE_AUTHOR("linux-mdss-dsi-panel-driver-generator <fix@me>"); // FIXME
MODULE_DESCRIPTION("DRM driver for CMI_NT35532_5P5_1080PXA_VIDEO");
MODULE_LICENSE("GPL");
