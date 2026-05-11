// SPDX-License-Identifier: GPL-2.0-only
/*
 * tcs3472.c - Support for TAOS TCS3472 color light-to-digital converter
 *
 * Copyright (c) 2013 Peter Meerwald <pmeerw@pmeerw.net>
 *
 * Color light sensor with 16-bit channels for red, green, blue, clear);
 * 7-bit I2C slave address 0x39 (TCS34721, TCS34723) or 0x29 (TCS34725,
 * TCS34727)
 *
 * Datasheet: http://ams.com/eng/content/download/319364/1117183/file/TCS3472_Datasheet_EN_v2.pdf
 *
 * TODO: wait time
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cleanup.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/regulator/consumer.h>

#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/events.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/iio/buffer.h>
#include <linux/iio/triggered_buffer.h>

#define TCS3472_DRV_NAME "tcs3472"

#define TCS3472_COMMAND BIT(7)
#define TCS3472_AUTO_INCR BIT(5)
#define TCS3472_SPECIAL_FUNC (BIT(5) | BIT(6))

#define TCS3472_INTR_CLEAR (TCS3472_COMMAND | TCS3472_SPECIAL_FUNC | 0x06)
#define TCS3472_ALL_INTR_CLEAR	(TCS3472_COMMAND | TCS3472_SPECIAL_FUNC | 0x07)

#define TCS3472_ENABLE (TCS3472_COMMAND | 0x00)
#define TCS3472_ATIME (TCS3472_COMMAND | 0x01)
#define TCS3472_WTIME (TCS3472_COMMAND | 0x03)
#define TCS3472_AILT (TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x04)
#define TCS3472_AIHT (TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x06)
#define TCS3472_PERS (TCS3472_COMMAND | 0x0c)
#define TCS3472_CONFIG (TCS3472_COMMAND | 0x0d)
#define TCS3472_CONTROL (TCS3472_COMMAND | 0x0f)
#define TCS3472_ID (TCS3472_COMMAND | 0x12)
#define TCS3472_STATUS (TCS3472_COMMAND | 0x13)
#define TCS3472_CDATA (TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x14)
#define TCS3472_RDATA (TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x16)
#define TCS3472_GDATA (TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x18)
#define TCS3472_BDATA (TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x1a)

/* TMD3782 proximity registers */
#define TCS3472_PILT		(TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x08)
#define TCS3472_PIHT		(TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x0a)
#define TCS3472_PPULSE		(TCS3472_COMMAND | 0x0e)
#define TCS3472_PDATA		(TCS3472_COMMAND | TCS3472_AUTO_INCR | 0x1c)
#define TCS3472_REVID		(TCS3472_COMMAND | 0x11)

/* ENABLE register: proximity bits */
#define TCS3472_ENABLE_PIEN	BIT(5)
#define TCS3472_ENABLE_WEN	BIT(3)
#define TCS3472_ENABLE_PEN	BIT(2)

#define TMD3782_CONTROL_PDRIVE_MASK	GENMASK(7, 6)
/* TMD3782 datasheet page 25, Figure 34: bit 5 must be written as 1 */
#define TCS3472_CONTROL_RSVD5		BIT(5)

/* STATUS register: proximity bits */
#define TCS3472_STATUS_PINT	BIT(5)
#define TCS3472_STATUS_PVALID	BIT(1)

/* Interrupt clear: proximity */
#define TCS3472_PROX_INTR_CLEAR	(TCS3472_COMMAND | TCS3472_SPECIAL_FUNC | 0x05)

#define TCS3472_STATUS_AINT BIT(4)
#define TCS3472_STATUS_AVALID BIT(0)
#define TCS3472_ENABLE_AIEN BIT(4)
#define TCS3472_ENABLE_AEN BIT(1)
#define TCS3472_ENABLE_PON BIT(0)
#define TCS3472_CONTROL_AGAIN_MASK (BIT(0) | BIT(1))

/* Chip ID register values */
#define TCS34721_CHIP_ID	0x44
#define TCS34723_CHIP_ID	0x4d
#define TMD37821_CHIP_ID	0x60
#define TMD37823_CHIP_ID	0x69

struct tcs3472_chip_info {
	const struct iio_chan_spec *channels;
	int num_channels;
	bool has_proximity;
	const char *name;
};

static const char *const tcs3472_supply_names[] = {
	"vdd",
	"vddio",
};

struct tcs3472_data {
	struct i2c_client *client;
	const struct tcs3472_chip_info *chip_info;
	struct mutex lock;
	bool prox_event_enabled;
	bool prox_buf_enabled;
	u16 low_thresh;
	u16 high_thresh;
	u16 prox_low_thresh;
	u16 prox_high_thresh;
	u8 enable;
	u8 enable_saved;
	u8 control;
	u8 atime;
	u8 apers;
	u8 ppers;
	u8 ppulse;
	/* Ensure timestamp is naturally aligned */
	struct {
		/* 5 channels: RGBC (4) + proximity (1, TMD3782 only) */
		u16 chans[5];
		s64 timestamp __aligned(8);
	} scan;
};

static const struct iio_event_spec tcs3472_events[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_FALLING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_EITHER,
		.mask_separate = BIT(IIO_EV_INFO_ENABLE) |
				 BIT(IIO_EV_INFO_PERIOD),
	},
};

/*
 * Proximity events: threshold rising/falling + enable.
 * No IIO_EV_INFO_PERIOD — PPERS is a linear 0-15 count, not the non-linear
 * APERS mapping. Fixed at 3 in v1 (not configurable by userspace).
 */
static const struct iio_event_spec tmd3782_prox_events[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_FALLING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE),
	}, {
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_EITHER,
		.mask_separate = BIT(IIO_EV_INFO_ENABLE),
	},
};

#define TCS3472_CHANNEL(_color, _si, _addr) { \
	.type = IIO_INTENSITY, \
	.modified = 1, \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW), \
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_CALIBSCALE) | \
		BIT(IIO_CHAN_INFO_INT_TIME), \
	.channel2 = IIO_MOD_LIGHT_##_color, \
	.address = _addr, \
	.scan_index = _si, \
	.scan_type = { \
		.sign = 'u', \
		.realbits = 16, \
		.storagebits = 16, \
		.endianness = IIO_CPU, \
	}, \
	.event_spec = _si ? NULL : tcs3472_events, \
	.num_event_specs = _si ? 0 : ARRAY_SIZE(tcs3472_events), \
}

static const int tcs3472_agains[] = { 1, 4, 16, 60 };

static const int tcs3472_led_currents[][2] = {
	{ 100000, 0x00 },
	{  50000, 0x01 },
	{  25000, 0x02 },
	{  12500, 0x03 },
	{      0, 0x00 },  /* sentinel, also default = 100mA */
};

static const struct iio_chan_spec tcs3472_channels[] = {
	TCS3472_CHANNEL(CLEAR, 0, TCS3472_CDATA),
	TCS3472_CHANNEL(RED, 1, TCS3472_RDATA),
	TCS3472_CHANNEL(GREEN, 2, TCS3472_GDATA),
	TCS3472_CHANNEL(BLUE, 3, TCS3472_BDATA),
	IIO_CHAN_SOFT_TIMESTAMP(4),
};

static const struct iio_chan_spec tmd3782_channels[] = {
	TCS3472_CHANNEL(CLEAR, 0, TCS3472_CDATA),
	TCS3472_CHANNEL(RED, 1, TCS3472_RDATA),
	TCS3472_CHANNEL(GREEN, 2, TCS3472_GDATA),
	TCS3472_CHANNEL(BLUE, 3, TCS3472_BDATA),
	{
		.type = IIO_PROXIMITY,
		.address = TCS3472_PDATA,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
		.scan_index = 4,
		.scan_type = {
			.sign = 'u',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_CPU,
		},
		.event_spec = tmd3782_prox_events,
		.num_event_specs = ARRAY_SIZE(tmd3782_prox_events),
	},
	IIO_CHAN_SOFT_TIMESTAMP(5),
};

static const struct tcs3472_chip_info tcs3472_chip_info = {
	.channels = tcs3472_channels,
	.num_channels = ARRAY_SIZE(tcs3472_channels),
	.has_proximity = false,
	.name = "tcs3472",
};

static const struct tcs3472_chip_info tmd3782_chip_info = {
	.channels = tmd3782_channels,
	.num_channels = ARRAY_SIZE(tmd3782_channels),
	.has_proximity = true,
	.name = "tmd3782",
};

static int tcs3472_req_data(struct tcs3472_data *data, unsigned int status_mask)
{
	int tries = 50;
	int ret;

	while (tries--) {
		ret = i2c_smbus_read_byte_data(data->client, TCS3472_STATUS);
		if (ret < 0)
			return ret;
		if ((ret & status_mask) == status_mask)
			break;
		msleep(20);
	}

	if (tries < 0) {
		dev_err(&data->client->dev, "data not ready\n");
		return -EIO;
	}

	return 0;
}

static int tcs3472_read_raw(struct iio_dev *indio_dev,
			   struct iio_chan_spec const *chan,
			   int *val, int *val2, long mask)
{
	struct tcs3472_data *data = iio_priv(indio_dev);
	int ret;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		ret = iio_device_claim_direct_mode(indio_dev);
		if (ret)
			return ret;

		if (chan->type == IIO_PROXIMITY) {
			bool cold;

			{
				guard(mutex)(&data->lock);
				cold = !data->prox_event_enabled &&
				       !data->prox_buf_enabled;
				if (cold) {
					data->enable |= TCS3472_ENABLE_PEN |
							TCS3472_ENABLE_WEN;
					ret = i2c_smbus_write_byte_data(
						data->client, TCS3472_ENABLE,
						data->enable);
					if (ret) {
						data->enable &=
							~(TCS3472_ENABLE_PEN |
							  TCS3472_ENABLE_WEN);
						iio_device_release_direct_mode(
							indio_dev);
						return ret;
					}
				}
			}

			ret = tcs3472_req_data(data, TCS3472_STATUS_PVALID);
			if (ret >= 0)
				ret = i2c_smbus_read_word_data(data->client,
							       chan->address);

			if (cold) {
				guard(mutex)(&data->lock);
				if (!data->prox_event_enabled &&
				    !data->prox_buf_enabled) {
					data->enable &= ~(TCS3472_ENABLE_PEN |
							  TCS3472_ENABLE_WEN);
					i2c_smbus_write_byte_data(data->client,
								  TCS3472_ENABLE,
								  data->enable);
				}
			}
		} else {
			ret = tcs3472_req_data(data, TCS3472_STATUS_AVALID);
			if (ret >= 0)
				ret = i2c_smbus_read_word_data(data->client,
							       chan->address);
		}

		iio_device_release_direct_mode(indio_dev);
		if (ret < 0)
			return ret;
		*val = ret;
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_CALIBSCALE:
		*val = tcs3472_agains[data->control &
			TCS3472_CONTROL_AGAIN_MASK];
		return IIO_VAL_INT;
	case IIO_CHAN_INFO_INT_TIME:
		*val = 0;
		*val2 = (256 - data->atime) * 2400;
		return IIO_VAL_INT_PLUS_MICRO;
	}
	return -EINVAL;
}

static int tcs3472_write_raw(struct iio_dev *indio_dev,
			       struct iio_chan_spec const *chan,
			       int val, int val2, long mask)
{
	struct tcs3472_data *data = iio_priv(indio_dev);
	int i;

	switch (mask) {
	case IIO_CHAN_INFO_CALIBSCALE:
		if (val2 != 0)
			return -EINVAL;
		for (i = 0; i < ARRAY_SIZE(tcs3472_agains); i++) {
			if (val == tcs3472_agains[i]) {
				guard(mutex)(&data->lock);
				data->control &= ~TCS3472_CONTROL_AGAIN_MASK;
				data->control |= i;
				return i2c_smbus_write_byte_data(
					data->client, TCS3472_CONTROL,
					data->control);
			}
		}
		return -EINVAL;
	case IIO_CHAN_INFO_INT_TIME:
		if (val != 0)
			return -EINVAL;
		for (i = 0; i < 256; i++) {
			if (val2 == (256 - i) * 2400) {
				data->atime = i;
				return i2c_smbus_write_byte_data(
					data->client, TCS3472_ATIME,
					data->atime);
			}

		}
		return -EINVAL;
	}
	return -EINVAL;
}

/*
 * Translation from APERS field value to the number of consecutive out-of-range
 * clear channel values before an interrupt is generated
 */
static const int tcs3472_intr_pers[] = {
	0, 1, 2, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60
};

static int tcs3472_read_event(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir, enum iio_event_info info, int *val,
	int *val2)
{
	struct tcs3472_data *data = iio_priv(indio_dev);
	unsigned int period;

	guard(mutex)(&data->lock);

	switch (info) {
	case IIO_EV_INFO_VALUE:
		if (chan->type == IIO_PROXIMITY)
			*val = (dir == IIO_EV_DIR_RISING) ?
				data->prox_high_thresh : data->prox_low_thresh;
		else
			*val = (dir == IIO_EV_DIR_RISING) ?
				data->high_thresh : data->low_thresh;
		return IIO_VAL_INT;
	case IIO_EV_INFO_PERIOD:
		period = (256 - data->atime) * 2400 *
			tcs3472_intr_pers[data->apers];
		*val = period / USEC_PER_SEC;
		*val2 = period % USEC_PER_SEC;
		return IIO_VAL_INT_PLUS_MICRO;
	default:
		return -EINVAL;
	}
}

static int tcs3472_write_event(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir, enum iio_event_info info, int val,
	int val2)
{
	struct tcs3472_data *data = iio_priv(indio_dev);
	int ret;
	u8 command;
	int period;
	int i;

	guard(mutex)(&data->lock);

	switch (info) {
	case IIO_EV_INFO_VALUE:
		if (chan->type == IIO_PROXIMITY) {
			switch (dir) {
			case IIO_EV_DIR_RISING:
				command = TCS3472_PIHT;
				break;
			case IIO_EV_DIR_FALLING:
				command = TCS3472_PILT;
				break;
			default:
				return -EINVAL;
			}
		} else {
			switch (dir) {
			case IIO_EV_DIR_RISING:
				command = TCS3472_AIHT;
				break;
			case IIO_EV_DIR_FALLING:
				command = TCS3472_AILT;
				break;
			default:
				return -EINVAL;
			}
		}
		ret = i2c_smbus_write_word_data(data->client, command, val);
		if (ret)
			return ret;

		if (chan->type == IIO_PROXIMITY) {
			if (dir == IIO_EV_DIR_RISING)
				data->prox_high_thresh = val;
			else
				data->prox_low_thresh = val;
		} else {
			if (dir == IIO_EV_DIR_RISING)
				data->high_thresh = val;
			else
				data->low_thresh = val;
		}
		return 0;
	case IIO_EV_INFO_PERIOD:
		if (chan->type == IIO_PROXIMITY)
			return -EINVAL;

		period = val * USEC_PER_SEC + val2;
		for (i = 1; i < ARRAY_SIZE(tcs3472_intr_pers) - 1; i++) {
			if (period <= (256 - data->atime) * 2400 *
					tcs3472_intr_pers[i])
				break;
		}
		ret = i2c_smbus_write_byte_data(data->client, TCS3472_PERS,
						(data->ppers << 4) | i);
		if (ret)
			return ret;

		data->apers = i;
		return 0;
	default:
		return -EINVAL;
	}
}

static int tcs3472_read_event_config(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir)
{
	struct tcs3472_data *data = iio_priv(indio_dev);

	guard(mutex)(&data->lock);

	if (chan->type == IIO_PROXIMITY)
		return data->prox_event_enabled;

	return !!(data->enable & TCS3472_ENABLE_AIEN);
}

static int tcs3472_write_event_config(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, enum iio_event_type type,
	enum iio_event_direction dir, int state)
{
	struct tcs3472_data *data = iio_priv(indio_dev);
	int ret = 0;
	u8 enable_old;

	/* No IRQ handler → enabling interrupts would leave INT stuck low */
	if (!data->client->irq)
		return -EINVAL;

	guard(mutex)(&data->lock);

	enable_old = data->enable;

	if (chan->type == IIO_PROXIMITY) {
		data->prox_event_enabled = !!state;
		if (state) {
			data->enable |= TCS3472_ENABLE_PIEN |
					TCS3472_ENABLE_PEN |
					TCS3472_ENABLE_WEN;
		} else {
			data->enable &= ~TCS3472_ENABLE_PIEN;
			if (!data->prox_buf_enabled)
				data->enable &= ~(TCS3472_ENABLE_PEN |
						  TCS3472_ENABLE_WEN);
		}
	} else {
		if (state)
			data->enable |= TCS3472_ENABLE_AIEN;
		else
			data->enable &= ~TCS3472_ENABLE_AIEN;
	}

	if (enable_old != data->enable) {
		ret = i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE,
						data->enable);
		if (ret) {
			data->enable = enable_old;
			if (chan->type == IIO_PROXIMITY)
				data->prox_event_enabled = !state;
		}
	}

	return ret;
}

static irqreturn_t tcs3472_event_handler(int irq, void *priv)
{
	struct iio_dev *indio_dev = priv;
	struct tcs3472_data *data = iio_priv(indio_dev);
	int ret;

	ret = i2c_smbus_read_byte_data(data->client, TCS3472_STATUS);
	if (ret < 0)
		return IRQ_HANDLED;

	if (ret & TCS3472_STATUS_AINT)
		iio_push_event(indio_dev,
			       IIO_UNMOD_EVENT_CODE(IIO_INTENSITY, 0,
						    IIO_EV_TYPE_THRESH,
						    IIO_EV_DIR_EITHER),
			       iio_get_time_ns(indio_dev));

	if (ret & TCS3472_STATUS_PINT)
		iio_push_event(indio_dev,
			       IIO_UNMOD_EVENT_CODE(IIO_PROXIMITY, 0,
						    IIO_EV_TYPE_THRESH,
						    IIO_EV_DIR_EITHER),
			       iio_get_time_ns(indio_dev));

	if (ret & TCS3472_STATUS_AINT)
		i2c_smbus_read_byte_data(data->client, TCS3472_INTR_CLEAR);
	if (ret & TCS3472_STATUS_PINT)
		i2c_smbus_read_byte_data(data->client, TCS3472_PROX_INTR_CLEAR);

	return IRQ_HANDLED;
}

static irqreturn_t tcs3472_trigger_handler(int irq, void *p)
{
	struct iio_poll_func *pf = p;
	struct iio_dev *indio_dev = pf->indio_dev;
	struct tcs3472_data *data = iio_priv(indio_dev);
	unsigned int status_mask = TCS3472_STATUS_AVALID;
	int i, j = 0;
	int ret;

	if (data->prox_event_enabled || data->prox_buf_enabled)
		status_mask |= TCS3472_STATUS_PVALID;

	ret = tcs3472_req_data(data, status_mask);
	if (ret < 0)
		goto done;

	iio_for_each_active_channel(indio_dev, i) {
		ret = i2c_smbus_read_word_data(data->client,
			TCS3472_CDATA + 2 * i);
		if (ret < 0)
			goto done;

		data->scan.chans[j++] = ret;
	}

	iio_push_to_buffers_with_timestamp(indio_dev, &data->scan,
		iio_get_time_ns(indio_dev));

done:
	iio_trigger_notify_done(indio_dev->trig);

	return IRQ_HANDLED;
}

static int tcs3472_buffer_preenable(struct iio_dev *indio_dev)
{
	struct tcs3472_data *data = iio_priv(indio_dev);
	int ret;

	if (!data->chip_info->has_proximity)
		return 0;

	/* Check if proximity channel (scan_index 4) is in the scan mask */
	if (!test_bit(4, indio_dev->active_scan_mask))
		return 0;

	guard(mutex)(&data->lock);
	data->prox_buf_enabled = true;
	data->enable |= TCS3472_ENABLE_PEN | TCS3472_ENABLE_WEN;
	ret = i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE, data->enable);
	if (ret) {
		data->prox_buf_enabled = false;
		if (!data->prox_event_enabled)
			data->enable &= ~(TCS3472_ENABLE_PEN | TCS3472_ENABLE_WEN);
	}

	return ret;
}

static int tcs3472_buffer_postdisable(struct iio_dev *indio_dev)
{
	struct tcs3472_data *data = iio_priv(indio_dev);

	if (!data->prox_buf_enabled)
		return 0;

	guard(mutex)(&data->lock);
	data->prox_buf_enabled = false;
	if (!data->prox_event_enabled) {
		data->enable &= ~(TCS3472_ENABLE_PEN | TCS3472_ENABLE_WEN);
		i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE,
					  data->enable);
	}

	return 0;
}

static const struct iio_buffer_setup_ops tcs3472_buffer_setup_ops = {
	.preenable = tcs3472_buffer_preenable,
	.postdisable = tcs3472_buffer_postdisable,
};

static ssize_t tcs3472_show_int_time_available(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	size_t len = 0;
	int i;

	for (i = 1; i <= 256; i++)
		len += scnprintf(buf + len, PAGE_SIZE - len, "0.%06d ",
			2400 * i);

	/* replace trailing space by newline */
	buf[len - 1] = '\n';

	return len;
}

static IIO_CONST_ATTR(calibscale_available, "1 4 16 60");
static IIO_DEV_ATTR_INT_TIME_AVAIL(tcs3472_show_int_time_available);

static struct attribute *tcs3472_attributes[] = {
	&iio_const_attr_calibscale_available.dev_attr.attr,
	&iio_dev_attr_integration_time_available.dev_attr.attr,
	NULL
};

static const struct attribute_group tcs3472_attribute_group = {
	.attrs = tcs3472_attributes,
};

static const struct iio_info tcs3472_info = {
	.read_raw = tcs3472_read_raw,
	.write_raw = tcs3472_write_raw,
	.read_event_value = tcs3472_read_event,
	.write_event_value = tcs3472_write_event,
	.read_event_config = tcs3472_read_event_config,
	.write_event_config = tcs3472_write_event_config,
	.attrs = &tcs3472_attribute_group,
};

static int tcs3472_probe(struct i2c_client *client)
{
	struct tcs3472_data *data;
	struct iio_dev *indio_dev;
	const struct tcs3472_chip_info *match_info;
	int ret;

	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));
	if (indio_dev == NULL)
		return -ENOMEM;

	data = iio_priv(indio_dev);
	i2c_set_clientdata(client, indio_dev);
	data->client = client;
	mutex_init(&data->lock);
	match_info = i2c_get_match_data(client);

	ret = devm_regulator_bulk_get_enable(&client->dev,
					     ARRAY_SIZE(tcs3472_supply_names),
					     tcs3472_supply_names);
	if (ret)
		return dev_err_probe(&client->dev, ret,
				     "failed to get regulators\n");

	/* 2.4ms PON warm-up after regulator enable */
	usleep_range(2500, 3000);

	ret = i2c_smbus_read_byte_data(data->client, TCS3472_ID);
	if (ret < 0)
		return ret;

	if (match_info) {
		data->chip_info = match_info;
	} else if (ret == TCS34721_CHIP_ID || ret == TCS34723_CHIP_ID) {
		data->chip_info = &tcs3472_chip_info;
	} else if (ret == TMD37821_CHIP_ID || ret == TMD37823_CHIP_ID) {
		data->chip_info = &tmd3782_chip_info;
	} else {
		return -ENODEV;
	}

	dev_info(&client->dev, "%s (id 0x%02x, rev 0x%02x) found\n",
		 data->chip_info->name, ret,
		 i2c_smbus_read_byte_data(client, TCS3472_REVID));

	indio_dev->info = &tcs3472_info;
	indio_dev->name = data->chip_info->name;
	indio_dev->channels = data->chip_info->channels;
	indio_dev->num_channels = data->chip_info->num_channels;
	indio_dev->modes = INDIO_DIRECT_MODE;

	ret = i2c_smbus_read_byte_data(data->client, TCS3472_CONTROL);
	if (ret < 0)
		return ret;
	data->control = ret;

	if (data->chip_info->has_proximity) {
		u32 led_ua;
		int i, pdrive = 0x00; /* default 100mA */

		/* TMD3782 datasheet page 25, Figure 34: bit 5 must be 1 */
		data->control |= TCS3472_CONTROL_RSVD5;

		if (!device_property_read_u32(&client->dev, "led-max-microamp",
					      &led_ua)) {
			for (i = 0; tcs3472_led_currents[i][0]; i++) {
				if (led_ua == tcs3472_led_currents[i][0]) {
					pdrive = tcs3472_led_currents[i][1];
					break;
				}
			}
		}
		data->control &= ~TMD3782_CONTROL_PDRIVE_MASK;
		data->control |= (pdrive << 6);

		ret = i2c_smbus_write_byte_data(data->client, TCS3472_CONTROL,
						data->control);
		if (ret < 0)
			return ret;
	}

	ret = i2c_smbus_read_byte_data(data->client, TCS3472_ATIME);
	if (ret < 0)
		return ret;
	data->atime = ret;

	ret = i2c_smbus_read_word_data(data->client, TCS3472_AILT);
	if (ret < 0)
		return ret;
	data->low_thresh = ret;

	ret = i2c_smbus_read_word_data(data->client, TCS3472_AIHT);
	if (ret < 0)
		return ret;
	data->high_thresh = ret;

	if (data->chip_info->has_proximity) {
		u32 ppulse_val = 8; /* default: datasheet Figure 11 test conditions */

		device_property_read_u32(&client->dev,
					 "amstaos,proximity-pulse-count",
					 &ppulse_val);
		data->ppulse = clamp_val(ppulse_val, 1, 255);
		ret = i2c_smbus_write_byte_data(data->client, TCS3472_PPULSE,
						data->ppulse);
		if (ret < 0)
			return ret;

		/*
		 * PPERS=3 matches downstream (intr_filter=0x33): interrupt fires
		 * after 3 consecutive out-of-range readings, filtering transient
		 * reflections. Downstream uses APERS=3 too, but we keep APERS=1
		 * (existing tcs3472 default) for backward compatibility.
		 */
		data->ppers = 3;

		/* Read proximity thresholds from hardware */
		ret = i2c_smbus_read_word_data(data->client, TCS3472_PILT);
		if (ret < 0)
			return ret;
		data->prox_low_thresh = ret;

		ret = i2c_smbus_read_word_data(data->client, TCS3472_PIHT);
		if (ret < 0)
			return ret;
		data->prox_high_thresh = ret;

		/* vled regulator for IR LED — optional */
		ret = devm_regulator_get_enable_optional(&client->dev, "vled");
		if (ret && ret != -ENODEV)
			return dev_err_probe(&client->dev, ret,
					     "failed to get vled regulator\n");
	}

	data->apers = 1;
	ret = i2c_smbus_write_byte_data(data->client, TCS3472_PERS,
					(data->ppers << 4) | data->apers);
	if (ret < 0)
		return ret;

	ret = i2c_smbus_read_byte_data(data->client, TCS3472_ENABLE);
	if (ret < 0)
		return ret;

	/* enable device */
	data->enable = ret | TCS3472_ENABLE_PON | TCS3472_ENABLE_AEN;
	data->enable &= ~TCS3472_ENABLE_AIEN;
	ret = i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE,
		data->enable);
	if (ret < 0)
		return ret;

	data->enable_saved = data->enable;

	ret = iio_triggered_buffer_setup(indio_dev, NULL,
		tcs3472_trigger_handler, &tcs3472_buffer_setup_ops);
	if (ret < 0)
		return ret;

	if (client->irq) {
		ret = request_threaded_irq(client->irq, NULL,
					   tcs3472_event_handler,
					   IRQF_SHARED | IRQF_ONESHOT,
					   client->name, indio_dev);
		if (ret)
			goto buffer_cleanup;
	}

	ret = iio_device_register(indio_dev);
	if (ret < 0)
		goto free_irq;

	return 0;

free_irq:
	if (client->irq)
		free_irq(client->irq, indio_dev);
buffer_cleanup:
	iio_triggered_buffer_cleanup(indio_dev);
	return ret;
}

static int tcs3472_powerdown(struct tcs3472_data *data)
{
	int ret;

	guard(mutex)(&data->lock);

	data->enable_saved = data->enable;
	ret = i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE, 0x00);
	if (!ret)
		data->enable = 0;

	return ret;
}

static void tcs3472_remove(struct i2c_client *client)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(client);

	iio_device_unregister(indio_dev);
	if (client->irq)
		free_irq(client->irq, indio_dev);
	iio_triggered_buffer_cleanup(indio_dev);
	tcs3472_powerdown(iio_priv(indio_dev));
}

static int tcs3472_suspend(struct device *dev)
{
	struct tcs3472_data *data = iio_priv(i2c_get_clientdata(
		to_i2c_client(dev)));
	return tcs3472_powerdown(data);
}

static int tcs3472_resume(struct device *dev)
{
	struct tcs3472_data *data = iio_priv(i2c_get_clientdata(
		to_i2c_client(dev)));
	int ret;

	guard(mutex)(&data->lock);

	/* Write PON first, then wait for oscillator warm-up */
	ret = i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE,
					TCS3472_ENABLE_PON);
	if (ret)
		return ret;

	usleep_range(2500, 3000);

	/* Restore all configuration registers */
	i2c_smbus_write_byte_data(data->client, TCS3472_ATIME, data->atime);
	i2c_smbus_write_byte_data(data->client, TCS3472_WTIME, 0xff);
	i2c_smbus_write_word_data(data->client, TCS3472_AILT, data->low_thresh);
	i2c_smbus_write_word_data(data->client, TCS3472_AIHT, data->high_thresh);
	if (data->chip_info->has_proximity) {
		i2c_smbus_write_word_data(data->client, TCS3472_PILT,
					  data->prox_low_thresh);
		i2c_smbus_write_word_data(data->client, TCS3472_PIHT,
					  data->prox_high_thresh);
		i2c_smbus_write_byte_data(data->client, TCS3472_PPULSE,
					  data->ppulse);
	}
	i2c_smbus_write_byte_data(data->client, TCS3472_PERS,
				  (data->ppers << 4) | data->apers);
	i2c_smbus_write_byte_data(data->client, TCS3472_CONFIG, 0x00);
	i2c_smbus_write_byte_data(data->client, TCS3472_CONTROL, data->control);

	/* Clear stale interrupts before re-enabling sources */
	i2c_smbus_read_byte_data(data->client, TCS3472_ALL_INTR_CLEAR);

	/* Restore ENABLE last — re-activates AEN, interrupt enables, etc. */
	ret = i2c_smbus_write_byte_data(data->client, TCS3472_ENABLE,
					data->enable_saved);
	if (!ret)
		data->enable = data->enable_saved;

	return ret;
}

static DEFINE_SIMPLE_DEV_PM_OPS(tcs3472_pm_ops, tcs3472_suspend,
				tcs3472_resume);

static const struct of_device_id tcs3472_of_match[] = {
	{ .compatible = "amstaos,tcs3472",
	  .data = &tcs3472_chip_info },
	{ .compatible = "amstaos,tmd3782",
	  .data = &tmd3782_chip_info },
	{ }
};
MODULE_DEVICE_TABLE(of, tcs3472_of_match);

static const struct i2c_device_id tcs3472_id[] = {
	{ "tcs3472", (kernel_ulong_t)&tcs3472_chip_info },
	{ "tmd3782", (kernel_ulong_t)&tmd3782_chip_info },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tcs3472_id);

static struct i2c_driver tcs3472_driver = {
	.driver = {
		.name	= TCS3472_DRV_NAME,
		.pm	= pm_sleep_ptr(&tcs3472_pm_ops),
		.of_match_table = tcs3472_of_match,
	},
	.probe		= tcs3472_probe,
	.remove		= tcs3472_remove,
	.id_table	= tcs3472_id,
};
module_i2c_driver(tcs3472_driver);

MODULE_AUTHOR("Peter Meerwald <pmeerw@pmeerw.net>");
MODULE_DESCRIPTION("TCS3472/TMD3782 color light and proximity sensors driver");
MODULE_LICENSE("GPL");
