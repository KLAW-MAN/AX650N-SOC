/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <rtdevice.h>
#include <stdint.h>

#define LOG_MAGIC           0x55aa55aa
#define LOG_VERSION_LEN     8
#define LOG_VERSION         "1.0.0"

#ifndef LOG_MEM_DDR_START
#define LOG_MEM_DDR_START   0x40400000
#endif

#ifndef LOG_MEM_DDR_LEN
#define LOG_MEM_DDR_LEN     0x1000
#endif

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

static log_header_t *log_header;
static int log_2_mem_ready = 0;

void log_2_mem_store(uint8_t *log_buff, rt_size_t log_len)
{
    if (log_2_mem_ready == 0) {
        return;
    }

    rt_spin_lock("lock");

    if ((log_header->log_mem_cnt + log_len) <= log_header->log_total_len) {
        rt_memcpy((void *)log_header->log_mem_write, log_buff, log_len);
        log_header->log_mem_write += log_len;
    } else {
        uint32_t offset = log_header->log_mem_cnt % log_header->log_total_len;
        uint32_t left = log_header->log_total_len - offset;
        if (left >= log_len) {
            rt_memcpy((void *)log_header->log_mem_write, log_buff, log_len);
            log_header->log_mem_write += log_len;
        } else {
            rt_memcpy((void *)log_header->log_mem_write, log_buff, left);
            rt_memcpy((void *)log_header->log_addr, log_buff + left, log_len - left);
            log_header->log_mem_write = log_header->log_addr + (log_len - left);
        }
    }
    if (log_header->log_mem_write == (log_header->log_addr + log_header->log_total_len)) {
        log_header->log_mem_write = log_header->log_addr;
    }

    log_header->log_mem_cnt += log_len;
    rt_spin_unlock("lock");
}

int log_2_mem_init(void)
{
    log_header = (log_header_t *)LOG_MEM_DDR_START;

    log_header->magic = LOG_MAGIC;
    rt_strcpy(log_header->version, LOG_VERSION);
    log_header->mem_len = LOG_MEM_DDR_LEN;
    log_header->header_addr = LOG_MEM_DDR_START;
    log_header->header_len = sizeof(log_header_t);
    log_header->log_addr = log_header->header_addr + log_header->header_len;
    log_header->log_total_len = LOG_MEM_DDR_LEN - log_header->header_len;
    log_header->log_mem_write = log_header->header_addr + log_header->header_len;
    log_header->log_mem_cnt = 0;

    log_2_mem_ready = 1;

    return 0;
}
