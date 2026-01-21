/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_MEMORY_H__
#define __AXCL_RT_MEMORY_H__

#include "axcl_rt_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AXCL_DEF_MEMORY_ERR(e)              AXCL_DEF_RUNTIME_ERR(AXCL_RUNTIME_MEMORY, (e))

#define AXCL_ERR_MEMORY_NULL_POINTER        AXCL_DEF_MEMORY_ERR(AXCL_ERR_NULL_POINTER)
#define AXCL_ERR_MEMORY_ENCODE              AXCL_DEF_MEMORY_ERR(AXCL_ERR_ENCODE)
#define AXCL_ERR_MEMORY_DECODE              AXCL_DEF_MEMORY_ERR(AXCL_ERR_DECODE)
#define AXCL_ERR_MEMORY_UNEXPECT_RESPONSE   AXCL_DEF_MEMORY_ERR(AXCL_ERR_UNEXPECT_RESPONSE)
#define AXCL_ERR_MEMORY_EXECUTE_FAIL        AXCL_DEF_MEMORY_ERR(AXCL_ERR_EXECUTE_FAIL)

#define AXCL_ERR_MEMORY_NOT_DEVICE_MEMORY   AXCL_DEF_MEMORY_ERR(0x81)

/**
 * @ingroup AxeraCL Runtime
 * @brief Allocate device memory
 *
 * @param[out] devPtr        Pointer to allocated device memory
 * @param[in]  size          Size of memory to allocate
 * @param[in]  policy        Memory allocation policy
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMalloc(void **devPtr, size_t size, axclrtMemMallocPolicy policy);

/**
 * @ingroup AxeraCL Runtime
 * @brief Allocate cached device memory
 *
 * @param[out] devPtr        Pointer to allocated cached device memory
 * @param[in]  size          Size of memory to allocate
 * @param[in]  policy        Memory allocation policy
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMallocCached(void **devPtr, size_t size, axclrtMemMallocPolicy policy);

/**
 * @ingroup AxeraCL Runtime
 * @brief Free device memory
 *
 * @param[in]  devPtr        Pointer to device memory to free
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtFree(void *devPtr);

/**
 * @ingroup AxeraCL Runtime
 * @brief Flush device memory
 *
 * @param[in]  devPtr        Pointer to device memory to flush
 * @param[in]  size          Size of memory to flush
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemFlush(void *devPtr, size_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Invalidate device memory
 *
 * @param[in]  devPtr        Pointer to device memory to invalidate
 * @param[in]  size          Size of memory to invalidate
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemInvalidate(void *devPtr, size_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Allocate host memory
 *
 * @param[out] hostPtr       Pointer to allocated host memory
 * @param[in]  size          Size of memory to allocate
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMallocHost(void **hostPtr, size_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Free host memory
 *
 * @param[in]  hostPtr       Pointer to host memory to free
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtFreeHost(void *hostPtr);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set device memory
 *
 * @param[in]  devPtr        Pointer to device memory
 * @param[in]  value         Value to set
 * @param[in]  count         Number of bytes to set
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemset(void *devPtr, uint8_t value, size_t count);

/**
 * @ingroup AxeraCL Runtime
 * @brief Copy memory
 *
 * @param[in]  dstPtr        Destination pointer
 * @param[in]  srcPtr        Source pointer
 * @param[in]  count         Number of bytes to copy
 * @param[in]  kind          Memory copy kind
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemcpy(void *dstPtr, const void *srcPtr, size_t count, axclrtMemcpyKind kind);

/**
 * @ingroup AxeraCL Runtime
 * @brief Compare device memories
 *
 * @param[in]  devPtr1       First device memory pointer
 * @param[in]  devPtr2       Second device memory pointer
 * @param[in]  count         Number of bytes to compare
 *
 * @par Restriction
 * Only available for device memory.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemcmp(const void *devPtr1, const void *devPtr2, size_t count);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set device memory async
 *
 * @param[in]  devPtr        Pointer to device memory
 * @param[in]  value         Value to set
 * @param[in]  count         Number of bytes to set
 * @param[in]  stream        stream
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemsetAsync(void *devPtr, uint8_t value, size_t count, axclrtStream stream);

/**
 * @ingroup AxeraCL Runtime
 * @brief Copy memory async
 *
 * @param[in]  dstPtr        Destination pointer
 * @param[in]  srcPtr        Source pointer
 * @param[in]  count         Number of bytes to copy
 * @param[in]  kind          Memory copy kind
 * @param[in]  stream        stream
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemcpyAsync(void *dstPtr, const void *srcPtr, size_t count, axclrtMemcpyKind kind, axclrtStream stream);

/**
 * @ingroup AxeraCL Runtime
 * @brief Compare device memories async
 *
 * @param[in]  devPtr1       First device memory pointer
 * @param[in]  devPtr2       Second device memory pointer
 * @param[in]  count         Number of bytes to compare
 * @param[in]  stream        stream
 *
 * @par Restriction
 * Only available for device memory.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtMemcmpAsync(const void *devPtr1, const void *devPtr2, size_t count, axclrtStream stream);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_MEMORY_H__ */
