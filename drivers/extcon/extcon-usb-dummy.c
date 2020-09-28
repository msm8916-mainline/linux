// SPDX-License-Identifier: GPL-2.0-only

#include <linux/extcon-provider.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

static const unsigned int extcon_dummy_cable[] = {
	EXTCON_USB,
	EXTCON_NONE,
};

static int extcon_dummy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct extcon_dev *edev;
	int ret;

	edev = devm_extcon_dev_allocate(dev, extcon_dummy_cable);
	if (IS_ERR(edev))
		return PTR_ERR(edev);

	ret = devm_extcon_dev_register(dev, edev);
	if (ret < 0) {
		dev_err(dev, "failed to register extcon device: %d\n", ret);
		return ret;
	}

	/* Pretend that USB is always connected */
	extcon_set_state_sync(edev, EXTCON_USB, true);

	return 0;
}

static const struct of_device_id extcon_dummy_of_match[] = {
	{ .compatible = "linux,extcon-usb-dummy", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, extcon_dummy_of_match);

static struct platform_driver extcon_dummy_driver = {
	.probe = extcon_dummy_probe,
	.driver = {
		.name = "extcon-usb-dummy",
		.of_match_table = extcon_dummy_of_match,
	},
};
module_platform_driver(extcon_dummy_driver);

MODULE_DESCRIPTION("Dummy USB extcon driver");
MODULE_LICENSE("GPL v2");
