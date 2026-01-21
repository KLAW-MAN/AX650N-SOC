/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include "msensor.h"

ax_msensor_callback_t a_gyro_callback[AX_MSENSOR_MAX];
static ax_msensor_type_e use_msensor;

AX_S32 ax_msensor_call_read_data(ax_msensor_fifo_data_t *pdata)
{
	if (!a_gyro_callback[use_msensor].ax_msensor_read_data)
		printk("use_msensor: %d is not register\n", use_msensor);
	return a_gyro_callback[use_msensor].ax_msensor_read_data(pdata);
}

AX_VIN_PYRO_CALLBACK_T read_data_callback = {
	.pfn_vin_pyro_read_data = ax_msensor_call_read_data,
};

AX_S32 ax_msensor_register_callback(ax_msensor_type_e msensor_type, ax_msensor_callback_t *msensor_callback)
{
	printk("enter ax_msensor_register_callback\n");
	use_msensor = msensor_type;
	a_gyro_callback[msensor_type].ax_msensor_read_data = msensor_callback->ax_msensor_read_data;
	CB_VIN_PRYO_RegisterCallback(&read_data_callback);
	return 0;
}
EXPORT_SYMBOL(ax_msensor_register_callback);


