/*
 * Battery charger driver for RT5033
 *
 * Copyright (C) 2014 Samsung Electronics, Co., Ltd.
 * Author: Beomho Seo <beomho.seo@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published bythe Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/mfd/rt5033-private.h>
#include <linux/mfd/rt5033.h>

static int rt5033_get_charger_state(struct rt5033_charger *charger)
{
	struct regmap *regmap = charger->rt5033->regmap;
	int state = POWER_SUPPLY_STATUS_UNKNOWN;
	u32 reg_data;

	if (!regmap)
		return state;

	regmap_read(regmap, RT5033_REG_CHG_STAT, &reg_data);

	switch (reg_data & RT5033_CHG_STAT_MASK) {
	case RT5033_CHG_STAT_DISCHARGING:
		state = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	case RT5033_CHG_STAT_CHARGING:
		state = POWER_SUPPLY_STATUS_CHARGING;
		break;
	case RT5033_CHG_STAT_FULL:
		state = POWER_SUPPLY_STATUS_FULL;
		break;
	case RT5033_CHG_STAT_NOT_CHARGING:
		state = POWER_SUPPLY_STATUS_NOT_CHARGING;
		break;
	}

	return state;
}

static int rt5033_get_charger_type(struct rt5033_charger *charger)
{
	struct regmap *regmap = charger->rt5033->regmap;
	int state = POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
	u32 reg_data;

	regmap_read(regmap, RT5033_REG_CHG_STAT, &reg_data);

	switch (reg_data & RT5033_CHG_STAT_TYPE_MASK) {
	case RT5033_CHG_STAT_TYPE_FAST:
		state = POWER_SUPPLY_CHARGE_TYPE_FAST;
		break;
	case RT5033_CHG_STAT_TYPE_PRE:
		state = POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
		break;
	}

	return state;
}

static int rt5033_get_charger_current(struct rt5033_charger *charger,
		enum power_supply_property psp)
{
	struct regmap *regmap = charger->rt5033->regmap;
	unsigned int state, reg_data, data;

	if (psp == POWER_SUPPLY_PROP_CURRENT_MAX)
		return RT5033_CHG_MAX_CURRENT;

	regmap_read(regmap, RT5033_REG_CHG_CTRL5, &reg_data);

	state = (reg_data >> RT5033_CHGCTRL5_ICHG_SHIFT) & 0xf;

	if (state > RT5033_CHG_MAX_CURRENT)
		state = RT5033_CHG_MAX_CURRENT;

	data = state * 100 + 700;

	return data;
}

static int rt5033_get_charge_voltage(struct rt5033_charger *charger,
		enum power_supply_property psp)
{
	struct regmap *regmap = charger->rt5033->regmap;
	unsigned int state, reg_data, data;

	if (psp == POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX)
		return RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MAX;

	regmap_read(regmap, RT5033_REG_CHG_CTRL2, &reg_data);

	state = reg_data >> 2;

	data = RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MIN +
		RT5033_CHARGER_CONST_VOLTAGE_STEP_NUM * state;

	if (data > RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MAX)
		data = RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MAX;

	return data;
}

static inline int rt5033_init_const_charge(struct rt5033_charger *psy)
{
	struct rt5033_charger_data *chg = psy->chg;
	unsigned val;
	int ret;
	u8 reg_data;

	/* Set Constant voltage mode */
	if (chg->const_uvolt < RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MIN ||
		chg->const_uvolt > RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MAX)
		return -EINVAL;

	if (chg->const_uvolt == RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MIN)
		reg_data = 0x0;
	else if (chg->const_uvolt == RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MAX)
		reg_data = 0xfc;
	else {
		val = chg->const_uvolt;
		val -= RT5033_CHARGER_CONST_VOLTAGE_LIMIT_MIN;
		val /= RT5033_CHARGER_CONST_VOLTAGE_STEP_NUM;
		reg_data = val;
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL2,
			RT5033_CHGCTRL2_CV_MASK, reg_data << 2);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	/* Set end of charge current */
	if (chg->eoc_uamp < RT5033_CHARGER_EOC_MIN ||
		chg->eoc_uamp > RT5033_CHARGER_EOC_MAX)
		return -EINVAL;

	if (chg->eoc_uamp == RT5033_CHARGER_EOC_MIN)
		reg_data = 0x1;
	else if (chg->eoc_uamp == RT5033_CHARGER_EOC_MAX)
		reg_data = 0x7;
	else {
		val = chg->eoc_uamp;
		if (val < RT5033_CHARGER_EOC_REF) {
			val -= RT5033_CHARGER_EOC_MIN;
			val /= RT5033_CHARGER_EOC_STEP_NUM1;
			reg_data = 0x01 + val;
		} else if (val > RT5033_CHARGER_EOC_REF) {
			val -= RT5033_CHARGER_EOC_REF;
			val /= RT5033_CHARGER_EOC_STEP_NUM2;
			reg_data = 0x04 + val;
		} else {
			reg_data = 0x04;
		}
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL4,
			RT5033_CHGCTRL4_EOC_MASK, reg_data);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	return 0;
}

static inline int rt5033_init_fast_charge(struct rt5033_charger *psy)
{
	struct rt5033_charger_data *chg = psy->chg;
	int ret;
	u8 reg_data;

	/* Set limit input current */
	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL1,
			RT5033_AICR_MODE_MASK, RT5033_AICR_2000_MODE);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	/* Set internal timer */
	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL3,
		RT5033_CHGCTRL3_TIMER_MASK | RT5033_CHGCTRL3_TIMER_EN_MASK,
		RT5033_FAST_CHARGE_TIMER4 | RT5033_INT_TIMER_ENABLE);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	/* Set fast-charge mode Carging current */
	if (chg->fast_uamp < RT5033_CHARGER_FAST_CURRENT_MIN ||
			chg->fast_uamp > RT5033_CHARGER_FAST_CURRENT_MAX)
		return -EINVAL;

	if (chg->fast_uamp == RT5033_CHARGER_FAST_CURRENT_MIN)
		reg_data = 0x0;
	else if (chg->fast_uamp == RT5033_CHARGER_FAST_CURRENT_MAX)
		reg_data = 0xd0;
	else {
		unsigned int val = chg->fast_uamp;

		val -= RT5033_CHARGER_FAST_CURRENT_MIN;
		val /= RT5033_CHARGER_FAST_CURRENT_STEP_NUM;
		reg_data = 0x10 + val;
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL5,
			RT5033_CHGCTRL5_ICHG_MASK, reg_data);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	return 0;
}

static inline int rt5033_init_pre_charge(struct rt5033_charger *psy)
{
	struct rt5033_charger_data *chg = psy->chg;
	int ret;
	u8 reg_data;

	/* Set pre-charge threshold voltage */
	if (chg->pre_uvolt < RT5033_CHARGER_PRE_THRESHOLD_LIMIT_MIN ||
		chg->pre_uvolt > RT5033_CHARGER_PRE_THRESHOLD_LIMIT_MAX)
		return -EINVAL;

	if (chg->pre_uvolt == RT5033_CHARGER_PRE_THRESHOLD_LIMIT_MIN)
		reg_data = 0x00;
	else if (chg->pre_uvolt == RT5033_CHARGER_PRE_THRESHOLD_LIMIT_MAX)
		reg_data = 0x0f;
	else {
		unsigned int val = chg->pre_uvolt;

		val -= RT5033_CHARGER_PRE_THRESHOLD_LIMIT_MIN;
		val /= RT5033_CHARGER_PRE_THRESHOLD_STEP_NUM;
		reg_data = 0x00 + val;
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL5,
			RT5033_CHGCTRL5_VPREC_MASK, reg_data);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	/* Set pre-charge mode charging current */
	if (chg->pre_uamp < RT5033_CHARGER_PRE_CURRENT_LIMIT_MIN ||
		chg->pre_uamp > RT5033_CHARGER_PRE_CURRENT_LIMIT_MAX)
		return -EINVAL;

	if (chg->pre_uamp == RT5033_CHARGER_PRE_CURRENT_LIMIT_MIN)
		reg_data = 0x00;
	else if (chg->pre_uamp == RT5033_CHARGER_PRE_CURRENT_LIMIT_MAX)
		reg_data = 0x18;
	else {
		unsigned int val = chg->pre_uamp;

		val -= RT5033_CHARGER_PRE_CURRENT_LIMIT_MIN;
		val /= RT5033_CHARGER_PRE_CURRENT_STEP_NUM;
		reg_data = 0x08 + val;
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL4,
			RT5033_CHGCTRL4_IPREC_MASK, reg_data);
	if (ret) {
		dev_err(psy->dev, "Failed regmap update\n");
		return -EINVAL;
	}

	return 0;
}

static int rt5033_charger_reg_init(struct rt5033_charger *psy)
{
	int ret = 0;

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL1,
			RT5033_CHGCTRL1_MODE_MASK, RT5033_CHARGER_MODE);
	if (ret) {
		dev_err(psy->dev, "Failed to update charger mode.\n");
		return -EINVAL;
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_RT_CTRL1,
			RT5033_RT_CTRL1_UUG_MASK, RT5033_CHARGER_UUG_ENABLE);
	if (ret) {
		dev_err(psy->dev, "Failed to update rt ctrl register.\n");
		return -EINVAL;
	}

	ret = rt5033_init_pre_charge(psy);
	if (ret)
		return ret;

	ret = rt5033_init_fast_charge(psy);
	if (ret)
		return ret;

	ret = rt5033_init_const_charge(psy);
	if (ret)
		return ret;

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL3,
			RT5033_CHGCTRL3_CFO_EN_MASK, RT5033_CFO_ENABLE);
	if (ret) {
		dev_err(psy->dev, "Failed to set enable.\n");
		return -EINVAL;
	}

	ret = regmap_update_bits(psy->rt5033->regmap, RT5033_REG_CHG_CTRL1,
			RT5033_RT_HZ_MASK, RT5033_CHARGER_HZ_DISABLE);
	if (ret) {
		dev_err(psy->dev, "Failed to set high impedance mode.\n");
		return -EINVAL;
	}

	return 0;
}

static enum power_supply_property rt5033_charger_props[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE,
	POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_MANUFACTURER,
};

static int rt5033_charger_get_property(struct power_supply *psy,
			enum power_supply_property psp,
			union power_supply_propval *val)
{
	struct rt5033_charger *charger = container_of(psy,
			struct rt5033_charger, psy);
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = rt5033_get_charger_state(charger);
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = rt5033_get_charger_type(charger);
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		val->intval = rt5033_get_charger_current(charger, psp);
		break;
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE:
	case POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX:
		val->intval = rt5033_get_charge_voltage(charger, psp);
		break;
	case POWER_SUPPLY_PROP_MODEL_NAME:
		val->strval = RT5033_CHARGER_MODEL;
		break;
	case POWER_SUPPLY_PROP_MANUFACTURER:
		val->strval = RT5033_MANUFACTURER;
		break;
	default:
		return -EINVAL;
	}

	return ret;
}

static struct rt5033_charger_data *rt5033_charger_dt_init(
						struct platform_device *pdev)
{
	struct rt5033_charger_data *chg;
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (!np) {
		dev_err(&pdev->dev, "No charger of_node\n");
		return ERR_PTR(-EINVAL);
	}

	chg = devm_kzalloc(&pdev->dev, sizeof(*chg), GFP_KERNEL);
	if (!chg)
		return ERR_PTR(-ENOMEM);

	ret = of_property_read_u32(np, "richtek,pre-uamp", &chg->pre_uamp);
	if (ret)
		return ERR_PTR(ret);

	ret = of_property_read_u32(np, "richtek,pre-threshold-uvolt",
			&chg->pre_uvolt);
	if (ret)
		return ERR_PTR(ret);

	/*
	 * Charging current is decided by external sensing register and
	 * regulated voltage. In this driver, external sensing regster value
	 * is 10 mili ohm
	 */
	ret = of_property_read_u32(np, "richtek,fast-uamp", &chg->fast_uamp);
	if (ret)
		return ERR_PTR(ret);

	ret = of_property_read_u32(np, "richtek,const-uvolt",
			&chg->const_uvolt);
	if (ret)
		return ERR_PTR(ret);

	ret = of_property_read_u32(np, "richtek,eoc-uamp", &chg->eoc_uamp);
	if (ret)
		return ERR_PTR(ret);

	return chg;
}

static int rt5033_charger_probe(struct platform_device *pdev)
{
	struct rt5033_charger *charger;
	struct rt5033_dev *rt5033 = dev_get_drvdata(pdev->dev.parent);
	int ret;

	charger = devm_kzalloc(&pdev->dev, sizeof(*charger), GFP_KERNEL);
	if (!charger)
		return -ENOMEM;

	platform_set_drvdata(pdev, charger);
	charger->dev = &pdev->dev;
	charger->rt5033 = rt5033;

	charger->chg = rt5033_charger_dt_init(pdev);
	if (IS_ERR_OR_NULL(charger->chg))
		return -ENODEV;

	ret = rt5033_charger_reg_init(charger);
	if (ret)
		return ret;

	charger->psy.name = "rt5033-charger",
	charger->psy.type = POWER_SUPPLY_TYPE_BATTERY,
	charger->psy.properties = rt5033_charger_props,
	charger->psy.num_properties = ARRAY_SIZE(rt5033_charger_props),
	charger->psy.get_property = rt5033_charger_get_property,

	ret = power_supply_register(&pdev->dev, &charger->psy);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register power supply\n");
		return ret;
	}

	return 0;
}

static int rt5033_charger_remove(struct platform_device *pdev)
{
	struct rt5033_charger *charger = platform_get_drvdata(pdev);

	power_supply_unregister(&charger->psy);

	return 0;
}

static const struct platform_device_id rt5033_charger_id[] = {
	{ "rt5033-charger", },
	{ }
};
MODULE_DEVICE_TABLE(platform, rt5033_charger_id);

static struct platform_driver rt5033_charger_driver = {
	.driver = {
		.name = "rt5033-charger",
	},
	.probe = rt5033_charger_probe,
	.remove = rt5033_charger_remove,
	.id_table = rt5033_charger_id,
};
module_platform_driver(rt5033_charger_driver);

MODULE_DESCRIPTION("Richtek RT5033 charger driver");
MODULE_AUTHOR("Beomho Seo <beomho.seo@samsung.com>");
MODULE_LICENSE("GPL");
