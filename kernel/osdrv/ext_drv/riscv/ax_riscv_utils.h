/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_RISCV_UTILS_H__
#define __AX_RISCV_UTILS_H__

#define RISCV_INTERRUPT_VECTOR_MAILBOX 1

#define RISCV_PC_START               0x40000000
#define RISCV_DTS_NODE_LOG_MEM      "/reserved-memory/riscv_log_memserved"
#define RISCV_DTS_NODE_RISCV        "/reserved-memory/riscv_memserved"

int ax_riscv_utils_loadbin(const char *path, unsigned long paddr);
int ax_riscv_utils_savebin(const char *path, unsigned long paddr, unsigned long size);
int ax_riscv_utils_get_dts_reg(const char *dts_path, uint64_t *addr, uint64_t *size);
int ax_riscv_utils_interrupt_umask(int vector);

#endif //__AX_RISCV_UTILS_H__
