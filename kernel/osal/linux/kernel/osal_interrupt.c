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
#include <linux/interrupt.h>

#include "osal_ax.h"
#include "osal_dev_ax.h"

AX_S32 AX_OSAL_DEV_request_threaded_irq_ex(AX_U32 irq, AX_IRQ_HANDLER_T handler, AX_IRQ_HANDLER_T thread_fn,
					   AX_ULONG flags, const AX_S8 * name, AX_VOID * dev)
{
	return request_threaded_irq(irq, (irq_handler_t) handler, (irq_handler_t) thread_fn, flags, name, dev);
}

EXPORT_SYMBOL(AX_OSAL_DEV_request_threaded_irq_ex);

AX_S32 AX_OSAL_DEV_request_threaded_irq(AX_U32 irq, AX_IRQ_HANDLER_T handler, AX_IRQ_HANDLER_T thread_fn,
					const AX_S8 * name, AX_VOID * dev)
{
	unsigned long flags = IRQF_SHARED;

	return request_threaded_irq(irq, (irq_handler_t) handler, (irq_handler_t) thread_fn, flags, name, dev);
}

EXPORT_SYMBOL(AX_OSAL_DEV_request_threaded_irq);

const AX_VOID *AX_OSAL_DEV_free_irq(AX_U32 irq, AX_VOID * dev)
{
	const char *devname = free_irq(irq, dev);

	return devname;
}

EXPORT_SYMBOL(AX_OSAL_DEV_free_irq);

AX_S32 AX_OSAL_DEV_in_interrupt(AX_VOID)
{
	return in_interrupt();
}

EXPORT_SYMBOL(AX_OSAL_DEV_in_interrupt);

AX_VOID AX_OSAL_DEV_enable_irq(AX_U32 irq)
{
	enable_irq(irq);
}

EXPORT_SYMBOL(AX_OSAL_DEV_enable_irq);

AX_VOID AX_OSAL_DEV_disable_irq(AX_U32 irq)
{
	disable_irq(irq);
}

EXPORT_SYMBOL(AX_OSAL_DEV_disable_irq);

AX_VOID AX_OSAL_DEV_disable_irq_nosync(AX_U32 irq)
{
	disable_irq_nosync(irq);
}

EXPORT_SYMBOL(AX_OSAL_DEV_disable_irq_nosync);

AX_S32 AX_OSAL_DEV_irq_get_irqchip_state(AX_U32 irq, enum AX_OSAL_irqchip_irq_state which, AX_S32 * state)
{
	return irq_get_irqchip_state(irq, which, (bool *) state);
}

EXPORT_SYMBOL(AX_OSAL_DEV_irq_get_irqchip_state);

AX_S32 AX_OSAL_DEV_irq_set_irqchip_state(AX_U32 irq, enum AX_OSAL_irqchip_irq_state which, AX_S32 val)
{
	return irq_set_irqchip_state(irq, which, val);
}

EXPORT_SYMBOL(AX_OSAL_DEV_irq_set_irqchip_state);
