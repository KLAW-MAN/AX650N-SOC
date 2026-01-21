/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/reset.h>

#define CHANNELS_PER_PWM                  8
#define CHANNEL_CLK_SEL                   1	/* 1:24Mhz 0:32Khz */
#define PWM_TIMERN_LOADCOUNT_OFF(N)       (0x0 + (N) * 0x14)
#define PWM_TIMERN_CONTROLREG_OFF(N)      (0x8 + (N) * 0x14)
#define PWM_TIMERN_LOADCOUNT2_OFF(N)      (0xB0 + (N) * 0x4)
#define PWM_TIMERN_MODE                   0x1E	/* PWM mode but not enable */
#define PWM_TIMERN_EN                     0x1	/* PWM enable bit */

#define CHANNEL_CLK_SEL_FREQ          24000000	/* 24MHZ */

struct axera_pwm_chip {
	struct pwm_chip chip;
	struct resource *pc_resource;
	struct clk *pwm_clk[CHANNELS_PER_PWM];
	struct clk *pwm_pclk;
	struct reset_control *glb_rst;
	struct reset_control *ch_rst[CHANNELS_PER_PWM];
};

static inline struct axera_pwm_chip *to_axera_pwm_chip(struct pwm_chip *chip)
{
	return container_of(chip, struct axera_pwm_chip, chip);
}

static int pwm_axera_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct axera_pwm_chip *our_chip = to_axera_pwm_chip(chip);
	void __iomem *pwm_timer_base;
	u32 reg;
	int ret;

	/* use reset interface */
	reset_control_deassert(our_chip->ch_rst[pwm->hwpwm]);

	ret = clk_prepare_enable(our_chip->pwm_pclk);

	if (ret) {
		pr_err("%s %s chan %u pclk prepare enable fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return ret;
	}

	ret = clk_prepare_enable(our_chip->pwm_clk[pwm->hwpwm]);

	if (ret) {
		pr_err("%s %s chan %u clk prepare enable fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return ret;
	}

	clk_set_rate(our_chip->pwm_clk[pwm->hwpwm], CHANNEL_CLK_SEL_FREQ);

	pwm_timer_base = devm_ioremap(chip->dev, our_chip->pc_resource->start,
				      resource_size(our_chip->pc_resource));
	if (IS_ERR(pwm_timer_base)) {
		pr_err("%s %s chan %u ioremap source fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return PTR_ERR(pwm_timer_base);
	}

	reg = readl(pwm_timer_base + PWM_TIMERN_CONTROLREG_OFF(pwm->hwpwm));
	reg |= PWM_TIMERN_EN;
	writel(reg, pwm_timer_base + PWM_TIMERN_CONTROLREG_OFF(pwm->hwpwm));

	devm_iounmap(chip->dev, pwm_timer_base);
	return 0;
}

static void pwm_axera_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
	struct axera_pwm_chip *our_chip = to_axera_pwm_chip(chip);
	void __iomem *pwm_timer_base;
	u32 reg;

	pwm_timer_base = devm_ioremap(chip->dev, our_chip->pc_resource->start,
				      resource_size(our_chip->pc_resource));
	if (IS_ERR(pwm_timer_base)) {
		pr_err("%s %s chan %u ioremap source fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return;
	}

	reg = readl(pwm_timer_base + PWM_TIMERN_CONTROLREG_OFF(pwm->hwpwm));
	reg &= ~PWM_TIMERN_EN;
	writel(reg, pwm_timer_base + PWM_TIMERN_CONTROLREG_OFF(pwm->hwpwm));

	devm_iounmap(chip->dev, pwm_timer_base);
	clk_disable_unprepare(our_chip->pwm_pclk);
	clk_disable_unprepare(our_chip->pwm_clk[pwm->hwpwm]);

}

static int pwm_axera_config(struct pwm_chip *chip, struct pwm_device *pwm,
			    int duty_ns, int period_ns)
{
	struct axera_pwm_chip *our_chip = to_axera_pwm_chip(chip);
	u64 temp;
	u32 period_count;
	u32 duty_count;
	void __iomem *pwm_timer_base;
	struct pwm_state cur_state;
	int ret;

	/* use reset interface */
	reset_control_deassert(our_chip->ch_rst[pwm->hwpwm]);

	ret = clk_prepare_enable(our_chip->pwm_pclk);

	if (ret) {
		pr_err("%s %s chan %u pclk prepare enable fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return ret;
	}

	ret = clk_prepare_enable(our_chip->pwm_clk[pwm->hwpwm]);

	if (ret) {
		pr_err("%s %s chan %u clk prepare enable fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return ret;
	}

	clk_set_rate(our_chip->pwm_clk[pwm->hwpwm], CHANNEL_CLK_SEL_FREQ);

	pwm_get_state(pwm, &cur_state);

	if (period_ns < (1000000000 / CHANNEL_CLK_SEL_FREQ)) {
		pr_err
		    ("%s %s chan %u period is to smaller, even smaller than input clock\n",
		     __func__, dev_name(chip->dev), pwm->hwpwm);
		return -EINVAL;
	}

	pwm_timer_base = devm_ioremap(chip->dev, our_chip->pc_resource->start,
				      resource_size(our_chip->pc_resource));
	if (IS_ERR(pwm_timer_base)) {
		pr_err("%s %s chan %u ioremap source fail\n", __func__,
		       dev_name(chip->dev), pwm->hwpwm);
		return PTR_ERR(pwm_timer_base);
	}

	/* disable pwm timer and config pwm mode */
	writel(PWM_TIMERN_MODE,
	       pwm_timer_base + PWM_TIMERN_CONTROLREG_OFF(pwm->hwpwm));

	temp = (u64) period_ns * CHANNEL_CLK_SEL_FREQ;
	do_div(temp, 1000000000);
	period_count = (u32) temp;
	temp = (u64) duty_ns * CHANNEL_CLK_SEL_FREQ;
	do_div(temp, 1000000000);
	duty_count = (u32) temp;

	pr_debug("%s: config %s channel %u period_count:%u duty_count:%u\n",
		 __func__, dev_name(chip->dev), pwm->hwpwm, period_count,
		 duty_count);

	writel(duty_count,
	       pwm_timer_base + PWM_TIMERN_LOADCOUNT2_OFF(pwm->hwpwm));
	writel(period_count - duty_count,
	       pwm_timer_base + PWM_TIMERN_LOADCOUNT_OFF(pwm->hwpwm));

	pr_debug("period = %u, duty = %u\n",
		readl(pwm_timer_base + PWM_TIMERN_LOADCOUNT2_OFF(pwm->hwpwm)),
		readl(pwm_timer_base + PWM_TIMERN_LOADCOUNT_OFF(pwm->hwpwm)));

	/* if current pwm is enabled, then keep enable */
	if (cur_state.enabled)
		writel((PWM_TIMERN_EN | PWM_TIMERN_MODE),
		       pwm_timer_base + PWM_TIMERN_CONTROLREG_OFF(pwm->hwpwm));

	devm_iounmap(chip->dev, pwm_timer_base);

	return 0;
}

static const struct pwm_ops pwm_axera_ops = {
	.enable = pwm_axera_enable,
	.disable = pwm_axera_disable,
	.config = pwm_axera_config,
	.owner = THIS_MODULE,
};

static int pwm_axera_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct axera_pwm_chip *chip;
	struct resource *res;
	char buf[20];
	int ret, i;

	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (chip == NULL)
		return -ENOMEM;

	chip->chip.dev = &pdev->dev;
	chip->chip.ops = &pwm_axera_ops;
	chip->chip.base = -1;
	chip->chip.npwm = CHANNELS_PER_PWM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODATA;
	chip->pc_resource = res;

	platform_set_drvdata(pdev, chip);

	for (i = 0; i < CHANNELS_PER_PWM; i++) {
		sprintf(buf, "pwm-ch%d-clk", i);
		chip->pwm_clk[i] = devm_clk_get(&pdev->dev, buf);
		if (IS_ERR(chip->pwm_clk[i])) {
			pr_err("axera get %s fail\n", buf);
			return PTR_ERR(chip->pwm_clk[i]);
		}
	}

	chip->pwm_pclk = devm_clk_get(&pdev->dev, "pclk");
	if (IS_ERR(chip->pwm_pclk)) {
		pr_err("axera get pclk fail\n");
		return PTR_ERR(chip->pwm_pclk);
	}

	/* global reset */
	chip->glb_rst = devm_reset_control_get_optional(&pdev->dev, "pwm-rst");
	if (IS_ERR(chip->glb_rst)) {
		pr_err("axera get global reset failed\n");
		return PTR_ERR(chip->glb_rst);
	}

	reset_control_deassert(chip->glb_rst);

	for (i = 0; i < CHANNELS_PER_PWM; i++) {
		sprintf(buf, "pwm-ch%d-rst", i);
		chip->ch_rst[i] = devm_reset_control_get_optional(&pdev->dev, buf);
		if (IS_ERR(chip->ch_rst[i])) {
			pr_err("axera get %s fail\n", buf);
			return PTR_ERR(chip->ch_rst[i]);
		}
	}

	ret = pwmchip_add(&chip->chip);
	if (ret < 0) {
		dev_err(dev, "failed to register PWM chip\n");
		return ret;
	}

	return 0;
}

static int pwm_axera_remove(struct platform_device *pdev)
{
	struct axera_pwm_chip *chip;
	chip = platform_get_drvdata(pdev);
	pwmchip_remove(&chip->chip);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int pwm_axera_suspend(struct device *dev)
{

	return 0;
}

static int pwm_axera_resume(struct device *dev)
{

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(pwm_axera_pm_ops, pwm_axera_suspend, pwm_axera_resume);

static const struct of_device_id axera_pwm_matches[] = {
	{.compatible = "axera,ax650x-pwm"},
	{},
};

static struct platform_driver pwm_axera_driver = {
	.driver = {
		   .name = "axera-pwm",
		   .pm = &pwm_axera_pm_ops,
		   .of_match_table = axera_pwm_matches,
		   },
	.probe = pwm_axera_probe,
	.remove = pwm_axera_remove,
};

module_platform_driver(pwm_axera_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
MODULE_ALIAS("platform:axera-pwm");