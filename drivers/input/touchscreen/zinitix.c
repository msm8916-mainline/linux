// SPDX-License-Identifier: GPL-2.0-only

#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <asm/unaligned.h>

/* Register Map */

#define BT541_SWRESET_CMD			0x0000
#define BT541_WAKEUP_CMD			0x0001

#define BT541_IDLE_CMD				0x0004
#define BT541_SLEEP_CMD				0x0005

#define BT541_CLEAR_INT_STATUS_CMD		0x0003
#define BT541_CALIBRATE_CMD			0x0006
#define BT541_SAVE_STATUS_CMD			0x0007
#define BT541_SAVE_CALIBRATION_CMD		0x0008
#define BT541_RECALL_FACTORY_CMD		0x000f

#define BT541_THRESHOLD				0x0020

#define BT541_LARGE_PALM_REJECT_AREA_TH		0x003F

#define BT541_DEBUG_REG				0x0115 /* 0~7 */

#define BT541_TOUCH_MODE			0x0010
#define BT541_CHIP_REVISION			0x0011
#define BT541_FIRMWARE_VERSION			0x0012

#define ZINITIX_USB_DETECT			0x116

#define BT541_MINOR_FW_VERSION			0x0121

#define BT541_VENDOR_ID				0x001C
#define BT541_HW_ID				0x0014

#define BT541_DATA_VERSION_REG			0x0013
#define BT541_SUPPORTED_FINGER_NUM		0x0015
#define BT541_EEPROM_INFO			0x0018
#define BT541_INITIAL_TOUCH_MODE		0x0019

#define BT541_TOTAL_NUMBER_OF_X			0x0060
#define BT541_TOTAL_NUMBER_OF_Y			0x0061

#define BT541_DELAY_RAW_FOR_HOST		0x007f

#define BT541_BUTTON_SUPPORTED_NUM		0x00B0
#define BT541_BUTTON_SENSITIVITY		0x00B2
#define BT541_DUMMY_BUTTON_SENSITIVITY		0X00C8

#define BT541_X_RESOLUTION			0x00C0
#define BT541_Y_RESOLUTION			0x00C1

#define BT541_POINT_STATUS_REG			0x0080
#define BT541_ICON_STATUS_REG			0x00AA

#define BT541_POINT_COORD_REG			(BT541_POINT_STATUS_REG + 2)

#define BT541_AFE_FREQUENCY			0x0100
#define BT541_DND_N_COUNT			0x0122
#define BT541_DND_U_COUNT			0x0135

#define BT541_RAWDATA_REG			0x0200

#define BT541_EEPROM_INFO_REG			0x0018

#define BT541_INT_ENABLE_FLAG			0x00f0
#define BT541_PERIODICAL_INTERRUPT_INTERVAL	0x00f1

#define BT541_BTN_WIDTH				0x016d

#define BT541_CHECKSUM_RESULT			0x012c

#define BT541_INIT_FLASH			0x01d0
#define BT541_WRITE_FLASH			0x01d1
#define BT541_READ_FLASH			0x01d2

#define ZINITIX_INTERNAL_FLAG_02		0x011e
#define ZINITIX_INTERNAL_FLAG_03		0x011f

#define ZINITIX_I2C_CHECKSUM_WCNT		0x016a
#define ZINITIX_I2C_CHECKSUM_RESULT		0x016c

/* Interrupt & status register flags */

#define BIT_PT_CNT_CHANGE	BIT(0)
#define BIT_DOWN		BIT(1)
#define BIT_MOVE		BIT(2)
#define BIT_UP			BIT(3)
#define BIT_PALM		BIT(4)
#define BIT_PALM_REJECT		BIT(5)
#define BIT_RESERVED_0		BIT(6)
#define BIT_RESERVED_1		BIT(7)
#define BIT_WEIGHT_CHANGE	BIT(8)
#define BIT_PT_NO_CHANGE	BIT(9)
#define BIT_REJECT		BIT(10)
#define BIT_PT_EXIST		BIT(11)
#define BIT_RESERVED_2		BIT(12)
#define BIT_ERROR		BIT(13)
#define BIT_DEBUG		BIT(14)
#define BIT_ICON_EVENT		BIT(15)

#define SUB_BIT_EXIST		BIT(0)
#define SUB_BIT_DOWN		BIT(1)
#define SUB_BIT_MOVE		BIT(2)
#define SUB_BIT_UP		BIT(3)
#define SUB_BIT_UPDATE		BIT(4)
#define SUB_BIT_WAIT		BIT(5)

#define DEFAULT_TOUCH_POINT_MODE	2
#define MAX_SUPPORTED_FINGER_NUM	5

#define CHIP_ON_DELAY	15 // ms
#define FIRMWARE_ON_DELAY 40 // ms

#define DELAY_FOR_TRANSACTION		50 // μs
#define DELAY_FOR_POST_TRANSCATION	10 // μs

struct point_coord {
	__le16	x;
	__le16	y;
	u8	width;
	u8	sub_status;
	// currently unused, but needed as padding:
	u8	minor_width;
	u8	angle;
};

struct touch_event {
	__le16	status;
	union {
		__le16	event_flag; // mode == 1
		struct { // mode == 2
			u8	finger_cnt;
			u8	time_stamp;
		};
	};
	struct point_coord point_coord[MAX_SUPPORTED_FINGER_NUM];
};

struct bt541_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct touchscreen_properties prop;
	struct regulator_bulk_data supplies[2];
	u32 zinitix_mode;
};

static int zinitix_read_data(struct i2c_client *client,
			     u16 reg, u8 *values, size_t length)
{
	int error;
	__le16 reg_le = cpu_to_le16(reg);

	error = i2c_master_send(client, (u8 *)&reg_le, sizeof(reg_le));
	if (error < 0)
		return error;

	udelay(DELAY_FOR_TRANSACTION);
	error = i2c_master_recv(client, values, length);
	if (error < 0)
		return error;
	if (error != length)
		return -EIO;

	udelay(DELAY_FOR_POST_TRANSCATION);
	return 0;
}

static int zinitix_write_data(struct i2c_client *client,
			      u16 reg, u8 *values, size_t length)
{
	int error;
	u8 *packet;
	__le16 reg_le = cpu_to_le16(reg);

	packet = kmalloc(length + sizeof(reg_le), GFP_KERNEL);
	if (!packet)
		return -ENOMEM;
	memcpy(packet, (u8 *)&reg_le, sizeof(reg_le));
	memcpy(packet + sizeof(reg_le), values, length);

	error = i2c_master_send(client, packet, length + sizeof(reg_le));
	kfree(packet);
	if (error < 0)
		return error;

	udelay(DELAY_FOR_POST_TRANSCATION);
	return 0;
}

static int zinitix_write_u16(struct i2c_client *client, u16 reg, u16 value)
{
	__le16 value_le = cpu_to_le16(value);

	return zinitix_write_data(client, reg, (u8 *)&value_le, sizeof(reg));
}

static int zinitix_write_cmd(struct i2c_client *client, u16 reg)
{
	int error;
	__le16 reg_le = cpu_to_le16(reg);

	error = i2c_master_send(client, (u8 *)&reg_le, sizeof(reg_le));
	if (error < 0)
		return error;

	udelay(DELAY_FOR_POST_TRANSCATION);
	return 0;
}

static bool zinitix_init_touch(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;
	int i;
	int error;

	error = zinitix_write_cmd(client, BT541_SWRESET_CMD);
	if (error) {
		dev_err(&client->dev, "Failed to write reset command\n");
		return error;
	}

	error = zinitix_write_u16(client, BT541_INT_ENABLE_FLAG, 0x0);
	if (error) {
		dev_err(&client->dev, "failed to reset interrupt enable flag\n");
		return error;
	}

	/* initialize */
	error = zinitix_write_u16(client, BT541_X_RESOLUTION, (u16)bt541->prop.max_x);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_Y_RESOLUTION, (u16)bt541->prop.max_y);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_SUPPORTED_FINGER_NUM,
				  (u16)MAX_SUPPORTED_FINGER_NUM);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_INITIAL_TOUCH_MODE, bt541->zinitix_mode);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_TOUCH_MODE, bt541->zinitix_mode);
	if (error)
		return error;

	error = zinitix_write_u16(client, BT541_INT_ENABLE_FLAG,
				  BIT_PT_CNT_CHANGE | BIT_DOWN | BIT_MOVE | BIT_UP);
	if (error)
		return error;

	/* clear queue */
	for (i = 0; i < 10; i++) {
		zinitix_write_cmd(client, BT541_CLEAR_INT_STATUS_CMD);
		udelay(10);
	}

	return 0;
}

static int zinitix_init_regulators(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;
	int error;

	bt541->supplies[0].supply = "vdd";
	bt541->supplies[1].supply = "vddo";
	error = devm_regulator_bulk_get(&client->dev, ARRAY_SIZE(bt541->supplies),
					bt541->supplies);
	if (error < 0) {
		dev_err(&client->dev, "Failed to get regulators: %d\n", error);
		return error;
	}

	return 0;
}

static int zinitix_send_power_on_sequence(struct bt541_ts_data *bt541)
{
	int error;
	struct i2c_client *client = bt541->client;

	error = zinitix_write_u16(client, 0xc000, 0x0001);
	if (error) {
		dev_err(&client->dev, "Failed to send power sequence(vendor cmd enable)\n");
		return error;
	}
	udelay(10);

	error = zinitix_write_cmd(client, 0xc004);
	if (error) {
		dev_err(&client->dev, "Failed to send power sequence(intn clear)\n");
		return error;
	}
	udelay(10);

	error = zinitix_write_u16(client, 0xc002, 0x0001);
	if (error) {
		dev_err(&client->dev, "Failed to send power sequence(nvm init)\n");
		return error;
	}
	mdelay(2);

	error = zinitix_write_u16(client, 0xc001, 0x0001);
	if (error) {
		dev_err(&client->dev, "Failed to send power sequence(program start)\n");
		return error;
	}
	msleep(FIRMWARE_ON_DELAY);	/* wait for checksum cal */

	return 0;
}

static irqreturn_t zinitix_ts_irq_handler(int irq, void *bt541_handler)
{
	struct bt541_ts_data *bt541 = (struct bt541_ts_data *)bt541_handler;
	struct i2c_client *client = bt541->client;
	int i;
	int error;
	unsigned long event_flag = 0;
	struct touch_event touch_event;

	memset(&touch_event, 0, sizeof(struct touch_event));

	// in mode 2, we can get away with just one read
	error = zinitix_read_data(bt541->client, BT541_POINT_STATUS_REG,
				  (u8 *)&touch_event, sizeof(struct touch_event));
	if (error) {
		dev_err(&client->dev, "%s: Failed to read point status\n", __func__);

		zinitix_write_cmd(bt541->client, BT541_CLEAR_INT_STATUS_CMD);
		return IRQ_HANDLED;
	}

	if (bt541->zinitix_mode == 1)
		event_flag = le16_to_cpu(touch_event.event_flag);

	for (i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++) {
		if (bt541->zinitix_mode == 1) {
			// in mode 1, we need to read the points one by one
			if (!(event_flag & BIT(i)))
				continue;

			error = zinitix_read_data(bt541->client, BT541_POINT_COORD_REG +
						  (i * sizeof(struct point_coord) / sizeof(u16)),
						  (u8 *)&touch_event.point_coord[i], sizeof(struct point_coord));
			if (error) {
				dev_err(&client->dev, "%s: Failed to read point info\n", __func__);
				goto out;
			}
		}

		if (!(touch_event.point_coord[i].sub_status & SUB_BIT_EXIST))
			continue;

		input_mt_slot(bt541->input_dev, i);
		input_mt_report_slot_state(bt541->input_dev, MT_TOOL_FINGER, true);
		touchscreen_report_pos(bt541->input_dev, &bt541->prop,
				       le16_to_cpu(touch_event.point_coord[i].x),
				       le16_to_cpu(touch_event.point_coord[i].y), true);
		input_report_abs(bt541->input_dev, ABS_MT_TOUCH_MAJOR, touch_event.point_coord[i].width);
	}

	input_mt_sync_frame(bt541->input_dev);
	input_sync(bt541->input_dev);

out:
	zinitix_write_cmd(bt541->client, BT541_CLEAR_INT_STATUS_CMD);

	return IRQ_HANDLED;
}

static int zinitix_init_input_dev(struct bt541_ts_data *bt541)
{
	int error;

	bt541->input_dev = devm_input_allocate_device(&bt541->client->dev);
	if (!bt541->input_dev) {
		dev_err(&bt541->client->dev, "Failed to allocate input device.");
		return -ENOMEM;
	}

	bt541->input_dev->name = "Zinitix Capacitive TouchScreen";
	bt541->input_dev->phys = "input/ts";
	bt541->input_dev->id.bustype = BUS_I2C;

	input_set_capability(bt541->input_dev, EV_ABS, ABS_MT_POSITION_X);
	input_set_capability(bt541->input_dev, EV_ABS, ABS_MT_POSITION_Y);
	input_set_abs_params(bt541->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(bt541->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);

	touchscreen_parse_properties(bt541->input_dev, true, &bt541->prop);

	if (!bt541->prop.max_x || !bt541->prop.max_y) {
		dev_err(&bt541->client->dev, "touchscreen-size-x and/or touchscreen-size-y not set in dts\n");
		return -EINVAL;
	}

	error = input_mt_init_slots(bt541->input_dev, MAX_SUPPORTED_FINGER_NUM,
				    INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (error) {
		dev_err(&bt541->client->dev,
			"Failed to initialize MT slots: %d", error);
		return error;
	}

	error = input_register_device(bt541->input_dev);
	if (error) {
		dev_err(&bt541->client->dev,
			"Failed to register input device: %d", error);
		return error;
	}

	return 0;
}

static int zinitix_start(struct bt541_ts_data *bt541)
{
	int error;

	error = regulator_bulk_enable(ARRAY_SIZE(bt541->supplies), bt541->supplies);
	if (error < 0) {
		dev_err(&bt541->client->dev, "Failed to enable regulators: %d\n", error);
		return error;
	}

	msleep(CHIP_ON_DELAY);

	error = zinitix_send_power_on_sequence(bt541);
	if (error) {
		dev_err(&bt541->client->dev, "sending power-on sequence failed: %d\n", error);
		return error;
	}

	error = zinitix_init_touch(bt541);
	if (error) {
		dev_err(&bt541->client->dev, "Failed to init touchscreen ic\n");
		return error;
	}

	return 0;
}

static int zinitix_stop(struct bt541_ts_data *bt541)
{
	int error;

	error = regulator_bulk_disable(ARRAY_SIZE(bt541->supplies), bt541->supplies);
	if (error) {
		dev_err(&bt541->client->dev, "failed to disable regulators: %d\n", error);
		return error;
	}

	return 0;
}

static void zinitix_disable_regulators(void *arg)
{
	struct bt541_ts_data *bt541 = arg;

	zinitix_stop(bt541);
}

static int zinitix_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct bt541_ts_data *bt541;
	int error;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C check functionality failed.\n");
		return -ENXIO;
	}

	bt541 = devm_kzalloc(&client->dev, sizeof(*bt541), GFP_KERNEL);
	if (!bt541)
		return -ENOMEM;

	bt541->client = client;
	i2c_set_clientdata(client, bt541);

	error = zinitix_init_regulators(bt541);
	if (error) {
		dev_err(&client->dev, "regulator initialization failed: %d\n", error);
		return error;
	}

	error = zinitix_init_input_dev(bt541);
	if (error) {
		dev_err(&client->dev, "input dev initialization failed: %d\n", error);
		return error;
	}

	error = device_property_read_u32(&client->dev, "zinitix,mode", &bt541->zinitix_mode);
	if (error < 0) {
		// fall back to mode 2
		bt541->zinitix_mode = DEFAULT_TOUCH_POINT_MODE;
	}

	if (bt541->zinitix_mode < 1 || bt541->zinitix_mode > 2) {
		// if devices are found which fail to support either, mode 0 support may be needed.
		dev_err(&client->dev, "malformed zinitix,mode property, must be in range 1 to 2. (supplied: %d)\n", bt541->zinitix_mode);
		return -EINVAL;
	}

	error = zinitix_start(bt541);
	if (error) {
		return error;
	}

	error = devm_add_action_or_reset(&client->dev,
					 zinitix_disable_regulators, bt541);
	if (error) {
		dev_err(&client->dev, "failed to install poweroff handler: %d\n", error);
		return error;
	}

	error = devm_request_threaded_irq(&bt541->client->dev, bt541->client->irq,
					  NULL, zinitix_ts_irq_handler,
					  IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
					  bt541->client->name, bt541);
	if (error) {
		dev_err(&client->dev, "request IRQ failed: %d\n", error);
		return error;
	}

	dev_info(&client->dev, "initialized a zinitix touchscreen\n");

	return 0;
}

static int __maybe_unused zinitix_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bt541_ts_data *bt541 = i2c_get_clientdata(client);
	int i;

	/* Release all fingers */
	for (i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++) {
		input_mt_slot(bt541->input_dev, i);
		input_mt_report_slot_state(bt541->input_dev, MT_TOOL_FINGER, false);
	}

	input_mt_sync_frame(bt541->input_dev);
	input_sync(bt541->input_dev);

	mutex_lock(&bt541->input_dev->mutex);

	disable_irq(client->irq);

	zinitix_stop(bt541);

	mutex_unlock(&bt541->input_dev->mutex);

	return 0;
}

static int __maybe_unused zinitix_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct bt541_ts_data *bt541 = i2c_get_clientdata(client);
	int error;

	mutex_lock(&bt541->input_dev->mutex);

	error = zinitix_start(bt541);
	if (error) {
		mutex_unlock(&bt541->input_dev->mutex);
		return error;
	}

	enable_irq(client->irq);

	mutex_unlock(&bt541->input_dev->mutex);

	return 0;
}

static SIMPLE_DEV_PM_OPS(zinitix_pm_ops, zinitix_suspend, zinitix_resume);

#ifdef CONFIG_OF
static const struct of_device_id zinitix_of_match[] = {
	{ .compatible = "zinitix,bt541" },
	{ }
};
MODULE_DEVICE_TABLE(of, zinitix_of_match);
#endif

static struct i2c_driver zinitix_ts_driver = {
	.probe = zinitix_ts_probe,
	.driver = {
		.name = "Zinitix-TS",
		.pm = &zinitix_pm_ops,
		.of_match_table = of_match_ptr(zinitix_of_match),
	},
};
module_i2c_driver(zinitix_ts_driver);

MODULE_AUTHOR("Michael Srba <Michael.Srba@seznam.cz>");
MODULE_DESCRIPTION("Zinitix touchscreen driver");
MODULE_LICENSE("GPL v2");
