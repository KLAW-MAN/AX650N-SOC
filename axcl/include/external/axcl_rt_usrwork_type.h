/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_USRWORK_TYPE_H__
#define __AXCL_RT_USRWORK_TYPE_H__

#include "axcl_rt_type.h"

// clang-format off
#ifdef __cplusplus
extern "C" {
#endif

#define AXCL_DEF_USRWORK_ERR(e)              AXCL_DEF_RUNTIME_ERR(AXCL_RUNTIME_USRWORK, (e))

#define AXCL_ERR_USRWORK_ILLEGAL_PARAM       AXCL_DEF_USRWORK_ERR(AXCL_ERR_ILLEGAL_PARAM)
#define AXCL_ERR_USRWORK_NULL_POINTER        AXCL_DEF_USRWORK_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_USRWORK_ENCODE              AXCL_DEF_USRWORK_ERR(AXCL_ERR_ENCODE)
#define AXCL_ERR_USRWORK_DECODE              AXCL_DEF_USRWORK_ERR(AXCL_ERR_DECODE)
#define AXCL_ERR_USRWORK_UNEXPECT_RESPONSE   AXCL_DEF_USRWORK_ERR(AXCL_ERR_UNEXPECT_RESPONSE)
#define AXCL_ERR_USRWORK_EXECUTE_FAIL        AXCL_DEF_USRWORK_ERR(AXCL_ERR_EXECUTE_FAIL)
#define AXCL_ERR_USRWORK_TIMEOUT             AXCL_DEF_USRWORK_ERR(AXCL_ERR_TIMEOUT)

#define AXCL_ERR_USRWORK_OPEN_FILE           AXCL_DEF_USRWORK_ERR(0x81)
#define AXCL_ERR_USRWORK_EMPTY_FILE          AXCL_DEF_USRWORK_ERR(0x82)
#define AXCL_ERR_USRWORK_PROCESS_NOT_FOUND   AXCL_DEF_USRWORK_ERR(0x83)
#define AXCL_ERR_USRWORK_INVALID_PATH        AXCL_DEF_USRWORK_ERR(0x84)
#define AXCL_ERR_USRWORK_OPEN_CHANNEL        AXCL_DEF_USRWORK_ERR(0x85)
#define AXCL_ERR_USRWORK_SEND_DATA           AXCL_DEF_USRWORK_ERR(0x86)
#define AXCL_ERR_USRWORK_BUFFER_TOO_SMALL    AXCL_DEF_USRWORK_ERR(0x87)


#ifdef __cplusplus
}
#endif
// clang-format on

#endif /* __AXCL_RT_USRWORK_H__ */