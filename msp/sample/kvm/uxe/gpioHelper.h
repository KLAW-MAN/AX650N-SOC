/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __GPIO_HELPER_H__
#define __GPIO_HELPER_H__

#include "ax_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *GPIO_HANDLE;
#define INVALID_GPIO_HANDLE ((GPIO_HANDLE)0)

typedef enum { GPIO_IN = 0, GPIO_OUT = 1 } GPIO_DIRECTION_E;
typedef enum { GPIO_EDGE_NONE = 0, GPIO_EDGE_RISING = 1, GPIO_EDGE_FALLING = 2, GPIO_EDGE_BOTH = 3 } GPIO_EDGE_E;
typedef void (*GpioPollingCallback)(AX_U32 gpio, AX_U8 value, AX_VOID *userdata);

GPIO_HANDLE CreateGpioHelper(AX_U32 gpio, GPIO_DIRECTION_E direction, GPIO_EDGE_E edge);
AX_S32 SetGpioValue(GPIO_HANDLE handle, AX_U8 value);
AX_S32 GetGpioValue(GPIO_HANDLE handle, AX_U8 *value);
AX_S32 StartGpioPolling(GPIO_HANDLE handle, GpioPollingCallback cb, AX_VOID *userdata);
AX_S32 StopGpioPolling(GPIO_HANDLE handle);
AX_S32 DestoryGpioHelper(GPIO_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif /* #__GPIO_HELPER_H__ */
