// SPDX-License-Identifier: GPL-2.0
/*
 * Dynaimage AP3216C/AP3426 Ambient Light Sensor/Proximity/IR Driver
 *
 * Copyright (c) Vladislav Dubrovin <foxy9855@gmail.com>
 *
 * Data sheets:
 *  https://github.com/RT-Thread-packages/ap3216c/blob/master/ap3216c_datasheet.pdf
 *
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/mod_devicetable.h>
#include <linux/iio/iio.h>
#include <linux/delay.h>
#include <linux/property.h>

/* Registers and masks */
#define AP3426_REG_SYS_CONF        0x00
#define AP3426_REG_SYS_INTSTATUS   0x01
#define AP3426_REG_SYS_INTCTRL     0x02

#define AP3426_REG_IR_DATA_LOW     0x0A
#define AP3426_REG_ALS_DATA_LOW    0x0C
#define AP3426_REG_PS_DATA_LOW     0x0E

/* System modes */
#define AP3426_SYS_DEV_DOWN        0x00
#define AP3426_SYS_ALS_ENABLE      0x01
#define AP3426_SYS_PS_ENABLE       0x02
#define AP3426_SYS_ALS_PS_ENABLE   0x03
#define AP3426_SYS_DEV_RESET       0x04

struct ap3216c_data {
	struct i2c_client *client;
	struct mutex lock;
	u32 prox_near_level;
};

/* --- Nearlevel attribute --- */
static ssize_t ap3216c_read_nearlevel(struct iio_dev *indio_dev,
				      uintptr_t private,
				      const struct iio_chan_spec *chan,
				      char *buf)
{
	struct ap3216c_data *data = iio_priv(indio_dev);

	return sysfs_emit(buf, "%u\n", data->prox_near_level);
}
static const struct iio_chan_spec_ext_info ap3216c_prox_ext_info[] = {
	{
		.name = "nearlevel",
		.shared = IIO_SEPARATE,
		.read = ap3216c_read_nearlevel,
	},
	{ }
};
/* ---------------------------------------------- */

static const struct iio_chan_spec ap3216c_channels[] = {
	{
		.type = IIO_INTENSITY,
		.modified = 1,
		.channel2 = IIO_MOD_LIGHT_IR,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	},
	{
		.type = IIO_LIGHT,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	},
	{
		.type = IIO_PROXIMITY,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
		.ext_info = ap3216c_prox_ext_info,
	},
};

/**
 * ap3216c_read_raw_data() - Read 16-bit raw data from the sensor
 * @client: I2C client structure
 * @reg_low: Address of the lower byte register
 *
 * The sensor stores 16-bit data in two consecutive registers (little-endian).
 *
 * Return: 16-bit positive integer on success, negative error code on failure.
 */
static int ap3216c_read_raw_data(struct i2c_client *client, u8 reg_low)
{
	return i2c_smbus_read_word_data(client, reg_low);
}

/* Smart "on-demand" measurement with fixed delay */
static int ap3216c_get_measurement(struct ap3216c_data *data, u8 mode,
				   u8 reg_low, int delay_ms, int *val)
{
	struct i2c_client *client = data->client;
	int ret, ret2;

	ret = i2c_smbus_write_byte_data(client, AP3426_REG_SYS_CONF, mode);
	if (ret < 0)
		return ret;

	msleep(delay_ms);

	ret = ap3216c_read_raw_data(client, reg_low);
	if (ret < 0)
		goto out_power_down;

	*val = ret;
	ret = 0;

out_power_down:
	ret2 = i2c_smbus_write_byte_data(client, AP3426_REG_SYS_CONF,
					AP3426_SYS_DEV_DOWN);
	if (ret == 0 && ret2 < 0)
		ret = ret2;

	return ret;
}

static int ap3216c_read_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan,
			    int *val, int *val2, long mask)
{
	struct ap3216c_data *data = iio_priv(indio_dev);
	int ret, raw_val;

	if (mask != IIO_CHAN_INFO_RAW)
		return -EINVAL;

	mutex_lock(&data->lock);

	switch (chan->type) {
	case IIO_INTENSITY:
		ret = ap3216c_get_measurement(data, AP3426_SYS_PS_ENABLE,
					      AP3426_REG_IR_DATA_LOW, 20, &raw_val);
		if (ret < 0)
			goto out_unlock;
		*val = raw_val & 0x3FF;
		ret = IIO_VAL_INT;
		break;

	case IIO_LIGHT:
		ret = ap3216c_get_measurement(data, AP3426_SYS_ALS_ENABLE,
					      AP3426_REG_ALS_DATA_LOW, 120, &raw_val);
		if (ret < 0)
			goto out_unlock;
		*val = raw_val & 0xFFFF;
		ret = IIO_VAL_INT;
		break;

	case IIO_PROXIMITY:
		ret = ap3216c_get_measurement(data, AP3426_SYS_PS_ENABLE,
					      AP3426_REG_PS_DATA_LOW, 20, &raw_val);
		if (ret < 0)
			goto out_unlock;
		*val = raw_val & 0x3FF;
		ret = IIO_VAL_INT;
		break;

	default:
		ret = -EINVAL;
	}

out_unlock:
	mutex_unlock(&data->lock);
	return ret;
}

static const struct iio_info ap3216c_info = {
	.read_raw = ap3216c_read_raw,
};

static int ap3216c_chip_init(struct i2c_client *client)
{
	int ret;

	ret = i2c_smbus_write_byte_data(client, AP3426_REG_SYS_CONF, AP3426_SYS_DEV_RESET);
	if (ret < 0)
		return ret;

	msleep(20);

	ret = i2c_smbus_write_byte_data(client, AP3426_REG_SYS_CONF, AP3426_SYS_DEV_DOWN);
	if (ret < 0)
		return ret;

	return 0;
}

static int ap3216c_probe(struct i2c_client *client)
{
	struct iio_dev *indio_dev;
	struct ap3216c_data *data;
	int ret;

	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));
	if (!indio_dev)
		return -ENOMEM;

	data = iio_priv(indio_dev);
	data->client = client;
	mutex_init(&data->lock);

	if (device_property_read_u32(&client->dev, "proximity-near-level",
					&data->prox_near_level)) {
		data->prox_near_level = 300;
		dev_warn(&client->dev,
			"proximity near-level property missing, using default %u\n",
			data->prox_near_level);
	}

	indio_dev->name = "ap3216c";
	indio_dev->info = &ap3216c_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = ap3216c_channels;
	indio_dev->num_channels = ARRAY_SIZE(ap3216c_channels);

	ret = ap3216c_chip_init(client);
	if (ret < 0)
		return dev_err_probe(&client->dev, ret, "Failed to reset chip via I2C\n");

	return devm_iio_device_register(&client->dev, indio_dev);
}

static const struct of_device_id ap3216c_of_match[] = {
	{ .compatible = "dynaimage,ap3426" },
	{ .compatible = "dynaimage,ap3216c" },
	{ }
};
MODULE_DEVICE_TABLE(of, ap3216c_of_match);

static const struct i2c_device_id ap3216c_id[] = {
	{ "ap3426", 0 },
	{ "ap3216c", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ap3216c_id);

static struct i2c_driver ap3216c_driver = {
	.driver = {
		.name = "ap3216c",
		.of_match_table = ap3216c_of_match,
	},
	.probe = ap3216c_probe,
	.id_table = ap3216c_id,
};
module_i2c_driver(ap3216c_driver);

MODULE_AUTHOR("Vladislav Dubrovin <foxy9855@gmail.com>");
MODULE_DESCRIPTION("AP3216C/AP3426 Ambient Light Sensor/Proximity/IR Driver");
MODULE_LICENSE("GPL");
