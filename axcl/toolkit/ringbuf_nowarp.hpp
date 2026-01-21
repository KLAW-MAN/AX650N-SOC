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

#include <cstring>
#include <cstdint>
#include <mutex>

namespace axcl {

#define RINGBUF_TAIL_SIZE          (4)
#define RINGBUF_ALIGN_TO(n, s)     ((((n) + (s) - 1) / (s)) * (s))

typedef struct _ringbuf_data {
    uint32_t size;
    void*    data;
    _ringbuf_data() {
        size = 0;
        data = NULL;
    }
}ringbuf_data;

/*
 get -> put:  get block to save data, and put to make sure data is saved.
 pop -> free: pop to get data ptr, and free to release it

*/

class ringbuf_nowarp {
public:
    struct ringbuf_data_head {
        uint32_t total;
        uint32_t size;
        uint8_t  is_key;
        uint8_t  is_busy;
        uint16_t ref;
    };

    ringbuf_nowarp(uint32_t capacity, const char* pszName = nullptr) {
        m_capacity = capacity;
        m_capacity = RINGBUF_ALIGN_TO(m_capacity, sizeof(ringbuf_data_head)); // make sure ringbuf_data_head not be splited
        m_buf = new uint8_t[m_capacity];
        memset(m_buf, 0x0, m_capacity);
        m_head = 0;
        m_tail = 0;
        m_has_lost = false;
        if (pszName) {
            m_name = pszName;
        }
    }

    ~ringbuf_nowarp() {
        m_mutex.lock();
        m_head = 0;
        m_tail = 0;
        if (m_buf) {
            delete [] m_buf;
        }
        m_mutex.unlock();
    }

    bool is_full() {
        std::lock_guard<std::mutex> lck(m_mutex);
        return ((m_tail - m_head) == m_capacity);
    }

    bool is_empty() {
        std::lock_guard<std::mutex> lck(m_mutex);
        return (m_tail == m_head);
    }

    bool get(ringbuf_data &data, bool key_data) {
        std::lock_guard<std::mutex> lck(m_mutex);
        uint32_t capacity = data.size + sizeof(ringbuf_data_head) + RINGBUF_TAIL_SIZE;
        capacity = RINGBUF_ALIGN_TO(capacity, sizeof(ringbuf_data_head));

        if (!m_buf || capacity > m_capacity || m_tail < m_head || (uint32_t)(m_tail - m_head) > m_capacity) {
            // must not go to here
            m_has_lost = true;
            return false;
        }

        if (m_tail - m_head > 0) {
            ringbuf_data_head * h_last = (ringbuf_data_head*)(m_buf + m_last % m_capacity);
            if (h_last->is_busy) {
                // last element data is not saved
                // must not go to here
                m_has_lost = true;
                return false;
            }
        }

        bool is_warp = false;
        bool need_reset = false;
        uint32_t left = m_capacity - (uint32_t)(m_tail - m_head);
        uint32_t tail_pos = m_tail % m_capacity;
        uint32_t head_pos = m_head % m_capacity;
        if (tail_pos > head_pos) {
            is_warp = true;
        }

        if (is_warp) {
            // s             h              t              e
            // |-------------xxxxxxxxxxxxxxxx--------------|
            if ((m_capacity - tail_pos) >= capacity) {
                // t->e is enough
            } else if (head_pos >= capacity) {
                // s->h is enough
                // update last data size info
                ringbuf_data_head * h_last = (ringbuf_data_head*)(m_buf + m_last % m_capacity);
                h_last->total = m_buf + m_capacity - (uint8_t*)h_last;
                *(uint32_t*)(m_buf + m_capacity - RINGBUF_TAIL_SIZE) = h_last->total;
                m_tail += m_capacity - tail_pos;
            } else if ((m_capacity - head_pos) >= capacity) {
                // h->e is enough
                left = m_capacity - tail_pos;
                if (!drop(key_data, capacity, left, m_buf + tail_pos - 1)) {
                    return false;
                }
            } else {
                need_reset = true;
            }
        } else {
            // s             t               h             e
            // |xxxxxxxxxxxxxx---------------xxxxxxxxxxxxxx|
            if (left < capacity) {
                // t->h is not enough
                if (head_pos >= capacity) {
                    // s->h is enough
                    if (!drop(key_data, capacity, left, m_buf)) {
                        return false;
                    }
                } else if ((m_capacity - head_pos) >= capacity) {
                    // h->e is enough
                    if (!key_data) {
                        m_has_lost = true;
                        return false;
                    }

                    // drop s->t all data
                    m_tail -= tail_pos;
                    tail_pos = m_tail % m_capacity;

                    // drop e->h some data
                    left = 0;
                    if (!drop(key_data, capacity, left, m_buf + head_pos)) {
                        return false;
                    }
                } else {
                    need_reset = true;
                }
            }
        }

        if (need_reset) {
            if (!key_data) {
                m_has_lost = true;
                return false;
            }
            ringbuf_data_head * h = (ringbuf_data_head*)(m_buf + (m_head % m_capacity));
            if (h->ref == 0) {
                m_head = 0;
                m_tail = 0;
                m_last = 0;
                is_warp = false;
            }
        }

        uint32_t offset = m_tail % m_capacity;
        ringbuf_data_head * h = (ringbuf_data_head*)(m_buf + offset);
        memset(h, 0, sizeof(ringbuf_data_head));

        h->is_busy = 1;
        h->size = data.size;
        h->is_key = key_data ? 1 : 0;
        h->total = capacity;
        uint32_t tail_size_offset = (m_tail + capacity - RINGBUF_TAIL_SIZE) % m_capacity;
        *(uint32_t*)(m_buf + tail_size_offset) = capacity;
        data.data = (void*)(h + 1);

        m_last = m_tail;
        m_tail += capacity;

        //printf("get: head=%lu, tail=%lu, last=%lu, cap=%u\n", m_head%m_capacity, m_tail%m_capacity, m_last%m_capacity, m_capacity);

        return true;
    }


    bool put(ringbuf_data &data) {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (m_head >= m_tail) {
            return false;
        }

        if (((uint8_t*)data.data - m_buf) < (int32_t)sizeof(ringbuf_data_head)) {
            return false;
        }

        ringbuf_data_head * h = (ringbuf_data_head*)((uint8_t*)data.data - sizeof(ringbuf_data_head));
        if (((uint8_t*)h - m_buf) != (int32_t)(m_last % m_capacity)) {
            return false;
        }

        if (h->is_busy) {
            h->is_busy = 0;
            return true;
        } else {
            return false;
        }
    }

    bool pop(ringbuf_data &data) {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (m_head >= m_tail) {
            return false;
        }
        ringbuf_data_head * h = (ringbuf_data_head*)(m_buf + (m_head % m_capacity));
        if (h->ref == 0) {
            h->ref = 1;
            data.data = (void*)(h + 1);
            data.size = h->size;
            return true;
        } else {
            return false;
        }
    }

    void free(ringbuf_data &data) {
        if (!data.data || data.size == 0) {
            return;
        }
        std::lock_guard<std::mutex> lck(m_mutex);
        if (!m_buf) {
            return;
        }

        if (((uint8_t*)data.data - m_buf) < (int32_t)sizeof(ringbuf_data_head)) {
            return;
        }

        ringbuf_data_head * h = (ringbuf_data_head*)((uint8_t*)data.data - sizeof(ringbuf_data_head));
        if (h->ref == 1) {
            h->ref = 0;
            m_head += h->total;
            if (m_head >= m_tail) {
                // is empty, reset head and tail
                m_head = 0;
                m_tail = 0;
                m_last = 0;
            }
            //printf("free: head=%lu, tail=%lu, last=%lu, cap=%u\n", m_head%m_capacity, m_tail%m_capacity, m_last%m_capacity, m_capacity);
        } else {
            return;
        }
    }

    void clear() {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (m_buf) {
            m_head = 0;
            m_tail = 0;
            m_last = 0;
            m_has_lost = false;
            memset(m_buf, 0, m_capacity);
        }
    }

    uint32_t size() {
        std::lock_guard<std::mutex> lck(m_mutex);
        if (m_tail < m_head) {
            return 0;
        }
        return m_tail - m_head;
    }

    uint32_t capacity() {
        return m_capacity;
    }

private:
    bool drop(bool key_data, uint32_t capacity, uint32_t &left, uint8_t* valid_pos) {
        if (!key_data) {
            m_has_lost = true;  // mark to lost all not key data behand
            return false;
        } else {
            // find enough space to replace the tail frame(s) with new I Frame
            do {
                uint32_t size_pre = *(uint32_t*)(m_buf + ((m_tail - RINGBUF_TAIL_SIZE) % m_capacity));
                ringbuf_data_head * h = (ringbuf_data_head*)(m_buf + ((m_tail - size_pre) % m_capacity));

                if (size_pre == 0 || size_pre != h->total || size_pre > m_capacity || (uint8_t*)h > valid_pos) {
                    break;
                }

                if (h->ref == 0 && size_pre <= m_tail) {
                    m_tail -= size_pre;
                    left += size_pre;
                } else {
                    break;
                }
            } while (left < capacity && m_tail > m_head);

            if (left >= capacity) {
                m_has_lost = false;
                return true;
            } else {
                m_has_lost = true;
                return false;
            }
        }
    }

private:
    uint8_t* m_buf;
    uint32_t m_capacity;
    uint64_t m_head;
    uint64_t m_tail;
    uint64_t m_last;
    bool m_has_lost;
    std::mutex m_mutex;
    std::string m_name;
};

} // namespace axcl