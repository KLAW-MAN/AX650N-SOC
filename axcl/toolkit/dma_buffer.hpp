/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include "os.hpp"

#define MAX_DMA_MEM_BLOCK_NUM (64)

struct dma_mem_block {
    uint64_t phy = 0;
    void *vir = nullptr;
    uint32_t size = 0;
};

struct dma_mem_operation {
    std::function<bool(uint64_t, void *, uint32_t)> flush = nullptr;
    std::function<bool(uint64_t, void *, uint32_t)> invalidate = nullptr;
};

struct dma_mem {
    bool scattered = false;
    bool cached = false;
    uint32_t total_size = 0;
    uint32_t blk_cnt = 0;
    struct dma_mem_block blks[MAX_DMA_MEM_BLOCK_NUM];
    struct dma_mem_operation ops;
};

class dma_buffer {
public:
    dma_buffer(uint32_t device_id);
    ~dma_buffer();

    dma_buffer(dma_buffer &&) noexcept;
    dma_buffer &operator=(dma_buffer &&) noexcept;
    dma_buffer(const dma_buffer &) = delete;
    dma_buffer &operator=(const dma_buffer &) = delete;

    [[nodiscard]] bool alloc(size_t size, bool cached = false, bool scattered = false);
    void free();

    [[nodiscard]] bool flush(uint64_t phy, void *vir, uint32_t size);
    [[nodiscard]] bool invalidate(uint64_t phy, void *vir, uint32_t size);

    const struct dma_mem &get() const {
        return m_mem;
    }

    /* get free CMA in kB from /proc/meminfo */
    static unsigned long get_cma_free_size();

private:
    xp_dev_t m_fd;
    uint32_t m_device_id;
    struct dma_mem m_mem;
};
