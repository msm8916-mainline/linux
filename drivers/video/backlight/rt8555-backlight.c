// SPDX-License-Identifier: GPL-2.0-only

#include <linux/err.h>
#include <linux/backlight.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define RT8555_MAX_BRIGHTNESS 1023

/* Pulled from datashet */
#define RT8555_DEFAULT_CURRENT_LIMIT 0x92
#define RT8555_DEFAULT_LED_DRIVER_HEADROOM 0x00

#define RT8555_REG_CFG0 0x00
#define RT8555_REG_CFG1 0x01
#define RT8555_REG_CFG8 0x08

#define RT8555_REG_ILED_CURRENT_LIMIT 0x02
#define RT8555_REG_ILED1_LSB 0x04
#define RT8555_REG_ILED1_MSB 0x05

#define RT8555_DIM_MODE_MASK BIT(0)
#define RT8555_BRIGHTNESS_SOURCE_MASK BIT(1)
#define RT8555_CHANGE_DUTY_MASK GENMASK(3, 2)
#define RT8555_LED_DRIVER_HEADROOM_MASK GENMASK(3, 2)
#define RT8555_MIX_26K_MASK BIT(7)
#define RT8555_EN10BIT_MASK BIT(7)
#define RT8555_LSB_MASK GENMASK(7, 0)

struct rt8555_priv {
	struct device *dev;
	struct regmap *regmap;
	struct backlight_device *bl;
	struct gpio_desc *enable;

	int change_duty;
	int driver_headroom;
	int current_limit;
	bool pwm_dim_mode;
};

static int rt8555_bl_enable(struct rt8555_priv *priv)
{
	int ret;

	if (!IS_ERR_OR_NULL(priv->enable)) {
		gpiod_set_value(priv->enable, 1);

		/* Wait for RT8555 to power on */
		usleep_range(10000, 20000);
	}

	/* Set 10 bit mode */
	ret = regmap_update_bits(priv->regmap, RT8555_REG_CFG1, RT8555_EN10BIT_MASK, 0xFF);
	if (ret)
		return ret;

	/* Depend on i2c for brightness */
	ret = regmap_update_bits(priv->regmap, RT8555_REG_CFG0,
				 RT8555_BRIGHTNESS_SOURCE_MASK, 0xFF);
	if (ret)
		return ret;

	/* Whether we use PWM or mixed mode for dimming */
	ret = regmap_update_bits(priv->regmap, RT8555_REG_CFG0, RT8555_DIM_MODE_MASK,
				 priv->pwm_dim_mode ? 0x00 : 0xFF);
	if (ret)
		return ret;

	/* "Change Duty" for Mixed Mode */
	ret = regmap_update_bits(priv->regmap, RT8555_REG_CFG0, RT8555_CHANGE_DUTY_MASK,
				 priv->change_duty << 2);
	if (ret)
		return ret;

	/* Mixed mode frequency source, use fixed 26khz clock instead of PWM pin */
	ret = regmap_update_bits(priv->regmap, RT8555_REG_CFG0, RT8555_MIX_26K_MASK, 0xFF);
	if (ret)
		return ret;

	ret = regmap_update_bits(priv->regmap, RT8555_REG_CFG8, RT8555_LED_DRIVER_HEADROOM_MASK,
				 priv->driver_headroom << 2);
	if (ret)
		return ret;

	ret = regmap_write(priv->regmap, RT8555_REG_ILED_CURRENT_LIMIT, priv->current_limit);
	if (ret)
		return ret;

	return ret;
}

static int rt8555_bl_is_disabled(struct rt8555_priv *priv)
{
	if (!IS_ERR_OR_NULL(priv->enable))
		return gpiod_get_value(priv->enable) == 0;
	else
		return 0;
}

static int rt8555_bl_update_status(struct backlight_device *bl_dev)
{
	struct rt8555_priv *priv = bl_get_data(bl_dev);
	unsigned int brightness = backlight_get_brightness(bl_dev);
	int ret;

	/* Enable the IC (if disabled) before setting the brightness */
	if (brightness && !IS_ERR_OR_NULL(priv->enable)) {
		if (rt8555_bl_is_disabled(priv)) {
			ret = rt8555_bl_enable(priv);
			if (ret) {
				dev_err(priv->dev, "rt8555_bl_enable failed with error %d", ret);
				return ret;
			}
		}
	}

	ret = regmap_bulk_write(priv->regmap, RT8555_REG_ILED1_LSB, &brightness, 2);

	/* Disable the IC after setting it to 0 */
	if (brightness == 0)
		if (!IS_ERR_OR_NULL(priv->enable))
			gpiod_set_value(priv->enable, 0);

	return 0;
}

static int rt8555_bl_get_brightness(struct backlight_device *bl_dev)
{
	struct rt8555_priv *priv = bl_get_data(bl_dev);
	int ret;
	int brightness;

	/*
	 * If the RT8555 is disabled, there's no reason to turn it on just to read
	 * it back
	 */
	if (rt8555_bl_is_disabled(priv))
		return 0;

	ret = regmap_bulk_read(priv->regmap, RT8555_REG_ILED1_LSB, &brightness, 2);
	if (ret)
		return ret;

	return brightness;
}

static const struct backlight_ops rt8555_bl_ops = {
	.options = BL_CORE_SUSPENDRESUME,
	.update_status = rt8555_bl_update_status,
	.get_brightness = rt8555_bl_get_brightness,
};

static const struct regmap_config rt8555_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int rt8555_bl_probe(struct i2c_client *client)
{
	struct rt8555_priv *priv;
	struct backlight_properties bl_props = {
		.type = BACKLIGHT_RAW, .scale = BACKLIGHT_SCALE_LINEAR
	};
	int ret;
	u32 brightness;

	priv = devm_kzalloc(&client->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = &client->dev;

	priv->enable = devm_gpiod_get_optional(&client->dev, "enable", GPIOD_OUT_HIGH);

	priv->regmap = devm_regmap_init_i2c(client, &rt8555_regmap_config);
	if (!priv->regmap) {
		dev_err(&client->dev, "Failed to init regmap\n");
		return -ENODEV;
	}

	ret = device_property_read_u32(&client->dev, "max-brightness", &brightness);
	if (ret)
		brightness = RT8555_MAX_BRIGHTNESS;

	bl_props.max_brightness = min_t(u32, brightness, RT8555_MAX_BRIGHTNESS);

	ret = device_property_read_u32(&client->dev, "default-brightness", &brightness);
	if (ret)
		brightness = bl_props.max_brightness;

	bl_props.brightness = min_t(u32, brightness, bl_props.max_brightness);

	priv->pwm_dim_mode = device_property_read_bool(&client->dev, "use-pwm-dimming-mode");

	ret = device_property_read_u32(&client->dev, "change-duty", &priv->change_duty);
	if (ret)
		priv->change_duty = 3;

	/* i2c register is only 2 bits, clamp value */
	priv->change_duty = clamp(priv->change_duty, 0, 3);

	ret = device_property_read_u32(&client->dev, "driver-headroom", &priv->driver_headroom);
	if (ret)
		priv->driver_headroom = RT8555_DEFAULT_LED_DRIVER_HEADROOM;

	priv->driver_headroom = clamp(priv->driver_headroom, 0, 3);

	ret = device_property_read_u32(&client->dev, "current-limit", &priv->current_limit);
	if (ret)
		priv->current_limit = RT8555_DEFAULT_CURRENT_LIMIT;

	priv->bl = devm_backlight_device_register(&client->dev, client->name, &client->dev, priv,
						  &rt8555_bl_ops, &bl_props);
	if (IS_ERR(priv->bl)) {
		dev_err(&client->dev, "Failed to register backlight\n");
		return PTR_ERR(priv->bl);
	}

	ret = rt8555_bl_enable(priv);
	if (ret) {
		dev_err(priv->dev, "rt8555_bl_enable failed with error %d",
			ret);
		return ret;
	}

	backlight_update_status(priv->bl);
	i2c_set_clientdata(client, priv);

	return 0;
}

static void rt8555_bl_remove(struct i2c_client *client)
{
	struct rt8555_priv *priv = i2c_get_clientdata(client);
	struct backlight_device *bl_dev = priv->bl;

	bl_dev->props.brightness = 0;
	backlight_update_status(priv->bl);

	if (!IS_ERR_OR_NULL(priv->enable))
		gpiod_set_value(priv->enable, 0);
}

static const struct of_device_id __maybe_unused rt8555_bl_of_match[] = {
	{ .compatible = "richtek,rt8555-backlight", },
	{}
};
MODULE_DEVICE_TABLE(of, rt8555_bl_of_match);

static struct i2c_driver rt8555_bl_driver = {
	.driver = {
		.name = "rt8555-backlight",
		.of_match_table = rt8555_bl_of_match
	},
	.probe_new = rt8555_bl_probe,
	.shutdown = rt8555_bl_remove
};
module_i2c_driver(rt8555_bl_driver);
MODULE_AUTHOR("Michael Abood <person4265@gmail.com>");
MODULE_LICENSE("GPL");
