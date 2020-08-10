// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/completion.h>
#include <linux/i2c.h>
#include <linux/iio/consumer.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>

/* Charger Registers */
#define CFG_BATT_CHG_REG		0x00
#define CHG_ITERM_MASK			GENMASK(2, 0)
#define RECHG_MV_MASK			GENMASK(6, 5)
#define RECHG_MV_SHIFT			5

#define CFG_BATT_CHG_ICL_REG		0x05
#define AC_INPUT_ICL_PIN_BIT		BIT(7)
#define AC_INPUT_PIN_HIGH_BIT		BIT(6)

#define CFG_GLITCH_FLT_REG		0x06
#define AICL_ENABLED_BIT		BIT(0)
#define INPUT_UV_GLITCH_FLT_20MS_BIT	BIT(7)

#define CFG_CHG_MISC_REG		0x7
#define CHG_EN_BY_PIN_BIT		BIT(7)
#define CHG_EN_ACTIVE_LOW_BIT		BIT(6)
#define PRE_TO_FAST_REQ_CMD_BIT		BIT(5)
#define CHG_CURR_TERM_DIS_BIT		BIT(3)
#define CFG_AUTO_RECHG_DIS_BIT		BIT(2)
#define CFG_CHG_INHIBIT_EN_BIT		BIT(0)

#define CFG_STAT_CTRL_REG		0x09
#define CHG_STAT_IRQ_ONLY_BIT		BIT(4)
#define CHG_STAT_ACTIVE_HIGH_BIT	BIT(1)
#define CHG_STAT_DISABLE_BIT		BIT(0)

#define CFG_SFY_TIMER_CTRL_REG		0x0A
#define SAFETY_TIME_EN_BIT		BIT(4)
#define SAFETY_TIME_DISABLE_BIT		BIT(5)
#define SAFETY_TIME_MINUTES_SHIFT	2
#define SAFETY_TIME_MINUTES_MASK	GENMASK(3, 2)

#define CFG_FG_BATT_CTRL_REG		0x0E

#define IRQ_CFG_REG			0x0F
#define IRQ_INTERNAL_TEMPERATURE_BIT	BIT(0)
#define IRQ_DCIN_UV_BIT			BIT(2)
#define IRQ_BAT_HOT_COLD_SOFT_BIT	BIT(6)
#define IRQ_HOT_COLD_HARD_BIT		BIT(7)

#define IRQ2_CFG_REG			0x10
#define IRQ2_VBAT_LOW_BIT		BIT(0)
#define IRQ2_BATT_MISSING_BIT		BIT(1)
#define IRQ2_POWER_OK_BIT		BIT(2)
#define IRQ2_CHG_PHASE_CHANGE_BIT	BIT(4)
#define IRQ2_CHG_ERR_BIT		BIT(6)
#define IRQ2_SAFETY_TIMER_BIT		BIT(7)

#define IRQ3_CFG_REG			0x11
#define SOC_FULL_BIT			BIT(0)
#define SOC_EMPTY_BIT			BIT(1)
#define SOC_MAX_BIT			BIT(2)
#define SOC_MIN_BIT			BIT(3)
#define SOC_CHANGE_BIT			BIT(4)
#define FG_ACCESS_OK_BIT		BIT(6)

#define BATT_CHG_FLT_VTG_REG		0x15
#define VFLOAT_MASK			GENMASK(6, 0)

/* Command Registers */
#define CMD_I2C_REG			0x40
#define ALLOW_VOLATILE_BIT		BIT(6)
#define FG_ACCESS_ENABLED_BIT		BIT(5)
#define FG_RESET_BIT			BIT(4)
#define CYCLE_STRETCH_CLEAR_BIT		BIT(3)

#define CMD_CHG_REG			0x42
#define CMD_CHG_EN			BIT(1)
#define CMD_OTG_EN_BIT			BIT(0)

/* Status Registers */
#define STATUS_3_REG			0x4B
#define CHG_HOLD_OFF_BIT		BIT(3)
#define CHG_TYPE_MASK			GENMASK(2, 1)
#define CHG_TYPE_SHIFT			1
#define BATT_NOT_CHG_VAL		0x0
#define BATT_PRE_CHG_VAL		0x1
#define BATT_FAST_CHG_VAL		0x2
#define BATT_TAPER_CHG_VAL		0x3

#define REVISION_CTRL_REG		0x4F
#define DEVICE_REV_MASK			GENMASK(3, 0)

/* IRQ Status Registers */
#define IRQ_REG				0x50

#define IRQ_A_REG			0x50
#define IRQ_A_HOT_HARD_BIT		BIT(6)
#define IRQ_A_COLD_HARD_BIT		BIT(4)
#define IRQ_A_HOT_SOFT_BIT		BIT(2)
#define IRQ_A_COLD_SOFT_BIT		BIT(0)

#define IRQ_C_REG			0x52
#define IRQ_C_CHG_TERM_BIT		BIT(0)

#define IRQ_E_REG			0x54
#define IRQ_E_USBIN_UV_BIT		BIT(0)
#define IRQ_E_INHIBIT_BIT		BIT(6)

#define IRQ_F_REG			0x54
#define IRQ_F_OTG_FAIL_BIT		BIT(4)
#define IRQ_F_OTG_OC_BIT		BIT(6)

#define IRQ_G_REG			0x56
#define IRQ_G_SOC_CHANGE_BIT		BIT(0)

#define IRQ_H_REG			0x57
#define IRQ_H_EMPTY_SOC_BIT		BIT(4)

#define IRQ_I_REG			0x58
#define IRQ_I_FG_ACCESS_ALLOWED_BIT	BIT(0)

/* FG registers - IRQ config register */
#define SOC_DELTA_REG			0x28
#define VTG_MIN_REG			0x2B
#define SOC_MAX_REG			0x24
#define SOC_MIN_REG			0x25
#define VTG_EMPTY_REG			0x26

/* FG SHADOW registers */
#define SHDW_FG_ESR_ACTUAL		0x20
#define SHDW_FG_BATT_STATUS		0x60
#define SHDW_FG_MSYS_SOC		0x61
#define SHDW_FG_CAPACITY		0x62
#define SHDW_FG_VTG_NOW			0x69
#define SHDW_FG_CURR_NOW		0x6B
#define SHDW_FG_BATT_TEMP		0x6D

#define SHDN_CTRL_REG			0x1A
#define SHDN_CMD_USE_BIT		BIT(1)
#define SHDN_CMD_POLARITY_BIT		BIT(2)

#define VOLTAGE_PREDICTED_REG		0x80

#define CMD_IL_REG			0x41
#define USB_CTRL_MASK			GENMASK(1, 0)
#define USB_100_BIT			0x00
#define USB_500_BIT			0x01
#define USB_AC_BIT			0x11
#define SHDN_CMD_BIT			BIT(7)

/* Constants */
#define MAX_8_BITS			255
#define FG_RESET_THRESHOLD_MV		15
#define SMB1360_REV_1			0x01

#define MIN_FLOAT_MV			3460
#define MAX_FLOAT_MV			4730
#define VFLOAT_STEP_MV			10

#define MIN_RECHG_MV			50
#define MAX_RECHG_MV			300

#define SMB1360_FG_ACCESS_TIMEOUT_MS	15000

#define SMB1360_POWERON_DELAY_MS	2000
#define SMB1360_FG_RESET_DELAY_MS	1500

#define BATT_PROFILE_SELECT_MASK	GENMASK(3, 0)
#define BATT_PROFILEA_MASK		0x0
#define BATT_PROFILEB_MASK		0xF

#define BATT_ID_RESULT_BIT		GENMASK(6, 4)
#define BATT_ID_SHIFT			4

#define BATTERY_PROFILE_BIT		BIT(0)

enum {
	BATTERY_PROFILE_A,
	BATTERY_PROFILE_B,
	BATTERY_PROFILE_MAX,
};

enum {
	IRQ_A, IRQ_B, IRQ_C, IRQ_D, IRQ_E, IRQ_F, IRQ_G, IRQ_H, IRQ_I,
	IRQ_COUNT
};

static const u32 smb1360_irq_changed_mask[IRQ_COUNT] = {
	[IRQ_A] = IRQ_A_HOT_HARD_BIT | IRQ_A_COLD_HARD_BIT
		  | IRQ_A_HOT_SOFT_BIT | IRQ_A_COLD_SOFT_BIT,
	[IRQ_C] = IRQ_C_CHG_TERM_BIT,
	[IRQ_E] = IRQ_E_INHIBIT_BIT,
	[IRQ_G] = IRQ_G_SOC_CHANGE_BIT,
	[IRQ_H] = IRQ_H_EMPTY_SOC_BIT,
};

static const int chg_time[] = { 192, 384, 768, 1536, };

struct smb1360_otg_regulator {
	struct regulator_desc	rdesc;
	struct regulator_dev	*rdev;
};

struct smb1360_battery {
	struct device			*dev;
	struct regmap			*regmap;
	struct power_supply		*psy;
	struct smb1360_otg_regulator	otg_vreg;
	struct completion		fg_mem_access_granted;
	struct delayed_work		delayed_init_work;

	u32 irq_cfg_mask[3];
	u32 revision;
	u32 irqstat[IRQ_COUNT];
	u32 connected_rid;
	u32 profile_rid[BATTERY_PROFILE_MAX];

	int voltage_min_mv;
	int voltage_empty_mv;
	int iterm_ma;
	int vfloat_mv;
	int safety_time;
	int resume_delta_mv;
	int fg_reset_threshold_mv;

	bool recharge_disabled;
	bool chg_inhibit_disabled;
	bool iterm_disabled;
	bool shdn_after_pwroff;
	bool fg_reset_at_pon;
};

static enum power_supply_property smb1360_battery_props[] = {
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CURRENT_NOW,
	POWER_SUPPLY_PROP_TEMP,
};

static int smb1360_read_voltage(struct smb1360_battery *battery, u8 reg,
				int *voltage)
{
	__le16 val;
	int ret;

	ret = regmap_bulk_read(battery->regmap, reg, &val, sizeof(val));
	if (ret)
		return ret;

	*voltage = div_u64(le16_to_cpu(val) * 5000, 0x7FFF);
	return 0;
}

static int smb1360_get_prop_batt_status(struct smb1360_battery *battery,
					union power_supply_propval *val)
{
	int ret;
	u32 reg, chg_type;

	if (battery->irqstat[IRQ_C] & IRQ_C_CHG_TERM_BIT) {
		val->intval = POWER_SUPPLY_STATUS_FULL;
		return 0;
	}

	ret = regmap_read(battery->regmap, STATUS_3_REG, &reg);
	if (ret) {
		val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
		return ret;
	}

	if (reg & CHG_HOLD_OFF_BIT) {
		val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
		return 0;
	}

	chg_type = (reg & CHG_TYPE_MASK) >> CHG_TYPE_SHIFT;
	if (chg_type == BATT_NOT_CHG_VAL) {
		val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		return 0;
	}

	val->intval = POWER_SUPPLY_STATUS_CHARGING;
	return 0;
}

static int smb1360_get_prop_charge_type(struct smb1360_battery *battery,
					union power_supply_propval *val)
{
	int ret;
	u32 reg, chg_type;

	ret = regmap_read(battery->regmap, STATUS_3_REG, &reg);
	if (ret) {
		val->intval = POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
		return ret;
	}

	chg_type = (reg & CHG_TYPE_MASK) >> CHG_TYPE_SHIFT;
	switch (chg_type) {
	case BATT_NOT_CHG_VAL:
		val->intval = POWER_SUPPLY_CHARGE_TYPE_NONE;
		break;
	case BATT_FAST_CHG_VAL:
	case BATT_TAPER_CHG_VAL:
		val->intval = POWER_SUPPLY_CHARGE_TYPE_FAST;
		break;
	case BATT_PRE_CHG_VAL:
		val->intval = POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
		break;
	default:
		val->intval = POWER_SUPPLY_CHARGE_TYPE_NONE;
		break;
	}

	return 0;
}

static int smb1360_get_prop_batt_health(struct smb1360_battery *battery,
					union power_supply_propval *val)
{
	if (battery->irqstat[IRQ_A] & IRQ_A_HOT_HARD_BIT)
		val->intval = POWER_SUPPLY_HEALTH_OVERHEAT;
	else if (battery->irqstat[IRQ_A] & IRQ_A_COLD_HARD_BIT)
		val->intval = POWER_SUPPLY_HEALTH_COLD;
	else
		val->intval = POWER_SUPPLY_HEALTH_GOOD;

	return 0;
}

static int smb1360_get_prop_batt_capacity(struct smb1360_battery *battery,
					  union power_supply_propval *val)
{
	int ret, soc = 0;
	u32 reg;

	if (battery->irqstat[IRQ_H] & IRQ_H_EMPTY_SOC_BIT) {
		val->intval = 0;
		return 0;
	}

	ret = regmap_read(battery->regmap, SHDW_FG_MSYS_SOC, &reg);
	if (ret)
		return ret;

	soc = DIV_ROUND_CLOSEST((100 * reg), 255);
	val->intval = clamp(soc, 0, 100);

	return 0;
}

static int smb1360_get_prop_chg_full_design(struct smb1360_battery *battery,
					    union power_supply_propval *val)
{
	__le16 fcc_mah;
	int ret;

	ret = regmap_bulk_read(battery->regmap, SHDW_FG_CAPACITY, &fcc_mah, sizeof(fcc_mah));
	if (ret)
		return ret;

	val->intval = le16_to_cpu(fcc_mah) * 1000;
	return 0;
}

static int smb1360_get_prop_batt_temp(struct smb1360_battery *battery,
				      union power_supply_propval *val)
{
	__le16 temp;
	int ret;

	ret = regmap_bulk_read(battery->regmap, SHDW_FG_BATT_TEMP, &temp, sizeof(temp));
	if (ret)
		return ret;

	temp = div_u64(le16_to_cpu(temp) * 625, 10000UL); /* temperature in K */
	val->intval = (temp - 273) * 10; /* temperature in decideg */

	return 0;
}

static int smb1360_get_prop_voltage_now(struct smb1360_battery *battery,
					union power_supply_propval *val)
{
	return smb1360_read_voltage(battery, SHDW_FG_VTG_NOW, &val->intval);
}

static int smb1360_get_prop_current_now(struct smb1360_battery *battery,
					union power_supply_propval *val)
{
	__le16 temp;
	int ret;

	ret = regmap_bulk_read(battery->regmap, SHDW_FG_CURR_NOW, &temp, sizeof(temp));
	if (ret)
		return ret;

	val->intval = div_s64(((s16) le16_to_cpu(temp)) * 2500, 0x7FFF) * 1000;

	return 0;
}

static int smb1360_battery_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	struct smb1360_battery *battery = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_HEALTH:
		return smb1360_get_prop_batt_health(battery, val);
	case POWER_SUPPLY_PROP_STATUS:
		return smb1360_get_prop_batt_status(battery, val);
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		return smb1360_get_prop_charge_type(battery, val);
	case POWER_SUPPLY_PROP_CAPACITY:
		return smb1360_get_prop_batt_capacity(battery, val);
	case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN:
		return smb1360_get_prop_chg_full_design(battery, val);
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		return smb1360_get_prop_voltage_now(battery, val);
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		return smb1360_get_prop_current_now(battery, val);
	case POWER_SUPPLY_PROP_TEMP:
		return smb1360_get_prop_batt_temp(battery, val);
	default:
		return -EINVAL;
	}

	return 0;
}

static irqreturn_t smb1360_bat_irq(int irq, void *data)
{
	struct smb1360_battery *battery = data;
	u32 irqstat;
	int ret, i;
	bool changed = false;

	for (i = IRQ_A; i < IRQ_COUNT; ++i) {
		ret = regmap_read(battery->regmap, IRQ_REG + i, &irqstat);
		if (ret < 0)
			return IRQ_NONE;

		if ((irqstat ^ battery->irqstat[i]) & smb1360_irq_changed_mask[i])
			changed = true;

		battery->irqstat[i] = irqstat;
	}

	if (battery->irqstat[IRQ_F] & (IRQ_F_OTG_FAIL_BIT | IRQ_F_OTG_OC_BIT)) {
		dev_warn(battery->dev, "otg error: %d\n", battery->irqstat[IRQ_F]);
		regulator_disable_regmap(battery->otg_vreg.rdev);
	}

	if (battery->irqstat[IRQ_I] & IRQ_I_FG_ACCESS_ALLOWED_BIT)
		complete_all(&battery->fg_mem_access_granted);

	if (changed)
		power_supply_changed(battery->psy);

	return IRQ_HANDLED;
}

static const struct regulator_ops smb1360_regulator_ops = {
	.is_enabled	= regulator_is_enabled_regmap,
	.enable		= regulator_enable_regmap,
	.disable	= regulator_disable_regmap,
};

static const struct regulator_desc smb1360_regulator_descriptor = {
	.name		= "smb1360_otg_vreg",
	.of_match	= "smb1360_otg_vreg",
	.ops		= &smb1360_regulator_ops,
	.type		= REGULATOR_VOLTAGE,
	.owner		= THIS_MODULE,
	.enable_reg	= CMD_CHG_REG,
	.enable_mask	= CMD_OTG_EN_BIT,
	.enable_val	= CMD_OTG_EN_BIT,
};

static const struct regulator_init_data smb1360_vbus_init_data = {
	.constraints = {
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
};

static int smb1360_register_vbus_regulator(struct smb1360_battery *battery)
{
	struct regulator_config cfg = { };
	int ret = 0;

	battery->otg_vreg.rdesc = smb1360_regulator_descriptor;

	cfg.dev = battery->dev;
	cfg.init_data = &smb1360_vbus_init_data;
	cfg.driver_data = battery;

	battery->otg_vreg.rdev = devm_regulator_register(battery->dev, &battery->otg_vreg.rdesc, &cfg);
	if (IS_ERR(battery->otg_vreg.rdev)) {
		ret = PTR_ERR(battery->otg_vreg.rdev);
		dev_err(battery->dev, "can't register regulator: %d\n", ret);
	}

	return ret;
}

static int smb1360_enable_fg_access(struct smb1360_battery *battery)
{
	int ret = 0;
	u32 reg;

	ret = regmap_read(battery->regmap, IRQ_I_REG, &reg);
	if (ret || reg & IRQ_I_FG_ACCESS_ALLOWED_BIT)
		return ret;

	/* request FG access */
	ret = regmap_update_bits(battery->regmap, CMD_I2C_REG,
				 FG_ACCESS_ENABLED_BIT, FG_ACCESS_ENABLED_BIT);
	if (ret)
		return ret;

	ret = wait_for_completion_timeout(
		&battery->fg_mem_access_granted,
		msecs_to_jiffies(SMB1360_FG_ACCESS_TIMEOUT_MS));

	if (ret == 0) {
		/* Clear the FG access bit if request failed */
		regmap_update_bits(battery->regmap, CMD_I2C_REG,
				   FG_ACCESS_ENABLED_BIT, 0);
		return -ETIMEDOUT;
	}

	return 0;
}

static int smb1360_disable_fg_access(struct smb1360_battery *battery)
{
	int ret;

	ret = regmap_update_bits(battery->regmap, CMD_I2C_REG,
				 FG_ACCESS_ENABLED_BIT, 0);
	if (ret)
		dev_err(battery->dev, "couldn't disable FG access: %d\n", ret);

	reinit_completion(&battery->fg_mem_access_granted);

	return ret;
}

static int smb1360_force_fg_reset(struct smb1360_battery *battery)
{
	int ret;

	ret = regmap_update_bits(battery->regmap, CMD_I2C_REG, FG_RESET_BIT,
				FG_RESET_BIT);
	if (ret) {
		dev_err(battery->dev, "couldn't reset FG: %d\n", ret);
		return ret;
	}

	msleep(SMB1360_FG_RESET_DELAY_MS);

	ret = regmap_update_bits(battery->regmap, CMD_I2C_REG, FG_RESET_BIT, 0);
	if (ret)
		dev_err(battery->dev, "couldn't un-reset FG: %d\n", ret);

	return ret;
}

static int smb1360_fg_reset(struct smb1360_battery *battery)
{
	int ret, temp, v_predicted, v_now;

	ret = smb1360_read_voltage(battery, VOLTAGE_PREDICTED_REG, &v_predicted);
	if (ret)
		return ret;
	ret = smb1360_read_voltage(battery, SHDW_FG_VTG_NOW, &v_now);
	if (ret)
		return ret;

	temp = abs(v_predicted - v_now);
	if (temp >= battery->fg_reset_threshold_mv) {
		/* delay for the FG access to settle */
		msleep(1500);

		ret = smb1360_force_fg_reset(battery);
		if (ret)
			return ret;
	}

	return 0;
}

static int smb1360_fg_config(struct smb1360_battery *battery)
{
	int ret, temp;
	u8 val = 0;

	if (battery->fg_reset_at_pon) {
		ret = smb1360_enable_fg_access(battery);
		if (ret)
			return ret;

		ret = smb1360_fg_reset(battery);
		if (ret)
			dev_err(battery->dev, "smb1360_fg_reset failed");

		smb1360_disable_fg_access(battery);
	}

	if (battery->revision == SMB1360_REV_1)
		return 0;

	if (battery->voltage_min_mv != -EINVAL) {
		temp = (battery->voltage_min_mv - 2500) * MAX_8_BITS;
		val = DIV_ROUND_UP(temp, 2500);
		ret = regmap_write(battery->regmap, VTG_MIN_REG, val);
		if (ret)
			return ret;
	}

	if (battery->voltage_empty_mv != -EINVAL) {
		temp = (battery->voltage_empty_mv - 2500) * MAX_8_BITS;
		val = DIV_ROUND_UP(temp, 2500);
		ret = regmap_write(battery->regmap, VTG_EMPTY_REG, val);
		if (ret)
			return ret;
	}

	return 0;
}

static int smb1360_float_voltage_set(struct smb1360_battery *battery, int val)
{
	u8 temp;

	if (battery->vfloat_mv == -EINVAL)
		return 0;

	if ((val < MIN_FLOAT_MV) || (val > MAX_FLOAT_MV))
		return -EINVAL;

	temp = (val - MIN_FLOAT_MV) / VFLOAT_STEP_MV;

	return regmap_update_bits(battery->regmap,
				  BATT_CHG_FLT_VTG_REG, VFLOAT_MASK, temp);
}

static int smb1360_recharge_threshold_set(struct smb1360_battery *battery,
					  int val)
{
	u8 temp;

	if (battery->resume_delta_mv == -EINVAL)
		return 0;

	if ((val < MIN_RECHG_MV) || (val > MAX_RECHG_MV))
		return -EINVAL;

	temp = (val / 100) << RECHG_MV_SHIFT;

	return regmap_update_bits(battery->regmap,
				  CFG_BATT_CHG_REG, RECHG_MV_MASK, temp);

}

static int smb1360_iterm_set(struct smb1360_battery *battery)
{
	int ret;
	u8 val;

	if (battery->iterm_ma == -EINVAL && battery->iterm_disabled) {
		return regmap_update_bits(battery->regmap, CFG_CHG_MISC_REG,
					  CHG_CURR_TERM_DIS_BIT,
					  CHG_CURR_TERM_DIS_BIT);
	}

	if (battery->iterm_ma == -EINVAL)
		return 0;

	val = clamp(battery->iterm_ma, 25, 200);
	val = DIV_ROUND_UP(val, 25) - 1;

	ret = regmap_update_bits(battery->regmap, CFG_BATT_CHG_REG,
				 CHG_ITERM_MASK, val);
	if (ret)
		return ret;

	ret = regmap_update_bits(battery->regmap, CFG_CHG_MISC_REG,
				 CHG_CURR_TERM_DIS_BIT, 0);
	if (ret)
		return ret;

	return 0;
}

static int smb1360_safety_time_set(struct smb1360_battery *battery)
{
	int ret, i;
	u8 mask, val;

	if (battery->safety_time == -EINVAL)
		return 0;

	mask = SAFETY_TIME_DISABLE_BIT;
	val = SAFETY_TIME_DISABLE_BIT;

	if (battery->safety_time != 0) {
		mask = SAFETY_TIME_DISABLE_BIT | SAFETY_TIME_MINUTES_MASK;

		for (i = 0; i < ARRAY_SIZE(chg_time); i++) {
			if (battery->safety_time <= chg_time[i]) {
				val = i << SAFETY_TIME_MINUTES_SHIFT;
				break;
			}
		}
	}

	ret = regmap_update_bits(battery->regmap,
				 CFG_SFY_TIMER_CTRL_REG, mask, val);
	if (ret < 0) {
		dev_err(battery->dev, "couldn't update safety timer: %d\n", ret);
		return ret;
	}

	return 0;
}

static int smb1360_enable(struct smb1360_battery *battery, bool enable)
{
	int ret = 0;
	u32 val = 0;
	bool polarity;

	ret = regmap_read(battery->regmap, SHDN_CTRL_REG, &val);
	if (ret < 0) {
		dev_err(battery->dev, "couldn't read SHDN_CTRL_REG: %d\n", ret);
		return ret;
	}

	if (!(val & SHDN_CMD_USE_BIT))
		return 0;

	polarity = !!(val & SHDN_CMD_POLARITY_BIT);
	val = (polarity != enable) ? SHDN_CMD_BIT : 0;

	ret = regmap_update_bits(battery->regmap, CMD_IL_REG, SHDN_CMD_BIT, val);
	if (ret < 0)
		dev_err(battery->dev, "couldn't shutdown: %d\n", ret);

	return ret;
}

static inline int smb1360_poweroff(struct smb1360_battery *battery)
{
	return smb1360_enable(battery, false);
}

static inline int smb1360_poweron(struct smb1360_battery *battery)
{
	return smb1360_enable(battery, true);
}

static int smb1360_configure_irq(struct smb1360_battery *battery)
{
	int ret;

	/* enabling only interesting interrupts */
	ret = regmap_write(battery->regmap, IRQ_CFG_REG,
			   IRQ_INTERNAL_TEMPERATURE_BIT
			   | IRQ_DCIN_UV_BIT
			   | IRQ_BAT_HOT_COLD_SOFT_BIT
			   | IRQ_HOT_COLD_HARD_BIT);
	if (ret) {
		dev_err(battery->dev, "couldn't set irq1: %d\n", ret);
		return ret;
	}

	ret = regmap_write(battery->regmap, IRQ2_CFG_REG,
			   IRQ2_VBAT_LOW_BIT
			   | IRQ2_BATT_MISSING_BIT
			   | IRQ2_POWER_OK_BIT
			   | IRQ2_CHG_PHASE_CHANGE_BIT
			   | IRQ2_CHG_ERR_BIT
			   | IRQ2_SAFETY_TIMER_BIT);
	if (ret) {
		dev_err(battery->dev, "couldn't set irq2: %d\n", ret);
		return ret;
	}

	ret = regmap_write(battery->regmap, IRQ3_CFG_REG,
			   SOC_FULL_BIT
			   | SOC_EMPTY_BIT
			   | SOC_MAX_BIT
			   | SOC_MIN_BIT
			   | SOC_CHANGE_BIT
			   | FG_ACCESS_OK_BIT);
	if (ret)
		dev_err(battery->dev, "couldn't set irq3: %d\n", ret);

	return ret;
}

static int smb1360_hw_init(struct i2c_client *client)
{
	struct smb1360_battery *battery = i2c_get_clientdata(client);
	int ret;
	u8 val;

	ret = regmap_update_bits(battery->regmap, CMD_I2C_REG,
				 ALLOW_VOLATILE_BIT, ALLOW_VOLATILE_BIT);
	if (ret < 0) {
		dev_err(battery->dev, "couldn't configure volatile: %d\n", ret);
		return ret;
	}

	/* Bring SMB1360 out of shutdown, if it was enabled by default */
	ret = smb1360_poweron(battery);
	if (ret < 0) {
		dev_err(battery->dev, "smb1360 power on failed\n");
		return ret;
	}

	/* en chg by cmd reg, en chg by writing bit 1, en auto pre to fast */
	ret = regmap_update_bits(battery->regmap, CFG_CHG_MISC_REG,
				 CHG_EN_BY_PIN_BIT | CHG_EN_ACTIVE_LOW_BIT
				 | PRE_TO_FAST_REQ_CMD_BIT, 0);
	if (ret < 0)
		return ret;

	/* USB/AC pin settings */
	ret = regmap_update_bits(battery->regmap, CFG_BATT_CHG_ICL_REG,
				 AC_INPUT_ICL_PIN_BIT | AC_INPUT_PIN_HIGH_BIT,
				 AC_INPUT_PIN_HIGH_BIT);
	if (ret < 0)
		return ret;

	/* AICL enable and set input-uv glitch flt to 20ms */
	ret = regmap_update_bits(battery->regmap, CFG_GLITCH_FLT_REG,
				 AICL_ENABLED_BIT | INPUT_UV_GLITCH_FLT_20MS_BIT,
				 AICL_ENABLED_BIT | INPUT_UV_GLITCH_FLT_20MS_BIT);
	if (ret < 0)
		return ret;

	/* set the float voltage */
	ret = smb1360_float_voltage_set(battery, battery->vfloat_mv);
	if (ret < 0)
		return ret;

	/* set iterm */
	ret = smb1360_iterm_set(battery);
	if (ret < 0)
		return ret;

	ret = smb1360_safety_time_set(battery);
	if (ret < 0)
		return ret;

	/* configure resume threshold, auto recharge and charge inhibit */
	ret = smb1360_recharge_threshold_set(battery, battery->resume_delta_mv);
	if (ret)
		return ret;

	val = battery->recharge_disabled ? CFG_AUTO_RECHG_DIS_BIT : 0;
	ret = regmap_update_bits(battery->regmap, CFG_CHG_MISC_REG,
				 CFG_AUTO_RECHG_DIS_BIT, val);
	if (ret) {
		dev_err(battery->dev, "couldn't set rechg-cfg: %d\n", ret);
		return ret;
	}

	val = battery->chg_inhibit_disabled ? 0 : CFG_CHG_INHIBIT_EN_BIT;
	ret = regmap_update_bits(battery->regmap, CFG_CHG_MISC_REG,
				 CFG_CHG_INHIBIT_EN_BIT, val);
	if (ret) {
		dev_err(battery->dev, "couldn't set chg_inhibit: %d\n", ret);
		return ret;
	}

	/* interrupt enabling - active low */
	if (client->irq) {
		ret = regmap_update_bits(battery->regmap, CFG_STAT_CTRL_REG,
					 CHG_STAT_IRQ_ONLY_BIT
					 | CHG_STAT_ACTIVE_HIGH_BIT
					 | CHG_STAT_DISABLE_BIT,
					 CHG_STAT_IRQ_ONLY_BIT);
		if (ret < 0) {
			dev_err(battery->dev, "couldn't set irq: %d\n", ret);
			return ret;
		}

		ret = smb1360_configure_irq(battery);
	}

	return ret;
}

static int smb1360_check_batt_profile(struct smb1360_battery *battery)
{
	int ret, i, timeout;
	u8 loaded_profile, new_profile = 0, bid_mask;
	u32 val = 0;

	if (!battery->connected_rid)
		return 0;

	ret = regmap_read(battery->regmap, SHDW_FG_BATT_STATUS, &val);
	if (ret)
		return ret;

	loaded_profile = !!(val & BATTERY_PROFILE_BIT) ?
		BATTERY_PROFILE_B : BATTERY_PROFILE_A;

	for (i = 0; i < BATTERY_PROFILE_MAX; i++) {
		if (abs(battery->profile_rid[i] - battery->connected_rid) <
				(div_u64(battery->connected_rid, 10)))
			break;
	}

	if (i == BATTERY_PROFILE_MAX || i == loaded_profile)
		return 0;

	new_profile = (loaded_profile == BATTERY_PROFILE_A) ?
		BATTERY_PROFILE_B : BATTERY_PROFILE_A;
	bid_mask = (new_profile == BATTERY_PROFILE_A) ?
		BATT_PROFILEA_MASK : BATT_PROFILEB_MASK;

	/* set the BID mask */
	ret = regmap_update_bits(battery->regmap, CFG_FG_BATT_CTRL_REG,
				BATT_PROFILE_SELECT_MASK, bid_mask);
	if (ret)
		return ret;

	ret = smb1360_enable_fg_access(battery);
	if (ret)
		return ret;

	/* delay after handshaking for profile-switch to continue */
	msleep(1500);

	ret = smb1360_force_fg_reset(battery);
	if (ret) {
		smb1360_disable_fg_access(battery);
		return ret;
	}
	ret = smb1360_disable_fg_access(battery);
	if (ret)
		return ret;

	timeout = 10;
	while (timeout) {
		/* delay for profile to change */
		msleep(500);
		ret = regmap_read(battery->regmap, SHDW_FG_BATT_STATUS, &val);
		if (ret) {
			smb1360_disable_fg_access(battery);
			return ret;
		}

		val = !!(val & BATTERY_PROFILE_BIT);
		if (val == new_profile)
			break;
		timeout--;
	}

	if (!timeout)
		return -EBUSY;

	return 0;
}

static int smb1360_delayed_hw_init(struct smb1360_battery *battery)
{
	int ret;

	ret = smb1360_check_batt_profile(battery);
	if (ret) {
		dev_err(battery->dev, "unable to modify battery profile: %d\n", ret);
		return ret;
	}

	ret = smb1360_fg_config(battery);
	if (ret)
		dev_err(battery->dev, "couldn't configure FG\n");

	return ret;
}

static void smb1360_delayed_init_work_fn(struct work_struct *work)
{
	int ret = 0;
	struct smb1360_battery *battery = container_of(work,
						       struct smb1360_battery,
						       delayed_init_work.work);

	ret = smb1360_delayed_hw_init(battery);
	if (!ret) {
		power_supply_changed(battery->psy);
	} else if (ret == -ETIMEDOUT) {
		ret = smb1360_force_fg_reset(battery);
		if (ret)
			return;
		schedule_delayed_work(&battery->delayed_init_work, 0);
	}
}

static int smb1360_initial_status(struct smb1360_battery *battery)
{
	int ret, i;

	for (i = IRQ_A; i < IRQ_COUNT; ++i) {
		ret = regmap_read(battery->regmap, IRQ_REG + i, &battery->irqstat[i]);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int smb_parse_batt_id(struct smb1360_battery *battery)
{
	int ret = 0, rpull = 0, vref = 0, val;
	int64_t denom, batt_id_uv;
	struct device *dev = battery->dev;
	struct iio_channel *channel;

	channel = devm_iio_channel_get(dev, "batt-id");
	if (IS_ERR(channel)) {
		dev_err(dev, "error getting channel: %ld\n", PTR_ERR(channel));
		return PTR_ERR(channel);
	}

	ret = device_property_read_u32(dev, "qcom,profile-a-rid-kohm",
						&battery->profile_rid[0]);
	if (ret < 0)
		return ret;

	ret = device_property_read_u32(dev, "qcom,profile-b-rid-kohm",
						&battery->profile_rid[1]);
	if (ret < 0)
		return ret;

	ret = device_property_read_u32(dev, "qcom,batt-id-vref-uv", &vref);
	if (ret < 0)
		return ret;

	ret = device_property_read_u32(dev, "qcom,batt-id-rpullup-kohm", &rpull);
	if (ret < 0)
		return ret;

	/* read battery ID */
	ret = iio_read_channel_processed(channel, &val);
	iio_channel_release(channel);
	if (ret < 0)
		return ret;

	batt_id_uv = val * 1000;

	/* vadc not correct or batt id line grounded, report 0 kohms */
	if (batt_id_uv == 0)
		return 0;

	/* batt id connector might be open, return 0 kohms */
	denom = div64_s64(vref * 1000000LL, batt_id_uv) - 1000000LL;
	if (denom == 0)
		return 0;

	battery->connected_rid = div64_s64(rpull * 1000000LL + denom / 2, denom);

	return 0;
}

static int smb1360_parse_properties(struct smb1360_battery *battery)
{
	int ret = 0;
	struct device *dev = battery->dev;

	ret = device_property_read_u32(dev, "qcom,float-voltage-mv",
				       &battery->vfloat_mv);
	ret = device_property_read_u32(dev, "qcom,charging-timeout",
				       &battery->safety_time);

	if (!ret && (battery->safety_time > chg_time[ARRAY_SIZE(chg_time) - 1]))
		return -EINVAL;

	ret = device_property_read_u32(dev, "qcom,recharge-thresh-mv",
				       &battery->resume_delta_mv);
	ret = device_property_read_u32(dev, "qcom,iterm-ma",
				       &battery->iterm_ma);

	battery->recharge_disabled =
		device_property_read_bool(dev, "qcom,recharge-disabled");
	battery->iterm_disabled =
		device_property_read_bool(dev, "qcom,iterm-disabled");
	battery->chg_inhibit_disabled =
		device_property_read_bool(dev, "qcom,chg-inhibit-disabled");
	battery->shdn_after_pwroff =
		device_property_read_bool(dev,	"qcom,shdn-after-pwroff");
	battery->fg_reset_at_pon =
		device_property_read_bool(dev, "qcom,fg-reset-at-pon");

	if (device_property_read_bool(dev, "qcom,batt-profile-select")) {
		ret = smb_parse_batt_id(battery);
		if (ret < 0)
			return ret;
	}

	if (battery->fg_reset_at_pon) {
		ret = device_property_read_u32(dev, "qcom,fg-reset-threshold-mv",
					       &battery->fg_reset_threshold_mv);
		if (ret)
			battery->fg_reset_threshold_mv = FG_RESET_THRESHOLD_MV;
	}

	if (battery->recharge_disabled && battery->chg_inhibit_disabled) {
		dev_err(battery->dev, "recharge: both disabled and mv set\n");
		return -EINVAL;
	}

	if (battery->iterm_ma != -EINVAL && battery->iterm_disabled) {
		dev_err(battery->dev, "iterm: both disabled and ma set\n");
		return -EINVAL;
	}

	ret = device_property_read_u32(dev, "qcom,fg-voltage-min-mv",
				       &battery->voltage_min_mv);
	ret = device_property_read_u32(dev, "qcom,fg-voltage-empty-mv",
				       &battery->voltage_empty_mv);

	return 0;
}

static const struct regmap_config smb1360_battery_regmap_config = {
	.reg_bits	= 8,
	.val_bits	= 8,
};

static void smb1360_external_power_changed(struct power_supply *psy)
{
	power_supply_changed(psy);
}

static const struct power_supply_desc smb1360_battery_desc = {
	.name			= "smb1360-battery",
	.type			= POWER_SUPPLY_TYPE_BATTERY,
	.get_property		= smb1360_battery_get_property,
	.properties		= smb1360_battery_props,
	.num_properties		= ARRAY_SIZE(smb1360_battery_props),
	.external_power_changed	= smb1360_external_power_changed,
};

static int smb1360_probe(struct i2c_client *client)
{
	int ret;
	struct power_supply_config psy_cfg = {};
	struct device *dev = &client->dev;
	struct smb1360_battery *battery;

	battery = devm_kzalloc(&client->dev, sizeof(*battery), GFP_KERNEL);
	if (!battery) {
		dev_err(&client->dev, "failed to allocate memory\n");
		return -EINVAL;
	}

	battery->dev = dev;

	battery->regmap = devm_regmap_init_i2c(client,
					       &smb1360_battery_regmap_config);
	if (IS_ERR(battery->regmap)) {
		dev_err(&client->dev, "failed to init regmap\n");
		return -EINVAL;
	}

	INIT_DELAYED_WORK(&battery->delayed_init_work,
			  smb1360_delayed_init_work_fn);
	init_completion(&battery->fg_mem_access_granted);

	ret = regmap_read(battery->regmap, REVISION_CTRL_REG, &battery->revision);
	if (ret) {
		dev_err(battery->dev, "couldn't read revision: %d\n", ret);
		return ret;
	}

	battery->revision &= DEVICE_REV_MASK;

	dev_info(battery->dev, "device revision: %d\n", battery->revision);

	battery->vfloat_mv = -EINVAL;
	battery->safety_time = -EINVAL;
	battery->resume_delta_mv = -EINVAL;
	battery->iterm_ma = -EINVAL;
	battery->voltage_min_mv = -EINVAL;
	battery->voltage_empty_mv = -EINVAL;
	ret = smb1360_parse_properties(battery);
	if (ret) {
		dev_err(&client->dev, "error parsing device tree: %d\n", ret);
		return ret;
	}

	device_init_wakeup(battery->dev, 1);
	i2c_set_clientdata(client, battery);

	ret = smb1360_register_vbus_regulator(battery);
	if (ret < 0)
		return ret;

	ret = smb1360_hw_init(client);
	if (ret < 0) {
		dev_err(&client->dev, "unable to intialize hw: %d\n", ret);
		return ret;
	}

	ret = smb1360_initial_status(battery);
	if (ret < 0) {
		dev_err(&client->dev,
			"unable to determine init status: %d\n", ret);
		return ret;
	}

	psy_cfg.drv_data = battery;
	battery->psy = devm_power_supply_register(&client->dev,
						  &smb1360_battery_desc,
						  &psy_cfg);
	if (IS_ERR(battery->psy)) {
		dev_err(&client->dev, "failed to register power supply\n");
		ret = PTR_ERR(battery->psy);
		return ret;
	}

	if (client->irq) {
		ret = devm_request_threaded_irq(&client->dev, client->irq, NULL,
						smb1360_bat_irq, IRQF_ONESHOT,
						NULL, battery);
		if (ret) {
			dev_err(&client->dev,
				"request irq %d failed\n", client->irq);
			return ret;
		}

		enable_irq_wake(client->irq);
	}

	schedule_delayed_work(&battery->delayed_init_work,
			      msecs_to_jiffies(SMB1360_POWERON_DELAY_MS));

	return 0;
}

static void smb1360_shutdown(struct i2c_client *client)
{
	int ret;
	struct smb1360_battery *battery = i2c_get_clientdata(client);

	ret = regulator_disable_regmap(battery->otg_vreg.rdev);
	if (ret)
		dev_err(battery->dev, "couldn't disable OTG: %d\n", ret);

	if (battery->shdn_after_pwroff) {
		ret = smb1360_poweroff(battery);
		if (ret)
			dev_err(battery->dev, "couldn't shutdown: %d\n", ret);
	}
}

static int smb1360_suspend(struct device *dev)
{
	int ret;
	struct i2c_client *client = to_i2c_client(dev);
	struct smb1360_battery *battery = i2c_get_clientdata(client);

	ret = regmap_write(battery->regmap, IRQ_CFG_REG, IRQ_DCIN_UV_BIT
						| IRQ_BAT_HOT_COLD_SOFT_BIT
						| IRQ_HOT_COLD_HARD_BIT);
	if (ret < 0)
		dev_err(battery->dev, "couldn't set irq_cfg: %d\n", ret);

	ret = regmap_write(battery->regmap, IRQ2_CFG_REG, IRQ2_BATT_MISSING_BIT
						| IRQ2_VBAT_LOW_BIT
						| IRQ2_POWER_OK_BIT);
	if (ret < 0)
		dev_err(battery->dev, "couldn't set irq2_cfg: %d\n", ret);

	ret = regmap_write(battery->regmap, IRQ3_CFG_REG, SOC_FULL_BIT
					| SOC_MIN_BIT
					| SOC_EMPTY_BIT);
	if (ret < 0)
		dev_err(battery->dev, "couldn't set irq3_cfg: %d\n", ret);

	return 0;
}

static int smb1360_resume(struct device *dev)
{
	int ret;
	struct i2c_client *client = to_i2c_client(dev);
	struct smb1360_battery *battery = i2c_get_clientdata(client);

	ret = smb1360_configure_irq(battery);
	if (ret)
		return ret;

	power_supply_changed(battery->psy);

	return 0;
}

static const struct dev_pm_ops smb1360_pm_ops = {
	.resume = smb1360_resume,
	.suspend = smb1360_suspend,
};

#ifdef CONFIG_OF
static struct of_device_id smb1360_match_table[] = {
	{ .compatible = "qcom,smb1360" },
	{ },
};
MODULE_DEVICE_TABLE(of, smb1360_match_table);
#endif

static struct i2c_driver smb1360_driver = {
	.driver	= {
		.name = "smb1360-chg-fg",
		.of_match_table = of_match_ptr(smb1360_match_table),
		.pm = &smb1360_pm_ops,
	},
	.probe_new = smb1360_probe,
	.shutdown = smb1360_shutdown,
};

module_i2c_driver(smb1360_driver);

MODULE_DESCRIPTION("SMB1360 Charger and Fuel Gauge");
MODULE_LICENSE("GPL v2");
