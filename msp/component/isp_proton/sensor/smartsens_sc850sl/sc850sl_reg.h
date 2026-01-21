/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC850SL_REG_H__
#define __SC850SL_REG_H__

#include "ax_base_type.h"

/* Exposure control related registers */
#define SC850SL_EXPLINE_LONG_H      (0x3e00)  /* bit[3:0], long frame exposure in unit of rows */
#define SC850SL_EXPLINE_LONG_M      (0x3e01)  /* bit[7:0], long frame exposure in unit of rows */
#define SC850SL_EXPLINE_LONG_L      (0x3e02)  /* bit[7:4], long frame exposure in unit of rows */

#define SC850SL_EXPLINE_SHORT_H     (0x3e22)  /* bit[4:0], short frame exposure in unit of rows */
#define SC850SL_EXPLINE_SHORT_M     (0x3e04)  /* bit[7:0], short frame exposure in unit of rows */
#define SC850SL_EXPLINE_SHORT_L     (0x3e05)  /* bit[7:4], short frame exposure in unit of rows */

#define SC850SL_AGAIN_LONG_H        (0x3e08)  /* bit[5:0], real gain[13:8] long frame */
#define SC850SL_AGAIN_LONG_L        (0x3e09)  /* bit[7:0], real gain[7:0] long frame */
#define SC850SL_AGAIN_SHORT_H       (0x3e12)  /* bit[5:0], real gain[13:8] short frame */
#define SC850SL_AGAIN_SHORT_L       (0x3e13)  /* bit[7:0], real gain[7:0] short frame */
#define SC850SL_AGAIN_ADJUST        (0x363C)  /* adjust voltage for noise */

#define SC850SL_DGAIN_LONG          (0x3e06)
#define SC850SL_DGAIN_SHORT         (0x3e10)

#define SC850SL_VTS_L_H             (0x320E)  /* bit[6:0], vts[15:8] */
#define SC850SL_VTS_L_L             (0x320F)  /* bit[7:0], vts[7:0] */
#define SC850SL_VTS_S_H             (0x3E23)  /* bit[6:0], vts[15:8] */
#define SC850SL_VTS_S_L             (0x3E24)  /* bit[7:0], vts[7:0] */

#define SC850SL_RB_ROWS_H           (0x3230)  /* bit[6:0], vts[15:8] */
#define SC850SL_RB_ROWS_L           (0x3231)  /* bit[7:0], vts[7:0] */

#define SC850SL_HTS_H               (0x320C)  /* bit[6:0], vts[15:8] */
#define SC850SL_HTS_L               (0x320D)  /* bit[7:0], vts[7:0] */

typedef enum _AX_SNS_AE_REG_IDX_E_ {
    SC850SL_EXPLINE_LONG_H_IDX = 0,
    SC850SL_EXPLINE_LONG_M_IDX,
    SC850SL_EXPLINE_LONG_L_IDX,
    SC850SL_EXPLINE_SHORT_H_IDX,
    SC850SL_EXPLINE_SHORT_M_IDX,
    SC850SL_EXPLINE_SHORT_L_IDX,
    SC850SL_AGAIN_LONG_H_IDX,
    SC850SL_AGAIN_LONG_L_IDX,
    SC850SL_AGAIN_SHORT_H_IDX,
    SC850SL_AGAIN_SHORT_L_IDX,
    SC850SL_AGAIN_ADJUST_IDX,
    SC850SL_DGAIN_LONG_IDX,
    SC850SL_DGAIN_SHORT_IDX,
    SC850SL_VTS_L_H_IDX,
    SC850SL_VTS_L_L_IDX,
    SC850SL_VTS_S_H_IDX,
    SC850SL_VTS_S_L_IDX,
    SC850SL_RB_ROWS_H_IDX,
    SC850SL_RB_ROWS_L_IDX,
} AX_SNS_AE_REG_IDX_E;

AX_S32 sc850sl_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr);
AX_S32 sc850sl_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData);
AX_S32 sc850sl_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data);
AX_S32 sc850sl_update_regidx_table(ISP_PIPE_ID nPipeId, AX_U8 nRegIdx, AX_U8 nRegValue);

AX_S32 sc850sl_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts);
AX_S32 sc850sl_set_vts_s(ISP_PIPE_ID nPipeId, AX_U32 vts_s);
AX_S32 sc850sl_get_vts(ISP_PIPE_ID nPipeId);
AX_S32 sc850sl_get_vts_s(ISP_PIPE_ID nPipeId);

AX_S32 sc850sl_set_int_t_l(ISP_PIPE_ID nPipeId, AX_U32 int_t);
AX_S32 sc850sl_set_int_t_s(ISP_PIPE_ID nPipeId, AX_U32 int_t);

AX_S32 sc850sl_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts);

AX_S32 sc850sl_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt);
AX_S32 sc850sl_write_settings(ISP_PIPE_ID nPipeId);

#endif  //end __SC850SL_REG_H__
