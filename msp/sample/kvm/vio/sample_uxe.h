/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include "ax_base_type.h"
#include "ax_global_type.h"
#include "ax_vin_api.h"
#include "ax_sensor_struct.h"

#define MAX_MIPI_RX_DEV_ID_BUTT (8)

typedef struct axUXE_S
{
    AX_U8 nDevId;
    AX_IMG_FORMAT_E enVinFmt;
    AX_FRAME_INTERRUPT_ATTR_T tDevFrmIntAttr;
    AX_U32 nW;
    AX_U32 nH;
    AX_U32 nFps;
    AX_U16 nMHz; // mipi datarate
    AX_U32 nMipiDev;//{MAX_MIPI_RX_DEV_ID_BUTT};
}AX_UXE_T;


typedef struct axSNS_CLK_ATTR_T
{
    AX_U8 nSnsClkIdx;
    AX_SNS_CLK_RATE_E eSnsClkRate;
} AX_SNS_CLK_ATTR_T;

AX_BOOL SAMPLE_UXE_Init(AX_IMG_FORMAT_E enVinFmt, AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_U16 nMHz);
AX_U8 SAMPLE_UXE_GetDevID();
AX_BOOL SAMPLE_UXE_Start();
AX_BOOL SAMPLE_UXE_Stop();
AX_BOOL SAMPLE_UXE_DeInit();
