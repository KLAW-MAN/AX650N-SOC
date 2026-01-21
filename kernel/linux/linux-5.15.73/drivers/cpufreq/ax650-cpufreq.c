/*
 * Copyright (C) 2020 Axera
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/cpumask.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm_opp.h>
#include <linux/slab.h>
#include <linux/pwm.h>
#include <linux/of_address.h>
#include <linux/delay.h>

#define CPU_CLK_MUX_REG			(0x0)
#define PCLK_SEL_SHIFT      	(29)
#define PCLK_SEL_MASK       	(1 << PCLK_SEL_SHIFT)
#define GICCLK_SEL_SHIFT    	(28)
#define GICCLK_SEL_MASK			(1 << GICCLK_SEL_SHIFT)
#define DSUCLK_SEL_SHIFT    	(26)
#define DSUCLK_SEL_MASK			(0x3 << DSUCLK_SEL_SHIFT)
#define DSUPERICLK_SEL_SHIFT    (25)
#define DSUPERICLK_SEL_MASK		(1 << DSUPERICLK_SEL_SHIFT)
#define ATCLK_SEL_SHIFT			(0)
#define ATCLK_SEL_MASK			(1 << ATCLK_SEL_SHIFT)
#define CORECLK_SEL_SHIFT(n)	(1 + 3 * (n))
#define CORECLK_SEL_MASK(n)		(0x7 << CORECLK_SEL_SHIFT(n))
#define CORECLK_SEL_ALL_MASK	0x1FFFFFE

#define GRP0_PLL_OPEN_SET_REG	0xE0
#define GRP0_PLL_OPEN_CLR_REG	0xE4
#define BIT_CPLL1_OPEN			(1 << 1)
#define BIT_CPLL0_OPEN			(1 << 0)

#define CPLL0_ON_CFG_SET_REG	0xA4
#define CPLL0_ON_CFG_CLR_REG	0xA8
#define BIT_CPLL0_ON			(1 << 0)

#define CPLL1_ON_CFG_SET_REG	0xB0
#define CPLL1_ON_CFG_CLR_REG	0xB4
#define BIT_CPLL1_ON			(1 << 0)

#define CPLL0_CFG1_REG			0x10
#define CPLL1_CFG1_REG			0x44
#define POST_DIV_MASK			(0x3 << 23)
#define PRE_DIV_SHIFT			17
#define PRE_DIV_MASK			(0x3 << PRE_DIV_SHIFT)
#define STB_X2_EN				(1 << 14)
#define FBK_INT_MASK			0x1FF

#define CPLL0_CFG0_REG			0x4
#define CPLL1_CFG0_REG			0x38
#define FBK_FRA_MASK			(0xFFFFFF << 2)

#define CPLL0_STS_REG			0x0
#define BIT_CPLL0_LOCKED		(1 << 0)

#define CPLL1_STS_REG			0x34
#define BIT_CPLL1_LOCKED		(1 << 0)

#define GRP0_PLL_RDY_STS_REG	0x9C
#define BIT_CPLL1_RDY			(1 << 1)
#define BIT_CPLL0_RDY			(1 << 0)

#define FBK_INT_MAX				511 /* 9 bit */
#define PLL_PRE_DIV				1
#define CPLLX_CLK_IN_RATE		24000000
#define CPLLX_GRANU_RATE		((CPLLX_CLK_IN_RATE) / (1 << (PLL_PRE_DIV)))

#define AX650_CPU_CORE_CNT		8
#define AX650_CPU_MIN_FREQ		24000000

#define PLL_LOOP_MAX_CNT	1000

struct cpu_freq_resource {
	void __iomem *cpu_clk_base;
	void __iomem *cpu_pll_base;
};

struct ax650_cpu_dvfs_info {
	struct device *cpu_dev;
	struct platform_device *ax650_cpufreq_pdev;
	struct pwm_device *cpu_vol_pwm;
	void *pwm_transfer_param;
	unsigned int latency_ns;
	struct cpu_freq_resource cpu_freq_res;
	unsigned long cur_freq;
};

struct volt_duty_transfer
{
	int sub;
	int scale;
	int div;
};

/* according to hardware design duty = (2.2 - 2*Vout) / 1.2 * 100 unit:V */
struct volt_duty_transfer ax650_volt_duty_param = {2200, 2, 12};
struct ax650_cpu_dvfs_info *ax650_dvfs_info;

#define CPU_MAX_FREQ		1700000000
#define CPU_MAX_REAL_FREQ	1500000000
#define COMM_SYS_GLB		0x4200000
#define COMM_SYS_DUMMY_SW2	(COMM_SYS_GLB + 0x190) /* read chip type */

typedef enum chip_type {
	NONE_CHIP_TYPE = 0x0,
	AX650A_CHIP = 0x1,
	AX650N_CHIP = 0x2,
	AX650C_CHIP = 0x3,
	AX750_CHIP = 0x4,
	AX650_CHIP_MAX = 0x5,
} chip_type_e;

static int volt_to_duty(struct volt_duty_transfer *volt_duty_param, unsigned long volt_uv)
{
	unsigned long duty_scale;
	unsigned long volt_mv = (unsigned int)(volt_uv / 1000);

	duty_scale = (volt_duty_param->sub - volt_duty_param->scale * volt_mv) /
					volt_duty_param->div;

	return duty_scale;
}

static int ax650_cpufreq_set_voltage(struct ax650_cpu_dvfs_info *info, unsigned long volt)
{
	int ret, duty_ns, duty_scale;
	struct pwm_args pwm_param;
	struct pwm_state pwm_status;
	struct pwm_device *cpu_pwm = pwm_get(&ax650_dvfs_info->ax650_cpufreq_pdev->dev, NULL);
	struct volt_duty_transfer *pwm_transfer_param =
					(struct volt_duty_transfer *)info->pwm_transfer_param;

	if (IS_ERR(cpu_pwm)) {
		pr_err("%s get pwm device fail which may affect cannot change voltage\n",
				__func__);
		return -1;
	}

	pwm_get_state(cpu_pwm, &pwm_status);
	if (pwm_status.enabled != true) {
		pr_err("%s the pwm is not enabled, must something wrong\n",
				__func__);
		pwm_put(cpu_pwm);
		return -1;
	}

	duty_scale = volt_to_duty(pwm_transfer_param, volt);
	pwm_get_args(cpu_pwm, &pwm_param);

	duty_ns = (int)(duty_scale * pwm_param.period / 100);
	ret = pwm_config(cpu_pwm, duty_ns, (int)pwm_param.period);

	pwm_put(cpu_pwm);

	return ret;
}

static void ax650_cpufreq_pll_config(struct ax650_cpu_dvfs_info *info,
			unsigned long cpu_freq, unsigned long dsu_freq)
{
	unsigned int val, fbk_int;
	unsigned int loop_cnt_us = 0;
	void __iomem *cpu_pll_base = info->cpu_freq_res.cpu_pll_base;

	pr_debug("cpu cpll0 config begin\n");
	fbk_int = cpu_freq / CPLLX_GRANU_RATE;
	BUG_ON(fbk_int > FBK_INT_MAX);

	/* config OFF */
	writel(BIT_CPLL0_OPEN, cpu_pll_base + GRP0_PLL_OPEN_CLR_REG);
	writel(BIT_CPLL0_ON, cpu_pll_base + CPLL0_ON_CFG_CLR_REG);

	/* config post/pre_div stb_x2_en and fbk_int */
	val = readl(cpu_pll_base + CPLL0_CFG1_REG);
	val &= ~POST_DIV_MASK;   /* set post_div 0 */
	val &= ~PRE_DIV_MASK;    /* set pre_div 4*/
	val |= (PLL_PRE_DIV << PRE_DIV_SHIFT);
	val &= ~STB_X2_EN;        /* set stb_x2_en 0 */
	val &= ~FBK_INT_MASK;    /* set fbk_int */
	val |= fbk_int;
	writel(val, cpu_pll_base + CPLL0_CFG1_REG);

	/* config FBK_FRA */
	val = readl(cpu_pll_base + CPLL0_CFG0_REG);
	val &= ~FBK_FRA_MASK;   /* set fbk_fra 0 */
	writel(val, cpu_pll_base + CPLL0_CFG0_REG);

	/* config ON */
	writel(BIT_CPLL0_ON, cpu_pll_base + CPLL0_ON_CFG_SET_REG);

	/* CPLL0 wait pll locked and ready, together with CPLL1
	 * for decrease some wait time
	 */
	pr_debug("cpu cpll0 config end\n");

	pr_debug("dsu cpll1 config begin\n");
	fbk_int = dsu_freq / CPLLX_GRANU_RATE;
	BUG_ON(fbk_int > FBK_INT_MAX);

	/* config OFF */
	writel(BIT_CPLL1_OPEN, cpu_pll_base + GRP0_PLL_OPEN_CLR_REG);
	writel(BIT_CPLL1_ON, cpu_pll_base + CPLL1_ON_CFG_CLR_REG);

	/* config post/pre_div stb_x2_en and fbk_int */
	val = readl(cpu_pll_base + CPLL1_CFG1_REG);
	val &= ~POST_DIV_MASK;   /* set post_div 0 */
	val &= ~PRE_DIV_MASK;    /* set pre_div 4*/
	val |= (PLL_PRE_DIV << PRE_DIV_SHIFT);
	val &= ~STB_X2_EN;        /* set stb_x2_en 0 */
	val &= ~FBK_INT_MASK;    /* set fbk_int */
	val |= fbk_int;
	writel(val, cpu_pll_base + CPLL1_CFG1_REG);

	/* config FBK_FRA */
	val = readl(cpu_pll_base + CPLL1_CFG0_REG);
	val &= ~FBK_FRA_MASK;   /* set fbk_fra 0 */
	writel(val, cpu_pll_base + CPLL1_CFG0_REG);

	/* config ON */
	writel(BIT_CPLL1_ON, cpu_pll_base + CPLL1_ON_CFG_SET_REG);

	/* wait locked */
	loop_cnt_us = 0;
	while (1) {
		val = readl(cpu_pll_base + CPLL0_STS_REG);
		if ((val & BIT_CPLL0_LOCKED) == BIT_CPLL0_LOCKED)
			break;
		/* if exceed 1ms we think may be wrong */
		if (++loop_cnt_us > PLL_LOOP_MAX_CNT)
			BUG();
		udelay(1);
	}

	/* wait locked */
	loop_cnt_us = 0;
	while (1) {
		val = readl(cpu_pll_base + CPLL1_STS_REG);
		if ((val & BIT_CPLL1_LOCKED) == BIT_CPLL1_LOCKED)
			break;
		/* if exceed 1ms we think may be wrong */
		if (++loop_cnt_us > PLL_LOOP_MAX_CNT)
			BUG();
		udelay(1);
	}

	/* wait ready */
	loop_cnt_us = 0;
	while (1) {
		val = readl(cpu_pll_base + GRP0_PLL_RDY_STS_REG);
		if ((val & (BIT_CPLL0_RDY | BIT_CPLL1_RDY)) ==
					(BIT_CPLL0_RDY | BIT_CPLL1_RDY))
			break;
		/* if exceed 1ms we think may be wrong */
		if (++loop_cnt_us > PLL_LOOP_MAX_CNT)
			BUG();
		udelay(1);
	}

	/* config open */
	writel((BIT_CPLL1_OPEN | BIT_CPLL0_OPEN), cpu_pll_base + GRP0_PLL_OPEN_SET_REG);
	pr_debug("dsu cpll1 config end\n");
}

static int ax650_cpufreq_set_freq(struct ax650_cpu_dvfs_info *info,
			unsigned long cpu_freq)
{
	int i;
	unsigned long dsu_freq = cpu_freq / 100 * 75;  /* about 75% cpu freq */
	unsigned int reg_val = 0;

	if (cpu_freq <= AX650_CPU_MIN_FREQ) {
		writel(reg_val, info->cpu_freq_res.cpu_clk_base + CPU_CLK_MUX_REG);
		return 0;
	}

	//switch to 750Mhz
	reg_val = readl(info->cpu_freq_res.cpu_clk_base + CPU_CLK_MUX_REG);
	reg_val &= ~(DSUCLK_SEL_MASK | CORECLK_SEL_ALL_MASK);
	reg_val |= (1 << DSUCLK_SEL_SHIFT);
	for (i = 0; i < AX650_CPU_CORE_CNT; i++)
		reg_val |= (1 << CORECLK_SEL_SHIFT(i));
	writel(reg_val, info->cpu_freq_res.cpu_clk_base + CPU_CLK_MUX_REG);

	//cpll0/cpll1 config
	ax650_cpufreq_pll_config(info, cpu_freq, dsu_freq);

	//cpu&dsu switch to cpll0/cpll1
	reg_val &= ~(DSUCLK_SEL_MASK | CORECLK_SEL_ALL_MASK);
	reg_val |= (2 << DSUCLK_SEL_SHIFT);
	for (i = 0; i < AX650_CPU_CORE_CNT; i++)
		reg_val |= (4 << CORECLK_SEL_SHIFT(i));
	writel(reg_val, info->cpu_freq_res.cpu_clk_base + CPU_CLK_MUX_REG);

	return 0;
}

static int ax650_cpufreq_set_target(struct cpufreq_policy *policy,
				  unsigned int index)
{
	struct cpufreq_frequency_table *freq_table = policy->freq_table;
	struct ax650_cpu_dvfs_info *info = policy->driver_data;
	struct dev_pm_opp *opp;
	unsigned long freq_hz, old_freq_hz;
	unsigned long old_volt, new_volt;
	int ret = 0;
	int freq_increase = 0;
	unsigned int pwm_wait_ms = DIV_ROUND_UP(ax650_dvfs_info->latency_ns, NSEC_PER_MSEC);

	old_freq_hz = policy->cur * 1000;
	freq_hz = freq_table[index].frequency * 1000;
	if (freq_hz > old_freq_hz)
		freq_increase = 1;

	opp = dev_pm_opp_find_freq_ceil(info->cpu_dev, &old_freq_hz);
	if (IS_ERR(opp)) {
		pr_err("failed to find old freq OPP\n");
		return PTR_ERR(opp);
	}
	old_volt = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	opp = dev_pm_opp_find_freq_ceil(info->cpu_dev, &freq_hz);
	if (IS_ERR(opp)) {
		pr_err("failed to find new freq OPP\n");
		return PTR_ERR(opp);
	}
	new_volt = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	if (freq_increase && (old_volt != new_volt)) {
		ret = ax650_cpufreq_set_voltage(info, new_volt);
		if (ret) {
			pr_err("increase freq but volt set fail\n");
			return ret;
		}
		msleep(pwm_wait_ms);  /* increase voltage need wait */
	}

	ax650_cpufreq_set_freq(info, freq_hz);

	if (!freq_increase && (old_volt != new_volt)) {
		ret = ax650_cpufreq_set_voltage(info, new_volt);
		if (ret) {
			pr_err("%s volt set fail after freq set, reset old freq\n", __func__);
			freq_hz = old_freq_hz;
			ax650_cpufreq_set_freq(info, freq_hz);
		}
	}

	info->cur_freq = freq_hz;

	return ret;
}

static int ax650_cpufreq_init(struct cpufreq_policy *policy)
{
	struct cpufreq_frequency_table *freq_table;
	int ret, max_opps, duty_scale, duty_ns;
	struct device *cpu_dev;
	unsigned long max_freq, max_volt;
	struct dev_pm_opp *opp;
	struct pwm_device *cpu_pwm;
	struct pwm_args cpu_pwm_args;
	unsigned int pwm_wait_ms = DIV_ROUND_UP(ax650_dvfs_info->latency_ns, NSEC_PER_MSEC);

	cpu_dev = get_cpu_device(0);
	ret = dev_pm_opp_init_cpufreq_table(cpu_dev, &freq_table);
	if (ret) {
		pr_err("failed to init cpufreq table for policy\n");
		return ret;
	}

	cpumask_copy(policy->cpus, cpu_possible_mask);
	policy->freq_table = freq_table;
	policy->driver_data = (void *)ax650_dvfs_info;
	policy->cpuinfo.transition_latency = ax650_dvfs_info->latency_ns;

	/* boot stage run the high frequency, we set default freq */
	max_opps = dev_pm_opp_get_opp_count(cpu_dev);
	max_freq = freq_table[max_opps - 1].frequency * 1000;
	ax650_dvfs_info->cur_freq = max_freq;
	opp = dev_pm_opp_find_freq_ceil(cpu_dev, &max_freq);
	if (IS_ERR(opp)) {
		pr_err("failed to find old freq OPP\n");
		return PTR_ERR(opp);
	}
	max_volt = dev_pm_opp_get_voltage(opp);
	dev_pm_opp_put(opp);

	duty_scale = volt_to_duty(&ax650_volt_duty_param, max_volt);
	cpu_pwm = pwm_get(&ax650_dvfs_info->ax650_cpufreq_pdev->dev, NULL);
	if (IS_ERR(cpu_pwm)) {
		pr_warn("%s get pwm device fail which may affect cannot change voltage\n",
					__func__);
		return 0;
	}
	pwm_get_args(cpu_pwm, &cpu_pwm_args);
	duty_ns = (int)(duty_scale * cpu_pwm_args.period / 100);
	ret = pwm_config(cpu_pwm, duty_ns, (int)cpu_pwm_args.period);
	pwm_enable(cpu_pwm);
	pwm_put(cpu_pwm);
	msleep(pwm_wait_ms);
	ax650_cpufreq_set_freq(ax650_dvfs_info, max_freq);

	return ret;
}

static int ax650_cpufreq_exit(struct cpufreq_policy *policy)
{
	struct ax650_cpu_dvfs_info *info = policy->driver_data;

	dev_pm_opp_free_cpufreq_table(info->cpu_dev, &policy->freq_table);

	return 0;
}

static unsigned int ax650_cpufreq_get(unsigned int cpu)
{
	struct ax650_cpu_dvfs_info *info;
	struct cpufreq_policy *policy = cpufreq_cpu_get_raw(cpu);

	if (!policy) {
		pr_err("%s: No policy associated to cpu: %d\n", __func__, cpu);
		return 0;
	}
	info = policy->driver_data;

	return (info->cur_freq / 1000);
}

static struct cpufreq_driver ax650_cpufreq_driver = {
	.flags = CPUFREQ_NEED_INITIAL_FREQ_CHECK |
		 CPUFREQ_HAVE_GOVERNOR_PER_POLICY |
		 CPUFREQ_IS_COOLING_DEV,
	.verify = cpufreq_generic_frequency_table_verify,
	.target_index = ax650_cpufreq_set_target,
	.get = ax650_cpufreq_get,
	.init = ax650_cpufreq_init,
	.exit = ax650_cpufreq_exit,
	.name = "ax650-cpufreq",
	.attr = cpufreq_generic_attr,
};

static int ax650_cpu_dvfs_info_init(struct platform_device *pdev,
					struct ax650_cpu_dvfs_info *info)
{
	struct device *cpu_dev;
	struct resource *res;
	struct cpumask temp_cpu_mask;
	int ret;
	unsigned int transition_latency;
	void __iomem *regs;
	chip_type_e chip_id;
	struct dev_pm_opp *opp;
	unsigned long freq = CPU_MAX_FREQ;
	unsigned long volt;

	regs = ioremap(COMM_SYS_DUMMY_SW2, 0x4);
	chip_id = readl(regs);
	iounmap(regs);

	cpu_dev = get_cpu_device(0);
	if (!cpu_dev) {
		ret = -ENODEV;
		pr_err("failed to get cpu0 device\n");
		goto err_ret;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "cpu_clk");
	info->cpu_freq_res.cpu_clk_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(info->cpu_freq_res.cpu_clk_base)) {
		pr_err("failed to remap cpu clk addr\n");
		ret = -ENOSPC;
		goto err_ret;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pllc_glb");
	info->cpu_freq_res.cpu_pll_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(info->cpu_freq_res.cpu_clk_base)) {
		pr_err("failed to remap pllc glb addr\n");
		ret = -ENOSPC;
		goto unmap_cpu;
	}

	/* Get OPP-sharing information from "operating-points-v2" bindings */
	ret = dev_pm_opp_of_get_sharing_cpus(cpu_dev, &temp_cpu_mask);
	if (ret) {
		pr_err("failed to get OPP-sharing information\n");
		goto umap_pllc;
	} else if (!cpumask_equal(&temp_cpu_mask, cpu_possible_mask)) {
		pr_err("ax650 should all cpu cores share opps\n");
		ret = -EPERM;
		goto umap_pllc;
	}

	ret = dev_pm_opp_of_cpumask_add_table(&temp_cpu_mask);
	if (ret) {
		pr_err("no OPP table for cpus\n");
		goto umap_pllc;
	}

	if (chip_id == AX650A_CHIP || chip_id == AX650C_CHIP) {
		opp = dev_pm_opp_find_freq_ceil(cpu_dev, &freq);
		volt = dev_pm_opp_get_voltage(opp);
		dev_pm_opp_put(opp);
		dev_pm_opp_remove(cpu_dev, freq);
		dev_pm_opp_add(cpu_dev, CPU_MAX_REAL_FREQ, volt);
	}

	transition_latency = dev_pm_opp_get_max_transition_latency(cpu_dev);
	if (!transition_latency) {
		ret = -EPERM;
		pr_err("not get transition latency\n");
		goto rm_opp_table;
	}

	info->ax650_cpufreq_pdev = pdev;
	info->cpu_dev = cpu_dev;
	info->pwm_transfer_param = (void *)&ax650_volt_duty_param;
	info->latency_ns = transition_latency;

	return 0;

rm_opp_table:
	dev_pm_opp_of_cpumask_remove_table(&temp_cpu_mask);

umap_pllc:
	devm_iounmap(&pdev->dev, info->cpu_freq_res.cpu_pll_base);
unmap_cpu:
	devm_iounmap(&pdev->dev, info->cpu_freq_res.cpu_clk_base);

err_ret:
	return ret;
}

static void ax650_cpu_dvfs_info_release(struct platform_device *pdev, struct ax650_cpu_dvfs_info *info)
{
	devm_iounmap(&pdev->dev, info->cpu_freq_res.cpu_pll_base);
	devm_iounmap(&pdev->dev, info->cpu_freq_res.cpu_clk_base);

	kfree(info);

	dev_pm_opp_of_cpumask_remove_table(cpu_possible_mask);
}

static int ax650_cpufreq_hw_probe(struct platform_device *pdev)
{
	int ret;

	ax650_dvfs_info = kzalloc(sizeof(struct ax650_cpu_dvfs_info), GFP_KERNEL);
	if (!ax650_dvfs_info)
		return -1;

	ret = ax650_cpu_dvfs_info_init(pdev, ax650_dvfs_info);
	if (ret) {
		kfree(ax650_dvfs_info);
		pr_err("failed to initialize dvfs info for cpus\n");
		goto err_ret;
	}

	ret = cpufreq_register_driver(&ax650_cpufreq_driver);
	if (ret) {
		pr_err("failed to register ax650 cpufreq driver\n");
		goto release_dvfs_info;
	}

	return 0;

release_dvfs_info:
	ax650_cpu_dvfs_info_release(pdev, ax650_dvfs_info);
err_ret:
	return ret;
}


/* List of machines supported by this driver */
static const struct of_device_id ax650_cpufreq_match[] = {
	{ .compatible = "ax650,cpufreq", },
	{ }
};

static struct platform_driver ax650_cpufreq_hw_driver = {
	.driver = {
		.name	= "ax650-cpufreq",
		.of_match_table = ax650_cpufreq_match,
	},
	.probe		= ax650_cpufreq_hw_probe,
};

module_platform_driver(ax650_cpufreq_hw_driver);

MODULE_AUTHOR("bruce <wangkui@axera-chip.com>");
MODULE_DESCRIPTION("Axera cpufreq driver");
MODULE_LICENSE("GPL v2");
