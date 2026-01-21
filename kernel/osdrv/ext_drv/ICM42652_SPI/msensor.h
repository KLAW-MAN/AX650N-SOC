/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef MSENSOR_H_
#define MSENSOR_H_
#include <ax_vin_kernel_ext.h>

typedef enum {
    AX_MSENSOR_ICM42652,
    AX_MSENSOR_MAX,
} ax_msensor_type_e;

typedef struct _ax_msensor_callback_t_ {
    // mpu data read
    AX_S32 (*ax_msensor_read_data)(ax_msensor_fifo_data_t *pdata);
} ax_msensor_callback_t;

/* mpu api */
AX_S32 ax_msensor_register_callback(ax_msensor_type_e msensor_type, ax_msensor_callback_t *msensor_callback);
#endif
