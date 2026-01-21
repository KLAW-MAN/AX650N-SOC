/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/rtc.h>
#include <linux/clk.h>
#include <linux/ax_hrtimer.h>
#include "linux/sched/clock.h"

#include "osal_ax.h"
#include "osal_logdebug_ax.h"

static void osal_timer_func_entry(struct timer_list *t)
{
	struct AX_TIMER *axTimer = from_timer(axTimer, t, timer);
	if (axTimer == NULL) {
		printk("%s error axTimer == NULL\n", __func__);
		return;
	}

	if (axTimer->function)
		axTimer->function((AX_VOID *) axTimer->data);
	return;
}

AX_TIMER_T *AX_OSAL_TMR_alloc_timers(void (*function)(void *), AX_LONG data)
{
	AX_TIMER_T *axTimer;
	axTimer = kzalloc(sizeof(AX_TIMER_T), GFP_KERNEL);
	if(axTimer == NULL) {
		printk("%s alloc failed\n", __func__);
		return NULL;
	}

	axTimer->function = function;
	axTimer->data = data;
	return axTimer;
}

EXPORT_SYMBOL(AX_OSAL_TMR_alloc_timers);


AX_S32 AX_OSAL_TMR_init_timers(AX_TIMER_T * axTimer)
{
	if (axTimer == NULL) {
		printk("%s error axTimer == NULL\n", __func__);
		return -1;
	}

	timer_setup(&axTimer->timer, osal_timer_func_entry, 0);
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_TMR_init_timers);

AX_U32 AX_OSAL_TMR_mod_timer(AX_TIMER_T * axTimer, AX_ULONG interval)
{
	if (axTimer == NULL || axTimer->function == NULL || interval == 0) {
		printk("%s parameter error!!!\n", __func__);
		return -1;
	}

	return mod_timer(&axTimer->timer, jiffies + msecs_to_jiffies(interval) - 1);
}

EXPORT_SYMBOL(AX_OSAL_TMR_mod_timer);

AX_U32 AX_OSAL_TMR_del_timer(AX_TIMER_T * axTimer)
{
	if (axTimer == NULL || axTimer->function == NULL) {
		printk("%s parameter error!!!\n", __func__);
		return -1;
	}
	return del_timer(&axTimer->timer);
}

EXPORT_SYMBOL(AX_OSAL_TMR_del_timer);

AX_S32 AX_OSAL_TMR_destory_timer(AX_TIMER_T * axTimer)
{
	del_timer(&axTimer->timer);
	kfree(axTimer);
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_TMR_destory_timer);

AX_ULONG AX_OSAL_TM_msleep(AX_U32 msecs)
{
	msleep(msecs);
	return msecs;
}

EXPORT_SYMBOL(AX_OSAL_TM_msleep);

AX_VOID AX_OSAL_TM_udelay(AX_U32 usecs)
{
	udelay(usecs);
}

EXPORT_SYMBOL(AX_OSAL_TM_udelay);

AX_VOID AX_OSAL_TM_mdelay(AX_U32 msecs)
{
	mdelay(msecs);
}

EXPORT_SYMBOL(AX_OSAL_TM_mdelay);

AX_U32 AX_OSAL_TM_jiffies_to_msecs(AX_VOID)
{
	return jiffies_to_msecs(jiffies);
}

EXPORT_SYMBOL(AX_OSAL_TM_jiffies_to_msecs);

AX_U64 AX_OSAL_TM_sched_clock(AX_VOID)
{
	return sched_clock();
}

EXPORT_SYMBOL(AX_OSAL_TM_sched_clock);

AX_U64 AX_OSAL_TM_get_microseconds(AX_VOID)
{
	AX_U64 current_microsecond = 0;
	AX_U64 tmp_remainder = 0;

	current_microsecond = sched_clock();

	tmp_remainder = do_div(current_microsecond, 1000);

	return current_microsecond;
}

EXPORT_SYMBOL(AX_OSAL_TM_get_microseconds);

AX_VOID AX_OSAL_TM_do_gettimeofday(AX_TIMERVAL_T * tm)
{
	struct timespec64 now;
	if (tm == NULL) {
		printk("%s - parameter invalid!\n", __func__);
		return;
	}

	ktime_get_real_ts64(&now);
	tm->tv_sec = now.tv_sec;
	tm->tv_usec = now.tv_nsec / 1000;
}

EXPORT_SYMBOL(AX_OSAL_TM_do_gettimeofday);

AX_LONG __attribute__ ((__noinline__)) AX_OSAL_TM_msecs_to_jiffies(AX_LONG ax_msecs)
{
	long tmp_msecs = msecs_to_jiffies(ax_msecs);
	return tmp_msecs;
}

EXPORT_SYMBOL(AX_OSAL_TM_msecs_to_jiffies);

AX_VOID AX_OSAL_TM_get_jiffies(AX_U64 * pjiffies)
{
	*pjiffies = jiffies;
}

EXPORT_SYMBOL(AX_OSAL_TM_get_jiffies);


AX_VOID AX_OSAL_TM_ktime_get_real_ts64(AX_TIMERSPEC64_T * tm)
{
	struct timespec64 t;
	if (tm == NULL) {
		printk("%s ktime error\n", __func__);
		return;
	}
	ktime_get_real_ts64(&t);

	tm->tv_sec = t.tv_sec;
	tm->tv_nsec = t.tv_nsec;
}

EXPORT_SYMBOL(AX_OSAL_TM_ktime_get_real_ts64);

AX_LONG AX_OSAL_DEV_usleep(unsigned long use)
{
	return ax_usleep(use);
}

EXPORT_SYMBOL(AX_OSAL_DEV_usleep);


AX_VOID *AX_OSAL_DEV_hrtimer_alloc(unsigned long use,int (*function)(void *),void *private)
{
	struct ax_hrtimer *timer = ax_hrtimer_alloc();
	timer->delay = use;
	timer->function = function;
	timer->private = private;

	return (void *)timer;

}

EXPORT_SYMBOL(AX_OSAL_DEV_hrtimer_alloc);

AX_VOID AX_OSAL_DEV_hrtimer_destroy(void *timer)
{
	ax_hrtimer_destroy((struct ax_hrtimer *)timer);
}

EXPORT_SYMBOL(AX_OSAL_DEV_hrtimer_destroy);

AX_S32 AX_OSAL_DEV_hrtimer_start(void *timer)
{
	return ax_hrtimer_start((struct ax_hrtimer *)timer);
}

EXPORT_SYMBOL(AX_OSAL_DEV_hrtimer_start);


AX_S32 AX_OSAL_DEV_hrtimer_stop(void *timer)
{
	return ax_hrtimer_stop((struct ax_hrtimer *)timer);
}

EXPORT_SYMBOL(AX_OSAL_DEV_hrtimer_stop);

