/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_USRWORK_H__
#define __AXCL_RT_USRWORK_H__

#include "axcl_rt_type.h"
#include "axcl_rt_usrwork_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup AxeraCL Runtime
 * @brief Transfer file between HOST and device
 *
 * @param[in]  src_path     Source file path
 * @param[in]  dst_path     Destination file path
 * @param[in]  policy       File transfer policy
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtTransferFile(const char *src_path, const char *dst_path, axclrtFileTransferPolicy policy);

/**
 * @ingroup AxeraCL Runtime
 * @brief Execute user worker process
 *
 * @param[in]  path         Path to the worker executable
 * @param[in]  argc         Number of arguments
 * @param[in]  argv         Array of argument strings
 * @param[out] pid          Process ID of the started worker
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtExecWorker(const char *path, const int32_t *argc, const char *argv[], uint32_t *pid);

/**
 * @ingroup AxeraCL Runtime
 * @brief Kill user worker process
 *
 * @param[in]  pid          Process ID of the worker to kill
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtKillWorker(uint32_t pid);

/**
 * @ingroup AxeraCL Runtime
 * @brief Send data to user worker process
 *
 * @param[in]  pid          Process ID of the target worker
 * @param[in]  buf          Data buffer to send
 * @param[in]  size         Size of data to send
 * @param[in]  timeout      Timeout in milliseconds (-1 for infinite)
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtWorkerSend(uint32_t pid, const void *buf, uint32_t size, int32_t timeout);

/**
 * @ingroup AxeraCL Runtime
 * @brief Receive data from user worker process
 *
 * @param[in]  pid          Process ID of the source worker
 * @param[out] buf          Buffer to store received data
 * @param[in]  bufsize      Size of the buffer
 * @param[out] recvlen      Actual number of bytes received
 * @param[in]  timeout      Timeout in milliseconds (-1 for infinite)
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtWorkerRecv(uint32_t pid, void *buf, uint32_t bufsize, uint32_t* recvlen, int32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_USRWORK_H__ */