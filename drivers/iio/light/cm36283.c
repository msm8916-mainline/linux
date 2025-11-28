// SPDX-License-Identifier: GPL-2.0-only

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/iio/events.h>
#include <linux/iio/iio.h>
#include <linux/iio/types.h>

/* Device registers */
#define CM36283_REG_ALS_CONF		0x00
#define CM36283_REG_PS_CONF1		0x03
#define CM36283_REG_PS_CONF3		0x04
#define CM36283_REG_PS_THD		0x06
#define CM36686_REG_PS_THDL		0x06
#define CM36686_REG_PS_THDH		0x07
#define CM36283_REG_PS_DATA		0x08
#define CM36283_REG_ALS_DATA		0x09
#define CM36283_REG_INT_FLAG		0x0B
#define CM36283_REG_ID_FLAG		0x0C

/* ALS_CONF */
#define CM36283_ALS_IT			GENMASK(7, 6)
#define CM36283_ALS_GAIN		GENMASK(3, 2)
#define CM36283_ALS_INT_EN		BIT(1)
#define CM36283_ALS_SD			BIT(0)

/* PS_CONF1 bitfields for cm36283 */
#define CM36283_PS_DR			GENMASK(7, 6)
#define CM36283_PS_IT			GENMASK(5, 4)
#define CM36283_PS_PERS			GENMASK(3, 2)
#define CM36283_PS_RES_1		BIT(1)
#define CM36283_PS_SD			BIT(0)

#define CM36283_PS_INT_IN		BIT(8)
#define CM36283_PS_INT_OUT		BIT(9)

/* PS_CONF1 bitfields for cm36686 */
#define CM36686_PS_DR			GENMASK(7, 6)
#define CM36686_PS_PERS			GENMASK(5, 4)
#define CM36686_PS_IT			GENMASK(3, 1)
#define CM36686_PS_SD			BIT(0)

#define CM36686_PS_INT_IN		BIT(9)
#define CM36686_PS_INT_OUT		BIT(8)

#define CM36283_PS_ITB			GENMASK(15, 14)

/* PS_CONF3 bitfields for cm36283 */
#define CM36283_PS_MS			BIT(14)
#define CM36283_PS_PROL			GENMASK(13, 12)
#define CM36283_PS_SMART_PERS_ENABLE	BIT(4)
#define CM36283_PS_ACTIVE_FORCE_MODE	BIT(3)
#define CM36283_PS_ACTIVE_FORCE_TRIG	BIT(2)

/* PS_CONF3 bitfields for cm36686 */
#define CM36686_PS_SMART_PERS_ENABLE	BIT(4)

#define CM36686_LED_I			GENMASK(10, 8)

/* INT_FLAG */
#define CM36283_PS_IF			GENMASK(9, 8)

/* Default values */
#define CM36283_ALS_ENABLE		0x00
#define CM36283_PS_DR_1_320		FIELD_PREP_CONST(CM36283_PS_DR, 3)
#define CM36283_PS_IT_1_3T		FIELD_PREP_CONST(CM36283_PS_IT, 1)
#define CM36283_PS_PERS_2		FIELD_PREP_CONST(CM36283_PS_PERS, 1)

#define CM36686_PS_DR_1_320		FIELD_PREP_CONST(CM36283_PS_DR, 3)
#define CM36686_PS_PERS_2		FIELD_PREP_CONST(CM36686_PS_PERS, 1)
#define CM36686_PS_IT_2_5T		FIELD_PREP_CONST(CM36686_PS_IT, 3)
#define CM36686_LED_I_100		FIELD_PREP_CONST(CM36686_LED_I, 2)

/* Shifts */
#define CM36283_PS_IT_SHIFT		3
#define CM36686_PS_IT_SHIFT		5
#define CM36283_INT_FLAG_SHIFT		8
#define CM36283_PS_THDH_SHIFT		8

/* Max proximity thresholds */
#define CM36283_MAX_PS_VALUE		(BIT(8) - 1)
#define CM36686_MAX_PS_VALUE		(BIT(12) - 1)

enum cm36283_model {
	CM36283_ID = 0x83,
	CM36686_ID = 0x86,
	CM36672P_ID = 0x86
};

enum {
	CM36283,
	CM36686,
	CM36672P,
};

enum cm36283_distance {
	CM36283_AWAY = 1,
	CM36283_CLOSE,
	CM36283_BOTH
};

enum {
	CM36283_PS_CONF1,
	CM36283_PS_CONF3,
	CM36283_PS_CONF_NUM
};

enum {
	CM36283_SUPPLY_VDD,
	CM36283_SUPPLY_VIO,
	CM36283_SUPPLY_NUM,
};

static const int cm36283_als_it_times[][2] = {
	{0, 80000},
	{0, 160000},
	{0, 320000},
	{0, 640000}
};

static const int cm36283_ps_it_times[][2] = {
	{0, 320},
	{0, 420},
	{0, 520},
	{0, 640}
};

static const int cm36686_ps_it_times[][2] = {
	{0, 320},
	{0, 480},
	{0, 640},
	{0, 800},
	{0, 960},
	{0, 1120},
	{0, 1280},
	{0, 2560}
};

static const int cm36686_ps_led_current[] = {
	50,
	75,
	100,
	120,
	140,
	160,
	180,
	200
};

struct cm36283_data {
	struct mutex lock;
	struct i2c_client *client;
	struct regulator_bulk_data supplies[CM36283_SUPPLY_NUM];
	struct cm36283_chip_info *chip_info;
	int ps_close;
	int ps_away;
};

struct cm36283_chip_info {
	u8 partid;
	const char *name;
	const struct iio_chan_spec *channels;
	const int num_channels;
	const char **supplies;
	const int num_supplies;
	int als_conf;
	const int(*als_it_times)[][2];
	const int num_als_it;
	const int(*ps_it_times)[][2];
	const int num_ps_it;
	int ps_it_mask;
	int ps_it_shift;
	int ps_conf[CM36283_PS_CONF_NUM];
	int (*set_ps_thd)(struct cm36283_data *chip, int *thd, int val);
	int max_ps_value;
};

static int cm36283_current_to_index(int led_current)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cm36686_ps_led_current); i++)
		if (led_current < cm36686_ps_led_current[i])
			break;

	return i > 0 ? i - 1 : -EINVAL;
}

static ssize_t cm36283_read_near_level(struct iio_dev *indio_dev,
				       uintptr_t priv,
				       const struct iio_chan_spec *chan,
				       char *buf)
{
	struct cm36283_data *chip = iio_priv(indio_dev);

	return sprintf(buf, "%u\n", chip->ps_close);
}

static ssize_t cm36283_read_far_level(struct iio_dev *indio_dev,
				       uintptr_t priv,
				       const struct iio_chan_spec *chan,
				       char *buf)
{
	struct cm36283_data *chip = iio_priv(indio_dev);

	return sprintf(buf, "%u\n", chip->ps_away);
}

static const struct iio_chan_spec_ext_info cm36283_ext_info[] = {
	{
		.name = "nearlevel",
		.shared = IIO_SEPARATE,
		.read = cm36283_read_near_level,
	},
	{
		.name = "farlevel",
		.shared = IIO_SEPARATE,
		.read = cm36283_read_far_level,
	},
	{}
};

static const struct iio_event_spec cm36283_proximity_event_spec[] = {
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_FALLING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE) |
				 BIT(IIO_EV_INFO_ENABLE),
	},
	{
		.type = IIO_EV_TYPE_THRESH,
		.dir = IIO_EV_DIR_RISING,
		.mask_separate = BIT(IIO_EV_INFO_VALUE) |
				 BIT(IIO_EV_INFO_ENABLE),
	}
};

static const struct iio_chan_spec cm36283_channels[] = {
	{
		.type = IIO_LIGHT,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_INT_TIME),
		.info_mask_separate_available = BIT(IIO_CHAN_INFO_INT_TIME),
		.address = CM36283_REG_ALS_DATA,
	},
	{
		.type = IIO_PROXIMITY,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_INT_TIME),
		.info_mask_separate_available = BIT(IIO_CHAN_INFO_INT_TIME),
		.address = CM36283_REG_PS_DATA,
		.event_spec = cm36283_proximity_event_spec,
		.num_event_specs = ARRAY_SIZE(cm36283_proximity_event_spec),
		.ext_info = cm36283_ext_info
	}
};

static const struct iio_chan_spec cm36672p_channels[] = {
	{
		.type = IIO_PROXIMITY,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_INT_TIME),
		.info_mask_separate_available = BIT(IIO_CHAN_INFO_INT_TIME),
		.address = CM36283_REG_PS_DATA,
		.event_spec = cm36283_proximity_event_spec,
		.num_event_specs = ARRAY_SIZE(cm36283_proximity_event_spec),
		.ext_info = cm36283_ext_info
	}
};

static int cm36283_read_avail(struct iio_dev *indio_dev,
			      struct iio_chan_spec const *chan,
			      const int **vals, int *type, int *length,
			      long mask)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	struct cm36283_chip_info *info = chip->chip_info;

	if (mask != IIO_CHAN_INFO_INT_TIME)
		return -EINVAL;

	switch (chan->type) {
	case IIO_LIGHT:
		*vals = (int *)(info->als_it_times);
		*length = 2 * info->num_als_it;
		*type = IIO_VAL_INT_PLUS_MICRO;
		return IIO_AVAIL_LIST;
	case IIO_PROXIMITY:
		*vals = (int *)(info->ps_it_times);
		*length = 2 * info->num_ps_it;
		*type = IIO_VAL_INT_PLUS_MICRO;
		return IIO_AVAIL_LIST;
	default:
		return -EINVAL;
	}
}

static int cm36283_read_channel(struct cm36283_data *chip,
				struct iio_chan_spec const *chan, int *val)
{
	struct i2c_client *client = chip->client;
	int ret = IIO_VAL_INT;

	int data = i2c_smbus_read_word_data(client, chan->address);

	if (data < 0) {
		dev_err(&client->dev, "Failed to read register: %pe", ERR_PTR(data));
		ret = -EIO;
	} else {
		*val = data;
	}
	return ret;
}

static int cm36283_read_int_time(struct cm36283_data *chip,
				 struct iio_chan_spec const *chan, int *val,
				 int *val2)
{
	int als_it_index, ps_it_index;
	struct cm36283_chip_info *info = chip->chip_info;

	switch (chan->type) {
	case IIO_LIGHT:
		als_it_index = FIELD_GET(CM36283_ALS_IT, info->als_conf);
		*val = (*info->als_it_times)[als_it_index][0];
		*val2 = (*info->als_it_times)[als_it_index][1];
		return IIO_VAL_INT_PLUS_MICRO;
	case IIO_PROXIMITY:
		ps_it_index = info->ps_conf[CM36283_PS_CONF1] & info->ps_it_mask;
		ps_it_index >>= info->ps_it_shift;
		*val = (*info->ps_it_times)[ps_it_index][0];
		*val2 = (*info->ps_it_times)[ps_it_index][1];
		return IIO_VAL_INT_PLUS_MICRO;
	default:
		return -EINVAL;
	}
}

static int cm36283_write_light_int_time(struct cm36283_data *chip, int val2)
{
	struct i2c_client *client = chip->client;
	struct cm36283_chip_info *info = chip->chip_info;
	int index = -1, ret, new_int_time;

	for (int i = 0; i < info->num_als_it; i++) {
		if ((*info->als_it_times)[i][1] == val2) {
			index = i;
			break;
		}
	}

	if (index == -1)
		return -EINVAL;

	new_int_time = info->als_conf & ~CM36283_ALS_IT;
	new_int_time |= FIELD_PREP(CM36283_ALS_IT, index);

	ret = i2c_smbus_write_word_data(chip->client, CM36283_REG_ALS_CONF,
					new_int_time);
	if (ret < 0)
		dev_err(&client->dev,
			"Failed to set ALS integration time: %pe", ERR_PTR(ret));
	else
		info->als_conf = new_int_time;

	return ret;
}

static int cm36283_write_prox_int_time(struct cm36283_data *chip, int val2)
{
	struct i2c_client *client = chip->client;
	struct cm36283_chip_info *info = chip->chip_info;
	int index = -1, ret, new_int_time;

	for (int i = 0; i < info->num_ps_it; i++) {
		if ((*info->ps_it_times)[i][1] == val2) {
			index = i;
			break;
		}
	}

	if (index == -1)
		return -EINVAL;

	new_int_time = info->ps_conf[CM36283_PS_CONF1] & ~info->ps_it_mask;
	new_int_time |= index << info->ps_it_shift;

	ret = i2c_smbus_write_word_data(chip->client, CM36283_REG_PS_CONF1,
					new_int_time);
	if (ret < 0)
		dev_err(&client->dev, "Failed to set PS integration time: %pe",
			ERR_PTR(ret));
	else
		info->ps_conf[CM36283_PS_CONF1] = new_int_time;

	return ret;
}

static int cm36283_read_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan, int *val,
			    int *val2, long mask)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	int ret;

	mutex_lock(&chip->lock);

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		ret = cm36283_read_channel(chip, chan, val);
		break;
	case IIO_CHAN_INFO_INT_TIME:
		ret = cm36283_read_int_time(chip, chan, val, val2);
		break;
	default:
		ret = -EINVAL;
	}

	mutex_unlock(&chip->lock);
	return ret;
}

static int cm36283_write_raw(struct iio_dev *indio_dev,
			     struct iio_chan_spec const *chan, int val,
			     int val2, long mask)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	int ret;

	if (val) /* Integration time more than 1s is not supported */
		return -EINVAL;

	if (mask != IIO_CHAN_INFO_INT_TIME)
		return -EINVAL;

	mutex_lock(&chip->lock);

	switch (chan->type) {
	case IIO_LIGHT:
		ret = cm36283_write_light_int_time(chip, val2);
		break;
	case IIO_PROXIMITY:
		ret = cm36283_write_prox_int_time(chip, val2);
		break;
	default:
		ret = -EINVAL;
	}

	mutex_unlock(&chip->lock);
	return ret;
}

static int cm36283_set_prox_thresh(struct cm36283_data *chip, int *thd, int val)
{
	struct i2c_client *client = chip->client;
	int ret = 0, ps_data = chip->ps_close << CM36283_PS_THDH_SHIFT |
		chip->ps_away;

	if (thd == &chip->ps_away)
		ps_data = (ps_data & 0xff00) | val;
	else if (thd == &chip->ps_close)
		ps_data = (ps_data & 0xff) | (val << CM36283_PS_THDH_SHIFT);
	else
		return -EINVAL;

	ret = i2c_smbus_write_word_data(client, CM36283_REG_PS_THD, ps_data);
	if (!ret)
		*thd = val;

	return ret;
}

static int cm36686_set_prox_thresh(struct cm36283_data *chip, int *thd, int val)
{
	struct i2c_client *client = chip->client;
	int ret = 0, address;

	if (thd == &chip->ps_away)
		address = CM36686_REG_PS_THDL;
	else if (thd == &chip->ps_close)
		address = CM36686_REG_PS_THDH;
	else
		return -EINVAL;

	ret = i2c_smbus_write_word_data(client, address, val);
	if (!ret)
		*thd = val;

	return ret;
}

static int cm36283_read_prox_thresh(struct iio_dev *indio_dev,
				    const struct iio_chan_spec *chan,
				    enum iio_event_type type,
				    enum iio_event_direction dir,
				    enum iio_event_info info, int *val,
				    int *val2)
{
	struct cm36283_data *chip = iio_priv(indio_dev);

	if (chan->type != IIO_PROXIMITY)
		return -EINVAL;

	switch (dir) {
	case IIO_EV_DIR_RISING:
		*val = chip->ps_close;
		break;
	case IIO_EV_DIR_FALLING:
		*val = chip->ps_away;
		break;
	default:
		return -EINVAL;
	}

	return IIO_VAL_INT;
}

static int cm36283_write_prox_thresh(struct iio_dev *indio_dev,
				     const struct iio_chan_spec *chan,
				     enum iio_event_type type,
				     enum iio_event_direction dir,
				     enum iio_event_info info, int val,
				     int val2)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	struct i2c_client *client = chip->client;
	struct cm36283_chip_info *chip_info = chip->chip_info;
	int ret = 0;

	if (chan->type != IIO_PROXIMITY)
		return -EINVAL;

	switch (dir) {
	case IIO_EV_DIR_FALLING:
		if (val > chip->ps_close || val < 0)
			return -EINVAL;

		mutex_lock(&chip->lock);
		ret = chip_info->set_ps_thd(chip, &chip->ps_away, val);
		mutex_unlock(&chip->lock);
		break;
	case IIO_EV_DIR_RISING:
		if (val < chip->ps_away || val > chip_info->max_ps_value)
			return -EINVAL;

		mutex_lock(&chip->lock);
		ret = chip_info->set_ps_thd(chip, &chip->ps_close, val);
		mutex_unlock(&chip->lock);
		break;
	default:
		return -EINVAL;
	}

	if (ret < 0)
		dev_err(&client->dev,
			"Failed to set PS threshold value: %pe", ERR_PTR(ret));

	return ret;
}

static int cm36283_read_prox_event_config(struct iio_dev *indio_dev,
					  const struct iio_chan_spec *chan,
					  enum iio_event_type type,
					  enum iio_event_direction dir)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	struct cm36283_chip_info *info = chip->chip_info;

	if (chan->type != IIO_PROXIMITY)
		return -EINVAL;

	switch (dir) {
	case IIO_EV_DIR_FALLING:
		return FIELD_GET(CM36283_PS_INT_OUT, info->ps_conf[CM36283_PS_CONF1]);
	case IIO_EV_DIR_RISING:
		return FIELD_GET(CM36283_PS_INT_IN, info->ps_conf[CM36283_PS_CONF1]);
	default:
		return -EINVAL;
	}
}

static int cm36283_write_prox_event_config(struct iio_dev *indio_dev,
					   const struct iio_chan_spec *chan,
					   enum iio_event_type type,
					   enum iio_event_direction dir,
					   bool state)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	struct i2c_client *client = chip->client;
	struct cm36283_chip_info *info = chip->chip_info;
	int ret = 0, new_ps_conf;

	if (chan->type != IIO_PROXIMITY)
		return -EINVAL;

	switch (dir) {
	case IIO_EV_DIR_FALLING:
		new_ps_conf = info->ps_conf[CM36283_PS_CONF1] & ~CM36283_PS_INT_OUT;
		new_ps_conf |= FIELD_PREP(CM36283_PS_INT_OUT, state);
		break;
	case IIO_EV_DIR_RISING:
		new_ps_conf = info->ps_conf[CM36283_PS_CONF1] & ~CM36283_PS_INT_IN;
		new_ps_conf |= FIELD_PREP(CM36283_PS_INT_IN, state);
		break;
	default:
		return -EINVAL;
	}

	mutex_lock(&chip->lock);

	ret = i2c_smbus_write_word_data(chip->client, CM36283_REG_PS_CONF1, new_ps_conf);
	if (ret < 0)
		dev_err(&client->dev,
			"Failed to set proximity event interrupt config: %pe", ERR_PTR(ret));
	else
		info->ps_conf[CM36283_PS_CONF1] = new_ps_conf;

	mutex_unlock(&chip->lock);

	return ret;
}

static int cm36283_fallback_read_ps(struct iio_dev *indio_dev)
{
	struct cm36283_data *chip = iio_priv(indio_dev);
	struct i2c_client *client = chip->client;
	int data = i2c_smbus_read_word_data(client, CM36283_REG_PS_DATA);

	if (data < 0)
		return data;

	if (data < chip->ps_away)
		return IIO_EV_DIR_FALLING;
	else if (data > chip->ps_close)
		return IIO_EV_DIR_RISING;
	else
		return IIO_EV_DIR_EITHER;
}

static irqreturn_t cm36283_irq_handler(int irq, void *data)
{
	struct iio_dev *indio_dev = data;
	struct cm36283_data *chip = iio_priv(indio_dev);
	struct i2c_client *client = chip->client;
	int ev_dir, ret;
	u64 ev_code;

	/* Reading the interrupt flag acknowledges the interrupt */
	ret = i2c_smbus_read_word_data(client, CM36283_REG_INT_FLAG);
	if (ret < 0) {
		dev_err(&client->dev,
			"Interrupt flag register read failed: %pe", ERR_PTR(ret));
		return IRQ_HANDLED;
	}

	ret >>= CM36283_INT_FLAG_SHIFT;
	switch (ret) {
	case CM36283_CLOSE:
		ev_dir = IIO_EV_DIR_RISING;
		break;
	case CM36283_AWAY:
		ev_dir = IIO_EV_DIR_FALLING;
		break;
	case CM36283_BOTH:
		ev_dir = cm36283_fallback_read_ps(indio_dev);
		if (ev_dir < 0) {
			dev_err(&client->dev, "Failed to settle interrupt state: %pe",
				ERR_PTR(ret));
			return IRQ_HANDLED;
		}
		break;
	default:
		dev_err(&client->dev, "Unknown interrupt state: %x", ret);
		return IRQ_HANDLED;
	}
	ev_code = IIO_UNMOD_EVENT_CODE(IIO_PROXIMITY, IIO_EV_INFO_VALUE,
				       IIO_EV_TYPE_THRESH, ev_dir);

	iio_push_event(indio_dev, ev_code, iio_get_time_ns(indio_dev));
	return IRQ_HANDLED;
}

static const struct iio_info cm36283_info = {
	.read_avail =		cm36283_read_avail,
	.read_raw =		cm36283_read_raw,
	.write_raw =		cm36283_write_raw,
	.read_event_value =	cm36283_read_prox_thresh,
	.write_event_value =	cm36283_write_prox_thresh,
	.read_event_config =	cm36283_read_prox_event_config,
	.write_event_config =	cm36283_write_prox_event_config,
};

static struct cm36283_chip_info cm36283_chip_info_tbl[] = {
	[CM36283] = {
		.partid = CM36283_ID,
		.name = "cm36283",
		.channels = cm36283_channels,
		.num_channels = ARRAY_SIZE(cm36283_channels),
		.supplies = (const char *[]) { "vdd", "vddio" },
		.num_supplies = 2,
		.als_conf = CM36283_ALS_ENABLE,
		.als_it_times = &cm36283_als_it_times,
		.num_als_it = ARRAY_SIZE(cm36283_als_it_times),
		.ps_it_times = &cm36283_ps_it_times,
		.num_ps_it = ARRAY_SIZE(cm36283_ps_it_times),
		.ps_it_mask = CM36283_PS_IT,
		.ps_it_shift = CM36283_PS_IT_SHIFT,
		.ps_conf = {
			CM36283_PS_INT_IN | CM36283_PS_INT_OUT |
			CM36283_PS_DR_1_320 | CM36283_PS_IT_1_3T |
			CM36283_PS_PERS_2,
			CM36283_PS_SMART_PERS_ENABLE
		},
		.set_ps_thd = cm36283_set_prox_thresh,
		.max_ps_value = CM36283_MAX_PS_VALUE,
		.channels = cm36283_channels,
	},
	[CM36686] = {
		.partid = CM36686_ID,
		.name = "cm36686",
		.channels = cm36283_channels,
		.num_channels = ARRAY_SIZE(cm36283_channels),
		.supplies = (const char *[]) { "vdd", "vddio", "vled" },
		.num_supplies = 3,
		.als_conf = CM36283_ALS_ENABLE,
		.als_it_times = &cm36283_als_it_times,
		.num_als_it = ARRAY_SIZE(cm36283_als_it_times),
		.ps_it_times = &cm36686_ps_it_times,
		.num_ps_it = ARRAY_SIZE(cm36686_ps_it_times),
		.ps_it_mask = CM36686_PS_IT,
		.ps_it_shift = CM36686_PS_IT_SHIFT,
		.ps_conf = {
			CM36686_PS_INT_IN | CM36686_PS_INT_OUT |
			CM36686_PS_DR_1_320 | CM36686_PS_IT_2_5T |
			CM36686_PS_PERS_2,
			CM36686_PS_SMART_PERS_ENABLE,
		},
		.set_ps_thd = cm36686_set_prox_thresh,
		.max_ps_value = CM36686_MAX_PS_VALUE,
	},
	[CM36672P] = {
		.partid = CM36672P_ID,
		.name = "cm36672p",
		.channels = cm36672p_channels,
		.num_channels = ARRAY_SIZE(cm36672p_channels),
		.supplies = (const char *[]) { "vdd", "vddio", "vled" },
		.num_supplies = 3,
		.ps_it_times = &cm36686_ps_it_times,
		.num_ps_it = ARRAY_SIZE(cm36686_ps_it_times),
		.ps_it_mask = CM36686_PS_IT,
		.ps_it_shift = CM36686_PS_IT_SHIFT,
		.ps_conf = {
			CM36686_PS_INT_IN | CM36686_PS_INT_OUT |
			CM36686_PS_DR_1_320 | CM36686_PS_IT_2_5T |
			CM36686_PS_PERS_2,
			CM36686_PS_SMART_PERS_ENABLE,
		},
		.set_ps_thd = cm36686_set_prox_thresh,
		.max_ps_value = CM36686_MAX_PS_VALUE,

	},
};

static int cm36283_setup(struct cm36283_data *chip)
{
	struct i2c_client *client = chip->client;
	struct cm36283_chip_info *info = chip->chip_info;
	struct iio_dev *indio_dev = i2c_get_clientdata(client);
	int ret, led_current, led_index;

	indio_dev->name = info->name;

	ret = i2c_smbus_write_word_data(client, CM36283_REG_ALS_CONF,
					info->als_conf);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to enable ambient light sensor: %pe", ERR_PTR(ret));
		return ret;
	}

	ret = i2c_smbus_write_word_data(client, CM36283_REG_PS_CONF1,
					info->ps_conf[CM36283_PS_CONF1]);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to enable proximity sensor: %pe", ERR_PTR(ret));
		return ret;
	}

	ret = device_property_read_u32(&client->dev, "capella,proximity-led-current", &led_current);
	if (!ret) {
		led_index = cm36283_current_to_index(led_current);
		if (led_index < 0) {
			dev_err(&client->dev, "No appropriate current for IR LED found.");
			return led_index;
		}

		info->ps_conf[CM36283_PS_CONF3] &= ~CM36686_LED_I;
		info->ps_conf[CM36283_PS_CONF3] |= FIELD_PREP(CM36686_LED_I, led_index);
	}

	ret = i2c_smbus_write_word_data(client, CM36283_REG_PS_CONF3,
					info->ps_conf[CM36283_PS_CONF3]);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to enable proximity sensor: %pe", ERR_PTR(ret));
		return ret;
	}

	ret = device_property_read_u32(&client->dev, "proximity-near-level",
					    &chip->ps_close);
	if (ret < 0)
		chip->ps_close = 0;

	ret = info->set_ps_thd(chip, &chip->ps_close, chip->ps_close);
	if (ret < 0) {
		dev_err(&client->dev,
			"Failed to set close proximity threshold: %pe", ERR_PTR(ret));
		return ret;
	}

	ret = info->set_ps_thd(chip, &chip->ps_away, chip->ps_away);
	if (ret < 0) {
		dev_err(&client->dev,
			"Failed to set away proximity threshold: %pe", ERR_PTR(ret));
		return ret;
	}

	return 0;
}

static void cm36283_shutdown(void *data)
{
	struct cm36283_data *chip = data;
	struct i2c_client *client = chip->client;
	int ret, als_shutdown, ps_shutdown;

	als_shutdown = chip->chip_info->als_conf | CM36283_ALS_SD;

	ret = i2c_smbus_write_word_data(client, CM36283_REG_ALS_CONF,
					als_shutdown);
	if (ret < 0)
		dev_err(&client->dev, "Failed to shutdown ALS");

	ps_shutdown = chip->chip_info->ps_conf[CM36283_PS_CONF1] | CM36283_PS_SD;

	ret = i2c_smbus_write_word_data(client, CM36283_REG_PS_CONF1,
					ps_shutdown);
	if (ret < 0)
		dev_err(&client->dev, "Failed to shutdown PS");
}

static int cm36283_probe(struct i2c_client *client)
{
	struct iio_dev *indio_dev;
	struct cm36283_data *chip;
	int ret;

	indio_dev = devm_iio_device_alloc(&client->dev,
					  sizeof(struct cm36283_data));
	if (!indio_dev)
		return -ENOMEM;

	chip = iio_priv(indio_dev);

	const struct i2c_device_id *id = i2c_client_get_device_id(client);

	chip->chip_info = &cm36283_chip_info_tbl[id->driver_data];
	struct cm36283_chip_info *info = chip->chip_info;

	ret = i2c_smbus_read_byte_data(client, CM36283_REG_ID_FLAG);
	if (ret < 0)
		return dev_err_probe(&client->dev, ret, "Failed to read device ID");

	if (info->partid != ret)
		return dev_err_probe(&client->dev, -ENODEV, "Device not recognized!");

	i2c_set_clientdata(client, indio_dev);
	chip->client = client;
	mutex_init(&chip->lock);

	ret = devm_regulator_bulk_get_enable(&client->dev, info->num_supplies, info->supplies);
	if (ret < 0)
		return dev_err_probe(&client->dev, ret,
				     "Failed to enable regulators");

	ret = devm_add_action_or_reset(&client->dev, cm36283_shutdown, chip);
	if (ret)
		return dev_err_probe(&client->dev, ret,
				     "Failed to set shutdown action");

	ret = cm36283_setup(chip);
	if (ret < 0)
		return dev_err_probe(&client->dev, ret,
				     "Failed to set up registers");

	indio_dev->channels = info->channels;
	indio_dev->num_channels = info->num_channels;
	indio_dev->info = &cm36283_info;
	indio_dev->modes = INDIO_DIRECT_MODE;

	ret = devm_request_threaded_irq(&client->dev, client->irq, NULL,
					cm36283_irq_handler,
					IRQF_TRIGGER_LOW | IRQF_ONESHOT,
					indio_dev->name, indio_dev);
	if (ret)
		return dev_err_probe(&client->dev, ret,
				     "Failed to request irq");

	ret = devm_iio_device_register(&client->dev, indio_dev);
	if (ret)
		return dev_err_probe(&client->dev, ret,
				     "Failed to register iio device");

	return 0;
}

static const struct i2c_device_id cm36283_id[] = {
	{ "cm36283", CM36283 },
	{ "cm36686", CM36686 },
	{ "cm36672p", CM36672P },
	{}
};

MODULE_DEVICE_TABLE(i2c, cm36283_id);

static const struct of_device_id cm36283_of_match[] = {
	{ .compatible = "capella,cm36283" },
	{ .compatible = "capella,cm36686" },
	{ .compatible = "capella,cm36672p" },
	{}
};
MODULE_DEVICE_TABLE(of, cm36283_of_match);

static struct i2c_driver cm36283_driver = {
	.driver = {
		.name = "cm36283",
		.of_match_table = cm36283_of_match,
	},
	.probe = cm36283_probe,
	.id_table = cm36283_id
};

module_i2c_driver(cm36283_driver);

MODULE_AUTHOR("Erikas Bitovtas <xerikasxx@gmail.com>");
MODULE_DESCRIPTION("CM36283 ambient light and proximity sensor driver");
MODULE_LICENSE("GPL");
