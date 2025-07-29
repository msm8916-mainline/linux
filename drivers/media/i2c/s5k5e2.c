// SPDX-License-Identifier: GPL-2.0-only

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>

#include <media/v4l2-cci.h>

static const char * const s5k5e2_supply_name[] = {
	"vdda",
	"vddd",
	"vdddo",
};

#define S5K5E2_NUM_SUPPLIES ARRAY_SIZE(s5k5e2_supply_name)

struct s5k5e2 {
	struct device *dev;
	struct clk *xclk;
	struct regmap *regmap;

	/* struct ccs_pll pll; */

	/* struct v4l2_fwnode_endpoint bus_cfg; */

	/* struct v4l2_subdev sd; */
	/* struct media_pad pad; */

	/* struct v4l2_ctrl_handler ctrls; */
	/* struct v4l2_ctrl *pixel_rate; */
	/* struct v4l2_ctrl *link_freq; */
	/* struct v4l2_ctrl *vblank; */
	/* struct v4l2_ctrl *hblank; */
	/* struct v4l2_ctrl *exposure; */
	/* struct v4l2_ctrl *unit_size; */
	/* struct { */
	/* 	struct v4l2_ctrl *hflip; */
	/* 	struct v4l2_ctrl *vflip; */
	/* }; */

	struct regulator_bulk_data	supplies[S5K5E2_NUM_SUPPLIES];

	struct gpio_desc *enable_gpio;
};

static int s5k5e2_get_regulators(struct device *dev, struct s5k5e2 *s5k5e2)
{
	unsigned int i;

	for (i = 0; i < S5K5E2_NUM_SUPPLIES; i++)
		s5k5e2->supplies[i].supply = s5k5e2_supply_name[i];

	return devm_regulator_bulk_get(dev, S5K5E2_NUM_SUPPLIES,
				       s5k5e2->supplies);
}

static int __maybe_unused s5k5e2_power_on(struct s5k5e2 *s5k5e2)
{
	int ret;

	ret = regulator_bulk_enable(S5K5E2_NUM_SUPPLIES, s5k5e2->supplies);
	if (ret < 0) {
		dev_err(s5k5e2->dev, "failed to enable regulators: %d\n", ret);
		return ret;
	}

	usleep_range(2000, 3000); // TODO verify

	ret = clk_prepare_enable(s5k5e2->xclk);
	if (ret < 0) {
		regulator_bulk_disable(S5K5E2_NUM_SUPPLIES, s5k5e2->supplies);
		dev_err(s5k5e2->dev, "clk prepare enable failed\n");
		return ret;
	}

	gpiod_set_value_cansleep(s5k5e2->enable_gpio, 1);
	usleep_range(12000, 15000); // TODO verify

	return 0;
}

static int s5k5e2_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct s5k5e2 *s5k5e2;
	int ret;

	s5k5e2 = devm_kzalloc(dev, sizeof(*s5k5e2), GFP_KERNEL);
	if (!s5k5e2)
		return -ENOMEM;

	s5k5e2->dev = dev;

	s5k5e2->xclk = devm_clk_get(dev, NULL);
	if (IS_ERR(s5k5e2->xclk))
		return dev_err_probe(dev, PTR_ERR(s5k5e2->xclk),
				     "failed to get xclk\n");

	ret = s5k5e2_get_regulators(dev, s5k5e2);
	if (ret < 0)
		return dev_err_probe(dev, ret, "failed to get regulators\n");

	s5k5e2->enable_gpio = devm_gpiod_get(dev, "enable", GPIOD_OUT_LOW);
	if (IS_ERR(s5k5e2->enable_gpio))
		return dev_err_probe(dev, PTR_ERR(s5k5e2->enable_gpio),
				     "failed to get enable gpio\n");

	ret = s5k5e2_power_on(s5k5e2);
	if (ret < 0)
		return dev_err_probe(dev, ret, "failed to power on\n");

	// TODO read and enable power supply
	// dump i2c registers

	return 0;


/*	s5k5baf_hw_init(state);*/
/*	ret = s5k5baf_check_fw_revision(state);*/

/*	s5k5baf_power_off(state);*/
/*	if (ret < 0)*/
/*		goto err_me;*/

/*	ret = s5k5baf_initialize_ctrls(state);*/
/*	if (ret < 0)*/
/*		goto err_me;*/

/*	ret = v4l2_async_register_subdev(&state->sd);*/
/*	if (ret < 0)*/
/*		goto err_ctrl;*/

/*	return 0;*/

/*err_ctrl:*/
/*	v4l2_ctrl_handler_free(state->sd.ctrl_handler);*/
/*err_me:*/
/*	media_entity_cleanup(&state->sd.entity);*/
/*	media_entity_cleanup(&state->cis_sd.entity);*/
/*	return ret;*/
}

static void s5k5e2_remove(struct i2c_client *c)
{
/*	struct v4l2_subdev *sd = i2c_get_clientdata(c);*/
/*	struct s5k5baf *state = to_s5k5baf(sd);*/

/*	v4l2_async_unregister_subdev(sd);*/
/*	v4l2_ctrl_handler_free(sd->ctrl_handler);*/
/*	media_entity_cleanup(&sd->entity);*/

/*	sd = &state->cis_sd;*/
/*	v4l2_device_unregister_subdev(sd);*/
/*	media_entity_cleanup(&sd->entity);*/
}

static const struct i2c_device_id s5k5e2_id[] = {
	{ "s5k5e2" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, s5k5e2_id);

static const struct of_device_id s5k5e2_of_match[] = {
	{ .compatible = "samsung,s5k5e2" },
	{ }
};
MODULE_DEVICE_TABLE(of, s5k5e2_of_match);

static struct i2c_driver s5k5e2_i2c_driver = {
	.driver = {
		.of_match_table = s5k5e2_of_match,
		.name = "s5k5e2"
	},
	.probe		= s5k5e2_probe,
	.remove		= s5k5e2_remove,
	.id_table	= s5k5e2_id,
};

module_i2c_driver(s5k5e2_i2c_driver);

MODULE_DESCRIPTION("Samsung S5K5E2 camera driver");
MODULE_AUTHOR("André Apitzsch");
MODULE_LICENSE("GPL");
