/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "dma_buffer.hpp"
#include "os.hpp"
#include <string.h>
#include <utility>
#include "ax_base_type.h"
#include "axcl_logger.hpp"

// clang-format off
#if defined(WINDOWS)
    #define AX_MM_DEV "ax_mmb_dev"
#else
    #define AX_MM_DEV "/dev/ax_mmb_dev"
#endif
#define TAG "pcie"
#define MAX_SG_LIST_ENTRY 64


#if defined(WINDOWS)
#define AX_IOC_PCIe_BASE 0x50
#else
#define AX_IOC_PCIe_BASE 'H'
#endif

#define AX_IOC_PCIe_ALLOC_MEMORY        AX_IOWR(AX_IOC_PCIe_BASE, 4, struct ax_mem_info)
#define AX_IOC_PCIe_ALLOC_MEMCACHED     AX_IOWR(AX_IOC_PCIe_BASE, 5, struct ax_mem_info)
#define AX_IOC_PCIe_FLUSH_CACHED        AX_IOW( AX_IOC_PCIe_BASE, 6, struct ax_mem_info)
#define AX_IOC_PCIe_INVALID_CACHED      AX_IOW( AX_IOC_PCIe_BASE, 7, struct ax_mem_info)
#define AX_IOC_PCIe_SCATTERLIST_ALLOC   AX_IOW( AX_IOC_PCIe_BASE, 8, struct ax_sg_list)
// clang-format on

struct ax_mem_entry {
    uint32_t size;
    uint64_t phyaddr;
    uint64_t viraddr;
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

static void print_scatterlist(const struct ax_sg_list &sg) {
    LOG_MM_C(TAG, "pid: {} device: {}: size {:#x} num {}", getpid(), sg.id, sg.size, sg.num);
    for (uint32_t i = 0; i < sg.num; ++i) {
        LOG_MM_C(TAG, "  [{}]: phy {:#x}, size {:#x}", i, sg.mems[i].phyaddr, sg.mems[i].size);
    }
}

dma_buffer::dma_buffer(uint32_t device_id) : m_fd(XP_INVALID_DEV), m_device_id(device_id) {
}

dma_buffer::~dma_buffer() {
    free();
}

dma_buffer::dma_buffer(dma_buffer &&rhs) noexcept {
    m_fd = std::exchange(rhs.m_fd, XP_INVALID_DEV);
    m_device_id = std::exchange(rhs.m_device_id, 0);
    m_mem = std::exchange(rhs.m_mem, dma_mem{});
}

dma_buffer &dma_buffer::operator=(dma_buffer &&rhs) noexcept {
    m_fd = std::exchange(rhs.m_fd, XP_INVALID_DEV);
    m_device_id = std::exchange(rhs.m_device_id, 0);
    m_mem = std::exchange(rhs.m_mem, dma_mem{});

    return *this;
}

bool dma_buffer::alloc(size_t size, bool cached /* = false */, bool scattered /* = false */) {
    if (m_fd != XP_INVALID_DEV) {
        LOG_MM_E(TAG, "{} is already opened", AX_MM_DEV);
        return false;
    }

    xp_dev_t fd = xp_io_open(AX_MM_DEV);
    if (fd == XP_INVALID_DEV) {
        LOG_MM_E(TAG, "open {} fail, error: {}", AX_MM_DEV, xp_io_error());
        return false;
    }

#if defined(WINDOWS)
    // if (cached || scattered) {
    //     LOG_MM_W(TAG, "Windows platform does not support cached({}) or scattered({}) memory allocation, using default allocation", cached, scattered);
    //     cached = false;
    //     scattered = false;
    // }
#else
    #ifndef AXCL_CMA_CACHED
        cached = false;
    #endif
#endif

    int ret;

    if (scattered) {
        /* drv use kmalloc to alloc sg memory which means always cached */
        cached = true;

        struct ax_sg_list sg = {};
        sg.id = m_device_id;
        sg.size = static_cast<uint32_t>(size);
        if (ret = xp_io_ioctl(fd, AX_IOC_PCIe_SCATTERLIST_ALLOC, &sg, sizeof(sg), &sg, sizeof(sg)); ret < 0) {
            LOG_MM_E(TAG, "allocate sg (device: {}, size: {}) fail, error: {}", size, m_device_id, xp_io_error());
            xp_io_close(fd);
            return false;
        }

        if (0 == sg.num || sg.num > MAX_SG_LIST_ENTRY) {
            LOG_MM_E(TAG, "invalid allocated sg num: {} of device {}", sg.num, m_device_id);
            xp_io_close(fd);
            return false;
        }

        size_t total_size = 0;
        for (uint32_t i = 0; i < sg.num; ++i) {
            if (0 == sg.mems[i].phyaddr || 0 == sg.mems[i].size) {
                LOG_MM_E(TAG, "invalid allocated memory[{}]: phy {:#x} size: {:#x} of device {}", i, sg.mems[i].phyaddr, sg.mems[i].size, m_device_id);
                print_scatterlist(sg);
                xp_io_close(fd);
                return false;
            }

            total_size += sg.mems[i].size;
        }

        if (total_size < size) {
            LOG_MM_E(TAG, "invalid allocated size: expected {}, but {} of device {}", total_size, size, m_device_id);
            print_scatterlist(sg);
            xp_io_close(fd);
            return false;
        }

        m_mem.blk_cnt = sg.num;

        void *vir = xp_mmap_dev(fd, size);
        if (vir == nullptr) {
            LOG_MM_E(TAG, "mmap sg size {} of device {} fail, error: {}", size, m_device_id, xp_io_error());
            print_scatterlist(sg);
            xp_io_close(fd);
            return false;
        }

        /* make sure pages have been allocated, and flush is mandatory */
        ::memset(vir, 0xCC, size);

        m_fd = fd;
        if (!flush(sg.mems[0].phyaddr, vir, static_cast<uint32_t>(size))) {
            print_scatterlist(sg);
            xp_io_close(fd);
            m_fd = XP_INVALID_DEV;
            return false;
        }

        uint8_t *base = reinterpret_cast<uint8_t *>(vir);
        for (uint32_t i = 0; i < sg.num; ++i) {
            struct dma_mem_block &blk = m_mem.blks[i];
            blk.phy = sg.mems[i].phyaddr;
            blk.vir = reinterpret_cast<void *>(base);
            blk.size = sg.mems[i].size;
            base += blk.size;
        }
    } else {
        struct ax_mem_info info = {};
        info.id = m_device_id;
        info.mem.size = static_cast<uint32_t>(size);
        if (cached) {
            if (ret = xp_io_ioctl(fd, AX_IOC_PCIe_ALLOC_MEMCACHED, &info, sizeof(info), &info, sizeof(info)); ret < 0) {
                LOG_MM_E(TAG, "allocate size {} cached memory of device {} fail, error: {}", info.mem.size, m_device_id, xp_io_error());
                xp_io_close(fd);
                return false;
            }
        } else {
            if (ret = xp_io_ioctl(fd, AX_IOC_PCIe_ALLOC_MEMORY, &info, sizeof(info), &info, sizeof(info)); ret < 0) {
                LOG_MM_E(TAG, "allocate size {} non-cached memory of device {} fail, error: {}", info.mem.size, m_device_id, xp_io_error());
                xp_io_close(fd);
                return false;
            }
        }

        if (0 == info.mem.phyaddr) {
            LOG_MM_E(TAG, "phy addr of allocated size {} from device {} is 0", info.mem.size, m_device_id);
            xp_io_close(fd);
            return false;
        }

        void *vir = xp_mmap_dev(fd, info.mem.size);
        if (vir == nullptr) {
            LOG_MM_E(TAG, "mmap size {} of device {} fail, error: {}", info.mem.size, m_device_id, xp_io_error());
            xp_io_close(fd);
            return false;
        }

        if (cached) {
            /* make sure pages have been allocated, and flush is mandatory */
            ::memset(vir, 0xCC, size);

            m_fd = fd;
            if (!flush(info.mem.phyaddr, vir, static_cast<uint32_t>(size))) {
                xp_io_close(fd);
                m_fd = XP_INVALID_DEV;
                return false;
            }
        }

        m_mem.blk_cnt = 1;
        m_mem.blks[0].phy = info.mem.phyaddr;
        m_mem.blks[0].vir = vir;
        m_mem.blks[0].size = static_cast<uint32_t>(size);
    }

    m_fd = fd;
    m_mem.scattered = scattered;
    m_mem.cached = cached;
    m_mem.total_size = static_cast<uint32_t>(size);
    if (cached) {
        m_mem.ops.flush = std::bind(&dma_buffer::flush, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        m_mem.ops.invalidate =
            std::bind(&dma_buffer::invalidate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    return true;
}

void dma_buffer::free() {
    if (m_fd != XP_INVALID_DEV) {
        if (m_mem.blks[0].vir) {
            xp_munmap_dev(m_fd, m_mem.blks[0].vir, m_mem.total_size);
        }

        xp_io_close(m_fd);
        m_fd = XP_INVALID_DEV;
    }

    m_mem.blk_cnt = 0;
    m_mem.total_size = 0;
}

bool dma_buffer::flush(uint64_t phy, void *vir, uint32_t size) {
    struct ax_mem_info info;
    info.id = m_device_id;
    info.mem.size = size;
    info.mem.phyaddr = phy;
    info.mem.viraddr = reinterpret_cast<uint64_t>(vir);

    if (int ret = xp_io_ioctl(m_fd, AX_IOC_PCIe_FLUSH_CACHED, &info, sizeof(info), &info, sizeof(info)); ret < 0) {
        LOG_MM_E(TAG, "flush memory phy {} vir {} size {} of device {} fail, error: {}", phy, vir, size, m_device_id, xp_io_error());
        return false;
    }

    return true;
}

bool dma_buffer::invalidate(uint64_t phy, void *vir, uint32_t size) {
    struct ax_mem_info info;
    info.id = m_device_id;
    info.mem.size = size;
    info.mem.phyaddr = phy;
    info.mem.viraddr = reinterpret_cast<uint64_t>(vir);

    if (int ret = xp_io_ioctl(m_fd, AX_IOC_PCIe_INVALID_CACHED, &info, sizeof(info), &info, sizeof(info)); ret < 0) {
        LOG_MM_E(TAG, "invalidate memory phy {} vir {} size {} of device {} fail, error: {}", phy, vir, size, m_device_id, xp_io_error());
        return false;
    }

    return true;
}

unsigned long dma_buffer::get_cma_free_size() {
#if defined(WINDOWS)
    LOG_MM_W(TAG, "CMA free size query is not supported on Windows platform");
    return 0;
#else
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) {
        LOG_MM_E(TAG, "open /proc/meminfo fail, error: {}", xp_io_error());
        return 0;
    }

    unsigned long cma_free = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "CmaFree: %lu kB", &cma_free) == 1) {
            break;
        }
    }

    fclose(fp);
    return cma_free;
#endif
}