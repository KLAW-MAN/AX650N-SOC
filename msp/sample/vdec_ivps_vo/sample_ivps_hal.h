/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _SAMPLE_IVPS_HAL_H_
#define _SAMPLE_IVPS_HAL_H_

#include <signal.h>
#include "ax_sys_api.h"
#include "ax_ivps_api.h"


int SampleIVPS_Init(AX_BOOL bEnaIvpsBakFrm, AX_U32 u32StartGrpId, AX_U32 u32GrpNum, AX_U32 u32WinW, AX_U32 u32WinH);
AX_S32 SampleIvpsExit();
AX_S32 SampleIvpsReset();

#endif /* _SAMPLE_IVPS_HAL_H_ */
