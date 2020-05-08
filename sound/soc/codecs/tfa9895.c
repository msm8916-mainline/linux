// SPDX-License-Identifier: GPL-2.0-only
/*
 * Register definitions taken from tfa98xx kernel driver:
 * Copyright (C) 2014-2020 NXP Semiconductors, All Rights Reserved.
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <sound/soc.h>

#define TFA98XX_STATUSREG		0x00
#define TFA98XX_BATTERYVOLTAGE		0x01
#define TFA9891_TEMPERATURE		0x02
#define TFA98XX_REVISIONNUMBER		0x03
#define TFA98XX_I2SREG			0x04
#define TFA98XX_BAT_PROT		0x05
#define TFA98XX_AUDIO_CTR		0x06
#define TFA98XX_DCDCBOOST		0x07
#define TFA98XX_SPKR_CALIBRATION	0x08
#define TFA98XX_SYS_CTRL		0x09
#define TFA98XX_I2S_SEL_REG		0x0a

#define TFA98XX_I2SREG_CHSA		(0x3 << 6)
#define TFA98XX_I2SREG_I2SSR_SHIFT	12
#define TFA98XX_I2SREG_I2SSR_8000	(0 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_11025	(1 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_12000	(2 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_16000	(3 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_22050	(4 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_24000	(5 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_32000	(6 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_44100	(7 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_48000	(8 << TFA98XX_I2SREG_I2SSR_SHIFT)
#define TFA98XX_I2SREG_I2SSR_MASK	0xf000

#define TFA98XX_SYS_CTRL_CFE		(0x1 << 2)
#define TFA98XX_SYS_CTRL_AMPC		(0x1 << 6)

#define TFA9895_REVISION		0x12

static int tfa9895_hw_params(struct snd_pcm_substream *substream,
			     struct snd_pcm_hw_params *params,
			     struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	unsigned int sr;

	switch (params_rate(params)) {
	case 8000:
		sr = TFA98XX_I2SREG_I2SSR_8000;
		break;
	case 11025:
		sr = TFA98XX_I2SREG_I2SSR_11025;
		break;
	case 12000:
		sr = TFA98XX_I2SREG_I2SSR_12000;
		break;
	case 16000:
		sr = TFA98XX_I2SREG_I2SSR_16000;
		break;
	case 22050:
		sr = TFA98XX_I2SREG_I2SSR_22050;
		break;
	case 24000:
		sr = TFA98XX_I2SREG_I2SSR_24000;
		break;
	case 32000:
		sr = TFA98XX_I2SREG_I2SSR_32000;
		break;
	case 44100:
		sr = TFA98XX_I2SREG_I2SSR_44100;
		break;
	case 48000:
		sr = TFA98XX_I2SREG_I2SSR_48000;
		break;
	default:
		return -EINVAL;
	}

	return snd_soc_component_update_bits(component, TFA98XX_I2SREG,
					     TFA98XX_I2SREG_I2SSR_MASK, sr);
}

static const struct snd_soc_dapm_widget tfa9895_dapm_widgets[] = {
	SND_SOC_DAPM_OUTPUT("Speaker"),
	SND_SOC_DAPM_OUT_DRV_E("PWUP", TFA98XX_SYS_CTRL, 0, 1, NULL, 0, NULL, 0),
};

static const struct snd_soc_dapm_route tfa9895_dapm_routes[] = {
	{"PWUP", NULL, "HiFi Playback"},
	{"Speaker", NULL, "PWUP"},
};

static const struct snd_soc_component_driver tfa9895_component = {
	.dapm_widgets		= tfa9895_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(tfa9895_dapm_widgets),
	.dapm_routes		= tfa9895_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(tfa9895_dapm_routes),
	.idle_bias_on		= 1,
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static const struct snd_soc_dai_ops tfa9895_dai_ops = {
	.hw_params = tfa9895_hw_params,
};

static struct snd_soc_dai_driver tfa9895_dai = {
	.name = "tfa9895-hifi",
	.playback = {
		.stream_name	= "HiFi Playback",
		.formats	= SNDRV_PCM_FMTBIT_S16_LE |
				  SNDRV_PCM_FMTBIT_S24_LE,
		.rates		= SNDRV_PCM_RATE_8000_48000,
		.rate_min	= 8000,
		.rate_max	= 48000,
		.channels_min	= 1,
		.channels_max	= 2,
	},
	.ops = &tfa9895_dai_ops,
};

static const struct regmap_config tfa9895_regmap = {
	.reg_bits = 8,
	.val_bits = 16,
};

static const struct reg_sequence tfa9895_reg_init[] = {
	/* some other registers must be set for optimal amplifier behaviour */
	{ TFA98XX_BAT_PROT, 0x13ab },
	{ TFA98XX_AUDIO_CTR, 0x001f },

	/* peak voltage protection is always on, but may be written */
	{ TFA98XX_SPKR_CALIBRATION, 0x3c4e },

	/* TFA98XX_SYSCTRL_DCA = 0 */
	{ TFA98XX_SYS_CTRL, 0x024d },
	{ 0x41, 0x0308 },
	{ 0x49, 0x0e82 },
};

static int tfa9895_dsp_bypass(struct regmap *regmap)
{
	int ret;

	/* Clear CHSA to bypass DSP and take input from I2S 1 (left channel) */
	ret = regmap_update_bits(regmap, TFA98XX_I2SREG,
				 TFA98XX_I2SREG_CHSA, 0);
	if (ret)
		return ret;

	/* Clear CFE and AMPC to disable CoolFlux DSP */
	return regmap_update_bits(regmap, TFA98XX_SYS_CTRL,
				  TFA98XX_SYS_CTRL_CFE | TFA98XX_SYS_CTRL_AMPC,
				  0);
}

static int tfa9895_i2c_probe(struct i2c_client *i2c)
{
	struct device *dev = &i2c->dev;
	struct regmap *regmap;
	unsigned int val;
	int ret;

	regmap = devm_regmap_init_i2c(i2c, &tfa9895_regmap);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	ret = regmap_read(regmap, TFA98XX_REVISIONNUMBER, &val);
	if (ret) {
		dev_err(dev, "failed to read revision number: %d\n", ret);
		return ret;
	}

	if (val != TFA9895_REVISION) {
		dev_err(dev, "invalid revision number, expected %#x, got %#x\n",
			TFA9895_REVISION, val);
		return -EINVAL;
	}

	ret = regmap_multi_reg_write(regmap, tfa9895_reg_init,
				     ARRAY_SIZE(tfa9895_reg_init));
	if (ret) {
		dev_err(dev, "failed to initialize registers: %d\n", ret);
		return ret;
	}

	ret = tfa9895_dsp_bypass(regmap);
	if (ret) {
		dev_err(dev, "failed to enable dsp bypass: %d\n", ret);
		return ret;
	}

	return devm_snd_soc_register_component(dev, &tfa9895_component,
					       &tfa9895_dai, 1);
}

static const struct of_device_id tfa9895_of_match[] = {
	{ .compatible = "nxp,tfa9895", },
	{ }
};
MODULE_DEVICE_TABLE(of, tfa9895_of_match);

static struct i2c_driver tfa9895_i2c_driver = {
	.driver = {
		.name = "tfa9895",
		.of_match_table = tfa9895_of_match,
	},
	.probe_new = tfa9895_i2c_probe,
};
module_i2c_driver(tfa9895_i2c_driver);

MODULE_DESCRIPTION("ASoC NXP Semiconductors TFA9895 driver");
MODULE_LICENSE("GPL");
