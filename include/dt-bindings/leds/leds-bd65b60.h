/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */
/*
 * This header provides macros for the ROHM BD65B60 device tree bindings.
 *
 * Copyright (C) 2023 Bogdan Ionescu <bogdan.ionescu.work+kernel@gmail.com>
 */

#ifndef _DT_BINDINGS_LEDS_BD65B60_H
#define _DT_BINDINGS_LEDS_BD65B60_H

#define BD65B60_ENABLE_NONE 0
#define BD65B60_ENABLE_LED1 1
#define BD65B60_ENABLE_LED2 4
#define BD65B60_ENABLE_BOTH (BD65B60_ENABLE_LED1 | BD65B60_ENABLE_LED2)

#define BD65B60_OVP_25V 0
#define BD65B60_OVP_30V 0x08
#define BD65B60_OVP_35V 0x10

#endif /* _DT_BINDINGS_LEDS_BD65B60_H */
