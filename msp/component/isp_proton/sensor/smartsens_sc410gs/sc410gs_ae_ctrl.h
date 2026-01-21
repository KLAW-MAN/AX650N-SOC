/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC410GS_AE_CTRL_H__
#define __SC410GS_AE_CTRL_H__

#include "ax_base_type.h"
#include "ax_sensor_struct.h"

#define SC410GS_MAX_VTS         (0xFFFF)
#define SC410GS_MAX_RATIO       (16.0f)
#define SC410GS_MIN_RATIO       (1.0f)

#define SC410GS_MIN_AGAIN       (1.0f)
#define SC410GS_MAX_AGAIN       (26.988f)

#define SC410GS_MIN_DGAIN       (1.0f)
#define SC410GS_MAX_DGAIN       (3.969f)

typedef struct _SNSSC410GS_OBJ_T_ {
    AX_U32 vts;
    AX_U32 setting_vts;
    AX_F32 setting_fps;
} SNSSC410GS0_OBJ_T;

typedef struct _SC410GS_GAIN_TABLE_T_ {
    float gain;
    AX_U8 gain_in;
    AX_U8 gain_de;
} SC410GS_GAIN_TABLE_T;

AX_S32 sc410gs_cfg_aec_param(ISP_PIPE_ID nPipeId);

AX_S32 sc410gs_get_gain_table(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_TABLE_T *ptParams);
AX_S32 sc410gs_set_again(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptAGain);
AX_S32 sc410gs_set_dgain(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptDGain);

AX_S32 sc410gs_get_hw_exposure_params(ISP_PIPE_ID nPipeId, AX_SNS_EXP_CTRL_PARAM_T *ptAeCtrlParam);
AX_S32 sc410gs_get_integration_time_range(ISP_PIPE_ID nPipeId, AX_F32 fHdrRatio, AX_SNS_AE_INT_TIME_RANGE_T *ptIntTimeRange);
AX_S32 sc410gs_set_integration_time(ISP_PIPE_ID nPipeId, AX_SNS_AE_SHUTTER_CFG_T *ptIntTime);

AX_S32 sc410gs_set_fps(ISP_PIPE_ID nPipeId, AX_F32 fFps);
AX_S32 sc410gs_get_slow_shutter_param(ISP_PIPE_ID nPipeId, AX_SNS_AE_SLOW_SHUTTER_PARAM_T *ptSlowShutterParam);

AX_S32 sc410gs_update_regidx_table(ISP_PIPE_ID nPipeId, AX_U8 nRegIdx, AX_U8 nRegValue);
AX_S32 sc410gs_ae_get_reg_info(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo);
AX_U32 sc410gs_refresh_all_regs_from_tbl(ISP_PIPE_ID nPipeId);


#endif  //end __SC410GS_AE_CTRL_H__
