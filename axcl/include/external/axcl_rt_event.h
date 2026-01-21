/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_EVENT_H__
#define __AXCL_RT_EVENT_H__

#include "axcl_rt_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXCL_DEF_EVENT_ERR(e)          AXCL_DEF_RUNTIME_ERR(AXCL_RUNTIME_STREAM, (e))

#define AXCL_ERR_EVENT_NULL_POINTER    AXCL_DEF_EVENT_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_EVENT_TIMEOUT         AXCL_DEF_EVENT_ERR(AXCL_ERR_TIMEOUT)
#define AXCL_ERR_EVENT_CREATE          AXCL_DEF_EVENT_ERR(0x81)
#define AXCL_ERR_EVENT_DESTROY         AXCL_DEF_EVENT_ERR(0x82)
#define AXCL_ERR_EVENT_RECORD          AXCL_DEF_EVENT_ERR(0x83)
#define AXCL_ERR_EVENT_WAIT            AXCL_DEF_EVENT_ERR(0x84)

AXCL_EXPORT axclError axclrtCreateEvent(axclrtEvent *event);
AXCL_EXPORT axclError axclrtDestroyEvent(axclrtEvent event);
AXCL_EXPORT axclError axclrtRecordEvent(axclrtEvent event, axclrtStream stream);
AXCL_EXPORT axclError axclrtStreamWaitEvent(axclrtStream stream, axclrtEvent event);
AXCL_EXPORT axclError axclrtStreamWaitEventWithTimeout(axclrtStream stream, axclrtEvent event, int32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_EVENT_H__ */