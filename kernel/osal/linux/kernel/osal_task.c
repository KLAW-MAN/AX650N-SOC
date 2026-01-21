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
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include "osal_ax.h"

AX_TASK_T *AX_OSAL_TASK_kthread_run(AX_THREAD_FUNC_T thread, AX_VOID * data, AX_S8 * name)
{
	struct task_struct *k;
	AX_TASK_T *p = (AX_TASK_T *) kmalloc(sizeof(AX_TASK_T), GFP_KERNEL);
	if (p == NULL) {
		printk("%s error p == NULL\n", __func__);
		return NULL;
	}

	k = kthread_run(thread, data, name);
	if (IS_ERR(k)) {
		printk("%s kthread run error\n", __func__);
		kfree(p);
		return NULL;
	}
	p->task_struct = k;
	return p;
}

EXPORT_SYMBOL(AX_OSAL_TASK_kthread_run);

AX_TASK_T *AX_OSAL_TASK_kthread_create_ex(AX_THREAD_FUNC_T thread, AX_VOID * data, AX_S8 * name, AX_S32 prioirty)
{
	struct task_struct *k;
	AX_TASK_T *p = (AX_TASK_T *) kmalloc(sizeof(AX_TASK_T), GFP_KERNEL);
	if (p == NULL) {
		printk("%s error p == NULL\n", __func__);
		return NULL;
	}

	k = kthread_run(thread, data, name);
	if (IS_ERR(k)) {
		printk("%s - kthread create error!\n", __func__);
		kfree(p);
		return NULL;
	}
	p->task_struct = k;
	return p;

}

EXPORT_SYMBOL(AX_OSAL_TASK_kthread_create_ex);

AX_S32 AX_OSAL_TASK_kthread_stop(AX_TASK_T * task, AX_U32 stop_flag)
{
	if (task == NULL) {
		printk("%s error task == NULL\n", __func__);
		return -1;
	}

	if (0 != stop_flag) {
		kthread_stop((struct task_struct *)(task->task_struct));
	}
	task->task_struct = NULL;
	kfree(task);

	return 0;
}

EXPORT_SYMBOL(AX_OSAL_TASK_kthread_stop);

AX_S32 AX_OSAL_TASK_cond_resched(AX_VOID)
{
	cond_resched();
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_TASK_cond_resched);

AX_BOOL AX_OSAL_TASK_kthread_should_stop(AX_VOID)
{
	return kthread_should_stop();
}

EXPORT_SYMBOL(AX_OSAL_TASK_kthread_should_stop);

AX_VOID AX_OSAL_TASK_schedule(void)
{
	schedule();
	return;
}

EXPORT_SYMBOL(AX_OSAL_TASK_schedule);

AX_VOID AX_OSAL_set_current_state(AX_S32 state_value)
{
	set_current_state(state_value);
	return;
}

EXPORT_SYMBOL(AX_OSAL_set_current_state);

AX_S32 AX_OSAL_sched_setscheduler(AX_TASK_T * task, AX_S32 policy, const struct AX_TASK_SCHED_PARAM * param)
{
	struct task_struct *p = (struct task_struct *)task->task_struct;
	return sched_setscheduler(p, policy, (struct sched_param *)param);
}

EXPORT_SYMBOL(AX_OSAL_sched_setscheduler);

AX_VOID AX_OSAL_get_current(struct AX_CURRENT_TASK *ax_task)
{
	ax_task->pid = current->pid;
	ax_task->tgid = current->tgid;
	ax_task->mm = (void *)current->mm;
	memcpy(ax_task->comm,current->comm,16);
}

EXPORT_SYMBOL(AX_OSAL_get_current);

