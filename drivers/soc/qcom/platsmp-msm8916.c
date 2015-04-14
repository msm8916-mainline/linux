/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Copyright (c) 2013 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* MSM ARMv8 CPU Operations
 * Based on arch/arm64/kernel/smp_spin_table.c
 */

#include <linux/bitops.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/smp.h>
#include <linux/qcom_scm.h>

#include <asm/barrier.h>
#include <asm/cacheflush.h>
#include <asm/cputype.h>
#include <asm/smp_plat.h>

#ifdef CONFIG_ARM64
#include <asm/cpu_ops.h>

DEFINE_PER_CPU(int, cold_boot_done);
#endif

/* CPU power domain register offsets */
#define CPU_PWR_CTL		0x4
#define CPU_PWR_GATE_CTL	0x14
#define LDO_BHS_PWR_CTL		0x28

/* L2 power domain register offsets */
#define L2_PWR_CTL_OVERRIDE	0xc
#define L2_PWR_CTL		0x14
#define L2_PWR_STATUS		0x18
#define L2_CORE_CBCR		0x58
#define L1_RST_DIS		0x284

#define L2_SPM_STS		0xc
#define L2_VREG_CTL		0x1c

/*
 * struct qcom_l2ccc_of_info: represents of data for l2 cache clock controller.
 * @compat: compat string for l2 cache clock controller
 * @l2_pon: l2 cache power on routine
 */
struct qcom_l2ccc_of_info {
	const char *compat;
	int (*l2_power_on) (struct device_node *dn, u32 l2_mask, int cpu);
	u32 l2_power_on_mask;
};

static int power_on_l2_msm8916(struct device_node *l2ccc_node, u32 pon_mask,
				int cpu)
{
	u32 pon_status;
	void __iomem *l2_base;

	l2_base = of_iomap(l2ccc_node, 0);
	if (!l2_base)
		return -ENOMEM;

	/* Skip power-on sequence if l2 cache is already powered up*/
	pon_status = (__raw_readl(l2_base + L2_PWR_STATUS) & pon_mask)
				== pon_mask;
	if (pon_status) {
		iounmap(l2_base);
		return 0;
	}

	/* Close L2/SCU Logic GDHS and power up the cache */
	writel_relaxed(0x10D700, l2_base + L2_PWR_CTL);

	/* Assert PRESETDBGn */
	writel_relaxed(0x400000, l2_base + L2_PWR_CTL_OVERRIDE);
	mb();
	udelay(2);

	/* De-assert L2/SCU memory Clamp */
	writel_relaxed(0x101700, l2_base + L2_PWR_CTL);

	/* Wakeup L2/SCU RAMs by deasserting sleep signals */
	writel_relaxed(0x101703, l2_base + L2_PWR_CTL);
	mb();
	udelay(2);

	/* Enable clocks via SW_CLK_EN */
	writel_relaxed(0x01, l2_base + L2_CORE_CBCR);

	/* De-assert L2/SCU logic clamp */
	writel_relaxed(0x101603, l2_base + L2_PWR_CTL);
	mb();
	udelay(2);

	/* De-assert PRESSETDBg */
	writel_relaxed(0x0, l2_base + L2_PWR_CTL_OVERRIDE);

	/* De-assert L2/SCU Logic reset */
	writel_relaxed(0x100203, l2_base + L2_PWR_CTL);
	mb();
	udelay(54);

	/* Turn on the PMIC_APC */
	writel_relaxed(0x10100203, l2_base + L2_PWR_CTL);

	/* Set H/W clock control for the cpu CBC block */
	writel_relaxed(0x03, l2_base + L2_CORE_CBCR);
	mb();
	iounmap(l2_base);

	return 0;
}

static const struct qcom_l2ccc_of_info l2ccc_info[] = {
	{
		.compat = "qcom,8916-l2ccc",
		.l2_power_on = power_on_l2_msm8916,
		.l2_power_on_mask = BIT(9),
	},
};

static int power_on_l2_cache(struct device_node *l2ccc_node, int cpu)
{
	int ret, i;
	const char *compat;

	ret = of_property_read_string(l2ccc_node, "compatible", &compat);
	if (ret)
		return ret;

	for (i = 0; i < ARRAY_SIZE(l2ccc_info); i++) {
		const struct qcom_l2ccc_of_info *ptr = &l2ccc_info[i];

		if (!of_compat_cmp(ptr->compat, compat, strlen(compat)))
				return ptr->l2_power_on(l2ccc_node,
						ptr->l2_power_on_mask, cpu);
	}
	pr_err("Compat string not found for L2CCC node\n");
	return -EIO;
}

int qcom_cortex_a_release_secondary(unsigned int cpu)
{

	int ret = 0;
	struct device_node *cpu_node, *acc_node, *l2_node, *l2ccc_node;
	void __iomem *reg;

	cpu_node = of_get_cpu_node(cpu, NULL);
	if (!cpu_node)
		return -ENODEV;

	acc_node = of_parse_phandle(cpu_node, "qcom,acc", 0);
	if (!acc_node) {
			ret = -ENODEV;
			goto out_acc;
	}

	l2_node = of_parse_phandle(cpu_node, "next-level-cache", 0);
	if (!l2_node) {
		ret = -ENODEV;
		goto out_l2;
	}

	l2ccc_node = of_parse_phandle(l2_node, "power-domain", 0);
	if (!l2ccc_node) {
		ret = -ENODEV;
		goto out_l2;
	}

	/* Ensure L2-cache of the CPU is powered on before
	 * unclamping cpu power rails.
	 */
	ret = power_on_l2_cache(l2ccc_node, cpu);
	if (ret) {
		pr_err("L2 cache power up failed for CPU%d\n", cpu);
		goto out_l2ccc;
	}

	reg = of_iomap(acc_node, 0);
	if (!reg) {
		ret = -ENOMEM;
		goto out_acc_reg;
	}

	/* Assert Reset on cpu-n */
	writel_relaxed(0x00000033, reg + CPU_PWR_CTL);
	mb();

	/*Program skew to 16 X0 clock cycles*/
	writel_relaxed(0x10000001, reg + CPU_PWR_GATE_CTL);
	mb();
	udelay(2);

	/* De-assert coremem clamp */
	writel_relaxed(0x00000031, reg + CPU_PWR_CTL);
	mb();

	/* Close coremem array gdhs */
	writel_relaxed(0x00000039, reg + CPU_PWR_CTL);
	mb();
	udelay(2);

	/* De-assert cpu-n clamp */
	writel_relaxed(0x00020038, reg + CPU_PWR_CTL);
	mb();
	udelay(2);

	/* De-assert cpu-n reset */
	writel_relaxed(0x00020008, reg + CPU_PWR_CTL);
	mb();

	/* Assert PWRDUP signal on core-n */
	writel_relaxed(0x00020088, reg + CPU_PWR_CTL);
	mb();

	/* Secondary CPU-N is now alive */
	iounmap(reg);
out_acc_reg:
	of_node_put(l2ccc_node);
out_l2ccc:
	of_node_put(l2_node);
out_l2:
	of_node_put(acc_node);
out_acc:
	of_node_put(cpu_node);

	return ret;
}

#ifdef CONFIG_ARM64
static int __init qcom_cpu_init(unsigned int cpu)
{
	/* Mark CPU0 cold boot flag as done */
	if (!cpu && !per_cpu(cold_boot_done, cpu))
		per_cpu(cold_boot_done, cpu) = true;

	return 0;
}

static int __init qcom_cpu_prepare(unsigned int cpu)
{
	const cpumask_t *mask = cpumask_of(cpu);

	if (qcom_scm_set_cold_boot_addr(secondary_entry, mask)) {
		pr_warn("CPU%d:Failed to set boot address\n", cpu);
		return -ENOSYS;
	}

	return 0;
}

static int qcom_cpu_boot(unsigned int cpu)
{
	int ret = 0;

	if (per_cpu(cold_boot_done, cpu) == false) {
		ret = qcom_cortex_a_release_secondary(cpu);
		if (ret)
			return ret;
		per_cpu(cold_boot_done, cpu) = true;
	}

	return ret;
}

const struct cpu_operations qcom_cortex_a_ops = {
	.name		= "qcom,arm-cortex-acc",
	.cpu_init	= qcom_cpu_init,
	.cpu_prepare	= qcom_cpu_prepare,
	.cpu_boot	= qcom_cpu_boot,
};
#endif
