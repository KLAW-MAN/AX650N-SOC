/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "ax_base_type.h"

typedef AX_U64 uint64_t;
typedef AX_U32 uint32_t;

#define AX_MM_DEV "/dev/ax_mmb_dev"

#define MAX_SG_LIST_ENTRY	64

struct ax_mem_entry {
    uint32_t size;
    uint64_t phyaddr;
    uint64_t virtaddr;
};

struct ax_mem_info {
    uint32_t id;
    struct ax_mem_entry mem;
};

struct ax_sg_list {
    uint32_t id;
    uint32_t size;
    uint32_t num;
    struct ax_mem_entry mems[MAX_SG_LIST_ENTRY];
};

// ioctl cmd
#define AX_IOC_PCIe_BASE           'H'
#define AX_IOC_PCIe_ALLOC_MEMORY   _IOWR(AX_IOC_PCIe_BASE, 4, struct ax_mem_info)
#define AX_IOC_PCIe_ALLOC_MEMCACHED   _IOWR(AX_IOC_PCIe_BASE, 5, struct ax_mem_info)
#define AX_IOC_PCIe_FLUSH_CACHED	_IOW(AX_IOC_PCIe_BASE, 6, struct ax_mem_info)
#define AX_IOC_PCIe_INVALID_CACHED	_IOW(AX_IOC_PCIe_BASE, 7, struct ax_mem_info)
#define AX_IOC_PCIe_SCATTERLIST_ALLOC	_IOW(AX_IOC_PCIe_BASE, 8, struct ax_sg_list)

static int g_MmbFd[256];

AX_S32 MM_BufferInit(AX_S32 TargetId, AX_U8 **MM_VirtualAddr, AX_U64 *MM_PhyBaseAddr, AX_U64 Size)
{
    AX_S32 AX_MM_Fd;
    AX_S32  Ret = 0;
    struct ax_mem_info minfo = {0,};

    if (g_MmbFd[TargetId] > 0) {
        printf("Target %d has applied for buf \n", TargetId);
        return -1;
    }

    AX_MM_Fd = open(AX_MM_DEV, O_RDWR);
    if (AX_MM_Fd == -1) {
        printf("open %s failed!\n", AX_MM_DEV);
        return -1;
    }
    g_MmbFd[TargetId] = AX_MM_Fd;

    minfo.id = TargetId;
    minfo.mem.size = Size;
    Ret = ioctl(AX_MM_Fd, AX_IOC_PCIe_ALLOC_MEMORY, &minfo);
    if (Ret < 0) {
        printf("alloc MM buffer failed\n");
        return -1;
    }

    *MM_PhyBaseAddr = minfo.mem.phyaddr;
    printf("MM buffer physical base addr:%llx, size:%x\n", *MM_PhyBaseAddr, minfo.mem.size);

    *MM_VirtualAddr = (AX_U8 *)mmap(NULL, minfo.mem.size, PROT_READ | PROT_WRITE, MAP_SHARED, AX_MM_Fd, 0);
    if (*MM_VirtualAddr <= 0) {
        printf("mmap fail\n");
        return -1;
    }

    printf("MM buffer virtual addr:%lx\n", (long)(*MM_VirtualAddr));

    return 0;
}

AX_U8 *MM_Phy2VirtAddr(AX_U64 PhyAddr, AX_U64 BasePhyAddr, AX_U8 *BaseVirtAddr)
{
    return BaseVirtAddr + (PhyAddr - BasePhyAddr);
}

AX_S32 MM_BufferDeInit(AX_S32 TargetId, AX_U8 *MM_VirtualAddr, AX_U64 Size)
{
    if (g_MmbFd[TargetId] <= 0) {
        printf("Target %d has free \n", TargetId);
        return -1;
    }
    munmap(MM_VirtualAddr, Size);
    close(g_MmbFd[TargetId]);
    g_MmbFd[TargetId] = -1;
    return 0;
}
