// SPDX-License-Identifier: GPL-2.0-only
/*
 * camss-vfe-vbif.c
 *
 * Qualcomm MSM Camera Subsystem - VFE VBIF Module
 *
 * Copyright (c) 2025, The Linux Foundation. All rights reserved.
 *
 */

#include <linux/io.h>

#include "camss.h"
#include "camss-vfe.h"
#include "camss-vfe-vbif.h"

void vfe_vbif_reg_write(struct vfe_device *vfe, u32 reg, u32 val)
{
	writel_relaxed(val, vfe->vbif_base + reg);
}

int vfe_vbif_apply_settings(struct vfe_device *vfe)
{
	switch (vfe->camss->res->version) {
	default:
		break;
	}

	return 0;
}
