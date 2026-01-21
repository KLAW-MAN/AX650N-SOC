/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef AXCL_PCIE_DMA_H__
#define AXCL_PCIE_DMA_H__

#include "ax_base_type.h"
#include "axcl_base.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct PCIE_DMA_BLOCK {
    AX_U64  u64SrcAddr;  /* source address of dma task */
    AX_U64  u64DstAddr;  /* destination address of dma task */
    AX_U32  u32BlkSize;  /* data block size of dma task */
} AX_PCIE_DMA_BLOCK_T;

typedef enum PCIE_DMA_CH_TYPE {
    DMA_READ   = 0x0,
    DMA_WRITE  = 0x1,
} AX_PCIE_DMA_CH_TYPE_E;

// clang-format off
#if defined(WINDOWS)
    #define DMA_HANDLE AX_S64
#else
    #define DMA_HANDLE AX_S32
#endif

AXCL_EXPORT DMA_HANDLE AX_PCIe_OpenDmaDev();
AXCL_EXPORT AX_S32     AX_PCIe_CloseDmaDev(DMA_HANDLE PCIeDmaHandle);
AXCL_EXPORT AX_S32     AX_PCIe_CreatDmaTask(DMA_HANDLE PCIeDmaHandle, AX_PCIE_DMA_CH_TYPE_E enDir, AX_U64 u64Src, AX_U64 u64Dst, AX_U32 u32Len, AX_U32 u32Last);
AXCL_EXPORT AX_S32     AX_PCIe_CreatDmaLlpTask(DMA_HANDLE PCIeDmaHandle, AX_PCIE_DMA_BLOCK_T stDmaBlk[], AX_U32 u32Count, AX_BOOL bIsRead);
// clang-format on

#ifdef __cplusplus
}
#endif

#endif