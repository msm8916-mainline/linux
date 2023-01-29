// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Simple driver for ROHM Semiconductor BD65B60GWL Backlight driver chip
 * Copyright (C) 2014 ROHM Semiconductor.com
 * Copyright (C) 2014 MMI
 * Copyright (C) 2023 Bogdan Ionescu <bogdan.ionescu.work@gmail.com>
*/

#ifndef __BD65B60_H__
#define __BD65B60_H__

enum bd65b60_ovp {
	BD65B60_25V_OVP = 0x00,
	BD65B60_30V_OVP = 0x08,
	BD65B60_35V_OVP = 0x10,
};

enum bd65b60_ledsel {
	BD65B60_DISABLE = 0x00,
	BD65B60_LED1SEL = 0x01,
	BD65B60_LED2SEL = 0x04,
	BD65B60_LED12SEL = 0x05,
};

enum bd65b60_pwm_ctrl {
	BD65B60_PWM_DISABLE = 0x00,
	BD65B60_PWM_ENABLE = 0x20,
};

/*
 *@init_level  : led a init brightness. 4~255
 *@no_reset : disable reset on init enable/disable
 *@led_sel  : led rail enable/disable
 *@ovp_val  : LED OVP Settings
 *@name  : device name
 *@trigger  : trigger
 *@default_on  : default state on enable/disable
 */
struct bd65b60_platform_data {
	int init_level;
	bool no_reset;
	enum bd65b60_ledsel led_sel;
	enum bd65b60_ovp ovp_val;
	const char *name;
	const char *trigger;
	bool default_on;
};

#endif
