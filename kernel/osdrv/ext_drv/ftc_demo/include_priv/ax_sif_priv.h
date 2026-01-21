/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_ISP_SIF_PRIV_H__
#define __DEF_ISP_SIF_PRIV_H__

//#include "ax_base_type.h"
#include "ax_global_type.h"

typedef struct _sen_shadow_cfg {
    AX_U8 sen_shadow_en0;
    AX_U8 sen_shadow_en1;
    AX_U8 sen_shadow_sts0;
    AX_U8 sen_shadow_sts1;
    AX_U8 sen_shadow_wr_id0;
    AX_U8 sen_shadow_rd_id0;
    AX_U8 sen_shadow_wr_id1;
    AX_U8 sen_shadow_rd_id1;
} sen_shadow_cfg_s;

typedef struct _sen_ctrl_cfg {
    AX_U8 sen_ctrl_en;
    AX_U8 sen_ctrl_mode;
    AX_U8 sen_expo_v0_ctrl;
    AX_U8 sen_expo_v1_ctrl;
    AX_U8 sen_expo_v2_ctrl;
    AX_U8 sen_expo_v3_ctrl;
    AX_U8 sen_expo_h0_ctrl;
    AX_U8 sen_expo_h1_ctrl;
    AX_U8 sen_expo_h2_ctrl;
    AX_U8 sen_expo_h3_ctrl;
} sen_ctrl_cfg_s;

typedef struct _sen_flash_ctrl_cfg {
    AX_U8 sen_flash0_ctrl;
    AX_U8 sen_flash1_ctrl;
    AX_U8 sen_flash2_ctrl;
    AX_U8 sen_flash3_ctrl;
    AX_U8 sen_flash4_ctrl;
    AX_U8 sen_flash5_ctrl;
    AX_U8 sen_flash6_ctrl;
    AX_U8 sen_flash7_ctrl;
    AX_U8 sen_flash8_ctrl;
    AX_U8 sen_flash9_ctrl;
} sen_flash_ctrl_cfg_s;

#endif // __DEF_ISP_SIF_PRIV_H__
