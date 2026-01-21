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

#include <stdio.h>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include "axcl_fifo.h"

#define ALIGN_NALU_SIZE_UP(n, s) ((((n) + (s) - 1) / (s)) * (s))

struct nalu_data {
    uint8_t* nalu;
    uint8_t* nalu2;
    uint32_t len;
    uint32_t len2;
    uint32_t type;
    uint64_t pts;
    uint64_t userdata;
};

typedef struct nalu_data nalu_data;

class nalu_lock_fifo final {
    struct nalu_meta {
        uint32_t total_len;
        uint32_t len;
        uint64_t pts;
        uint64_t userdata;
        uint32_t type;
        uint32_t reserved;
    };

    static constexpr uint32_t ALIGN_SIZE = sizeof(nalu_meta);

public:
    explicit nalu_lock_fifo(uint32_t size) {
        const uint32_t fifo_size = ALIGN_NALU_SIZE_UP(size, ALIGN_SIZE);
        if (0 != axcl_fifo_alloc(&m_fifo, fifo_size)) {
            throw std::runtime_error("allocate fifo failure");
        }
    }

    ~nalu_lock_fifo() {
        axcl_fifo_free(&m_fifo);
    }

    uint32_t size() const {
        return axcl_fifo_len(&m_fifo);
    }

    int32_t push(const nalu_data& nalu, int32_t timeout) {
        uint32_t total_len = sizeof(nalu_meta) + nalu.len;
        total_len = ALIGN_NALU_SIZE_UP(total_len, ALIGN_SIZE);
        if (total_len > axcl_fifo_size(&m_fifo)) {
            return -EINVAL;
        }

        nalu_meta meta = {};
        meta.total_len = total_len;
        meta.len = nalu.len;
        meta.type = nalu.type;
        meta.pts = nalu.pts;
        meta.userdata = nalu.userdata;

        std::unique_lock<std::mutex> lock(m_mtx);

        axcl_fifo_element ele;
        ele.buf[0].buf = reinterpret_cast<void*>(&meta);
        ele.buf[0].len = sizeof(nalu_meta);
        ele.buf[1].buf = reinterpret_cast<void*>(nalu.nalu);
        ele.buf[1].len = nalu.len;

        if (0 == timeout) {
            if (!avail_space(total_len)) {
                return -ENOSPC;
            }
        } else if (timeout < 0) {
            m_cv_put.wait(lock, [&] { return avail_space(total_len) || m_wakeup; });
        } else {
            if (!m_cv_put.wait_for(lock, std::chrono::milliseconds(timeout), [&] { return avail_space(total_len) || m_wakeup; })) {
                return -ETIMEDOUT;
            }
        }

        if (m_wakeup) {
            m_wakeup = false;
            return -EINTR;
        }

        if (axcl_fifo_put_element(&m_fifo, &ele, total_len) != total_len) {
            return -EFAULT;
        }

        m_cv_pop.notify_one();
        return 0;
    }

    int32_t peek(nalu_data& nalu, uint32_t& total_len, int32_t timeout) {
        std::unique_lock<std::mutex> lock(m_mtx);

        if (0 == timeout) {
            if (axcl_fifo_is_empty(&m_fifo)) {
                return -ENOSPC;
            }
        } else if (timeout < 0) {
            m_cv_pop.wait(lock, [this] { return !axcl_fifo_is_empty(&m_fifo) || m_wakeup; });
        } else {
            if (!m_cv_pop.wait_for(lock, std::chrono::milliseconds(timeout), [this] { return !axcl_fifo_is_empty(&m_fifo) || m_wakeup; })) {
                return -ETIMEDOUT;
            }
        }

        if (m_wakeup) {
            m_wakeup = false;
            return -EINTR;
        }

        nalu_meta meta;
        if (sizeof(nalu_meta) != axcl_fifo_pop(&m_fifo, reinterpret_cast<void*>(&meta), sizeof(nalu_meta))) {
            return -EFAULT;
        }

        if (meta.total_len > axcl_fifo_size(&m_fifo)) {
            printf("invalid meta: total_len = %d, len = %d, fifo capacity = %d\n", meta.total_len, meta.len, axcl_fifo_size(&m_fifo));
            return -EFAULT;
        }

        axcl_fifo_element ele;
        total_len = meta.total_len - sizeof(meta);
        if (uint32_t len = axcl_fifo_peek_element(&m_fifo, &ele, total_len); len != total_len) {
            return -EFAULT;
        }

        nalu.pts = meta.pts;
        nalu.type = meta.type;
        nalu.userdata = meta.userdata;
        nalu.nalu = reinterpret_cast<uint8_t*>(ele.buf[0].buf);
        if (ele.buf[0].len >= meta.len) {
            /* buf[0] is enough to hold all nalu frame, nalu2 set nullptr  */
            nalu.len = meta.len;
            nalu.nalu2 = nullptr;
            nalu.len2 = 0;
        } else {
            /* keep whole buf[0] frame and set valid left nalu frame */
            nalu.len = ele.buf[0].len;
            nalu.len2 = meta.len - ele.buf[0].len;
            nalu.nalu2 = reinterpret_cast<uint8_t*>(ele.buf[1].buf);
        }

        return 0;
    }

    void skip(uint32_t len) {
        std::lock_guard<std::mutex> lock(m_mtx);
        axcl_fifo_skip(&m_fifo, len);
        m_cv_put.notify_one();
    }

    void wakeup() {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_wakeup = true;
        m_cv_put.notify_one();
        m_cv_pop.notify_one();
    }

private:
    nalu_lock_fifo(const nalu_lock_fifo&) = delete;
    nalu_lock_fifo(nalu_lock_fifo&&) = delete;
    nalu_lock_fifo& operator=(const nalu_lock_fifo&) = delete;
    nalu_lock_fifo& operator=(nalu_lock_fifo&&) = delete;

    bool avail_space(uint32_t len) const {
        return axcl_fifo_avail(&m_fifo) >= len;
    }

private:
    axcl_fifo m_fifo;
    std::mutex m_mtx;
    std::condition_variable m_cv_put;
    std::condition_variable m_cv_pop;
    bool m_wakeup = false;
};
