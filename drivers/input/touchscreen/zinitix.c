// SPDX-License-Identifier: GPL-2.0-only

#include <linux/kernel.h>
#include <linux/dmi.h>
#include <linux/firmware.h>
#include <linux/gpio/consumer.h>
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
#include <linux/acpi.h>
#include <linux/of.h>
#include <asm/unaligned.h>

/* Register Map*/

#define BT541_SWRESET_CMD					0x0000
#define BT541_WAKEUP_CMD					0x0001

#define BT541_IDLE_CMD						0x0004
#define BT541_SLEEP_CMD						0x0005

#define BT541_CLEAR_INT_STATUS_CMD			0x0003
#define BT541_CALIBRATE_CMD					0x0006
#define BT541_SAVE_STATUS_CMD				0x0007
#define BT541_SAVE_CALIBRATION_CMD			0x0008
#define BT541_RECALL_FACTORY_CMD			0x000f

#define BT541_THRESHOLD						0x0020

#define BT541_LARGE_PALM_REJECT_AREA_TH		0x003F

#define BT541_DEBUG_REG						0x0115 /* 0~7 */

#define BT541_TOUCH_MODE					0x0010
#define BT541_CHIP_REVISION					0x0011
#define BT541_FIRMWARE_VERSION				0x0012

#define ZINITIX_USB_DETECT					0x116

#define BT541_MINOR_FW_VERSION				0x0121

#define BT541_VENDOR_ID						0x001C
#define BT541_HW_ID							0x0014

#define BT541_DATA_VERSION_REG				0x0013
#define BT541_SUPPORTED_FINGER_NUM			0x0015
#define BT541_EEPROM_INFO					0x0018
#define BT541_INITIAL_TOUCH_MODE			0x0019

#define BT541_TOTAL_NUMBER_OF_X				0x0060
#define BT541_TOTAL_NUMBER_OF_Y				0x0061

#define BT541_DELAY_RAW_FOR_HOST			0x007f

#define BT541_BUTTON_SUPPORTED_NUM			0x00B0
#define BT541_BUTTON_SENSITIVITY			0x00B2
#define BT541_DUMMY_BUTTON_SENSITIVITY		0X00C8

#define BT541_X_RESOLUTION					0x00C0
#define BT541_Y_RESOLUTION					0x00C1

#define BT541_POINT_STATUS_REG				0x0080
#define BT541_ICON_STATUS_REG				0x00AA

#define BT541_AFE_FREQUENCY					0x0100
#define BT541_DND_N_COUNT					0x0122
#define BT541_DND_U_COUNT					0x0135

#define BT541_RAWDATA_REG					0x0200

#define BT541_EEPROM_INFO_REG				0x0018

#define BT541_INT_ENABLE_FLAG				0x00f0
#define BT541_PERIODICAL_INTERRUPT_INTERVAL	0x00f1

#define BT541_BTN_WIDTH						0x016d

#define BT541_CHECKSUM_RESULT				0x012c

#define BT541_INIT_FLASH					0x01d0
#define BT541_WRITE_FLASH					0x01d1
#define BT541_READ_FLASH					0x01d2

#define ZINITIX_INTERNAL_FLAG_02			0x011e
#define ZINITIX_INTERNAL_FLAG_03			0x011f

#define	ZINITIX_I2C_CHECKSUM_WCNT			0x016a
#define	ZINITIX_I2C_CHECKSUM_RESULT			0x016c

/* Interrupt & status register flags */

#define BIT_PT_CNT_CHANGE	1<<0
#define BIT_DOWN			1<<1
#define BIT_MOVE			1<<2
#define BIT_UP				1<<3
#define BIT_PALM			1<<4
#define BIT_PALM_REJECT		1<<5
#define BIT_RESERVED_0		1<<6
#define BIT_RESERVED_1		1<<7
#define BIT_WEIGHT_CHANGE	1<<8
#define BIT_PT_NO_CHANGE	1<<9
#define BIT_REJECT			1<<10
#define BIT_PT_EXIST		1<<11
#define BIT_RESERVED_2		1<<12
#define BIT_ERROR			1<<13
#define BIT_DEBUG			1<<14
#define BIT_ICON_EVENT		1<<15


#define SUB_BIT_EXIST		1<<0
#define SUB_BIT_DOWN		1<<1
#define SUB_BIT_MOVE		1<<2
#define SUB_BIT_UP			1<<3
#define SUB_BIT_UPDATE		1<<4
#define SUB_BIT_WAIT		1<<5

#define TOUCH_POINT_MODE			1
#define MAX_SUPPORTED_FINGER_NUM	5 /* max 10 */ //depends on exact model ??

#define CHIP_ON_DELAY	15 //ms
#define FIRMWARE_ON_DELAY 40 //ms

#define DELAY_FOR_TRANSACTION		50 //μs
#define DELAY_FOR_POST_TRANSCATION	10 //μs

struct capa_info {
	u16	vendor_id;
	u16	ic_revision;
	u16	fw_version;
	u16	fw_minor_version;
	u16	reg_data_version;
	u16	threshold;
	u16	key_threshold;
	u16	dummy_threshold;
	u16	button_num;
	u16	ic_int_mask;
	u16	x_node_num;
	u16	y_node_num;
	u16	total_node_num;
	u16	hw_id;
	u16	afe_frequency;
	u16	i2s_checksum;
	u16	shift_value;
	u16	N_cnt;
	u16	u_cnt;
};

struct coord {
	u16	x;
	u16	y;
	u8	width;
	u8	sub_status;
};

struct point_info {
	u16	status;
	u16	event_flag;
	struct coord coord[MAX_SUPPORTED_FINGER_NUM];
};

struct bt541_ts_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct touchscreen_properties prop;
	struct gpio_desc *vdd_enable_gpio;
	struct capa_info cap_info;
	struct point_info touch_info;
	struct regulator *vddo;
};

static inline s32 zinitix_read_data(struct i2c_client *client,
	u16 reg, u8 *values, u16 length)
{
	int ret;

	ret = i2c_master_send(client , (u8 *)&reg , 2);
	if (ret < 0)
		return ret;

	udelay(DELAY_FOR_TRANSACTION);
	ret = i2c_master_recv(client , values , length);
	if (ret < 0)
		return ret;

	udelay(DELAY_FOR_POST_TRANSCATION);
	return 0;
}

static inline s32 zinitix_write_data(struct i2c_client *client,
	u16 reg, u8 *values, u16 length)
{
	int ret;
	u8* packet;

	packet = kmalloc(length + 2, GFP_KERNEL);
	packet[0] = (reg) & 0xff;
	packet[1] = reg >> 8;
	memcpy((u8 *)&packet[2], values, length);

	ret = i2c_master_send(client , packet , length + 2);
	kfree(packet);
	if (ret < 0)
		return ret;

	udelay(DELAY_FOR_POST_TRANSCATION);
	return 0;
}

static inline s32 zinitix_write_u16(struct i2c_client *client, u16 reg, u16 value)
{
	return zinitix_write_data(client, reg, (u8 *)&value, 2);
}

static inline s32 zinitix_write_cmd(struct i2c_client *client, u16 reg)
{
	int ret;
	
	ret = i2c_master_send(client , (u8 *)&reg , 2);
	if (ret < 0)
		return ret;

	udelay(DELAY_FOR_POST_TRANSCATION);
	return 0;
}

static bool zinitix_init_touch(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;
	struct capa_info *cap = &(bt541->cap_info);
	int i;
	int ret;
	u16 reg_val;
	u16 chip_eeprom_info;

	ret = zinitix_read_data(client, BT541_EEPROM_INFO_REG,
			(u8 *)&chip_eeprom_info, 2);
	if(ret) {
		dev_err(&client->dev, "Failed to read eeprom info");
		return ret;
	}


	ret = zinitix_write_cmd(client, BT541_SWRESET_CMD);
	if(ret) {
		dev_err(&client->dev, "Failed to write reset command\n");
		return ret;
	}

	ret = zinitix_write_u16(client, BT541_INT_ENABLE_FLAG, 0x0);
	if(ret) {
		dev_err(&client->dev, "failed to read chip revision\n");
		return ret;
	}

	ret = zinitix_read_data(client, BT541_CHIP_REVISION,
					(u8 *)&cap->ic_revision, 2);
	if(ret) {
		dev_err(&client->dev, "failed to read chip revision\n");
		return ret;
	}

	ret = zinitix_read_data(client, BT541_HW_ID, (u8 *)&cap->hw_id, 2);
	if(ret) {
		dev_err(&client->dev, "Failed to read hw id\n");
		return ret;
	}
	
	ret = zinitix_read_data(client, BT541_THRESHOLD, (u8 *)&cap->threshold, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_THRESHOLD, (u8 *)&cap->threshold, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_BUTTON_SENSITIVITY, (u8 *)&cap->key_threshold, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_DUMMY_BUTTON_SENSITIVITY, (u8 *)&cap->dummy_threshold, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_TOTAL_NUMBER_OF_X, (u8 *)&cap->x_node_num, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_TOTAL_NUMBER_OF_Y, (u8 *)&cap->y_node_num, 2);
	if(ret) 
		return ret;

	cap->total_node_num = cap->x_node_num * cap->y_node_num;

	ret = zinitix_read_data(client, BT541_DND_N_COUNT, (u8 *)&cap->N_cnt, 2);
	if(ret) 
		return ret;

	printk(KERN_INFO "N count = %d\n", cap->N_cnt);

	ret = zinitix_read_data(client, BT541_DND_U_COUNT, (u8 *)&cap->u_cnt, 2);
	if(ret) 
		return ret;

	printk(KERN_INFO "u count = %d\n", cap->u_cnt);

	ret = zinitix_read_data(client, BT541_AFE_FREQUENCY, (u8 *)&cap->afe_frequency, 2);
	if(ret) 
		return ret;

	printk(KERN_INFO "afe frequency = %d\n", cap->afe_frequency);


	/* get chip firmware version */
	ret = zinitix_read_data(client, BT541_FIRMWARE_VERSION, (u8 *)&cap->fw_version, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_MINOR_FW_VERSION, (u8 *)&cap->fw_minor_version, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_DATA_VERSION_REG, (u8 *)&cap->reg_data_version, 2);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, BT541_EEPROM_INFO_REG, (u8 *)&chip_eeprom_info, 2);
	if(ret) 
		return ret;

	/* initialize */
	ret = zinitix_write_u16(client, BT541_X_RESOLUTION, (u16)bt541->prop.max_x);
	if(ret) 
		return ret;

	ret = zinitix_write_u16(client, BT541_Y_RESOLUTION, (u16)bt541->prop.max_y);
	if(ret) 
		return ret;

	ret = zinitix_write_u16(client, BT541_BUTTON_SUPPORTED_NUM, (u16)cap->button_num);
	if(ret) 
		return ret;

	ret = zinitix_write_u16(client, BT541_SUPPORTED_FINGER_NUM, (u16)MAX_SUPPORTED_FINGER_NUM);
	if(ret) 
		return ret;

	printk(KERN_INFO "set other configuration\r\n");

	ret = zinitix_write_u16(client, BT541_INITIAL_TOUCH_MODE, TOUCH_POINT_MODE);
	if(ret) 
		return ret;

	ret = zinitix_write_u16(client, BT541_TOUCH_MODE, TOUCH_POINT_MODE);
	if(ret) 
		return ret;

	ret = zinitix_read_data(client, ZINITIX_INTERNAL_FLAG_02, (u8 *)&reg_val, 2);
	if(ret) 
		return ret;
	
	cap->ic_int_mask = BIT_PT_CNT_CHANGE | BIT_DOWN | BIT_MOVE | BIT_UP;
	
	ret = zinitix_write_u16(client, BT541_INT_ENABLE_FLAG, cap->ic_int_mask);
	if(ret) 
		return ret;

	/* read garbage data */
	for (i = 0; i < 10; i++) {
		zinitix_write_cmd(client, BT541_CLEAR_INT_STATUS_CMD);
		udelay(10);
	}

	printk(KERN_INFO "successfully initialized\r\n");
	return 0;
}

static int zinitix_init_gpio(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;
	int ret;

	bt541->vddo = devm_regulator_get(&client->dev, "vddo");
	if (IS_ERR(bt541->vddo)) {
		ret = PTR_ERR(bt541->vddo);
		if (ret != -EPROBE_DEFER)
			dev_err(&client->dev, "Failed to get vddo regulator: %d\n", ret);
		return ret;
	}

	bt541->vdd_enable_gpio = devm_gpiod_get(&client->dev, "vdd_enable", GPIOD_OUT_LOW);
	if (IS_ERR(bt541->vdd_enable_gpio)) {
		ret = PTR_ERR(bt541->vdd_enable_gpio);
		dev_err(&client->dev, "cannot get vdd_enable_gpio %d\n", ret);
		return ret;
	}

	return 0;
}

static int zinitix_send_power_on_sequence(struct bt541_ts_data *bt541) 
{
	int ret;
	struct i2c_client *client = bt541->client;
	u16 chip_code;
	
	ret = zinitix_write_u16(client, 0xc000, 0x0001);
	if(ret) {
		dev_err(&client->dev, "Failed to send power sequence(vendor cmd enable)\n");
		return ret;
	}
	udelay(10);

	ret = zinitix_read_data(client, 0xcc00, (u8 *)&chip_code, 2);
	if(ret) {
		dev_err(&client->dev, "Failed to read chip code\n");
		return ret;
	}

	dev_info(&client->dev, "%s: chip code = 0x%x\n", __func__, chip_code);
	udelay(10);

	ret = zinitix_write_cmd(client, 0xc004);
	if(ret) {
		dev_err(&client->dev, "Failed to send power sequence(intn clear)\n");
		return ret;
	}
	udelay(10);

	ret = zinitix_write_u16(client, 0xc002, 0x0001);
	if(ret) {
		dev_err(&client->dev, "Failed to send power sequence(nvm init)\n");
		return ret;
	}
	mdelay(2);

	ret = zinitix_write_u16(client, 0xc001, 0x0001);
	if(ret) {
		dev_err(&client->dev, "Failed to send power sequence(program start)\n");
		return ret;
	}
	msleep(FIRMWARE_ON_DELAY);	/* wait for checksum cal */
	
	return 0;
}

static bool zinitix_read_coord(struct bt541_ts_data *bt541)
{
	struct i2c_client *client = bt541->client;

	int i;
	int ret;

	memset(&(bt541->touch_info), 0x0, sizeof(struct point_info));

	ret = zinitix_read_data(bt541->client, BT541_POINT_STATUS_REG, (u8 *)(&bt541->touch_info), 4);
	if (ret) {
		dev_err(&client->dev, "%s: Failed to read point info\n", __func__);
		return ret;
	}

//	dev_info(&client->dev, "status reg = 0x%x , event_flag = 0x%04x\n", bt541->touch_info.status, bt541->touch_info.event_flag); //dev_dbg

	if (bt541->touch_info.event_flag == 0)
		goto out;

	for (i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++) {
		if (bt541->touch_info.event_flag & (1<<i)) {
			udelay(20);

			ret = zinitix_read_data(bt541->client, BT541_POINT_STATUS_REG + 2 + ( i * 4), (u8 *)(&bt541->touch_info.coord[i]), sizeof(struct coord));
			if (ret) {
				dev_err(&client->dev, "Failed to read point info\n");
				return ret;
			}
		}
	}

out:


	/* error */
	if (bt541->touch_info.status & BIT_ERROR) {
		dev_err(&client->dev, "Invalid must zero bit(%04x)\n", bt541->touch_info.status);
		
		return -1; //no idea what this is, so no idea what errno to use
	}
	
	zinitix_write_cmd(bt541->client, BT541_CLEAR_INT_STATUS_CMD);

	return 0;
	
}

static irqreturn_t zinitix_ts_irq_handler(int irq, void *bt541_handler) {
	struct bt541_ts_data *bt541 = (struct bt541_ts_data *)bt541_handler;
	struct i2c_client *client = bt541->client;
	int i;
	
	if(zinitix_read_coord(bt541)) {
		dev_err(&client->dev, "Failed to read touchscreen coord\n");
	}
	
	for (i = 0; i < MAX_SUPPORTED_FINGER_NUM; i++) {
//		printk(KERN_INFO "[bt541] yeeehaaw! finger: [%d] coord: x: %d, y: %d, w: %d || EXIST[%d] DOWN[%d] MOVE[%d] UP[%d] UPDATE[%d] WAIT[%d]\n", i, bt541->touch_info.coord[i].x, bt541->touch_info.coord[i].y, bt541->touch_info.coord[i].width, bt541->touch_info.coord[i].sub_status & SUB_BIT_EXIST, bt541->touch_info.coord[i].sub_status & SUB_BIT_DOWN, bt541->touch_info.coord[i].sub_status & SUB_BIT_MOVE, bt541->touch_info.coord[i].sub_status & SUB_BIT_UP, bt541->touch_info.coord[i].sub_status & SUB_BIT_UPDATE, bt541->touch_info.coord[i].sub_status & SUB_BIT_WAIT);
		
		if(bt541->touch_info.coord[i].sub_status & SUB_BIT_EXIST) {
			input_mt_slot(bt541->input_dev, i);
			input_mt_report_slot_state(bt541->input_dev, MT_TOOL_FINGER, true);
			touchscreen_report_pos(bt541->input_dev, &bt541->prop,
					   bt541->touch_info.coord[i].x, bt541->touch_info.coord[i].y, true);
			input_report_abs(bt541->input_dev, ABS_MT_TOUCH_MAJOR, bt541->touch_info.coord[i].width);
			input_report_abs(bt541->input_dev, ABS_MT_WIDTH_MAJOR, bt541->touch_info.coord[i].width);
		}
	}
	
	input_mt_sync_frame(bt541->input_dev);
	input_sync(bt541->input_dev);
	
	return IRQ_HANDLED;	
}

static int zinitix_init_input_dev(struct bt541_ts_data *bt541)
{
	int ret;

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

	ret = input_mt_init_slots(bt541->input_dev, MAX_SUPPORTED_FINGER_NUM, INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (ret) {
		dev_err(&bt541->client->dev,
			"Failed to initialize MT slots: %d", ret);
		return ret;
	}

	ret = input_register_device(bt541->input_dev);
	if (ret) {
		dev_err(&bt541->client->dev,
			"Failed to register input device: %d", ret);
		return ret;
	}

	return 0;
}

static int zinitix_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;

	struct bt541_ts_data *bt541;
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C check functionality failed.\n");
		return -ENXIO;
	}
	
	bt541 = devm_kzalloc(&client->dev, sizeof(*bt541), GFP_KERNEL);
	if (!bt541) {
		return -ENOMEM;
	}
	
	bt541->client = client;
	i2c_set_clientdata(client, bt541);
	
	ret = zinitix_init_gpio(bt541);
	if (ret) {
		dev_err(&client->dev, "gpio initialization failed: %d\n", ret);
		return ret;
	}

	ret = regulator_enable(bt541->vddo);
	if (ret) {
		dev_err(&client->dev, "Failed to enable vddo regulator: %d\n", ret);
		regulator_disable(bt541->vddo);
		return ret;
	}
	
	gpiod_set_value(bt541->vdd_enable_gpio, 1);
	
	msleep(CHIP_ON_DELAY);

	ret = zinitix_send_power_on_sequence(bt541);
	if (ret) {
		dev_err(&client->dev, "sending power-on sequence failed: %d\n", ret);
		return ret;
	}

	ret = zinitix_init_input_dev(bt541);
	if (ret) {
		dev_err(&client->dev, "input dev initialization failed: %d\n", ret);
		return ret;
	}

	ret = zinitix_init_touch(bt541);
	if(ret) {
		dev_err(&client->dev, "Failed to init touchscreen ic\n");
		return ret; //from downstream
	}	
	
	ret = devm_request_threaded_irq(&bt541->client->dev, bt541->client->irq,
	                                 NULL, zinitix_ts_irq_handler,
	                                 IRQF_TRIGGER_FALLING | IRQF_ONESHOT, bt541->client->name, bt541);
	if(ret) {
		dev_err(&client->dev, "request IRQ failed: %d\n", ret);
		return ret;
	}
	
	dev_info(&client->dev, "initialized a zinitix touchscreen\n");
	
	return 0;
}

static int zinitix_ts_remove(struct i2c_client *client)
{
	struct bt541_ts_data *bt541 = i2c_get_clientdata(client);

	gpiod_set_value(bt541->vdd_enable_gpio, 0);
	regulator_disable(bt541->vddo);
	
	return 0;
}

static const struct i2c_device_id zinitix_ts_id[] = {
	{ "Zinitix-TS", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, zinitix_ts_id);

#ifdef CONFIG_OF
static const struct of_device_id zinitix_of_match[] = {
	{ .compatible = "zinitix,bt541" },
	{ }
};
MODULE_DEVICE_TABLE(of, zinitix_of_match);
#endif

static struct i2c_driver zinitix_ts_driver = {
	.probe = zinitix_ts_probe,
	.remove = zinitix_ts_remove,
	.id_table = zinitix_ts_id,
	.driver = {
		.name = "Zinitix-TS",
		.of_match_table = of_match_ptr(zinitix_of_match),
	},
};
module_i2c_driver(zinitix_ts_driver);

MODULE_AUTHOR("Michael Srba <Michael.Srba@seznam.cz>");
MODULE_DESCRIPTION("Zinitix touchscreen driver");
MODULE_LICENSE("GPL v2");

