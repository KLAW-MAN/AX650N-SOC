/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/io.h>
#include "osal_ax.h"
#include "osal_dev_ax.h"
#include "osal_logdebug_ax.h"
#include "ax_riscv_log.h"

#define INTERRUPT_UMASK_GROUP_0    (0x4200000 + 0x2fc)
#define INTERRUPT_UMASK_GROUP_1    (0x4200000 + 0x308)
#define INTERRUPT_UMASK_GROUP_2    (0x4200000 + 0x314)
#define INTERRUPT_UMASK_GROUP_3    (0x4200000 + 0x320)
#define INTERRUPT_VECTOR_MAX    112

static uint32_t interrupt_umask_addr[] = {
    INTERRUPT_UMASK_GROUP_0,
    INTERRUPT_UMASK_GROUP_1,
    INTERRUPT_UMASK_GROUP_2,
    INTERRUPT_UMASK_GROUP_3
};

int ax_riscv_utils_loadbin(const char *path, unsigned long paddr)
{
    int ret, size;
    void *riscv_fw_base;
    void *fp;

    if (path == AX_NULL) {
        RISCV_LOG_ERROR("params error");
        return -1;
    }

    fp = AX_OSAL_FS_filp_open(path, AX_OSAL_O_RDONLY, 0644);
    if (fp == AX_NULL) {
        AX_OSAL_DEV_iounmap(riscv_fw_base);
        RISCV_LOG_ERROR("open %s fail", path);
        return -1;
    }
    size = AX_OSAL_FS_FILP_llseek(SEEK_END, 0, fp);
    AX_OSAL_FS_FILP_llseek(SEEK_SET, 0, fp);

    riscv_fw_base = AX_OSAL_DEV_ioremap_nocache(paddr, size);
    if (riscv_fw_base == AX_NULL) {
        RISCV_LOG_ERROR("remap 0x%lx fail", paddr);
        return -1;
    }

    ret = AX_OSAL_FS_filp_read(riscv_fw_base, size, fp);
    if (ret != size) {
        AX_OSAL_DEV_iounmap(riscv_fw_base);
        AX_OSAL_FS_filp_close(fp);
        RISCV_LOG_ERROR("read fail, size %u ret %u", size, ret);
        return -1;
    }

    AX_OSAL_DEV_iounmap(riscv_fw_base);
    AX_OSAL_FS_filp_close(fp);
    return 0;
}

int ax_riscv_utils_savebin(const char *path, unsigned long paddr, unsigned long size)
{
    int ret;
    void *vaddr_base;
    void *fp;

    if (path == AX_NULL) {
        RISCV_LOG_ERROR("params error");
        return -1;
    }

    vaddr_base = AX_OSAL_DEV_ioremap_nocache(paddr, size);
    if (vaddr_base == AX_NULL) {
        RISCV_LOG_ERROR("remap paddr 0x%lx size %lu fail", paddr, size);
        return -1;
    }

    fp = AX_OSAL_FS_filp_open(path, AX_OSAL_O_WRONLY | AX_OSAL_O_BINARY | AX_OSAL_O_CREAT, 0644);
    if (fp == AX_NULL) {
        AX_OSAL_DEV_iounmap(vaddr_base);
        RISCV_LOG_ERROR("open %s fail", path);
        return -1;
    }
    ret = AX_OSAL_FS_filp_write(vaddr_base, size, fp);
    if (ret != size) {
        AX_OSAL_DEV_iounmap(vaddr_base);
        AX_OSAL_FS_filp_close(fp);
        RISCV_LOG_ERROR("write fail, size %lu ret %u", size, ret);
        return -1;
    }

    AX_OSAL_FS_filp_close(fp);
    AX_OSAL_DEV_iounmap(vaddr_base);

    return 0;
}

int ax_riscv_utils_get_dts_reg(const char *dts_path, uint64_t *addr, uint64_t *size)
{
    struct device_node *dts_node;

    if (dts_path == AX_NULL || addr == AX_NULL || size == AX_NULL) {
        RISCV_LOG_ERROR("params error");
        return -1;
    }

    dts_node = of_find_node_by_path(dts_path);
    if (!dts_node) {
        RISCV_LOG_ERROR("find %s node error", dts_path);
        return -1;
    }
    if (of_property_read_u64(dts_node, "reg", addr) ||
        of_property_read_u64_index(dts_node, "reg", 1, size)) {
        RISCV_LOG_ERROR("Failed to read property %s: reg", dts_path);
        return -1;
    }

    return 0;
}

int ax_riscv_utils_interrupt_umask(int vector)
{
    int mask_index;
    int mask_shift;
    uint32_t umask_paddr;
    uint32_t *umask_vaddr;

    if (vector < 0 || vector >= INTERRUPT_VECTOR_MAX) {
        RISCV_LOG_ERROR("vector %d out of range", vector);
        return -1;
    }

    mask_index = vector / 32;
    mask_shift = vector % 32;
    umask_paddr = interrupt_umask_addr[mask_index];

    umask_vaddr = AX_OSAL_DEV_ioremap(umask_paddr, 0x100);
    if (umask_vaddr == AX_NULL) {
        RISCV_LOG_ERROR("remap 0x%x fail", umask_paddr);
        return -1;
    }
    *(umask_vaddr) |= (1 << mask_shift);
    AX_OSAL_DEV_iounmap(umask_vaddr);

    return 0;
}
