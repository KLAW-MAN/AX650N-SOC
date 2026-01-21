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

AX_BOOL SAMPLE_BUILD_Init(HDMI_IN_TYPE_E enHdmiInFormat, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps, AX_U16 nMHz, AX_BOOL bShowFb, AX_U8 nLogLevel);
AX_BOOL SAMPLE_BUILD_Start();
AX_BOOL SAMPLE_BUILD_ReStart(HDMI_IN_TYPE_E enHdmiInFormat, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps, AX_U16 nMHz, AX_BOOL bShowFb, AX_U8 nLogLevel);
AX_BOOL SAMPLE_BUILD_Stop();
AX_BOOL SAMPLE_BUILD_DeInit();
