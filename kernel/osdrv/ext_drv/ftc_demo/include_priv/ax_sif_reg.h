/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_ISP_SIF_REG_H__
#define __DEF_ISP_SIF_REG_H__

#include "ax_global_type.h"

typedef union _SEN_SHADOW {
    struct  {
        AX_U32 sen_shadow_en0                   : 1;    /* [0]*/
        AX_U32 sen_shadow_en1                   : 1;    /* [1]*/
        AX_U32 reserved                         : 2;    /* [3:2]*/
        AX_U32 sen_shadow_sts0                  : 1;    /* [4]*/
        AX_U32 sen_shadow_sts1                  : 1;    /* [5]*/
        AX_U32 reserved_1                       : 10;   /* [15:6]*/
        AX_U32 sen_shadow_wr_id0                : 4;    /* [19:16]*/
        AX_U32 sen_shadow_rd_id0                : 4;    /* [23:20]*/
        AX_U32 sen_shadow_wr_id1                : 4;    /* [27:24]*/
        AX_U32 sen_shadow_rd_id1                : 4;    /* [31:28]*/

    } bits;
    AX_U32 reg32;
} SEN_SHADOW_U;

typedef union _SEN_CTRL {
    struct  {
        AX_U32 sen_ctrl_en                      : 1;    /* [0]*/
        AX_U32 sen_ctrl_mode                    : 1;    /* [1]*/
        AX_U32 reserved                         : 14;   /* [15:2]*/
        AX_U32 sen_expo_v0_ctrl                 : 2;    /* [17:16]*/
        AX_U32 sen_expo_v1_ctrl                 : 2;    /* [19:18]*/
        AX_U32 sen_expo_v2_ctrl                 : 2;    /* [21:20]*/
        AX_U32 sen_expo_v3_ctrl                 : 2;    /* [23:22]*/
        AX_U32 sen_expo_h0_ctrl                 : 2;    /* [25:24]*/
        AX_U32 sen_expo_h1_ctrl                 : 2;    /* [27:26]*/
        AX_U32 sen_expo_h2_ctrl                 : 2;    /* [29:28]*/
        AX_U32 sen_expo_h3_ctrl                 : 2;    /* [31:30]*/
    } bits;
    AX_U32 reg32;
} SEN_CTRL_U;

typedef union _SEN_INTERNAL_TRIG {
    struct  {
        AX_U32 sen_internal_trig                : 1;    /* [0]*/
        AX_U32 reserved                         : 31;   /* [31:1]*/
    } bits;
    AX_U32 reg32;
} SEN_INTERNAL_TRIG_U;

typedef union _SEN_EXPO_V {
    struct  {
        AX_U32 sen_expo_v_num                   : 12;   /* [11:0]*/
        AX_U32 reserved                         : 4;    /* [15:12]*/
        AX_U32 sen_expo_v_num_total             : 14;   /* [29:16]*/
        AX_U32 reserved_1                       : 2;    /* [31:30]*/
    } bits;
    AX_U32 reg32;
} SEN_EXPO_V_U;

typedef union _SEN_EXPO_H {
    struct  {
        AX_U32 sen_expo_h_num                   : 12;   /* [11:0]*/
        AX_U32 reserved                         : 4;    /* [15:12]*/
        AX_U32 sen_expo_h_num_total             : 14;   /* [29:16]*/
        AX_U32 reserved_1                       : 2;    /* [31:30]*/
    } bits;
    AX_U32 reg32;
} SEN_EXPO_H_U;

typedef union _SEN_EXPO_ADJUST0 {
    struct  {
        AX_U32 sen_expo_v_dly_num               : 14;   /* [13:0]*/
        AX_U32 reserved                         : 18;   /* [31:14]*/
    } bits;
    AX_U32 reg32;
} SEN_EXPO_ADJUST0_U;

typedef union _SEN_EXPO_ADJUST1 {
    struct  {
        AX_U32 sen_expo_adjust_line_num         : 12;   /* [11:0]*/
        AX_U32 reserved                         : 4;    /* [15:12]*/
        AX_U32 sen_expo_adjust_line_start       : 14;   /* [29:16]*/
        AX_U32 reserved_1                       : 2;    /* [31:30]*/
    } bits;
    AX_U32 reg32;
} SEN_EXPO_ADJUST1_U;

typedef union _SEN_FLASH_CTRL {
    struct  {
        AX_U32 sen_flash0_ctrl                  : 3;    /* [2:0]*/
        AX_U32 sen_flash1_ctrl                  : 3;    /* [5:3]*/
        AX_U32 sen_flash2_ctrl                  : 3;    /* [8:6]*/
        AX_U32 sen_flash3_ctrl                  : 3;    /* [11:9]*/
        AX_U32 sen_flash4_ctrl                  : 3;    /* [14:12]*/
        AX_U32 sen_flash5_ctrl                  : 3;    /* [17:15]*/
        AX_U32 sen_flash6_ctrl                  : 3;    /* [20:18]*/
        AX_U32 sen_flash7_ctrl                  : 3;    /* [23:21]*/
        AX_U32 sen_flash8_ctrl                  : 3;    /* [26:24]*/
        AX_U32 sen_flash9_ctrl                  : 3;    /* [29:27]*/
        AX_U32 reserved                         : 2;    /* [31:30]*/
    } bits;
    AX_U32 reg32;
} SEN_FLASH_CTRL_U;

typedef union _SEN_FLASH {
    struct  {
        AX_U32 sen_flash_on_num                 : 18;   /* [17:0]*/
        AX_U32 sen_flash_dly_num                : 14;   /* [31:18]*/
    } bits;
    AX_U32 reg32;
} SEN_FLASH_U;

typedef union _SEN_DEBUG {
    struct  {
        AX_U32 sen_debug_status                 : 31;   /* [30:0]*/
        AX_U32 sen_debug_enable                 : 1;    /* [31]*/
    } bits;
    AX_U32 reg32;
} SEN_DEBUG_U;

typedef union _SIF_SEN_FRM {
    struct  {
        AX_U32 sif_frm_cnt                      : 32;   /* [31:0]*/
    } bits;
    AX_U32 reg32;
} SIF_SEN_FRM_U;

typedef struct _ISP_SIF_EXPO_V_H_REG_S {
    SEN_EXPO_V_U                                SEN_EXPO_V;
    SEN_EXPO_H_U                                SEN_EXPO_H;
} ISP_SIF_EXPO_V_H_REG_T;

/* Register ADDR: [0x200  ---  0x250] */
typedef struct _ISP_SIF_SNS_FLASH_REG_S {
    SEN_SHADOW_U                                SEN_SHADOW;/* 0x200 */
    SEN_CTRL_U                                  SEN_CTRL;/* 0x204 */
    SEN_INTERNAL_TRIG_U                         SEN_INTERNAL_TRIG;/* 0x208 */
    ISP_SIF_EXPO_V_H_REG_T                      SEN_EXPO[2];/* 0x20c - 0x218 */
    SEN_EXPO_ADJUST0_U                          SEN_EXPO_ADJUST0;/* 0x21c */
    SEN_EXPO_ADJUST1_U                          SEN_EXPO_ADJUST1;/* 0x220 */
    SEN_FLASH_CTRL_U                            SEN_FLASH_CTRL;/* 0x224 */
    SEN_FLASH_U                                 SEN_FLASH[10];/* 0x228 - 0x24c */
    SEN_DEBUG_U                                 SEN_DEBUG;/* 0x250 */
} ISP_SIF_SNS_FLASH_REG_T;
#endif // __DEF_ISP_SIF_REG_H__