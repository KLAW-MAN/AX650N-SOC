// AXERA-License-Identifier: GPL-2.0
/*
 * AXERA Hwspinlock driver
 *
 * Copyright (c) 2022, AXERA Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/hwspinlock.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#include "hwspinlock_internal.h"

/* hwspinlock registers definition */
#define SPINLOCK_MASTER_ID_SHIFT        (8)
#define SPINLOCK_COMMON_REG_NUM_SHIFT   (3)
#define SPINLOCK_MASTER_ID_UNLOCK_SHIFT (16)
#define CPU_MASTER_ID                   (0)
#define AXERA_HWLOCKS_NUM               (32)

/* hwspinlock operate */
#define HWSPINLOCK_LOCK_ADDR(_X_)	((CPU_MASTER_ID << SPINLOCK_MASTER_ID_SHIFT) | (_X_ << SPINLOCK_COMMON_REG_NUM_SHIFT))
#define HWSPINLOCK_UNLOCK_ADDR(_X_)	((CPU_MASTER_ID << SPINLOCK_MASTER_ID_SHIFT) | (_X_ << SPINLOCK_COMMON_REG_NUM_SHIFT) | 0x4)
#define HWSPINLOCK_UNLOCK_CMD		((((CPU_MASTER_ID << 12) | (CPU_MASTER_ID << 8) | (CPU_MASTER_ID << 4) | CPU_MASTER_ID) << SPINLOCK_MASTER_ID_UNLOCK_SHIFT) | 0xFFFF)

struct axera_hwspinlock_dev {
	void __iomem *base;
	struct clk *clk;
	struct hwspinlock_device bank;
	struct reset_control *rst;
};

/* try to lock the hardware spinlock */
static int axera_hwspinlock_trylock(struct hwspinlock *lock)
{
	struct axera_hwspinlock_dev *axera_hwlock =
		dev_get_drvdata(lock->bank->dev);
	int user_id, lock_id;

	lock_id = hwlock_to_id(lock);
	/* get the hardware spinlock master/user id */
	user_id = readl((axera_hwlock->base) + HWSPINLOCK_LOCK_ADDR(lock_id));
	if (!user_id) {
		return 1;
	}
	return 0;
}

/* unlock the hardware spinlock */
static void axera_hwspinlock_unlock(struct hwspinlock *lock)
{
	struct axera_hwspinlock_dev *axera_hwlock =
		dev_get_drvdata(lock->bank->dev);
	int lock_id;

	lock_id = hwlock_to_id(lock);
	writel(HWSPINLOCK_UNLOCK_CMD, (axera_hwlock->base) + HWSPINLOCK_UNLOCK_ADDR(lock_id));
}

/* The specs recommended below number as the retry delay time */
static void axera_hwspinlock_relax(struct hwspinlock *lock)
{
	ndelay(10);
}

static inline void ax_hwspinlock_reset(struct axera_hwspinlock_dev *axera_hwlock)
{
	reset_control_assert(axera_hwlock->rst);
	reset_control_deassert(axera_hwlock->rst);
}

static const struct hwspinlock_ops axera_hwspinlock_ops = {
	.trylock = axera_hwspinlock_trylock,
	.unlock = axera_hwspinlock_unlock,
	.relax = axera_hwspinlock_relax,
};

static void axera_hwspinlock_disable(void *data)
{
	struct axera_hwspinlock_dev *axera_hwlock = data;
	clk_disable_unprepare(axera_hwlock->clk);
	reset_control_assert(axera_hwlock->rst);
}

static int axera_hwspinlock_probe(struct platform_device *pdev)
{
	struct axera_hwspinlock_dev *axera_hwlock;
	struct hwspinlock *lock;
	int i, ret;

	if (!pdev->dev.of_node)
		return -ENODEV;

	axera_hwlock = devm_kzalloc(&pdev->dev,
				   sizeof(struct axera_hwspinlock_dev) +
				   AXERA_HWLOCKS_NUM * sizeof(*lock),
				   GFP_KERNEL);
	if (!axera_hwlock)
		return -ENOMEM;

	axera_hwlock->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(axera_hwlock->base))
		return PTR_ERR(axera_hwlock->base);

	axera_hwlock->clk = devm_clk_get(&pdev->dev, "spinlock_pclk");
	if (IS_ERR(axera_hwlock->clk)) {
		dev_err(&pdev->dev, "get hwspinlock clock failed!\n");
		return PTR_ERR(axera_hwlock->clk);
	}

	ret = clk_prepare_enable(axera_hwlock->clk);
	if (ret) {
		dev_err(&pdev->dev, "hwspinlock enable clock failed!\n");
		return ret;
	}

	axera_hwlock->rst = devm_reset_control_get_optional(&pdev->dev, "spinlock-sw-rst");
	if (IS_ERR(axera_hwlock->rst)) {
		dev_err(&pdev->dev, "get hwspinlock reset fail!\n");
		return PTR_ERR(axera_hwlock->rst);
	}

	ax_hwspinlock_reset(axera_hwlock);

	ret = devm_add_action_or_reset(&pdev->dev, axera_hwspinlock_disable, axera_hwlock);
	if (ret) {
		dev_err(&pdev->dev, "Failed to add hwspinlock disable action\n");
		return ret;
	}

	for (i = 0; i < AXERA_HWLOCKS_NUM; i++) {
		lock = &axera_hwlock->bank.lock[i];
	}

	platform_set_drvdata(pdev, axera_hwlock);

	dev_info(&pdev->dev, "hwspinlock done \n");
	return devm_hwspin_lock_register(&pdev->dev, &axera_hwlock->bank,
					 &axera_hwspinlock_ops, 0,
					 AXERA_HWLOCKS_NUM);
}

static const struct of_device_id axera_hwspinlock_of_match[] = {
	{ .compatible = "axera,hwspinlock-r1p0", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, axera_hwspinlock_of_match);

static struct platform_driver axera_hwspinlock_driver = {
	.probe = axera_hwspinlock_probe,
	.driver = {
		.name = "ax_hwspinlock",
		.of_match_table = axera_hwspinlock_of_match,
	},
};
static int __init ax_hwspinlock_init(void)
{
	platform_driver_register(&axera_hwspinlock_driver);
	return 0;
}
static void ax_hwspinlock_exit(void)
{
	platform_driver_unregister(&axera_hwspinlock_driver);
}
arch_initcall(ax_hwspinlock_init);
module_exit(ax_hwspinlock_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Hardware spinlock driver for Axera");