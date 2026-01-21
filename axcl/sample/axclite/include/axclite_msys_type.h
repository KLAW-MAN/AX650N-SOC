/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCLITE_MSYS_TYPE_H__
#define __AXCLITE_MSYS_TYPE_H__

#include "axclite.h"

#define AXCL_DEF_LITE_MSYS_ERR(e)           AXCL_DEF_LITE_ERR(AX_ID_SYS, (e))
#define AXCL_ERR_LITE_MSYS_NULL_POINTER     AXCL_DEF_LITE_MSYS_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_LITE_MSYS_ILLEGAL_PARAM    AXCL_DEF_LITE_MSYS_ERR(AXCL_ERR_ILLEGAL_PARAM)
#define AXCL_ERR_LITE_MSYS_NO_MEMORY        AXCL_DEF_LITE_MSYS_ERR(AXCL_ERR_NO_MEMORY)
#define AXCL_ERR_LITE_MSYS_NO_LINKED        AXCL_DEF_LITE_MSYS_ERR(0x81)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AX_VDEC_HWCLK_624M = 0, /* default hw clk */
    AX_VDEC_HWCLK_500M = 1,
    AX_VDEC_HWCLK_400M = 2,
    AX_VDEC_HWCLK_BUTT
} AX_VDEC_HWCLK_E;

typedef struct msys_attr {
    AX_U32 modules;
    AX_U32 max_vdec_grp;
    AX_U32 max_venc_thd;
    AX_VDEC_HWCLK_E vdec_clk;
} axclite_msys_attr;

#ifdef __cplusplus
}
#endif

#endif /* __AXCLITE_MSYS_TYPE_H__ */