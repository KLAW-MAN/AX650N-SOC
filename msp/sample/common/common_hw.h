/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __COMMON_HW_H__
#define __COMMON_HW_H__

#ifndef COMM_HW_PRT
#define COMM_HW_PRT(fmt...)   \
do {\
    printf("[COMM_FW][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(fmt);\
}while(0)
#endif


AX_S32 COMMON_VIN_GetSensorResetGpioNum(AX_U8 nDevId);
AX_S8 COMMON_ISP_GetI2cDevNode(AX_U8 nDevId);
AX_S8 COMMON_ISP_GetSpiDevNode(AX_U8 nDevId);

#endif //__COMMON_HW_H__