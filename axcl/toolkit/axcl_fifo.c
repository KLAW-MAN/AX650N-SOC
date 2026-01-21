/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axcl_fifo.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define AXCL_MIN(x, y) ((x) < (y) ? (x) : (y))
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

#if defined(WINDOWS)
#include <windows.h>
#define smp_wmb() MemoryBarrier()
#else
#define smp_wmb() __sync_synchronize()
#endif

static unsigned int roundup_pow_of_two(unsigned int x) {
    int position = 0;
    int i;

    for (i = (x - 1); i != 0; ++position) {
        i >>= 1;
    }

    return 1UL << position;
}

int axcl_fifo_alloc(axcl_fifo *fifo, unsigned int size) {
    /*
     * round up to the next power of 2, since our 'let the indices
     * wrap' technique works only in this case.
     */
    if (!is_power_of_2(size)) {
        size = roundup_pow_of_two(size);
    }

    fifo->in = 0;
    fifo->out = 0;

    if (size < 2) {
        fifo->data = NULL;
        fifo->mask = 0;
        return -EINVAL;
    }

    fifo->data = (unsigned char *)malloc(size);
    if (!fifo->data) {
        fifo->mask = 0;
        return -ENOMEM;
    }

    fifo->mask = size - 1;
    return 0;
}

void axcl_fifo_free(axcl_fifo *fifo) {
    free(fifo->data);

    fifo->in = 0;
    fifo->out = 0;
    fifo->data = NULL;
    fifo->mask = 0;
}

void axcl_fifo_reset(axcl_fifo *fifo) {
    fifo->in = fifo->out = 0;
}

static void axcl_fifo_copy_in(axcl_fifo *fifo, const void *src, unsigned int len, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int l;

    off &= fifo->mask;
    l = AXCL_MIN(len, size - off);

    memcpy((unsigned char*)fifo->data + off, src, l);
    memcpy(fifo->data, (const unsigned char*)src + l, len - l);

    /*
     * make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter
     */
    smp_wmb();
}

unsigned int axcl_fifo_put(axcl_fifo *fifo, const void *buf, unsigned int len) {
    unsigned int l;

    l = axcl_fifo_avail(fifo);
    if (len > l) {
        return 0;
    }

    axcl_fifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    return len;
}

static void axcl_fifo_copy_in_element(axcl_fifo *fifo, const axcl_fifo_element *ele, unsigned int len, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int l;
    unsigned int left; /* left of fifo forward space */
    unsigned int done;

    off &= fifo->mask;
    l = AXCL_MIN(len, size - off);

    if (0 == ele->buf[1].len) {
        left = l;
        done = (ele->buf[0].len <= l) ? ele->buf[0].len : l;
        memcpy((unsigned char*)fifo->data + off, ele->buf[0].buf, done);
        left -= done;
        if (0 == left) {
            /* rewind to copy remain buf[0] (>= 0) */
            left = ele->buf[0].len - done;
            memcpy(fifo->data, (unsigned char*)ele->buf[0].buf + done, left);
        }
    } else {
        left = l;
        done = (ele->buf[0].len <= l) ? ele->buf[0].len : l;
        memcpy((unsigned char*)fifo->data + off, ele->buf[0].buf, done);
        left -= done;
        if (left > 0) {
            /* has unused space */
            done = (ele->buf[1].len <= left) ? ele->buf[1].len : left;
            memcpy((unsigned char*)fifo->data + off + ele->buf[0].len, ele->buf[1].buf, done);
            left -= done;
            if (0 == left) {
                /* rewind to copy remain buf[1] */
                memcpy(fifo->data, (unsigned char*)ele->buf[1].buf + done, ele->buf[1].len - done);
            }
        } else {
            left = ele->buf[0].len - done;
            if (left > 0) {
                /* buf[0] left >= 0 */
                memcpy(fifo->data, (unsigned char*)ele->buf[0].buf + done, left);
                memcpy((unsigned char*)fifo->data + left, ele->buf[1].buf, ele->buf[1].len);
            } else {
                /* only remain buf[1] */
                memcpy(fifo->data, ele->buf[1].buf, ele->buf[1].len);
            }
        }
    }

    /*
     * make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter
     */
    smp_wmb();
}

unsigned int axcl_fifo_put_element(axcl_fifo *fifo, const axcl_fifo_element *ele, unsigned int len) {
    unsigned int l;

    l = axcl_fifo_avail(fifo);
    if (len > l) {
        return 0;
    }

    axcl_fifo_copy_in_element(fifo, ele, len, fifo->in);
    fifo->in += len;
    return len;
}

static void axcl_fifo_copy_out(axcl_fifo *fifo, void *dst, unsigned int len, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int l;

    off &= fifo->mask;
    l = AXCL_MIN(len, size - off);

    memcpy(dst, (unsigned char*)fifo->data + off, l);
    memcpy((unsigned char*)dst + l, fifo->data, len - l);
    /*
     * make sure that the data is copied before
     * incrementing the fifo->out index counter
     */
    smp_wmb();
}

unsigned int axcl_fifo_peek(axcl_fifo *fifo, void *buf, unsigned int len) {
    unsigned int l;

    l = fifo->in - fifo->out;
    if (len > l) {
        len = l;
    }

    axcl_fifo_copy_out(fifo, buf, len, fifo->out);
    return len;
}

unsigned int axcl_fifo_pop(axcl_fifo *fifo, void *buf, unsigned int len) {
    unsigned int l;

    l = fifo->in - fifo->out;
    if (len > l) {
        len = l;
    }

    axcl_fifo_copy_out(fifo, buf, len, fifo->out);
    fifo->out += len;

    return len;
}

unsigned int axcl_fifo_peek_element(axcl_fifo *fifo, axcl_fifo_element *ele, unsigned int len) {
    unsigned int size = fifo->mask + 1;
    unsigned int off;
    unsigned int l;

    l = fifo->in - fifo->out;
    if (len > l) {
        len = l;
    }

    off = fifo->out & fifo->mask;
    l = AXCL_MIN(len, size - off);

    ele->buf[0].buf = (unsigned char*)fifo->data + off;
    ele->buf[0].len = l;

    ele->buf[1].len = len - l;
    ele->buf[1].buf = (ele->buf[1].len > 0) ? fifo->data : NULL;

    return len;
}

void axcl_fifo_skip(axcl_fifo *fifo, unsigned int len) {
    fifo->out += len;
}