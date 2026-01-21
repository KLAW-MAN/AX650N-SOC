#define pr_fmt(fmt) KBUILD_MODNAME ":%s:%d: " fmt, __func__, __LINE__

#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/watchdog.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define WDOG_CONTROL_REG_EN                 0x00
#define WDOG_CONTROL_WDT_EN                 0x01
#define WDOG_CONTROL_WDT_DIS                0x00
#define WDOG_TIMEOUT_COUNT_REG              0x0C
#define WDOG_TIMEOUT_COUNT_CTRL_REG         0x18
#define WDOG_CONTROL_REG_INTR               0x54
#define WDOG_CONTROL_WDT_INTR_EN            0x01
#define WDOG_CONTROL_WDT_INTR_CLR           0x3C
#define WDOG_CURRENT_COUNT_REG              0x24
#define WDOG_COUNTER_RESTART_REG            0x30
#define WDOG_COUNTER_RESTART_KICK_VALUE     0x61696370

#define AX_MAX_COUNT                        0xFFFFFFFF
#define AX_WDT_DEFAULT_SECONDS              90
#define AX_PING_DELAY_US                    40
#define AX_WDT_CLK_32K                      32000
#define AX_WDT_CLK_24M                      24000000

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started "
		 "(default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

struct ax_wdt {
	void __iomem *regs;
	void __iomem *common_sys_regs;
	struct clk *clk;
	struct clk *pclk;
	int irq;
	u32 abort_cfg_offs;
	u32 abort_en_bit;
	u32 abort_clr_bit;

	u32 timeout;
	u32 udelay_time;
	u32 keep_alive_rate;
	u32 suspend_timeout;
	u32 rate;
	struct watchdog_device wdd;
	struct reset_control *rst;
	/* Save/restore */
};

#define to_ax_wdt(wdd)	container_of(wdd, struct ax_wdt, wdd)

static inline int ax_wdt_is_enabled(struct ax_wdt *ax_wdt)
{
	return readl(ax_wdt->regs + WDOG_CONTROL_REG_EN) & WDOG_CONTROL_WDT_EN;
}

static int ax_wdt_clk_enable(struct ax_wdt *ax_wdt, unsigned long int rate)
{
	int err = 0;
	unsigned long int current_rate;

	err = clk_prepare_enable(ax_wdt->pclk);
	if (err) {
		pr_err("err %d\n", err);
		return err;
	}

	current_rate = clk_get_rate(ax_wdt->clk);
	if (unlikely(current_rate != rate)) {
		err = clk_set_rate(ax_wdt->clk, rate);
		if (err) {
			pr_err("err %d\n", err);
			return err;
		}
		if (likely(rate == AX_WDT_CLK_24M))
			ax_wdt->udelay_time = 0;
		else
			ax_wdt->udelay_time = AX_PING_DELAY_US;
		ax_wdt->wdd.min_timeout = DIV_ROUND_UP(0xFFFF, rate);
		ax_wdt->wdd.max_timeout = AX_MAX_COUNT / rate;
		ax_wdt->wdd.max_hw_heartbeat_ms = AX_MAX_COUNT / rate * 1000;
	}

	err = clk_prepare_enable(ax_wdt->clk);
	if (err) {
		clk_disable_unprepare(ax_wdt->pclk);
		pr_err("err %d\n", err);
		return err;
	}

	return 0;
}

static void ax_wdt_clk_disable(struct ax_wdt *ax_wdt)
{
	clk_disable_unprepare(ax_wdt->clk);
	clk_disable_unprepare(ax_wdt->pclk);
	return;
}

static int ax_wdt_ping(struct watchdog_device *wdd)
{
	struct ax_wdt *ax_wdt = to_ax_wdt(wdd);
	writel(WDOG_COUNTER_RESTART_KICK_VALUE, ax_wdt->regs +
	       WDOG_COUNTER_RESTART_REG);
	udelay(ax_wdt->udelay_time);
	writel(0, ax_wdt->regs + WDOG_COUNTER_RESTART_REG);
	return 0;
}

static int __ax_wdt_set_timeout(struct ax_wdt *ax_wdt, unsigned long int rate,
				unsigned int timeout_s)
{
	unsigned long long count_val = 0;

	count_val = timeout_s * rate;
	count_val = count_val < AX_MAX_COUNT ? count_val >> 16 : AX_MAX_COUNT >> 16;
	writel(count_val, ax_wdt->regs + WDOG_TIMEOUT_COUNT_REG);
	writel(1, ax_wdt->regs + WDOG_TIMEOUT_COUNT_CTRL_REG);
	udelay(ax_wdt->udelay_time);
	writel(0, ax_wdt->regs + WDOG_TIMEOUT_COUNT_CTRL_REG);

	return 0;
}

static int ax_wdt_set_timeout(struct watchdog_device *wdd,
			      unsigned int timeout_s)
{
	struct ax_wdt *ax_wdt = to_ax_wdt(wdd);

	wdd->timeout = timeout_s;
	__ax_wdt_set_timeout(ax_wdt, ax_wdt->rate, timeout_s);

	return 0;
}

static irqreturn_t ax_wdt_interrupt(int irq, void *data)
{
	struct ax_wdt *ax_wdt = data;

	writel(1, ax_wdt->regs + WDOG_CONTROL_WDT_INTR_CLR);
	ax_wdt_ping(&ax_wdt->wdd);
	return IRQ_HANDLED;
}

static void ax_wdt_irq_enable(struct ax_wdt *ax_wdt)
{
	writel(WDOG_CONTROL_WDT_INTR_EN, ax_wdt->regs + WDOG_CONTROL_REG_INTR);
}

static int ax_wdt_enable(struct ax_wdt *ax_wdt, bool en)
{
	if (en)
		writel(WDOG_CONTROL_WDT_EN, ax_wdt->regs + WDOG_CONTROL_REG_EN);
	else
		writel(WDOG_CONTROL_WDT_DIS, ax_wdt->regs + WDOG_CONTROL_REG_EN);

	return 0;
}

static int ax_wdt_abort_enable(struct ax_wdt *ax_wdt, bool en)
{
	u32 offs;

	if (!IS_ERR(ax_wdt->common_sys_regs) && (ax_wdt->abort_cfg_offs != ~0)) {
		if (!IS_ERR(ax_wdt->rst))
			reset_control_assert(ax_wdt->rst);

		offs = ax_wdt->abort_cfg_offs + (en ? 0x4 : 0x8);
		writel(BIT(ax_wdt->abort_en_bit), ax_wdt->common_sys_regs + offs);

		if (!IS_ERR(ax_wdt->rst))
			reset_control_deassert(ax_wdt->rst);
	}

	return 0;
}

static int ax_wdt_start(struct watchdog_device *wdd)
{
	struct ax_wdt *ax_wdt = to_ax_wdt(wdd);

	ax_wdt_abort_enable(ax_wdt, 1);
	ax_wdt_set_timeout(wdd, wdd->timeout);
	ax_wdt_enable(ax_wdt, 1);

	return 0;
}

static int ax_wdt_stop(struct watchdog_device *wdd)
{
	struct ax_wdt *ax_wdt = to_ax_wdt(wdd);
	ax_wdt_enable(ax_wdt, 0);
	return 0;
}

static int ax_wdt_restart(struct watchdog_device *wdd,
			  unsigned long action, void *data)
{
	struct ax_wdt *ax_wdt = to_ax_wdt(wdd);

	ax_wdt_enable(ax_wdt, 1);
	ax_wdt_set_timeout(wdd, 0);
	/* wait for reset to assert... */
	mdelay(500);
	return 0;
}

static unsigned int ax_wdt_get_timeleft(struct watchdog_device *wdd)
{
	struct ax_wdt *ax_wdt = to_ax_wdt(wdd);
	return readl(ax_wdt->regs + WDOG_CURRENT_COUNT_REG) / ax_wdt->rate;
}

static const struct watchdog_info ax_wdt_ident = {
	.options = WDIOF_KEEPALIVEPING | WDIOF_SETTIMEOUT | WDIOF_MAGICCLOSE,
	.identity = "Axera Watchdog",
};

static const struct watchdog_ops ax_wdt_ops = {
	.owner = THIS_MODULE,
	.start = ax_wdt_start,
	.stop = ax_wdt_stop,
	.ping = ax_wdt_ping,
	.set_timeout = ax_wdt_set_timeout,
	.get_timeleft = ax_wdt_get_timeleft,
	.restart = ax_wdt_restart,
};

#ifdef CONFIG_PM_SLEEP
static int ax_wdt_suspend(struct device *dev)
{
	int ret;
	struct ax_wdt *ax_wdt = dev_get_drvdata(dev);

	if (ax_wdt->keep_alive_rate) {
		ax_wdt_enable(ax_wdt, 0);
		ret = ax_wdt_clk_enable(ax_wdt, ax_wdt->keep_alive_rate);
		if (ret) {
			pr_err("err %d\n", ret);
			return ret;
		}
		__ax_wdt_set_timeout(ax_wdt, ax_wdt->keep_alive_rate,
				     ax_wdt->suspend_timeout);
		ax_wdt_enable(ax_wdt, 1);
	} else {
		ax_wdt_enable(ax_wdt, 0);
		ax_wdt_clk_disable(ax_wdt);
	}
	return 0;
}

static int ax_wdt_resume(struct device *dev)
{
	struct ax_wdt *ax_wdt = dev_get_drvdata(dev);
	struct watchdog_device *wdd = &ax_wdt->wdd;
	int err = 0;

	err = ax_wdt_clk_enable(ax_wdt, ax_wdt->rate);
	if (err) {
		pr_err("err %d\n", err);
		return err;
	}
	if (ax_wdt->suspend_timeout)
		ax_wdt_set_timeout(wdd, wdd->timeout);
	ax_wdt_enable(ax_wdt, 1);

	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(ax_wdt_pm_ops, ax_wdt_suspend, ax_wdt_resume);

static int ax_wdt_drv_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct watchdog_device *wdd;
	struct ax_wdt *ax_wdt;
	struct resource *mem;
	int ret;
	bool for_user;

	ax_wdt = devm_kzalloc(dev, sizeof(*ax_wdt), GFP_KERNEL);
	if (!ax_wdt) {
		pr_err("err %d\n", -ENOMEM);
		return -ENOMEM;
	}

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	ax_wdt->regs = devm_ioremap_resource(dev, mem);
	if (IS_ERR(ax_wdt->regs)) {
		pr_err("err %ld\n", PTR_ERR(ax_wdt->regs));
		return PTR_ERR(ax_wdt->regs);
	}

	ax_wdt->abort_cfg_offs = ~0;
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!IS_ERR_OR_NULL(mem)) {
		ax_wdt->common_sys_regs = devm_ioremap(dev, mem->start, resource_size(mem));
		if (!IS_ERR(ax_wdt->common_sys_regs)) {
			u32 val[3];

			ret = device_property_read_u32_array(dev, "abort-cfg", val, ARRAY_SIZE(val));
			if (!ret) {
				ax_wdt->abort_cfg_offs = val[0];
				ax_wdt->abort_clr_bit = val[1];
				ax_wdt->abort_en_bit = val[2];
			}
		}
	}

	ax_wdt->rst = devm_reset_control_get_optional(&pdev->dev, "arst");

	ax_wdt->pclk = devm_clk_get(dev, "wdt-pclk");
	if (IS_ERR(ax_wdt->pclk)) {
		pr_err("err %ld\n", PTR_ERR(ax_wdt->pclk));
		return PTR_ERR(ax_wdt->pclk);
	}

	ax_wdt->clk = devm_clk_get(dev, "wdt-clk");
	if (IS_ERR(ax_wdt->clk)) {
		pr_err("err %ld\n", PTR_ERR(ax_wdt->clk));
		return PTR_ERR(ax_wdt->clk);
	}

	ret = device_property_read_u32(dev, "clock-frequency", &ax_wdt->rate);
	if (ret) {
		pr_err("failed to get clock frequency, set default 24MHz\n");
		ax_wdt->rate = AX_WDT_CLK_24M;
	}

	if (clk_set_rate(ax_wdt->clk, ax_wdt->rate)) {
		ax_wdt->rate = clk_get_rate(ax_wdt->clk);
		if (ax_wdt->rate == 0) {
			ret = -EINVAL;
			pr_err("err %d\n", ret);
			return ret;
		}
		pr_err("failed to set clock frequency, current clk is %d\n",
		       ax_wdt->rate);
	}

	ret = ax_wdt_clk_enable(ax_wdt, ax_wdt->rate);
	if (ret) {
		pr_err("err %d\n", ret);
		return ret;
	}

	if (unlikely(ax_wdt->rate != AX_WDT_CLK_24M))
		ax_wdt->udelay_time = AX_PING_DELAY_US;

	ret =
	    of_property_read_u32_index(dev->of_node, "keep-alive", 0,
				       &ax_wdt->keep_alive_rate);
	if (ret)
		ax_wdt->keep_alive_rate = 0;

	ret =
	    of_property_read_u32_index(dev->of_node, "keep-alive", 1,
				       &ax_wdt->suspend_timeout);
	if (ret)
		ax_wdt->suspend_timeout = 0;

	wdd = &ax_wdt->wdd;
	wdd->info = &ax_wdt_ident;
	wdd->ops = &ax_wdt_ops;
	wdd->min_timeout = DIV_ROUND_UP(0xFFFF, ax_wdt->rate);
	wdd->max_timeout = AX_MAX_COUNT / ax_wdt->rate;
	wdd->max_hw_heartbeat_ms = AX_MAX_COUNT / ax_wdt->rate * 1000;
	wdd->parent = dev;

	watchdog_set_drvdata(wdd, ax_wdt);
	watchdog_set_nowayout(wdd, nowayout);

	wdd->timeout = AX_WDT_DEFAULT_SECONDS;
	watchdog_init_timeout(wdd, 0, dev);

	platform_set_drvdata(pdev, ax_wdt);

	watchdog_set_restart_priority(wdd, 128);

	ret = watchdog_register_device(wdd);
	if (ret) {
		pr_err("err %d\n", ret);
		goto out_disable_clk;
	}

	for_user = device_property_present(dev, "for-user");
	if (for_user)
		ax_wdt_stop(wdd);

	ax_wdt->irq = platform_get_irq(pdev, 0);
	if (!for_user && ax_wdt->irq < 0) {
		ret = -EINVAL;
		pr_err("err %d\n", ret);
		goto out_unregister_device;
	}

	if (!for_user) {
		u32 flag;

		if (!device_property_read_u32(dev, "cpu_flag", &flag))
			irq_set_affinity(ax_wdt->irq, cpumask_of(flag));

		ret = devm_request_irq(&pdev->dev, ax_wdt->irq, ax_wdt_interrupt,
				       IRQF_SHARED, KBUILD_MODNAME, ax_wdt);
		if (ret) {
			pr_err("err %d\n", ret);
			goto out_unregister_device;
		}

		ax_wdt_irq_enable(ax_wdt);
		ax_wdt_start(wdd);
	}

	pr_info("%pOF done!", pdev->dev.of_node);

	return 0;

out_unregister_device:
	watchdog_unregister_device(wdd);
out_disable_clk:
	ax_wdt_clk_disable(ax_wdt);
	pr_err("Watchdog probe fail!\n");
	return ret;
}

static int ax_wdt_drv_remove(struct platform_device *pdev)
{
	struct ax_wdt *ax_wdt = platform_get_drvdata(pdev);

	watchdog_unregister_device(&ax_wdt->wdd);
	ax_wdt_clk_disable(ax_wdt);
	return 0;
}

static void ax_wdt_drv_shutdown(struct platform_device *pdev)
{
	struct ax_wdt *ax_wdt = platform_get_drvdata(pdev);

	ax_wdt_stop(&ax_wdt->wdd);
}

#ifdef CONFIG_OF
static const struct of_device_id ax_wdt_of_match[] = {
	{.compatible = "axera,ax-wdt",},
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, ax_wdt_of_match);
#endif

static struct platform_driver ax_wdt_driver = {
	.probe = ax_wdt_drv_probe,
	.remove = ax_wdt_drv_remove,
	.shutdown = ax_wdt_drv_shutdown,
	.driver = {
		   .name = "ax_wdt",
		   .of_match_table = of_match_ptr(ax_wdt_of_match),
		   .pm = &ax_wdt_pm_ops,
		   },
};

module_platform_driver(ax_wdt_driver);

MODULE_AUTHOR("axera");
MODULE_DESCRIPTION("Axera Watchdog Driver");
MODULE_LICENSE("GPL");
