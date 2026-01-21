/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SAMPLE_CREATECHNEX_H_
#define __SAMPLE_CREATECHNEX_H_

#include "ax_sys_api.h"
#include "ax_venc_comm.h"
#include "common_venc.h"
#include "sample_cmd_params.h"
#include "sample_global.h"
#include "sample_unit_test.h"
#include "sample_venc_log.h"

AX_S32 SAMPLE_VENC_CreateChnEx_Start(VENC_CHN *pVeChn, AX_PAYLOAD_TYPE_E enType, SAMPLE_VENC_RC_E rcMode,
                                     SAMPLE_VENC_CMD_PARA_T *pstArg);

AX_S32 SAMPLE_VENC_CreateChnEx(VENC_CHN *pVeChn, AX_PAYLOAD_TYPE_E enType, SAMPLE_VENC_RC_E rcMode,
                               SAMPLE_VENC_CMD_PARA_T *pstArg);

#endif