/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_STREAM_H__
#define __AXCL_RT_STREAM_H__

#include "axcl_rt_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXCL_DEF_STREAM_ERR(e)          AXCL_DEF_RUNTIME_ERR(AXCL_RUNTIME_STREAM, (e))

#define AXCL_ERR_STREAM_NULL_POINTER    AXCL_DEF_STREAM_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_STREAM_TIMEOUT         AXCL_DEF_STREAM_ERR(AXCL_ERR_TIMEOUT)
#define AXCL_ERR_STREAM_CREATE          AXCL_DEF_STREAM_ERR(0x81)
#define AXCL_ERR_STREAM_DESTROY         AXCL_DEF_STREAM_ERR(0x82)
#define AXCL_ERR_STREAM_SYNC_TIMEOUT    AXCL_DEF_STREAM_ERR(0x83)

AXCL_EXPORT axclError axclrtCreateStream(axclrtStream *stream);
AXCL_EXPORT axclError axclrtDestroyStream(axclrtStream stream);
AXCL_EXPORT axclError axclrtDestroyStreamForce(axclrtStream stream);
AXCL_EXPORT axclError axclrtSynchronizeStream(axclrtStream stream);
AXCL_EXPORT axclError axclrtSynchronizeStreamWithTimeout(axclrtStream stream, int32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_STREAM_H__ */