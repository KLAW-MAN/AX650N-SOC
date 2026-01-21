/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCLITE_IVPS_TYPE_H__
#define __AXCLITE_IVPS_TYPE_H__

#include "axclite.h"

#define INVALID_IVPS_GRP (-1)
#define INVALID_IVPS_CHN (-1)
#define CHECK_IVPS_GRP(ivGrp) ((ivGrp) >= 0 && (ivGrp) < AX_IVPS_MAX_GRP_NUM)
#define CHECK_IVPS_CHN(ivChn) ((ivChn) >= 0 && (ivChn) < AX_IVPS_MAX_OUTCHN_NUM)

#define AXCL_DEF_LITE_IVPS_ERR(e)           AXCL_DEF_LITE_ERR(AX_ID_IVPS, (e))
#define AXCL_ERR_LITE_IVPS_NULL_POINTER     AXCL_DEF_LITE_IVPS_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_LITE_IVPS_ILLEGAL_PARAM    AXCL_DEF_LITE_IVPS_ERR(AXCL_ERR_ILLEGAL_PARAM)
#define AXCL_ERR_LITE_IVPS_NO_MEMORY        AXCL_DEF_LITE_IVPS_ERR(AXCL_ERR_NO_MEMORY)
#define AXCL_ERR_LITE_IVPS_INVALID_GRP      AXCL_DEF_LITE_IVPS_ERR(0x81)
#define AXCL_ERR_LITE_IVPS_INVALID_CHN      AXCL_DEF_LITE_IVPS_ERR(0x82)
#define AXCL_ERR_LITE_IVPS_START_DISPATCH   AXCL_DEF_LITE_IVPS_ERR(0x83)
#define AXCL_ERR_LITE_IVPS_STOP_DISPATCH    AXCL_DEF_LITE_IVPS_ERR(0x84)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    AX_U32 fifo_depth;
    AX_U32 backup_depth;
} axclite_ivps_grp_attr;

typedef struct {
    AX_BOOL bypass;
    AX_BOOL link;
    AX_U32 fifo_depth;
    AX_IVPS_ENGINE_E engine;
    AX_BOOL crop;
    AX_IVPS_RECT_T box;
    AX_U32 width;
    AX_U32 height;
    AX_U32 stride;
    AX_IMG_FORMAT_E pix_fmt;
    AX_FRAME_COMPRESS_INFO_T fbc;
    AX_FRAME_RATE_CTRL_T frc;
    AX_BOOL inplace;
    AX_U32 blk_cnt;
} axclite_ivps_chn_attr;

typedef struct {
    axclite_ivps_grp_attr grp;
    AX_U32 chn_num;
    axclite_ivps_chn_attr chn[AX_IVPS_MAX_OUTCHN_NUM];
} axclite_ivps_attr;

#ifdef __cplusplus
}
#endif

#endif /* __AXCLITE_IVPS_TYPE_H__ */