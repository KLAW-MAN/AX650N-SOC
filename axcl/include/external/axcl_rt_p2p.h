/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_P2P_H__
#define __AXCL_RT_P2P_H__

#include "axcl_rt_type.h"
#include "axcl_rt_p2p_type.h"

#ifdef __cplusplus
extern "C" {
#endif

AXCL_EXPORT axclError axclrtCreateP2PUnit(const axclrtP2PUnitInfo *info, AXCL_P2P_UNIT_HANDLE *handle);
AXCL_EXPORT axclError axclrtDestoryP2PUnit(AXCL_P2P_UNIT_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_P2P_H__ */
