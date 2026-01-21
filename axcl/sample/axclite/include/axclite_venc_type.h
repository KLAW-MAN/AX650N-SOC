/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCLITE_VENC_TYPE_H__
#define __AXCLITE_VENC_TYPE_H__

#include "axclite.h"

#define INVALID_VENC_CHN (-1)
#define CHECK_VENC_CHN(veChn) ((veChn) >= 0 && (veChn) < MAX_VENC_CHN_NUM)

#define AXCL_DEF_LITE_VENC_ERR(e)           AXCL_DEF_LITE_ERR(AX_ID_VENC, (e))
#define AXCL_ERR_LITE_VENC_NULL_POINTER     AXCL_DEF_LITE_VENC_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_LITE_VENC_ILLEGAL_PARAM    AXCL_DEF_LITE_VENC_ERR(AXCL_ERR_ILLEGAL_PARAM)
#define AXCL_ERR_LITE_VENC_NO_MEMORY        AXCL_DEF_LITE_VENC_ERR(AXCL_ERR_NO_MEMORY)
#define AXCL_ERR_LITE_VENC_INVALID_CHN      AXCL_DEF_LITE_VENC_ERR(0x82)
#define AXCL_ERR_LITE_VENC_START_DISPATCH   AXCL_DEF_LITE_VENC_ERR(0x83)
#define AXCL_ERR_LITE_VENC_STOP_DISPATCH    AXCL_DEF_LITE_VENC_ERR(0x84)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    AX_PAYLOAD_TYPE_E payload;
    AX_U32 width;
    AX_U32 height;
    AX_VENC_PROFILE_E profile;
    AX_VENC_LEVEL_E level;
    AX_VENC_TIER_E tile;
    AX_BOOL link;
    AX_U32 in_fifo_depth;
    AX_U32 out_fifo_depth;
    AX_S32 flag;
} axclite_venc_chn_attr;

typedef struct {
    axclite_venc_chn_attr chn;
    AX_VENC_RC_ATTR_T rc;
    AX_VENC_GOP_ATTR_T gop;
} axclite_venc_attr;

#ifdef __cplusplus
}
#endif

#endif /* __AXCLITE_VENC_TYPE_H__ */