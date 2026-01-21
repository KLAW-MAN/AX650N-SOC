/**
 * @file axcl_rt_engine.h
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
**/

#ifndef __AXCL_RT_ENGINE_H__
#define __AXCL_RT_ENGINE_H__

#include "axcl_rt_engine_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup AxeraCL Runtime
 * @brief Initialize the runtime engine
 *
 * @par Function
 * Before using the runtime engine, the user needs to call this
 * function
 *
 * @par Restriction
 * User needs to call axclrtEngineFinalize to finalize the runtime
 * engine after using it
 *
 * @param[in]  npuKind       Initialize the runtime engine with
 * the specified VNPU type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineInit(axclrtEngineVNpuKind npuKind);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get visual NPU kind
 *
 * @par Function
 * Get the initialized VNPU type of the runtime engine
 *
 * @param[out] npuKind       VNPU type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetVNpuKind(axclrtEngineVNpuKind *npuKind);

/**
 * @ingroup AxeraCL Runtime
 * @brief Finalize the runtime engine
 *
 * @par Function
 * After using the runtime engine, the user needs to call this
 * function
 *
 * @par Restriction
 * User needs to call axclrtEngineInit to initialize the runtime
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineFinalize();

/**
 * @ingroup AxeraCL Runtime
 * @brief Load offline model data from files
 * and manage memory internally by the system
 *
 * @par Function
 * After the system finishes loading the model, the returned model ID
 * is used to identify the model during subsequent operations
 *
 * @param[in]  modelPath     Storage path for offline model files
 * @param[out] modelId       Model ID generated after
 *        the system finishes loading the model
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineLoadFromFile(const char *modelPath, uint64_t *modelId);

/**
 * @ingroup AxeraCL Runtime
 * @brief Load offline model data from memory and manage the memory of
 * model running internally by the system
 *
 * @par Function
 * After the system finishes loading the model, the returned model ID
 * is used to identify the model during subsequent operations
 *
 * @par Restriction
 * The model memory is device memory, and requires user allocation
 * and release
 *
 * @param[in]  model         Model data stored in memory
 * @param[in]  modelSize     Model data size
 * @param[out] modelId       Model ID generated after
 *        the system finishes loading the model
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineLoadFromMem(const void *model, uint64_t modelSize, uint64_t *modelId);

/**
 * @ingroup AxeraCL Runtime
 * @brief unload model with model id
 *
 * @param[in]  modelId       Model id to be unloaded
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineUnload(uint64_t modelId);

/**
 * @ingroup AxeraCL Runtime
 * @brief get model build toolchain version
 *
 * @param[in]  modelId       Model id
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT const char* axclrtEngineGetModelCompilerVersion(uint64_t modelId);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set model affinity
 *
 * @par Function
 * Set npu affinity for context
 *
 * @par Restriction
 * Zero is not allowed, and the masked bit of the set
 * cannot be out of the affinity range.
 *
 * @param[in]  modelId       Model id
 * @param[in]  set           The affinity set
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineCreateContext | axclrtEngineGetAffinity
**/
AXCL_EXPORT axclError axclrtEngineSetAffinity(uint64_t modelId, axclrtEngineSet set);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get model affinity
 *
 * @param[in]  modelId       Model id
 * @param[out] set           The affinity set
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineCreateContext | axclrtEngineSetAffinity
**/
AXCL_EXPORT axclError axclrtEngineGetAffinity(uint64_t modelId, axclrtEngineSet *set);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set context affinity, not supported yet.
 *
 * @param[in]  modelId       Model id
 * @param[in]  contextId     Context id
 * @param[in]  set           The affinity set
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineCreateContext | axclrtEngineSetContextAffinity
**/
AXCL_EXPORT axclError axclrtEngineSetContextAffinity(uint64_t modelId, uint64_t contextId, axclrtEngineSet set);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get context affinity, not supported yet.
 *
 * @param[in]  modelId       Model id
 * @param[in]  contextId     Context id
 * @param[out] set           The affinity set
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineCreateContext | axclrtEngineSetContextAffinity
**/
AXCL_EXPORT axclError axclrtEngineGetContextAffinity(uint64_t modelId, uint64_t contextId, axclrtEngineSet *set);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get memory usage
 *
 * @par Function
 * Get the system memory size and CMM memory size required for model execution
 * based on the model file
 *
 * @param[in]  modelPath     Model path to get memory information
 * @param[out] sysSize       The amount of working system memory for model executed
 * @param[out] cmmSize       The amount of cmm memory for model executed
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetUsage(const char *modelPath, int64_t *sysSize, int64_t *cmmSize);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get memory usage
 *
 * @par Function
 * Get the system memory size and CMM memory size required for model execution
 * based on the model data in memory
 *
 * @par Restriction
 * The model memory is Device memory,
 * and requires user application and release.
 *
 * @param[in]  model         Model memory which user manages
 * @param[in]  modelSize     Model data size
 * @param[out] sysSize       The amount of working system memory for model executed
 * @param[out] cmmSize       The amount of cmm memory for model executed
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetUsageFromMem(const void *model, uint64_t modelSize, int64_t *sysSize, int64_t *cmmSize);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get memory usage
 *
 * @par Function
 * Get the system memory size and CMM memory size
 * required for model execution according the model id
 *
 * @param[in]  modelId       Model id
 * @param[out] sysSize       The amount of working system memory for model executed
 * @param[out] cmmSize       The amount of cmm memory for model executed
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetUsageFromModelId(uint64_t modelId, int64_t *sysSize, int64_t *cmmSize);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get model type
 *
 * @par Function
 * Get the type of model according to the model file
 *
 * @param[in]  modelPath     Model path to get model type
 * @param[out] modelType     Model type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetModelType(const char *modelPath, axclrtEngineModelKind *modelType);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get model type
 *
 * @par Function
 * Get the type of model based on the model data in memory
 *
 * @par Restriction
 * The model memory is Device memory,
 * and requires user application and release.
 *
 * @param[in]  model         Model memory which user manages
 * @param[in]  modelSize     Model data size
 * @param[out] modelType     Model type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetModelTypeFromMem(const void *model, uint64_t modelSize, axclrtEngineModelKind *modelType);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get model type
 *
 * @par Function
 * Get the type of model according to the model id
 *
 * @param[in]  modelId       Model id
 * @param[out] modelType     Model type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetModelTypeFromModelId(uint64_t modelId, axclrtEngineModelKind *modelType);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get io information
 *
 * @par Function
 * Get io information of the model according to the model ID
 *
 * @param[in]  modelId       Model id
 * @param[out] ioInfo        axclrtEngineIOInfo pointer
 *
 * @par Restriction
 * Users should call axclrtEngineDestroyIOInfo to release the axclrtEngineIOInfo after using it.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineDestroyIOInfo | axclrtEngineGetIOInfoByIndex
**/
AXCL_EXPORT axclError axclrtEngineGetIOInfo(uint64_t modelId, axclrtEngineIOInfo *ioInfo);

/**
 * @ingroup AxeraCL Runtime
 * @brief destroy data of type axclrtEngineIOInfo
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineDestroyIOInfo(axclrtEngineIOInfo ioInfo);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get shape groups count
 *
 * @par Function
 * Get the number of the io shape groups
 *
 * @par Restriction
 * Pulsar2 toolchain can specify several shapes in model conversion
 * a time. There is only one shape in a normal model, and so it's no
 * needs to call this function for normally converted model.
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[out] count         Shape groups count
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex
**/
AXCL_EXPORT axclError axclrtEngineGetShapeGroupsCount(axclrtEngineIOInfo ioInfo, int32_t *count);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get the number of the inputs of
 *        the model according to data of axclrtEngineIOInfo
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 *
 * @retval input size with axclrtEngineIOInfo
**/
AXCL_EXPORT uint32_t axclrtEngineGetNumInputs(axclrtEngineIOInfo ioInfo);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get the number of the output of
 *        the model according to data of axclrtEngineIOInfo
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 *
 * @retval output size with axclrtEngineIOInfo
**/
AXCL_EXPORT uint32_t axclrtEngineGetNumOutputs(axclrtEngineIOInfo ioInfo);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get the size of the specified input according to
 *        the data of type axclrtEngineIOInfo
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  group         input shape group index
 * @param[in]  index         the size of the number of inputs to be obtained,
 *         the index value starts from 0
 *
 * @retval Specify the size of the input
**/
AXCL_EXPORT uint64_t axclrtEngineGetInputSizeByIndex(axclrtEngineIOInfo ioInfo, uint32_t group, uint32_t index);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get the size of the specified output according to
 *        the data of type axclrtEngineIOInfo
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  group         output shape group index
 * @param[in]  index         the size of the number of outputs to be obtained,
 *        the index value starts from 0
 *
 * @retval Specify the size of the output
**/
AXCL_EXPORT uint64_t axclrtEngineGetOutputSizeByIndex(axclrtEngineIOInfo ioInfo, uint32_t group, uint32_t index);

/**
 * @ingroup AxeraCL Runtime
 * @brief get input name
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  index         input io index
 *
 * @retval input tensor name,the same life cycle with ioInfo
 * @retval NULL if not found
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumInputs
**/
AXCL_EXPORT const char *axclrtEngineGetInputNameByIndex(axclrtEngineIOInfo ioInfo, uint32_t index);

/**
 * @ingroup AxeraCL Runtime
 * @brief get output name
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  index         output io index
 *
 * @retval output tensor name,the same life cycle with ioInfo
 * @retval NULL if not found
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumOutputs
**/
AXCL_EXPORT const char *axclrtEngineGetOutputNameByIndex(axclrtEngineIOInfo ioInfo, uint32_t index);

/**
 * @ingroup AxeraCL Runtime
 * @brief get input tensor index by name
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  name          input tensor name
 *
 * @retval input tensor index
 * @retval -1 if not found
**/
AXCL_EXPORT int32_t axclrtEngineGetInputIndexByName(axclrtEngineIOInfo ioInfo, const char *name);

/**
 * @ingroup AxeraCL Runtime
 * @brief get output tensor index by name
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  name          output tensor name
 *
 * @retval output tensor index
 * @retval -1 if not found
**/
AXCL_EXPORT int32_t axclrtEngineGetOutputIndexByName(axclrtEngineIOInfo ioInfo, const char *name);

/**
 * @ingroup AxeraCL Runtime
 * @brief get input dims info
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  group         input shape group index
 * @param[in]  index         input tensor index
 * @param[out] dims          dims info
 *
 * @par Restriction
 * Users should release the axclrtEngineIODims after using it.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumInputs
**/
AXCL_EXPORT axclError axclrtEngineGetInputDims(axclrtEngineIOInfo ioInfo, uint32_t group, uint32_t index, axclrtEngineIODims *dims);

/**
 * @ingroup AxeraCL Runtime
 * @brief get input data type
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  index         input io index
 * @param[out] type          input IO data type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumInputs
**/
AXCL_EXPORT axclError axclrtEngineGetInputDataType(axclrtEngineIOInfo ioInfo, uint32_t index, axclrtEngineDataType *type);

/**
 * @ingroup AxeraCL Runtime
 * @brief get output data type
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  index         output io index
 * @param[out] type          output IO data type
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumOutputs
**/
AXCL_EXPORT axclError axclrtEngineGetOutputDataType(axclrtEngineIOInfo ioInfo, uint32_t index, axclrtEngineDataType *type);

/**
 * @ingroup AxeraCL Runtime
 * @brief get input data layout
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  index         input io index
 * @param[out] layout        input IO data layout
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumInputs
**/
AXCL_EXPORT axclError axclrtEngineGetInputDataLayout(axclrtEngineIOInfo ioInfo, uint32_t index, axclrtEngineDataLayout *layout);

/**
 * @ingroup AxeraCL Runtime
 * @brief get output data layout
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  index         output io index
 * @param[out] layout        output IO data layout
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumOutputs
**/
AXCL_EXPORT axclError axclrtEngineGetOutputDataLayout(axclrtEngineIOInfo ioInfo, uint32_t index, axclrtEngineDataLayout *layout);

/**
 * @ingroup AxeraCL Runtime
 * @brief get output dims info
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[in]  group         output shape group index
 * @param[in]  index         output tensor index
 * @param[out] dims          dims info
 *
 * @par Restriction
 * Users should release the axclrtEngineIODims after using it.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineGetIOInfo | axclrtEngineGetIOInfoByIndex | axclrtEngineGetNumOutputs
**/
AXCL_EXPORT axclError axclrtEngineGetOutputDims(axclrtEngineIOInfo ioInfo, uint32_t group, uint32_t index, axclrtEngineIODims *dims);

/**
 * @ingroup AxeraCL Runtime
 * @brief Create data of type axclrtEngineIO
 *
 * @param[in]  ioInfo        axclrtEngineIOInfo pointer
 * @param[out] io            The created axclrtEngineIO pointer
 *
 * @par Restriction
 * Users should call axclrtEngineDestroyIO to release the axclrtEngineIO after using it.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineCreateIO(axclrtEngineIOInfo ioInfo, axclrtEngineIO *io);

/**
 * @ingroup AxeraCL Runtime
 * @brief destroy data of type axclrtEngineIO
 *
 * @param[in]  io            Pointer to axclrtEngineIO to be destroyed
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineDestroyIO(axclrtEngineIO io);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set input data buffer by io index
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be set
 * @param[in]  index         Input tensor index
 * @param[in]  dataBuffer    data buffer address to be added
 * @param[in]  size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineSetInputBufferByIndex(axclrtEngineIO io, uint32_t index, const void *dataBuffer, uint64_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set output data buffer by io index
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be set
 * @param[in]  index         Output tensor index
 * @param[in]  dataBuffer    data buffer address to be added
 * @param[in]  size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineSetOutputBufferByIndex(axclrtEngineIO io, uint32_t index, const void *dataBuffer, uint64_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set input data buffer by io name
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be set
 * @param[in]  name          Input tensor name
 * @param[in]  dataBuffer    data buffer address to be added
 * @param[in]  size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineSetInputBufferByName(axclrtEngineIO io, const char *name, const void *dataBuffer, uint64_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Set output data buffer by io name
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be set
 * @param[in]  name          Output tensor name
 * @param[in]  dataBuffer    data buffer address to be added
 * @param[in]  size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineSetOutputBufferByName(axclrtEngineIO io, const char *name, const void *dataBuffer, uint64_t size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get input data buffer by io index
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be got
 * @param[in]  index         Input tensor index
 * @param[out] dataBuffer    data buffer address
 * @param[out] size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetInputBufferByIndex(axclrtEngineIO io, uint32_t index, void **dataBuffer, uint64_t *size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get output data buffer by io index
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be got
 * @param[in]  index         Output tensor index
 * @param[out] dataBuffer    data buffer address
 * @param[out] size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetOutputBufferByIndex(axclrtEngineIO io, uint32_t index, void **dataBuffer, uint64_t *size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get input data buffer by io name
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be got
 * @param[in]  name          Input tensor name
 * @param[out] dataBuffer    data buffer address
 * @param[out] size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetInputBufferByName(axclrtEngineIO io, const char *name, void **dataBuffer, uint64_t *size);

/**
 * @ingroup AxeraCL Runtime
 * @brief Get output data buffer by io name
 *
 * @param[in]  io            axclrtEngineIO address of data buffer to be got
 * @param[in]  name          Output tensor name
 * @param[out] dataBuffer    data buffer address
 * @param[out] size          data buffer size
 *
 * @par Restriction
 * The data buffer is Device memory,
 * and requires user application and release.
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineGetOutputBufferByName(axclrtEngineIO io, const char *name, void **dataBuffer, uint64_t *size);

/**
 * @ingroup AxeraCL Runtime
 * @brief In dynamic batch scenarios,
 * it is used to set the number of images processed
 * at one time during model inference
 *
 * @param[in]  io            Model inference IOs
 * @param[in]  batchSize     Number of images processed at a time during model
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineCreateContext |
**/
AXCL_EXPORT axclError axclrtEngineSetDynamicBatchSize(axclrtEngineIO io, uint32_t batchSize);

/**
 * @ingroup AxeraCL Runtime
 * @brief Create a model context
 *
 * @par Function
 * Create a model running environment context for model id
 *
 * @par Restriction
 * One model id could create several running context, and
 * each of them running only with its own settings and
 * memory spaces.
 *
 * @param[in]  modelId       Model id
 * @param[out] contextId     The created context id
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclrtEngineLoadFromFile | axclrtEngineLoadFromMem
**/
AXCL_EXPORT axclError axclrtEngineCreateContext(uint64_t modelId, uint64_t *contextId);

/**
 * @ingroup AxeraCL Runtime
 * @brief Execute model synchronous inference until the inference result is returned
 *
 * @param[in]  modelId       Model id
 * @param[in]  contextId     Model inference context
 * @param[in]  group         Model shape group index
 * @param[in]  io            Model inference IOs
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
**/
AXCL_EXPORT axclError axclrtEngineExecute(uint64_t modelId, uint64_t contextId, uint32_t group, axclrtEngineIO io);

/**
 * @ingroup AxeraCL Runtime
 * @brief Execute model asynchronous inference
 *
 * @param[in]  modelId       Model id
 * @param[in]  contextId     Model inference context
 * @param[in]  group         Model shape group index
 * @param[in]  io            Model inference IOs
 * @param[in]  stream        stream
 *
 * @retval AXCL_SUCC The function is successfully executed.
 * @retval OtherValues Failure
 *
 * @see axclLoadFromFile | axclLoadFromMem | axclLoadFromFileWithMem |
 * axclLoadFromMemWithMem
**/
AXCL_EXPORT axclError axclrtEngineExecuteAsync(uint64_t modelId, uint64_t contextId, uint32_t group, axclrtEngineIO io, axclrtStream stream);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_ENGINE_H__ */
