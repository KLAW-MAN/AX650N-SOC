/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef MM_BUFFER_H__
#define MM_BUFFER_H__

#include "ax_base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

AX_S32 MM_BufferInit(AX_U8 **MM_VirtualAddr, AX_U64 *MM_PhyBaseAddr, AX_U64 Size);

AX_VOID MM_BufferDeInit(AX_U64 PhyAddr, AX_U8* MM_VirtualAddr);

#ifdef __cplusplus
}
#endif
#endif