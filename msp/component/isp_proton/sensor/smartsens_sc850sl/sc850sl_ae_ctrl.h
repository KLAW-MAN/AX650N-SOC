/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC850SL_AE_CTRL_H__
#define __SC850SL_AE_CTRL_H__

#include "ax_base_type.h"
#include "ax_sensor_struct.h"

#define SC850SL_MAX_VTS         (0xFFFF)
#define SC850SL_MIN_RATIO       (1.0f)
#define SC850SL_MAX_RATIO       (32.0f)

#define SC850SL_MIN_AGAIN       (1.0f)
#define SC850SL_MAX_AGAIN       (49.609f)

#define SC850SL_MIN_DGAIN       (1.0f)
#define SC850SL_MAX_DGAIN       (8.0f)

#define SC850SL_SDR_EXPLINE_MIN                         (2)
#define SC850SL_SDR_EXPLINE_MAX(vts)                    (vts - 4)
#define SC850SL_SDR_EXPLINE_STEP                        (1)

#define SC850SL_HDR_2X_EXPLINE_LONG_MIN                 (4)
#define SC850SL_HDR_2X_EXPLINE_LONG_MAX(vts, vts_s)     (vts - vts_s - 8)
#define SC850SL_HDR_2X_EXPLINE_LONG_STEP                (2)

#define SC850SL_HDR_2X_EXPLINE_SHORT_MIN                (4)
#define SC850SL_HDR_2X_EXPLINE_SHORT_MAX(vts_s)         (vts_s - 2)
#define SC850SL_HDR_2X_EXPLINE_SHORT_STEP               (2)


#define SC850SL_EPS         (1E-02)

typedef struct _SC850SL_OBJ_ {
    AX_U32 vts;
} SC850SL_OBJ;

typedef struct _SC850SL_GAIN_TABLE_T_ {
    AX_F32 gain;
    AX_U8  gain_in;
    AX_U8  gain_de;
} SC850SL_GAIN_TABLE_T;

AX_S32 sc850sl_cfg_aec_param(ISP_PIPE_ID nPipeId);
AX_S32 sc850sl_get_gain_table(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_TABLE_T *params);
AX_S32 sc850sl_set_again(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptAGain);
AX_S32 sc850sl_set_dgain(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptDGain);
AX_S32 sc850sl_hcglcg_ctrl(ISP_PIPE_ID nPipeId, AX_U32 nSnsHcgLcg);
AX_S32 sc850sl_get_integration_time_range(ISP_PIPE_ID nPipeId, AX_F32 fHdrRatio, AX_SNS_AE_INT_TIME_RANGE_T *ptIntTimeRange);
AX_S32 sc850sl_set_integration_time(ISP_PIPE_ID nPipeId, AX_SNS_AE_SHUTTER_CFG_T *ptIntTime);
AX_S32 sc850sl_get_hw_exposure_params(ISP_PIPE_ID nPipeId, AX_SNS_EXP_CTRL_PARAM_T *ptAeCtrlParam);
AX_S32 sc850sl_get_fps(ISP_PIPE_ID nPipeId, AX_F32 *pFps);
AX_S32 sc850sl_set_fps(ISP_PIPE_ID nPipeId, AX_F32 fFps);
AX_S32 sc850sl_set_slow_fps(ISP_PIPE_ID nPipeId, AX_F32 fFps);
AX_S32 sc850sl_get_slow_shutter_param(ISP_PIPE_ID nPipeId, AX_SNS_AE_SLOW_SHUTTER_PARAM_T *ptSlowShutterParam);
AX_S32 sc850sl_ae_get_sensor_reg_info(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo);
AX_U32 sc850sl_refresh_all_regs_from_tbl(ISP_PIPE_ID nPipeId);

#endif  //end __SC850SL_AE_CTRL_H__
