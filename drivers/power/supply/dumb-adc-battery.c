// SPDX-License-Identifier: GPL-2.0
/*
 * heavily based on drivers/power/supply/ingenic-battery.c
 */

#include <linux/iio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/property.h>

struct dumb_adc_battery {
	struct device *dev;
	struct iio_channel *channel;
	struct power_supply_desc desc;
	struct power_supply *battery;
	struct power_supply_battery_info info;
};

static int dumb_adc_battery_get_property(struct power_supply *psy,
					 enum power_supply_property psp,
					 union power_supply_propval *val)
{
	struct dumb_adc_battery *bat = power_supply_get_drvdata(psy);
	struct power_supply_battery_info *info = &bat->info;
	int ret;
	int very_raw_val;
	int raret;
	int before_multiply;

	switch (psp) {
	case POWER_SUPPLY_PROP_HEALTH:
		ret = iio_read_channel_processed(bat->channel, &val->intval);
		if (val->intval < info->voltage_min_design_uv)
			val->intval = POWER_SUPPLY_HEALTH_DEAD;
		else if (val->intval > info->voltage_max_design_uv)
			val->intval = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
		else
			val->intval = POWER_SUPPLY_HEALTH_GOOD;
		return ret;
	case POWER_SUPPLY_PROP_CAPACITY:
		// TODO: Use OCV curves from dts if available
		ret = iio_read_channel_processed(bat->channel, &val->intval);
		if (val->intval < info->voltage_min_design_uv)
			val->intval = 0;
		else if (val->intval > info->voltage_max_design_uv)
			val->intval = 100;
		else
			val->intval = (val->intval - info->voltage_min_design_uv)
				/ ((info->voltage_max_design_uv - info->voltage_min_design_uv) / 100);
		return ret;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		raret = iio_read_channel_raw(bat->channel, &very_raw_val);
		ret = iio_read_channel_processed(bat->channel, &val->intval);
		before_multiply = val->intval;
		return ret;
	case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
		val->intval = info->voltage_min_design_uv;
		return 0;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN:
		val->intval = info->voltage_max_design_uv;
		return 0;
	default:
		return -EINVAL;
	};
}

static enum power_supply_property dumb_adc_battery_properties[] = {
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN,
};

static int dumb_adc_battery_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dumb_adc_battery *bat;
	struct power_supply_config psy_cfg = {};
	struct power_supply_desc *desc;
	int ret;

	bat = devm_kzalloc(dev, sizeof(*bat), GFP_KERNEL);
	if (!bat)
		return -ENOMEM;

	bat->dev = dev;
	bat->channel = devm_iio_channel_get(dev, "battery");
	if (IS_ERR(bat->channel))
		return PTR_ERR(bat->channel);

	desc = &bat->desc;
	desc->name = "dumb-battery";
	desc->type = POWER_SUPPLY_TYPE_BATTERY;
	desc->properties = dumb_adc_battery_properties;
	desc->num_properties = ARRAY_SIZE(dumb_adc_battery_properties);
	desc->get_property = dumb_adc_battery_get_property;
	psy_cfg.drv_data = bat;
	psy_cfg.of_node = dev->of_node;

	bat->battery = devm_power_supply_register(dev, desc, &psy_cfg);
	if (IS_ERR(bat->battery)) {
		dev_err(dev, "Unable to register battery\n");
		return PTR_ERR(bat->battery);
	}

	ret = power_supply_get_battery_info(bat->battery, &bat->info);
	if (ret) {
		dev_err(dev, "Unable to get battery info: %d\n", ret);
		return ret;
	}
	if (bat->info.voltage_min_design_uv < 0) {
		dev_err(dev, "Unable to get voltage min design\n");
		return bat->info.voltage_min_design_uv;
	}
	if (bat->info.voltage_max_design_uv < 0) {
		dev_err(dev, "Unable to get voltage max design\n");
		return bat->info.voltage_max_design_uv;
	}

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id dumb_adc_battery_of_match[] = {
	{ .compatible = "dumb-adc-battery", },
	{ },
};
MODULE_DEVICE_TABLE(of, dumb_adc_battery_of_match);
#endif

static struct platform_driver dumb_adc_battery_driver = {
	.driver = {
		.name = "dumb-adc-battery",
		.of_match_table = of_match_ptr(dumb_adc_battery_of_match),
	},
	.probe = dumb_adc_battery_probe,
};
module_platform_driver(dumb_adc_battery_driver);

MODULE_DESCRIPTION("Battery driver for batteries connected to IIO ADC");
MODULE_AUTHOR("Nikita Travkin <nikitos.tr@gmail.com>");
MODULE_LICENSE("GPL");
