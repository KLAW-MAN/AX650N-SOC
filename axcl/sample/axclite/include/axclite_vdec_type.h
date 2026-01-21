/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCLITE_VDEC_TYPE_H__
#define __AXCLITE_VDEC_TYPE_H__

#include "axclite.h"

#define AX_SHIFT_LEFT_ALIGN(a) (1 << (a))

/* VDEC stride align 256 */
#define VDEC_STRIDE_ALIGN AX_SHIFT_LEFT_ALIGN(8)

#define INVALID_VDGRP_ID (-1)
#define INVALID_VDCHN_ID (-1)
#define CHECK_VDEC_GRP(vdGrp) ((vdGrp) >= 0 && (vdGrp) < AX_VDEC_MAX_GRP_NUM)
#define CHECK_VDEC_CHN(vdChn) ((vdChn) >= 0 && (vdChn) < AX_VDEC_MAX_CHN_NUM)

#define MAX_VDEC_RESET_GRP_RETRY_COUNT (3)

#define AXCL_DEF_LITE_VDEC_ERR(e)           AXCL_DEF_LITE_ERR(AX_ID_VDEC, (e))
#define AXCL_ERR_LITE_VDEC_NULL_POINTER     AXCL_DEF_LITE_VDEC_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_LITE_VDEC_ILLEGAL_PARAM    AXCL_DEF_LITE_VDEC_ERR(AXCL_ERR_ILLEGAL_PARAM)
#define AXCL_ERR_LITE_VDEC_NO_MEMORY        AXCL_DEF_LITE_VDEC_ERR(AXCL_ERR_NO_MEMORY)
#define AXCL_ERR_LITE_VDEC_INVALID_GRP      AXCL_DEF_LITE_VDEC_ERR(0x81)
#define AXCL_ERR_LITE_VDEC_INVALID_CHN      AXCL_DEF_LITE_VDEC_ERR(0x82)
#define AXCL_ERR_LITE_VDEC_NOT_STARTED      AXCL_DEF_LITE_VDEC_ERR(0x83)
#define AXCL_ERR_LITE_VDEC_NOT_REGISTED     AXCL_DEF_LITE_VDEC_ERR(0x84)
#define AXCL_ERR_LITE_VDEC_START_DISPATCH   AXCL_DEF_LITE_VDEC_ERR(0x85)
#define AXCL_ERR_LITE_VDEC_STOP_DISPATCH    AXCL_DEF_LITE_VDEC_ERR(0x86)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    AX_BOOL enable;
    AX_BOOL link;
    AX_U32 x; /* crop offset x */
    AX_U32 y; /* crop offset y */
    AX_U32 width;
    AX_U32 height;
    AX_FRAME_COMPRESS_INFO_T fbc;
    AX_U32 blk_cnt;
    AX_U32 fifo_depth;
} axclite_vdec_chn_attr;

typedef struct {
    AX_PAYLOAD_TYPE_E payload;
    AX_U32 width;
    AX_U32 height;
    AX_VDEC_OUTPUT_ORDER_E output_order;
    AX_VDEC_DISPLAY_MODE_E display_mode;
} axclite_vdec_grp_attr;

typedef struct {
    axclite_vdec_grp_attr grp;
    axclite_vdec_chn_attr chn[AX_DEC_MAX_CHN_NUM];
} axclite_vdec_attr;

#ifdef __cplusplus
}
#endif

#endif /* __AXCLITE_VDEC_TYPE_H__ */