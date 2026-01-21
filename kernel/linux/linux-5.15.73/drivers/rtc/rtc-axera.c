/*
 * DesignWare Real Time Clock Driver
 *
 * Copyright (C) 2019 AIChip, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/hrtimer.h>

#include "rtc-axera.h"

#define RTC_START_YEAR              1970

#define RTC_X32_CLK_SEL             1

#define CLK_CONFIG_ADDR             0x3c
#define RTC_CLOCK_EB                BIT(8)

#define RTC_PASSWORD                0x61696370

#define RTC_DEFAULT_CLOCK			4	// 4HZ

struct axera_rtc_dev {
	struct rtc_device *rtc;
	void __iomem *reg_base;
	void __iomem *clk_base;
	int alarm_irq;
	struct hrtimer debounce_timer;
	struct input_dev *input;
	unsigned int key_code;
	int wakeup;
	unsigned int key_type;
	unsigned int intr_reason;
	u32 debounce_interval;

};

static int convert2days(u16 *days, struct rtc_time *tm)
{
	int i;
	*days = 0;

	/* epoch == 1900 */
	if (tm->tm_year < 70 || tm->tm_year > 199)
		return -EINVAL;

	for (i = 1970; i < 1900 + tm->tm_year; i++)
		*days += rtc_year_days(1, 12, i);

	*days += rtc_year_days(tm->tm_mday, tm->tm_mon, 1900 + tm->tm_year);

	return 0;
}

static void convertfromdays(u16 days, struct rtc_time *tm)
{
	int tmp_days, year, mon;

	for (year = 1970;; year++) {
		tmp_days = rtc_year_days(1, 12, year);
		if (days >= tmp_days)
			days -= tmp_days;
		else {
			for (mon = 0;; mon++) {
				tmp_days = rtc_month_days(mon, year);
				if (days >= tmp_days) {
					days -= tmp_days;
				} else {
					tm->tm_year = year - 1900;
					tm->tm_mon = mon;
					tm->tm_mday = days + 1;
					break;
				}
			}
			break;
		}
	}
}

static int axera_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct axera_rtc_dev *axera_rtcdev = dev_get_drvdata(dev);
	u16 days, cfg_valid_intr, wait_times = 0;
	/*
	 * The value written will be updated after 1 sec into the
	 * seconds read register, so we need to program time +1 sec
	 * to get the correct time on read.
	 */

	if (convert2days(&days, tm)) {
		printk("%s year %d invalid\n", __func__, tm->tm_year);
		return -EINVAL;
	}

	/*
	 * Writing into calibration register will clear the Tick Counter and
	 * force the next second to be signaled exactly in 1 second period
	 */
	if ((tm->tm_sec + 2) >= 60) {
		tm->tm_sec -= 60;
		if ((tm->tm_min + 1) >= 60) {
			tm->tm_min -= 60;
			if ((tm->tm_hour + 1) >= 24) {
				tm->tm_hour -= 24;
				days += 1;
			}
		}
	} else {
		tm->tm_sec += 2;
	}

	dev_dbg(dev, "%s: %ds %dm %dh %dmd %dwd %dm %dy\n", __func__, tm->tm_sec, tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_wday, tm->tm_mon, tm->tm_year);

	writel(tm->tm_sec * RTC_DEFAULT_CLOCK, axera_rtcdev->reg_base + RTC_SET_SEC_ADDR);
	writel(tm->tm_min, axera_rtcdev->reg_base + RTC_SET_MIN_ADDR);
	writel(tm->tm_hour, axera_rtcdev->reg_base + RTC_SET_HOUR_ADDR);
	writel(days, axera_rtcdev->reg_base + RTC_SET_DAY_ADDR);
	writel(0xf, (axera_rtcdev->reg_base + RTC_SET_SW_ADDR));	//enable sec min hour day sw cfg

	while (wait_times++ < 50) {
		cfg_valid_intr = readl(axera_rtcdev->reg_base + RTC_CFG_VALID_RSTAT_ADDR);
		if (0xf == (cfg_valid_intr & 0xf)) {
			writel(0xf, axera_rtcdev->reg_base + RTC_CFG_VALID_CLR_INTR_ADDR);
			break;
		}
		msleep_interruptible(10);
	}
	return 0;
}

static int axera_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	u16 days;
	struct axera_rtc_dev *axera_rtcdev = dev_get_drvdata(dev);
	/*
	 * Time written in SET_TIME_WRITE has not yet updated into
	 * the seconds read register, so read the time from the
	 * SET_TIME_WRITE instead of CURRENT_TIME register.
	 * Since we add +1 sec while writing, we need to -1 sec while
	 * reading.
	 */
	tm->tm_sec = readl(axera_rtcdev->reg_base + RTC_SEC_CCVR_ADDR) / RTC_DEFAULT_CLOCK;
	tm->tm_min = readl(axera_rtcdev->reg_base + RTC_MIN_CCVR_ADDR);
	tm->tm_hour = readl(axera_rtcdev->reg_base + RTC_HOUR_CCVR_ADDR);
	days = readl(axera_rtcdev->reg_base + RTC_DAY_CCVR_ADDR);
	convertfromdays(days, tm);
	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	if (days == 0 && tm->tm_hour < 8) {
		tm->tm_hour = 8;
	}

	dev_dbg(dev, "%s: %ds %dm %dh %dmd %dwd %dm %dy\n", __func__, tm->tm_sec, tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_wday, tm->tm_mon, tm->tm_year);

	return rtc_valid_tm(tm);
}

static int axera_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct axera_rtc_dev *axera_rtcdev = dev_get_drvdata(dev);
	struct rtc_time *tm;
	u16 days;
	tm = &alrm->time;

	alrm->enabled = readl(axera_rtcdev->reg_base + RTC_ALARM_INTR_EN_ADDR) & 0x1;
	if (alrm->enabled) {
		tm->tm_sec = readl(axera_rtcdev->reg_base + RTC_SEC_MATCH_ADDR) / RTC_DEFAULT_CLOCK;
		tm->tm_min = readl(axera_rtcdev->reg_base + RTC_MIN_MATCH_ADDR);
		tm->tm_hour = readl(axera_rtcdev->reg_base + RTC_HOUR_MATCH_ADDR);
		days = readl(axera_rtcdev->reg_base + RTC_DAY_MATCH_ADDR);
	} else {
		tm->tm_sec = 0;
		tm->tm_min = 0;
		tm->tm_hour = 0;
		days = 0;
	}

	convertfromdays(days, tm);

	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	if (days == 0 && tm->tm_hour < 8) {
		tm->tm_hour = 8;
	}

	dev_dbg(dev, "%s: %ds %dm %dh %dmd %dwd %dm %dy\n", __func__, tm->tm_sec, tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_wday, tm->tm_mon, tm->tm_year);

	return rtc_valid_tm(tm);
}

static int axera_rtc_set_turn_mode(struct device *dev, unsigned int turn_mode)
{
	struct axera_rtc_dev *axera_rtcdev = dev_get_drvdata(dev);

	if (turn_mode == RTC_PWR_POWPER_ON) {
        	writel(RTC_PWR_POWPER_ON, axera_rtcdev->reg_base + RTC_ALARM_MODE);
        	writel(0x1, axera_rtcdev->reg_base + RTC_PWR_START);
	} else if (turn_mode == RTC_PWR_SHUTDOWN) {
		writel(RTC_PWR_SHUTDOWN, axera_rtcdev->reg_base + RTC_ALARM_MODE);
		writel(0x1, axera_rtcdev->reg_base + RTC_PWR_START);
	} else if (turn_mode == RTC_PWR_SOFT_RESTART) {
		writel(RTC_PWR_SOFT_RESTART, axera_rtcdev->reg_base + RTC_ALARM_MODE);
		writel(0x1, axera_rtcdev->reg_base + RTC_PWR_START);
	} else if (turn_mode == RTC_PWR_HARD_RESTART) {
		writel(RTC_PWR_HARD_RESTART, axera_rtcdev->reg_base + RTC_ALARM_MODE);
		writel(0x1, axera_rtcdev->reg_base + RTC_PWR_START);
	}

	return 0;
}

static int axera_rtc_alarm_irq_enable(struct device *dev, u32 enabled)
{
	struct axera_rtc_dev *axera_rtcdev = dev_get_drvdata(dev);

	if (enabled) {
		writel(0x1, axera_rtcdev->reg_base + RTC_ALARM_INTR_EN_ADDR);	//enable alarm int
		writel(0x0, axera_rtcdev->reg_base + RTC_ALARM_INTR_MASK_ADDR);	//unmask mask int
	} else {
		writel(0x0, axera_rtcdev->reg_base + RTC_ALARM_INTR_EN_ADDR);	//disable alarm int
		writel(0x1, axera_rtcdev->reg_base + RTC_ALARM_INTR_MASK_ADDR);	//mask mask int
	}
	msleep_interruptible(600);
	return 0;
}

static int axera_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct axera_rtc_dev *axera_rtcdev = dev_get_drvdata(dev);
	u16 days;
	struct rtc_time *tm = &alrm->time;

	writel(1, (axera_rtcdev->reg_base + RTC_ALARM_CLR_INTR_ADDR));	// clear alarm int
	writel(0, (axera_rtcdev->reg_base + RTC_ALARM_CLR_INTR_ADDR));	// clear alarm int
	writel(1, (axera_rtcdev->reg_base + RTC_ALARM_INTR_MASK_ADDR));	// mask alarm int

	if (convert2days(&days, tm)) {
		printk("%s year %d invalid\n", __func__, tm->tm_year);
		return -EINVAL;
	}

	writel((tm->tm_sec) * RTC_DEFAULT_CLOCK, axera_rtcdev->reg_base + RTC_SEC_MATCH_ADDR);
	writel(tm->tm_min, axera_rtcdev->reg_base + RTC_MIN_MATCH_ADDR);
	writel(tm->tm_hour, axera_rtcdev->reg_base + RTC_HOUR_MATCH_ADDR);
	writel(days, axera_rtcdev->reg_base + RTC_DAY_MATCH_ADDR);

	// start alram
	writel(1, axera_rtcdev->reg_base + RTC_MATCH_START_ADDR);
	msleep_interruptible(600);
	dev_dbg(dev, "%s: %ds %dm %dh %dmd %dwd %dm %dy\n", __func__, tm->tm_sec, tm->tm_min, tm->tm_hour, tm->tm_mday, tm->tm_wday, tm->tm_mon, tm->tm_year);

	axera_rtc_alarm_irq_enable(dev, alrm->enabled);

	return 0;
}

static const struct rtc_class_ops axera_rtc_ops = {
	.set_time = axera_rtc_set_time,
	.read_time = axera_rtc_read_time,
	.read_alarm = axera_rtc_read_alarm,
	.set_alarm = axera_rtc_set_alarm,
	.set_turn_mode = axera_rtc_set_turn_mode,
	.alarm_irq_enable = axera_rtc_alarm_irq_enable,
};

static void axera_init_rtc(struct axera_rtc_dev *axera_rtcdev)
{
	u32 val;

	val = readl(axera_rtcdev->clk_base + CLK_CONFIG_ADDR);
	writel(val | RTC_CLOCK_EB, (axera_rtcdev->clk_base + CLK_CONFIG_ADDR));

	writel(RTC_PASSWORD, (axera_rtcdev->reg_base + RTC_PASSWORD_ADDR));

	/*
	 * Rtc can be configured to use soc external and external 32k clocks.
	 * rtc registers use external clocks by default, and the configuration
	 * will not be lost after installing the battery.
	 */

	/*
	 * internal clk configured
	 writel(0, (axera_rtcdev->reg_base + RTC_X32K_XIN_EN_ADDR));
	 */
	writel(0, (axera_rtcdev->reg_base + RTC_CFG_VALID_INTR_EN_ADDR));
	writel(1, (axera_rtcdev->reg_base + RTC_PWR_INTR_EN_ADDR));
	/*The machine can be shut down after about 30s each time the machine is started */
	writel(0x78, (axera_rtcdev->reg_base + RTC_PWR_ON_TIME_ADDR));
	/*Press pwr key for about 8s to shut down the machine,also read write 0x294 reg*/
	writel(20, (axera_rtcdev->reg_base + RTC_SW_PWR_MODE_SET_ADDR));
	writel(1, (axera_rtcdev->reg_base + RTC_PWR_START));
}

static enum hrtimer_restart pwr_keys_timer(struct hrtimer *t)
{
	int value;
	struct axera_rtc_dev *axera_rtcdev = container_of(t, struct axera_rtc_dev,
							  debounce_timer);
	struct input_dev *input = axera_rtcdev->input;

	if (axera_rtcdev->intr_reason == 1) {

	} else if (axera_rtcdev->intr_reason == 2) {
		if ((readl(axera_rtcdev->reg_base + RTC_PTEST_PWR_SEQ0_IO_SET_ADDR) & (1 << 2)) == 0x00) {
			value = 0;
			input_event(axera_rtcdev->input, axera_rtcdev->key_type, axera_rtcdev->key_code, !!value);
			input_sync(input);
		}
	}

	enable_irq(axera_rtcdev->alarm_irq);
	return HRTIMER_NORESTART;
}

static irqreturn_t axera_rtc_interrupt(int irq, void *id)
{
	struct axera_rtc_dev *axera_rtcdev = (struct axera_rtc_dev *)id;
	unsigned int status;
	int value;
	/*printk("alarm status = %x   pwr_key_status = %x  time_status = %x\n", readl(axera_rtcdev->reg_base + RTC_ALARM_INTR_ADDR),
	       readl(axera_rtcdev->reg_base + RTC_PWR_INTR_STATUS), readl(axera_rtcdev->reg_base + RTC_CFG_VALID_INTR_ADDR));*/

	axera_rtcdev->intr_reason = 0;
	disable_irq_nosync(irq);
	if (readl(axera_rtcdev->reg_base + RTC_CFG_VALID_INTR_ADDR) != 0x0) {
		status = readl(axera_rtcdev->reg_base + RTC_CFG_VALID_INTR_ADDR);
		writel(status, axera_rtcdev->reg_base + RTC_CFG_VALID_CLR_INTR_ADDR);
	}

	if ((readl(axera_rtcdev->reg_base + RTC_ALARM_INTR_ADDR) & 0x01) == 0x01) {
		status = readl(axera_rtcdev->reg_base + RTC_ALARM_INTR_ADDR);	//read alarm int status
		/* Check if interrupt asserted */
		if (!(status & 0x1)) {
			enable_irq(irq);
			return IRQ_NONE;
		}

		/* Read Clearinterrupt only */
		writel(status, axera_rtcdev->reg_base + RTC_ALARM_CLR_INTR_ADDR);
		axera_rtcdev->intr_reason = 1;
		rtc_update_irq(axera_rtcdev->rtc, 1, RTC_IRQF | RTC_AF);

	} else if ((readl(axera_rtcdev->reg_base + RTC_PWR_INTR_STATUS) & 0x01) == 0x01) {

		value = 1;
		input_event(axera_rtcdev->input, axera_rtcdev->key_type, axera_rtcdev->key_code, !!value);
		input_sync(axera_rtcdev->input);
		axera_rtcdev->intr_reason = 2;
		writel(1, axera_rtcdev->reg_base + RTC_PWR_CLR_INTR_ADDR);
	}

	hrtimer_start(&axera_rtcdev->debounce_timer, ms_to_ktime(axera_rtcdev->debounce_interval), HRTIMER_MODE_REL_HARD);

	return IRQ_HANDLED;
}

static int pwr_key_open(struct input_dev *input)
{

	return 0;
}

static void pwr_key_close(struct input_dev *input)
{

}

static ssize_t pwr_key_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t len)
{
	return len;
}

static ssize_t pwr_key_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int ret = 1;
	return ret;
}

static DEVICE_ATTR(pwr_key, 0664, pwr_key_show, pwr_key_store);

static int axera_rtc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct axera_rtc_dev *axera_rtcdev;
	struct resource *res;
	int ret;

	axera_rtcdev = devm_kzalloc(&pdev->dev, sizeof(*axera_rtcdev), GFP_KERNEL);
	if (!axera_rtcdev) {
		printk("axera_rtcdev kzalloc failed\n");
		return -ENOMEM;
	}

	pdev->dev.driver_data = axera_rtcdev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(&pdev->dev, "axera rtc no memory resource defined IORESOURCE_MEM 0\n");
		return -ENODEV;
	}

	axera_rtcdev->reg_base = (void *)ioremap(res->start, resource_size(res));
	if (IS_ERR(axera_rtcdev->reg_base)) {
		printk("ioremap reg_base failed\n");
		return PTR_ERR(axera_rtcdev->reg_base);
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (res == NULL) {
		iounmap(axera_rtcdev->reg_base);
		dev_err(&pdev->dev, "axera rtc no memory resource defined IORESOURCE_MEM 1\n");
		return -ENODEV;
	}

	axera_rtcdev->clk_base = (void *)ioremap(res->start, resource_size(res));
	if (IS_ERR(axera_rtcdev->clk_base)) {
		printk("ioremap clk_base failed\n");
		iounmap(axera_rtcdev->reg_base);
		return PTR_ERR(axera_rtcdev->clk_base);
	}

	axera_rtcdev->alarm_irq = platform_get_irq(pdev, 0);
	if (axera_rtcdev->alarm_irq < 0) {
		printk("no irq resource\n");
		iounmap(axera_rtcdev->clk_base);
		iounmap(axera_rtcdev->reg_base);
		return axera_rtcdev->alarm_irq;
	}

	hrtimer_init(&axera_rtcdev->debounce_timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	axera_rtcdev->debounce_timer.function = pwr_keys_timer;

	axera_init_rtc(axera_rtcdev);

	axera_rtcdev->rtc = devm_rtc_device_register(&pdev->dev, pdev->name, &axera_rtc_ops, THIS_MODULE);
	if (IS_ERR(axera_rtcdev->rtc)) {
		iounmap(axera_rtcdev->clk_base);
		iounmap(axera_rtcdev->reg_base);
		return PTR_ERR(axera_rtcdev->rtc);
	}
	if (device_create_file(&pdev->dev, &dev_attr_pwr_key) < 0) {
		iounmap(axera_rtcdev->clk_base);
		iounmap(axera_rtcdev->reg_base);
		return -1;
	}

	if (of_property_read_u32(dev->of_node, "linux,input-type", &axera_rtcdev->key_type))
		axera_rtcdev->key_type = EV_KEY;

	if (of_property_read_u32(dev->of_node, "linux,code", &axera_rtcdev->key_code))
		axera_rtcdev->key_code = KEY_POWER;

	if (of_property_read_u32(dev->of_node, "debounce-interval", &axera_rtcdev->debounce_interval))
		axera_rtcdev->debounce_interval = 1000;

	axera_rtcdev->wakeup = of_property_read_bool(dev->of_node, "wakeup-source");

	axera_rtcdev->input = devm_input_allocate_device(dev);
	if (!axera_rtcdev->input) {
		iounmap(axera_rtcdev->clk_base);
		iounmap(axera_rtcdev->reg_base);
		dev_err(dev, "failed to allocate input device\n");
		return -ENOMEM;
	}

	axera_rtcdev->input->name = "pwr_key";
	axera_rtcdev->input->phys = "pwr-key/input0";
	axera_rtcdev->input->dev.parent = dev;
	axera_rtcdev->input->open = pwr_key_open;
	axera_rtcdev->input->close = pwr_key_close;

	axera_rtcdev->input->id.bustype = BUS_HOST;
	axera_rtcdev->input->id.vendor = 0x0001;
	axera_rtcdev->input->id.product = 0x0001;
	axera_rtcdev->input->id.version = 0x0100;

	set_bit(axera_rtcdev->key_type, axera_rtcdev->input->evbit);
	set_bit(axera_rtcdev->key_code, axera_rtcdev->input->keybit);
	ret = input_register_device(axera_rtcdev->input);
	if (ret) {
		dev_err(dev, "Unable to register input device, error: %d\n", ret);
		iounmap(axera_rtcdev->clk_base);
		iounmap(axera_rtcdev->reg_base);
		return ret;
	}

	ret = request_threaded_irq(axera_rtcdev->alarm_irq, axera_rtc_interrupt, NULL, 0, dev_name(&pdev->dev), axera_rtcdev);
	if (ret) {
		printk("request irq failed\n");
		input_free_device(axera_rtcdev->input);
		iounmap(axera_rtcdev->clk_base);
		iounmap(axera_rtcdev->reg_base);
		return ret;
	}

	device_init_wakeup(&pdev->dev, axera_rtcdev->wakeup);

	return PTR_ERR_OR_ZERO(axera_rtcdev->rtc);
}

static int axera_rtc_remove(struct platform_device *pdev)
{
	struct axera_rtc_dev *axera_rtcdev = pdev->dev.driver_data;

	axera_rtc_alarm_irq_enable(&pdev->dev, 0);
	device_init_wakeup(&pdev->dev, 0);
	device_remove_file(&pdev->dev, &dev_attr_pwr_key);
	input_free_device(axera_rtcdev->input);
	free_irq(axera_rtcdev->alarm_irq, axera_rtcdev);
	iounmap(axera_rtcdev->clk_base);
	iounmap(axera_rtcdev->reg_base);
	kfree(axera_rtcdev);

	return 0;
}

static int __maybe_unused axera_rtc_suspend(struct device *dev)
{
	struct platform_device *pdev = container_of((dev), struct platform_device, dev);
	struct axera_rtc_dev *axera_rtcdev = pdev->dev.driver_data;

	if (device_may_wakeup(&pdev->dev))
		enable_irq_wake(axera_rtcdev->alarm_irq);
	else
		axera_rtc_alarm_irq_enable(dev, 0);

	return 0;
}

static int __maybe_unused axera_rtc_resume(struct device *dev)
{
	struct platform_device *pdev = container_of((dev), struct platform_device, dev);
	struct axera_rtc_dev *axera_rtcdev = pdev->dev.driver_data;

	if (device_may_wakeup(&pdev->dev))
		disable_irq_wake(axera_rtcdev->alarm_irq);
	else
		axera_rtc_alarm_irq_enable(dev, 1);

	return 0;
}

static SIMPLE_DEV_PM_OPS(axera_rtc_pm_ops, axera_rtc_suspend, axera_rtc_resume);

static const struct of_device_id axera_rtc_of_match[] = {
	{.compatible = "axera,axi-top-rtc"},
	{}
};

static struct platform_driver axera_rtc_driver = {
	.probe = axera_rtc_probe,
	.remove = axera_rtc_remove,
	.driver = {
		   .name = KBUILD_MODNAME,
		   .pm = &axera_rtc_pm_ops,
		   .of_match_table = axera_rtc_of_match,
		   },
};

static int __init axera_rtc_init(void)
{
	return platform_driver_register(&axera_rtc_driver);
}

static void axera_rtc_exit(void)
{
	platform_driver_unregister(&axera_rtc_driver);
}

module_init(axera_rtc_init);
module_exit(axera_rtc_exit);

MODULE_DESCRIPTION("Axera RTC driver");
MODULE_AUTHOR("Axera Inc.");
MODULE_LICENSE("GPL v2");
