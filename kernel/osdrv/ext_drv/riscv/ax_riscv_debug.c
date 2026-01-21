/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "osal_ax.h"
#include "osal_dev_ax.h"
#include "osal_logdebug_ax.h"
#include "ax_riscv_drv.h"
#include "ax_riscv_debug.h"
#include "ax_riscv_log.h"
#include "ax_riscv_utils.h"

#define AX_MBOX_CMD_FLUSH_CACHE 0x0e

#define RISCV_PROC_DIR              "ax_proc/riscv"
#define RISCV_PROC_NODE_LOG_MEM     "log_mem"
#define RISCV_PROC_NODE_DUMP        "riscv_dump"

#define RISCV_FW_DUMP_PATH        "/opt/bin/riscv/riscv_dump.bin"

#define LOG_MAGIC           0x55aa55aa
#define LOG_VERSION_LEN     8

typedef struct {
    uint32_t magic;
    char version[LOG_VERSION_LEN];
    uint32_t mem_len;
    uint32_t header_addr;
    uint32_t header_len;
    uint32_t log_addr;
    uint32_t log_total_len;
    uint32_t log_mem_write;
    uint64_t log_mem_cnt;
} log_header_t;

static AX_ATOMIC_T riscv_atomic_debug_enable;
static AX_PROC_DIR_ENTRY_T *riscv_dir;
static AX_PROC_DIR_ENTRY_T *riscv_node_log_mem;
static AX_PROC_DIR_ENTRY_T *riscv_node_dump;

static void riscv_print_log(struct AX_PROC_DIR_ENTRY *entry, char *base, uint32_t offset, uint32_t len)
{
    int i;
    char *log_start = base + offset;
    for (i = 0; i < (len - offset); i++) {
        AX_OSAL_DBG_seq_printf(entry, "%c", *(log_start + i));
    }
    for (i = 0; i < offset; i++) {
        AX_OSAL_DBG_seq_printf(entry, "%c", *(base + i));
    }
}

static int riscv_log_mem_open(struct AX_PROC_DIR_ENTRY *entry)
{
    return 0;
}

static int riscv_log_mem_read(struct AX_PROC_DIR_ENTRY *entry)
{
    int ret;
    uint64_t addr, size;
    void *log_mem_base;
    log_header_t *log_header;
    char *log_base;
    uint32_t len, offset;

    if (ax_riscv_debug_get_state() != AX_TRUE) {
        AX_OSAL_DBG_seq_printf(entry, "riscv has not run\n");
        return -1;
    }

    ret = ax_riscv_utils_get_dts_reg(RISCV_DTS_NODE_LOG_MEM, &addr, &size);
    if (ret != 0) {
        AX_OSAL_DBG_seq_printf(entry, "get log memory info fail\n");
        return -1;
    }

    log_mem_base = AX_OSAL_DEV_ioremap_nocache(addr, size);
    if (log_mem_base == AX_NULL) {
        AX_OSAL_DBG_seq_printf(entry, "remap riscv log memory 0x%llx size %llu fail", addr, size);
        return -1;
    }
    log_header = (log_header_t *)log_mem_base;
    log_base = (char *)log_mem_base + sizeof(log_header_t);
    if (log_header->magic != LOG_MAGIC) {
        AX_OSAL_DBG_seq_printf(entry, "riscv log header magic error, 0x%x\n", log_header->magic);
        AX_OSAL_DEV_iounmap(log_mem_base);
        return -1;
    }
    AX_OSAL_DBG_seq_printf(entry, "riscv log dump version %s\n", log_header->version);
    if (log_header->log_mem_cnt <= log_header->log_total_len) {
        offset = 0;
        len = (uint32_t)log_header->log_mem_cnt;
    } else {
        offset = log_header->log_mem_cnt % log_header->log_total_len;
        len = log_header->log_total_len;
    }
    AX_OSAL_DBG_seq_printf(entry, "riscv log addr 0x%llx size 0x%llx offset %u len %u cnt %llu\n",
            addr, size, offset, len, log_header->log_mem_cnt);
    riscv_print_log(entry, log_base, offset, len);
    AX_OSAL_DEV_iounmap(log_mem_base);

    return 0;
}

static int riscv_dump_open(struct AX_PROC_DIR_ENTRY *entry)
{
    return 0;
}

static int riscv_dump_read(struct AX_PROC_DIR_ENTRY *entry)
{
    int ret;
    uint64_t addr, size;
    mbox_msg_t msg_clean_cache = {
        .id = AX_MBOX_CMD_FLUSH_CACHE
    };

    if (ax_riscv_debug_get_state() != AX_TRUE) {
        AX_OSAL_DBG_seq_printf(entry, "riscv has not run\n");
        return -1;
    }

    ret = ax_riscv_send_message(&msg_clean_cache);
    if (ret != 0) {
        AX_OSAL_DBG_seq_printf(entry, "send flush riscv cache mailbox message fail\n");
        return -1;
    }
    AX_OSAL_TM_msleep(20);

    ret = ax_riscv_utils_get_dts_reg(RISCV_DTS_NODE_RISCV, &addr, &size);
    if (ret != 0) {
        AX_OSAL_DBG_seq_printf(entry, "get riscv reg info fail\n");
        return -1;
    }

    ret = ax_riscv_utils_savebin(RISCV_FW_DUMP_PATH, addr, size);
    if (ret != 0) {
        AX_OSAL_DBG_seq_printf(entry, "save riscv to %s fail\n", RISCV_FW_DUMP_PATH);
        return -1;
    }

    return 0;
}

void ax_riscv_debug_enable(void)
{
    if (AX_OSAL_SYNC_atomic_read(&riscv_atomic_debug_enable) == 0) {
        AX_OSAL_SYNC_atomic_set(&riscv_atomic_debug_enable, 1);
    }
}

AX_BOOL ax_riscv_debug_get_state(void)
{
    if (AX_OSAL_SYNC_atomic_read(&riscv_atomic_debug_enable) != 0) {
        return AX_TRUE;
    } else {
        return AX_FALSE;
    }
}

int ax_riscv_debug_init(void)
{
    int ret;

    ret = AX_OSAL_SYNC_atomic_init(&riscv_atomic_debug_enable);
    if (ret != 0) {
        RISCV_LOG_ERROR("init atomic riscv_atomic_debug_enable fail");
        return ret;
    }
    AX_OSAL_SYNC_atomic_set(&riscv_atomic_debug_enable, 0);

    riscv_dir = AX_OSAL_DBG_proc_mkdir(RISCV_PROC_DIR, NULL);

    riscv_node_log_mem = AX_OSAL_DBG_create_proc_entry(RISCV_PROC_NODE_LOG_MEM, riscv_dir);
    riscv_node_log_mem->open = riscv_log_mem_open;
    riscv_node_log_mem->read = riscv_log_mem_read;

    riscv_node_dump = AX_OSAL_DBG_create_proc_entry(RISCV_PROC_NODE_DUMP, riscv_dir);
    riscv_node_dump->open = riscv_dump_open;
    riscv_node_dump->read = riscv_dump_read;

    return 0;
}

int ax_riscv_debug_deinit(void)
{
    AX_OSAL_DBG_remove_proc_entry(RISCV_PROC_NODE_LOG_MEM, riscv_dir);
    AX_OSAL_DBG_remove_proc_entry(RISCV_PROC_NODE_DUMP, riscv_dir);
    AX_OSAL_DBG_remove_proc_entry(RISCV_PROC_DIR, NULL);

    AX_OSAL_SYNC_atomic_destroy(&riscv_atomic_debug_enable);
    return 0;
}
