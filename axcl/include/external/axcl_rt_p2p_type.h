/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_P2P_TYPE_H__
#define __AXCL_RT_P2P_TYPE_H__

#include "axcl_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXCL_DEF_P2P_ERR(e)             AXCL_DEF_RUNTIME_ERR(AXCL_RUNTIME_P2P, (e))
#define AXCL_ERR_P2P_NULL_POINTER       AXCL_DEF_P2P_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_P2P_INIT               AXCL_DEF_P2P_ERR(AXCL_ERR_OPEN)
#define AXCL_ERR_P2P_NO_MEMORY          AXCL_DEF_P2P_ERR(AXCL_ERR_NO_MEMORY)
#define AXCL_ERR_P2P_BWT                AXCL_DEF_P2P_ERR(AXCL_ERR_EXECUTE_FAIL)
#define AXCL_ERR_P2P_VERIFY_FAIL        AXCL_DEF_P2P_ERR(AXCL_ERR_ILLEGAL_PARAM)

#define AXCL_MAX_P2P_DEVICE_COUNT (8)

typedef void* AXCL_P2P_UNIT_HANDLE;

typedef struct axclrtP2PUnitInfo {
    uint32_t u32DeviceNum;
    int32_t n32DeviceId[AXCL_MAX_P2P_DEVICE_COUNT];
    uint32_t u32DeviceMemSize[AXCL_MAX_P2P_DEVICE_COUNT];
} axclrtP2PUnitInfo;

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_P2P_TYPE_H__ */
