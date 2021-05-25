// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *  Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *  Copyright (c) 2014 The Linux Foundation. All rights reserved.
 */

#include <linux/bitops.h>
#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/smp.h>
#include <linux/qcom_scm.h>

#include <asm/smp_plat.h>

#ifdef CONFIG_ARM64
#include <asm/cpu_ops.h>

DEFINE_PER_CPU(int, cold_boot_done);
#endif

#define CPU_PWR_CTL		0x4
#define CPU_PWR_GATE_CTL	0x14

#define APCS_CPU_PWR_CTL	0x04
#define PLL_CLAMP		BIT(8)
#define CORE_PWRD_UP		BIT(7)
#define COREPOR_RST		BIT(5)
#define CORE_RST		BIT(4)
#define L2DT_SLP		BIT(3)
#define CORE_MEM_CLAMP		BIT(1)
#define CLAMP			BIT(0)

#define APC_PWR_GATE_CTL	0x14
#define BHS_CNT_SHIFT		24
#define LDO_PWR_DWN_SHIFT	16
#define LDO_BYP_SHIFT		8
#define BHS_SEG_SHIFT		1
#define BHS_EN			BIT(0)

/* Taken from https://lore.kernel.org/linux-arm-msm/20210513153442.52941-3-bartosz.dudziak@snejp.pl/ */
int qcom_cortex_a_release_secondary(unsigned int cpu)
{
	int ret = 0;
	void __iomem *reg;
	struct device_node *cpu_node, *acc_node;
	u32 reg_val;

	cpu_node = of_get_cpu_node(cpu, NULL);
	if (!cpu_node)
		return -ENODEV;

	acc_node = of_parse_phandle(cpu_node, "qcom,acc", 0);
	if (!acc_node) {
		ret = -ENODEV;
		goto out_acc;
	}

	reg = of_iomap(acc_node, 0);
	if (!reg) {
		ret = -ENOMEM;
		goto out_acc_map;
	}

	/* Put the CPU into reset. */
	reg_val = CORE_RST | COREPOR_RST | CLAMP | CORE_MEM_CLAMP;
	writel(reg_val, reg + APCS_CPU_PWR_CTL);

	/* Turn on the BHS, set the BHS_CNT to 16 XO clock cycles */
	writel(BHS_EN | (0x10 << BHS_CNT_SHIFT), reg + APC_PWR_GATE_CTL);
	/* Wait for the BHS to settle */
	udelay(2);

	reg_val &= ~CORE_MEM_CLAMP;
	writel(reg_val, reg + APCS_CPU_PWR_CTL);

	reg_val |= L2DT_SLP;
	writel(reg_val, reg + APCS_CPU_PWR_CTL);
	udelay(2);

	reg_val = (reg_val | BIT(17)) & ~CLAMP;
	writel(reg_val, reg + APCS_CPU_PWR_CTL);
	udelay(2);

	/* Release CPU out of reset and bring it to life. */
	reg_val &= ~(CORE_RST | COREPOR_RST);
	writel(reg_val, reg + APCS_CPU_PWR_CTL);
	reg_val |= CORE_PWRD_UP;
	writel(reg_val, reg + APCS_CPU_PWR_CTL);

	iounmap(reg);

out_acc_map:
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
