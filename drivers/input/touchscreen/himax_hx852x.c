// SPDX-License-Identifier: GPL-2.0-only
/*
 * Himax HX852x(ES) Touchscreen Driver
 *
 * Based on the Himax Android Driver Sample Code Ver 0.3 for HMX852xES chipset:
 * Copyright (c) 2014 Himax Corporation.
 */

#include <asm/unaligned.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>

#define HX852X_COORD_SIZE(fingers)	((fingers) * sizeof(struct hx852x_coord))
#define HX852X_WIDTH_SIZE(fingers)	ALIGN(fingers, 4)
#define HX852X_BUF_SIZE(fingers)	(HX852X_COORD_SIZE(fingers) + \
					 HX852X_WIDTH_SIZE(fingers) + \
					 sizeof(struct hx852x_touch_info))

#define HX852X_MAX_FINGERS		12
#define HX852X_MAX_KEY_COUNT		3
#define HX852X_MAX_BUF_SIZE		HX852X_BUF_SIZE(HX852X_MAX_FINGERS)

#define HX852X_TS_SLEEP_IN		{0x80}
#define HX852X_TS_SLEEP_OUT		{0x81}
#define HX852X_TS_SENSE_OFF		{0x82}
#define HX852X_TS_SENSE_ON		{0x83}
#define HX852X_READ_ONE_EVENT		0x85
#define HX852X_READ_ALL_EVENTS		0x86
#define HX852X_READ_LATEST_EVENT	0x87
#define HX852X_CLEAR_EVENT_STACK	{0x88}

#define HX852X_REG_SRAM_SWITCH		0x8C
#define HX852X_REG_SRAM_ADDR		0x8B
#define HX852X_REG_FLASH_RPLACE		0x5A

#define HX852X_ENTER_TEST_MODE_SEQ	{HX852X_REG_SRAM_SWITCH, 0x14}
#define HX852X_LEAVE_TEST_MODE_SEQ	{HX852X_REG_SRAM_SWITCH, 0x00}
#define HX852X_GET_CONFIG_SEQ		{HX852X_REG_SRAM_ADDR, 0x00, 0x70}

#define HX852X_CMD(d, extra...)		{ .data = d, .len = sizeof((u8[])d), extra }
#define HX852X_CMD_MAX_LEN		3

struct hx852x {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct touchscreen_properties props;

	struct gpio_desc *reset_gpiod;
	struct regulator_bulk_data supplies[2];

	int max_fingers;
	int keycount;
	u32 keycodes[HX852X_MAX_KEY_COUNT];

	bool had_finger_pressed;
	u8 last_key;
};

struct hx852x_cmd {
	u8 data[HX852X_CMD_MAX_LEN];
	u8 len;
	u8 sleep_ms;
};

struct hx852x_config {
	u8 rx_num;
	u8 tx_num;
	u8 max_pt;
	u8 padding1[3];
	__be16 x_res;
	__be16 y_res;
	u8 padding2[2];
};

struct hx852x_coord {
	__be16 x;
	__be16 y;
};

struct hx852x_touch_info {
	u8 finger_num;
	__le16 finger_pressed;
	u8 padding;
} __packed;

static const u8 hx852x_internal_keymappings[HX852X_MAX_KEY_COUNT] = {0x01, 0x02, 0x04};

static int hx852x_i2c_read(struct hx852x *hx, u8 cmd, u8 *data,	u8 len)
{
	struct i2c_client *client = hx->client;
	int ret;

	struct i2c_msg msg[] = {
		{
			.addr = client->addr,
			.flags = 0,
			.len = 1,
			.buf = &cmd,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = data,
		}
	};

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
	if (ret != ARRAY_SIZE(msg)) {
		dev_err(&client->dev, "failed to read command %#x: %d\n", cmd, ret);
		return ret;
	}

	return 0;
}

static int hx852x_i2c_write(struct hx852x *hx, struct hx852x_cmd *cmds,
			    unsigned int ncmds)
{
	struct i2c_client *client = hx->client;
	int ret, i;

	struct i2c_msg msg = {
		.addr = client->addr,
		.flags = 0,
	};

	for (i = 0; i < ncmds; i++) {
		msg.len = cmds[i].len;
		msg.buf = cmds[i].data;

		ret = i2c_transfer(client->adapter, &msg, 1);
		if (ret != 1) {
			dev_err(&client->dev,
				"failed to write command %d (%#x): %d\n", i,
				cmds[i].data[0], ret);
			return ret;
		}

		msleep(cmds[i].sleep_ms);
	}

	return 0;
}

static int hx852x_power_on(struct hx852x *hx)
{
	struct device *dev = &hx->client->dev;
	int error;

	struct hx852x_cmd resume_cmd[] = {
		HX852X_CMD(HX852X_TS_SENSE_ON, .sleep_ms = 30),
		HX852X_CMD(HX852X_TS_SLEEP_OUT),
	};

	error = regulator_bulk_enable(ARRAY_SIZE(hx->supplies), hx->supplies);
	if (error < 0) {
		dev_err(dev, "failed to enable regulators: %d\n", error);
		return error;
	}

	gpiod_set_value_cansleep(hx->reset_gpiod, 1);
	msleep(20);
	gpiod_set_value_cansleep(hx->reset_gpiod, 0);
	msleep(20);

	error = hx852x_i2c_write(hx, resume_cmd, ARRAY_SIZE(resume_cmd));
	if (error) {
		regulator_bulk_disable(ARRAY_SIZE(hx->supplies), hx->supplies);
		return error;
	}

	return 0;
}

static void hx852x_power_off(struct hx852x *hx)
{
	struct device *dev = &hx->client->dev;
	int error;

	struct hx852x_cmd sleep_cmd[] = {
		HX852X_CMD(HX852X_TS_SENSE_OFF, .sleep_ms = 40),
		HX852X_CMD(HX852X_TS_SLEEP_IN, .sleep_ms = 10),
	};

	error = hx852x_i2c_write(hx, sleep_cmd, ARRAY_SIZE(sleep_cmd));
	if (error)
		dev_warn(dev, "failed to send sleep commands: %d\n", error);

	error = regulator_bulk_disable(ARRAY_SIZE(hx->supplies), hx->supplies);
	if (error)
		dev_warn(dev, "failed to disable regulators: %d\n", error);
}

static int hx852x_read_config(struct hx852x *hx)
{
	struct hx852x_config conf;
	int x_max, y_max;
	int error;

	struct hx852x_cmd get_conf_cmd[] = {
		HX852X_CMD(HX852X_TS_SENSE_OFF, .sleep_ms = 50),
		HX852X_CMD(HX852X_TS_SLEEP_IN, .sleep_ms = 50),
		HX852X_CMD(HX852X_ENTER_TEST_MODE_SEQ, .sleep_ms = 10),
		HX852X_CMD(HX852X_GET_CONFIG_SEQ, .sleep_ms = 10),
	};
	struct hx852x_cmd end_conf_cmd[] = {
		HX852X_CMD(HX852X_LEAVE_TEST_MODE_SEQ, .sleep_ms = 10),
	};

	error = hx852x_power_on(hx);
	if (error)
		return error;

	msleep(50);

	/* Try to read the touchscreen configuration */
	error = hx852x_i2c_write(hx, get_conf_cmd, ARRAY_SIZE(get_conf_cmd));
	if (error)
		goto err;

	error = hx852x_i2c_read(hx, HX852X_REG_FLASH_RPLACE,
				(u8 *)&conf, sizeof(conf));
	if (error)
		goto err;

	error = hx852x_i2c_write(hx, end_conf_cmd, ARRAY_SIZE(end_conf_cmd));
	if (error)
		goto err;

	hx->max_fingers = (conf.max_pt & 0xF0) >> 4;
	if (hx->max_fingers > HX852X_MAX_FINGERS) {
		dev_err(&hx->client->dev, "max supported fingers: %d, yours: %d\n",
			HX852X_MAX_FINGERS, hx->max_fingers);
		error = -EINVAL;
		goto err;
	}

	x_max = be16_to_cpu(conf.x_res) - 1;
	y_max = be16_to_cpu(conf.y_res) - 1;

	if (x_max && y_max) {
		input_set_abs_params(hx->input_dev, ABS_MT_POSITION_X, 0, x_max, 0, 0);
		input_set_abs_params(hx->input_dev, ABS_MT_POSITION_Y, 0, y_max, 0, 0);
	}

err:
	regulator_bulk_disable(ARRAY_SIZE(hx->supplies), hx->supplies);
	return error;
}

static void hx852x_process_btn_touch(struct hx852x *hx, int current_key)
{
	int i;

	for (i = 0; i < hx->keycount; i++) {
		if (hx852x_internal_keymappings[i] == current_key)
			input_report_key(hx->input_dev, hx->keycodes[i], 1);
		else if (hx852x_internal_keymappings[i] == hx->last_key)
			input_report_key(hx->input_dev, hx->keycodes[i], 0);
	}
	hx->last_key = current_key;
}

static void hx852x_process_display_touch(struct hx852x *hx,
					 struct hx852x_coord *coord, u8 *width,
					 unsigned long finger_pressed)
{
	unsigned int i, x, y, w;

	hx->had_finger_pressed = false;
	for_each_set_bit(i, &finger_pressed, hx->max_fingers) {
		x = be16_to_cpu(coord[i].x);
		y = be16_to_cpu(coord[i].y);
		w = width[i];

		input_mt_slot(hx->input_dev, i);
		input_mt_report_slot_state(hx->input_dev, MT_TOOL_FINGER, 1);
		touchscreen_report_pos(hx->input_dev, &hx->props, x, y, true);
		input_report_abs(hx->input_dev, ABS_MT_TOUCH_MAJOR, w);
		hx->had_finger_pressed = true;
	}
	input_mt_sync_frame(hx->input_dev);
}

static irqreturn_t hx852x_interrupt(int irq, void *ptr)
{
	u8 buf[HX852X_MAX_BUF_SIZE] __aligned(__alignof(struct hx852x_coord));
	struct hx852x *hx = ptr;
	u16 finger_pressed;
	u8 current_key;
	int error;

	struct hx852x_coord *coord = (struct hx852x_coord *)buf;
	u8 *width = &buf[HX852X_COORD_SIZE(hx->max_fingers)];
	struct hx852x_touch_info *info = (struct hx852x_touch_info *)
		&width[HX852X_WIDTH_SIZE(hx->max_fingers)];

	error = hx852x_i2c_read(hx, HX852X_READ_ALL_EVENTS, buf,
				HX852X_BUF_SIZE(hx->max_fingers));
	if (error)
		return IRQ_NONE;

	finger_pressed = get_unaligned_le16(&info->finger_pressed);

	current_key = finger_pressed >> HX852X_MAX_FINGERS;
	if (current_key == 0x0F)
		current_key = 0x00;

	if (info->finger_num == 0xff || !(info->finger_num & 0x0f))
		finger_pressed = 0;

	if (finger_pressed || hx->had_finger_pressed)
		hx852x_process_display_touch(hx, coord, width, finger_pressed);
	else if (hx->keycount && (current_key || hx->last_key))
		hx852x_process_btn_touch(hx, current_key);

	input_sync(hx->input_dev);
	return IRQ_HANDLED;
}

static int hx852x_start(struct hx852x *hx)
{
	int error = hx852x_power_on(hx);

	if (error)
		return error;

	enable_irq(hx->client->irq);
	return 0;
}

static void hx852x_stop(struct hx852x *hx)
{
	disable_irq(hx->client->irq);
	hx852x_power_off(hx);
}

static int hx852x_input_open(struct input_dev *dev)
{
	struct hx852x *hx = input_get_drvdata(dev);

	return hx852x_start(hx);
}

static void hx852x_input_close(struct input_dev *dev)
{
	struct hx852x *hx = input_get_drvdata(dev);

	hx852x_stop(hx);
}

static int hx852x_parse_properties(struct hx852x *hx)
{
	struct device *dev = &hx->client->dev;
	int error;

	hx->keycount = device_property_count_u32(dev, "linux,keycodes");
	if (hx->keycount <= 0) {
		hx->keycount = 0;
		return 0;
	}

	if (hx->keycount > HX852X_MAX_KEY_COUNT) {
		dev_err(dev, "max supported keys: %d, yours: %d\n",
			HX852X_MAX_KEY_COUNT, hx->keycount);
		return -EINVAL;
	}

	error = device_property_read_u32_array(dev, "linux,keycodes",
					       hx->keycodes, hx->keycount);
	if (error)
		dev_err(dev, "failed to read linux,keycodes: %d\n", error);

	return error;
}

static int hx852x_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct hx852x *hx;
	int error, i;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(dev, "plain i2c not supported\n");
		return -ENXIO;
	}

	hx = devm_kzalloc(dev, sizeof(*hx), GFP_KERNEL);
	if (!hx)
		return -ENOMEM;

	hx->client = client;
	hx->input_dev = devm_input_allocate_device(dev);
	if (!hx->input_dev)
		return -ENOMEM;

	hx->input_dev->name = "Himax HX852x";
	hx->input_dev->id.bustype = BUS_I2C;
	hx->input_dev->open = hx852x_input_open;
	hx->input_dev->close = hx852x_input_close;

	i2c_set_clientdata(client, hx);
	input_set_drvdata(hx->input_dev, hx);

	hx->supplies[0].supply = "vcca";
	hx->supplies[1].supply = "vccd";
	error = devm_regulator_bulk_get(dev, ARRAY_SIZE(hx->supplies), hx->supplies);
	if (error < 0)
		return dev_err_probe(dev, error, "failed to get regulators");

	hx->reset_gpiod = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(hx->reset_gpiod))
		return dev_err_probe(dev, error, "failed to get reset gpio");

	error = devm_request_threaded_irq(dev, client->irq, NULL, hx852x_interrupt,
					  IRQF_ONESHOT | IRQF_NO_AUTOEN, NULL, hx);
	if (error) {
		dev_err(dev, "failed to request irq %d: %d\n", client->irq, error);
		return error;
	}

	error = hx852x_read_config(hx);
	if (error)
		return error;

	input_set_capability(hx->input_dev, EV_ABS, ABS_MT_POSITION_X);
	input_set_capability(hx->input_dev, EV_ABS, ABS_MT_POSITION_Y);
	input_set_abs_params(hx->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);

	touchscreen_parse_properties(hx->input_dev, true, &hx->props);
	error = hx852x_parse_properties(hx);
	if (error)
		return error;

	hx->input_dev->keycode = hx->keycodes;
	hx->input_dev->keycodemax = hx->keycount;
	hx->input_dev->keycodesize = sizeof(hx->keycodes[0]);
	for (i = 0; i < hx->keycount; i++)
		input_set_capability(hx->input_dev, EV_KEY, hx->keycodes[i]);

	error = input_mt_init_slots(hx->input_dev, hx->max_fingers,
				    INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (error) {
		dev_err(dev, "failed to init MT slots: %d\n", error);
		return error;
	}

	error = input_register_device(hx->input_dev);
	if (error) {
		dev_err(dev, "failed to register input device: %d\n", error);
		return error;
	}

	return 0;
}

static int __maybe_unused hx852x_suspend(struct device *dev)
{
	struct hx852x *hx = dev_get_drvdata(dev);
	int i;

	mutex_lock(&hx->input_dev->mutex);
	if (input_device_enabled(hx->input_dev))
		hx852x_stop(hx);
	mutex_unlock(&hx->input_dev->mutex);

	if (hx->had_finger_pressed)
		input_mt_sync_frame(hx->input_dev);

	if (hx->last_key) {
		for (i = 0; i < hx->keycount; i++) {
			if (hx852x_internal_keymappings[i] == hx->last_key)
				input_report_key(hx->input_dev, hx->keycodes[i], 0);
		}
	}

	if (hx->had_finger_pressed || hx->last_key)
		input_sync(hx->input_dev);

	hx->last_key = 0;
	hx->had_finger_pressed = false;

	return 0;
}

static int __maybe_unused hx852x_resume(struct device *dev)
{
	struct hx852x *hx = dev_get_drvdata(dev);
	int error = 0;

	mutex_lock(&hx->input_dev->mutex);
	if (input_device_enabled(hx->input_dev))
		error = hx852x_start(hx);
	mutex_unlock(&hx->input_dev->mutex);

	return error;
}

static SIMPLE_DEV_PM_OPS(hx852x_pm_ops, hx852x_suspend, hx852x_resume);

#ifdef CONFIG_OF
static const struct of_device_id hx852x_of_match[] = {
	{ .compatible = "himax,hx852x-es" },
	{ }
};
MODULE_DEVICE_TABLE(of, hx852x_of_match);
#endif

static struct i2c_driver hx852x_driver = {
	.probe_new = hx852x_probe,
	.driver = {
		.name = "himax_hx852x",
		.pm = &hx852x_pm_ops,
		.of_match_table = of_match_ptr(hx852x_of_match),
	},
};
module_i2c_driver(hx852x_driver);

MODULE_DESCRIPTION("Himax HX852x(ES) Touchscreen Driver");
MODULE_AUTHOR("Jonathan Albrieux <jonathan.albrieux@gmail.com>");
MODULE_AUTHOR("Stephan Gerhold <stephan@gerhold.net>");
MODULE_LICENSE("GPL v2");
