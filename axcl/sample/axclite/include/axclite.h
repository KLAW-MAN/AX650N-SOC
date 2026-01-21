/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCLITE_H__
#define __AXCLITE_H__

#include "axcl.h"
#include "axclite_ivps_type.h"
#include "axclite_msys_type.h"
#include "axclite_vdec_type.h"
#include "axclite_venc_type.h"

#ifndef AXCL_ALIGN_UP
#define AXCL_ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AXCL_LITE_NONE = 0,
    AXCL_LITE_VDEC = (1 << 0),
    AXCL_LITE_VENC = (1 << 1),
    AXCL_LITE_IVPS = (1 << 2),
    AXCL_LITE_JDEC = (1 << 3),
    AXCL_LITE_JENC = (1 << 4),
    AXCL_LITE_DEFAULT = (AXCL_LITE_VDEC | AXCL_LITE_VENC | AXCL_LITE_IVPS),
    AXCL_LITE_MODULE_BUTT
} AXCLITE_MODULE_E;

#ifdef __cplusplus
}
#endif

#endif /* __AXCLITE_H__ */