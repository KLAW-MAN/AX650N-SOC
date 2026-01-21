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


AX_BOOL SAMPLE_UXE_GPIO_Init(HDMI_IN_TYPE_E enHdmiInFormat, AX_U32 nWidth, AX_U32 nHeight, AX_U8 nFps, AX_BOOL bShowFb, AX_U8 nLogLevel);
AX_BOOL SAMPLE_UXE_GPIO_Start();
AX_VOID SAMPLE_UXE_GPIO_WaitForDetectNotify(AX_BOOL bIsRunning);
AX_VOID SAMPLE_UXE_GPIO_Wakeup(AX_CHAR* reason);
AX_BOOL SAMPLE_UXE_GPIO_Stop();
AX_BOOL SAMPLE_UXE_GPIO_DeInit();
