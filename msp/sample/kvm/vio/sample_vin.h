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
#include "sample_common.h"

AX_BOOL SAMPLE_VIN_Init(HDMI_IN_TYPE_E enHdmi, AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_U16 nMHz);
AX_BOOL SAMPLE_VIN_InitEx(HDMI_IN_TYPE_E enHdmi, AX_U32 nW, AX_U32 nH, AX_U8 nFps, AX_U16 nMHz);
AX_BOOL SAMPLE_VIN_Start();
AX_BOOL SAMPLE_VIN_Stop();
AX_BOOL SAMPLE_VIN_DeInit();