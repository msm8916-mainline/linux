// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Simple driver for ROHM Semiconductor BD65B60GWL Backlight driver chip
 *
 * Copyright (C) 2014 ROHM Semiconductor.com
 * Copyright (C) 2014 MMI
 * Copyright (C) 2023 Bogdan Ionescu <bogdan.ionescu.work+kernel@gmail.com>
 */

#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#define BD65B60_MAX_BRIGHTNESS 255
#define BD65B60_DEFAULT_BRIGHTNESS 255
#define BD65B60_DEFAULT_TRIGGER "backlight"
#define BD65B60_DEFAULT_NAME "white"
#define BD65B60_DEFAULT_OVP_VAL BD65B60_35V_OVP

#define INT_DEBOUNCE_MSEC 10

#define PWMEN_MASK 0x20
#define OVP_MASK 0x18
#define LEDSEL_MASK 0x05

enum bd65b60_regs {
	REG_SFTRST = 0x00,
	REG_COMSET1 = 0x01,
	REG_COMSET2 = 0x02,
	REG_LEDSEL = 0x03,
	REG_ILED = 0x05,
	REG_CTRLSET = 0x07,
	REG_SLEWSET = 0x08,
	REG_PON = 0x0E,
	REG_MAX = REG_PON,
};

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

enum bd65b60_state {
	BD65B60_OFF = 0,
	BD65B60_ON = 1,
	BD65B60_KEEP = 2,
};

struct bd65b60_led {
	struct led_classdev cdev;
	struct i2c_client *client;
	struct regmap *regmap;
	struct mutex lock; /* Held when writing to registers */
	enum bd65b60_ledsel enable;
	enum bd65b60_state state;
	enum bd65b60_ovp ovp;
};

static const struct regmap_config bd65b60_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = REG_MAX,
};

static void bd65b60_brightness_set(struct led_classdev *cdev,
				   enum led_brightness brightness)
{
	int ret;
	enum bd65b60_state new_state;
	struct bd65b60_led *led = container_of(cdev, struct bd65b60_led, cdev);

	mutex_lock(&led->lock);

	ret = regmap_write(led->regmap, REG_ILED, brightness);

	new_state = (brightness) ? BD65B60_ON : BD65B60_OFF;

	if (new_state != led->state) {
		ret |= regmap_write(led->regmap, REG_PON, new_state);
		led->state = new_state;
	}

	mutex_unlock(&led->lock);

	if (ret)
		dev_err(&led->client->dev, "Failed to set brightness: %d", ret);
}

static int bd65b60_init(struct bd65b60_led *led)
{
	int ret;

	mutex_lock(&led->lock);

	if (led->state != BD65B60_KEEP) {
		/* Reset the chip */
		ret = regmap_write(led->regmap, REG_SFTRST, 0x01);
	}

	ret |= regmap_update_bits(led->regmap, REG_COMSET1, OVP_MASK, led->ovp);
	ret |= regmap_update_bits(led->regmap, REG_LEDSEL, LEDSEL_MASK,
				  led->enable);
	ret |= regmap_update_bits(led->regmap, REG_CTRLSET, PWMEN_MASK,
				  BD65B60_PWM_ENABLE);
	ret |= regmap_write(led->regmap, REG_PON,
			    led->state ? BD65B60_ON : BD65B60_OFF);

	mutex_unlock(&led->lock);

	return ret;
}

static int bd65b60_parse_dt(struct bd65b60_led *led,
			    struct fwnode_handle **fwnode)
{
	struct device *dev = &led->client->dev;
	const char *default_state;
	u32 src[2] = { 0, 0 };
	int src_n;
	u32 ovp;
	int ret;

	led->enable = BD65B60_DISABLE;
	src_n = ret = device_property_count_u32(dev, "led-sources");
	if (ret < 0) {
		dev_err(dev, "No led-sources property found");
		return ret;
	}

	if (src_n < 1 || src_n > 2) {
		dev_err(dev, "Invalid number of sources %d", src_n);
		return src_n ? -EOVERFLOW : -ENODATA;
	}

	if (device_property_read_u32_array(dev, "led-sources", src, src_n))
		return ret;

	while (--src_n) {
		if (src[src_n] > 2) {
			dev_err(dev, "Invalid led-source %d", src[src_n]);
			return -EINVAL;
		}
		led->enable |= src[src_n] == 2 ? BD65B60_LED2SEL :
						 BD65B60_LED1SEL;
	}

	/* Check optional properties */
	led->state = BD65B60_OFF;
	ret = device_property_read_string(dev, "default-state", &default_state);
	if (!ret) {
		if (!strcmp(default_state, "keep")) {
			led->state = BD65B60_KEEP;
		} else if (!strcmp(default_state, "on")) {
			led->state = BD65B60_ON;
		} else if (!strcmp(default_state, "off")) {
			led->state = BD65B60_OFF;
		} else {
			dev_err(dev, "Invalid default-state %s", default_state);
			return -EINVAL;
		}
	}

	led->ovp = BD65B60_DEFAULT_OVP_VAL;
	ret = device_property_read_u32(dev, "rohm,ovp-microvolt", &ovp);
	if (!ret) {
		switch (ovp) {
		case (25 * 1000 * 1000):
			led->ovp = BD65B60_25V_OVP;
			break;
		case (30 * 1000 * 1000):
			led->ovp = BD65B60_30V_OVP;
			break;
		case (35 * 1000 * 1000):
			led->ovp = BD65B60_35V_OVP;
			break;
		default:
			dev_err(dev, "Invalid rohm,ovp-microvolt %d", ovp);
			return -EINVAL;
		}
	}

	*fwnode = dev_fwnode(dev);

	return 0;
}

static int bd65b60_probe(struct i2c_client *client)
{
	struct bd65b60_led *led;
	struct led_init_data init_data = {};
	struct fwnode_handle *fwnode = NULL;
	int ret;

	led = devm_kzalloc(&client->dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	led->client = client;
	i2c_set_clientdata(client, led);

	led->cdev.name = BD65B60_DEFAULT_NAME;
	led->cdev.brightness_set = bd65b60_brightness_set;
	led->cdev.brightness = BD65B60_DEFAULT_BRIGHTNESS;
	led->cdev.max_brightness = BD65B60_MAX_BRIGHTNESS;
	led->cdev.default_trigger = BD65B60_DEFAULT_TRIGGER;
	led->client = client;

	led->regmap = devm_regmap_init_i2c(client, &bd65b60_regmap_config);
	if (IS_ERR(led->regmap)) {
		ret = PTR_ERR(led->regmap);
		return dev_err_probe(&client->dev, ret,
				     "Failed to allocate register map");
	}

	ret = bd65b60_parse_dt(led, &fwnode);
	if (ret)
		return dev_err_probe(&client->dev, ret,
				     "Failed to parse devicetree");

	mutex_init(&led->lock);

	ret = bd65b60_init(led);
	if (ret) {
		mutex_destroy(&led->lock);
		return dev_err_probe(&client->dev, ret,
				     "Failed to initialiese led");
	}

	init_data.fwnode = fwnode;
	init_data.devicename = led->client->name;
	init_data.default_label = ":";
	ret = devm_led_classdev_register_ext(&client->dev, &led->cdev,
					     &init_data);
	if (ret) {
		mutex_destroy(&led->lock);
		return dev_err_probe(&client->dev, ret,
				     "Failed to register led");
	}

	return 0;
}

static void bd65b60_remove(struct i2c_client *client)
{
	int ret;
	struct bd65b60_led *led = i2c_get_clientdata(client);

	ret = regmap_write(led->regmap, REG_PON, BD65B60_OFF);
	if (ret)
		dev_err(&client->dev, "Failed to turn off led: %d", ret);

	mutex_destroy(&led->lock);
}

static const struct i2c_device_id bd65b60_id[] = {
	{ "bd65b60", 0 },
	{},
};
MODULE_DEVICE_TABLE(i2c, bd65b60_id);

static const struct of_device_id bd65b60_leds_match[] = {
	{ .compatible = "rohm,bd65b60" },
	{},
};
MODULE_DEVICE_TABLE(of, bd65b60_leds_match);

static struct i2c_driver bd65b60_i2c_driver = {
	.driver = {
		.name = "bd65b60",
		.owner = THIS_MODULE,
		.of_match_table = bd65b60_leds_match,
		  },
	.probe_new = bd65b60_probe,
	.remove = bd65b60_remove,
	.id_table = bd65b60_id,
};

module_i2c_driver(bd65b60_i2c_driver);

MODULE_AUTHOR("Bogdan Ionescu <bogdan.ionescu.work+kernel@gmail.com>");
MODULE_DESCRIPTION("ROHM Semiconductor led driver for bd65b60");
MODULE_LICENSE("GPL");
