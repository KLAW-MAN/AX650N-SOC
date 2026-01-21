/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "kfifo.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x)-1)) == 0))
#define kmalloc_array malloc
#define kfree free

static void barrier() {
    __sync_synchronize();
}

static unsigned int roundup_pow_of_two(unsigned int x) {
    int position = 0;
    int i;

    for (i = (x - 1); i != 0; ++position) {
        i >>= 1;
    }

    return 1UL << position;
}

int kfifo_alloc(kfifo *fifo, unsigned int size) {
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

    fifo->data = (unsigned char *)kmalloc_array(size);

    if (!fifo->data) {
        fifo->mask = 0;
        return -ENOMEM;
    }

    fifo->mask = size - 1;
    return 0;
}

void kfifo_free(kfifo *fifo) {
    kfree(fifo->data);
    fifo->in = 0;
    fifo->out = 0;
    fifo->data = NULL;
    fifo->mask = 0;
}

static void kfifo_copy_in(kfifo *fifo, const void *src, unsigned int len, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int l;

    off &= fifo->mask;
    l = min(len, size - off);

    memcpy(fifo->data + off, src, l);
    memcpy(fifo->data, src + l, len - l);

    /*
     * make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter
     */
    barrier();
}

unsigned int kfifo_put(kfifo *fifo, const void *buf, unsigned int len) {
    unsigned int l;

    l = (fifo->mask + 1) - (fifo->in - fifo->out);
    if (len > l) {
        return 0; /* return if no left space */
    }

    kfifo_copy_in(fifo, buf, len, fifo->in);
    fifo->in += len;
    return len;
}

static void kfifo_copy_in2(kfifo *fifo, const kfifo_ele *in, kfifo_ele *out, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int l;
    unsigned int len = in->buf[0].len + in->buf[1].len;
    unsigned int left;
    unsigned int done;

    off &= fifo->mask;
    l = min(len, size - off);

    if (0 == in->buf[1].len) {
        /* one src buf */
        memcpy(fifo->data + off, in->buf[0].buf, l);
        memcpy(fifo->data, in->buf[0].buf + l, len - l);
    } else {
        /* two src buf */
        left = l;
        done = (in->buf[0].len <= l) ? in->buf[0].len : l;
        memcpy(fifo->data + off, in->buf[0].buf, done);
        left -= done;
        if (left > 0) {
            /* buf[0] has been copied, copy left tail space and rewind */
            done = (in->buf[1].len <= left) ? in->buf[1].len : left;
            memcpy(fifo->data + off + in->buf[0].len, in->buf[1].buf, done);
            memcpy(fifo->data, in->buf[1].buf + done, len - l);
        } else {
            /* part of buf[0] has been copied and no space in tail, rewind to head */
            left = in->buf[0].len - done;
            if (left > 0) {
                memcpy(fifo->data, in->buf[0].buf + done, left);
            }
            memcpy(fifo->data + left, in->buf[1].buf, in->buf[1].len);
        }
    }

    out->buf[0].buf = fifo->data + off;
    out->buf[0].len = l;
    /* rewind */
    out->buf[1].len = len - l;
    if (out->buf[1].len > 0) {
        out->buf[1].buf = fifo->data;
    } else {
        out->buf[1].buf = NULL;
    }

    /*
     * make sure that the data in the fifo is up to date before
     * incrementing the fifo->in index counter
     */
    barrier();
}

unsigned int kfifo_put2(kfifo *fifo, const kfifo_ele *in, kfifo_ele *out) {
    unsigned int l;
    unsigned int len = in->buf[0].len + in->buf[1].len;

    l = (fifo->mask + 1) - (fifo->in - fifo->out);
    if (len > l) {
        return 0; /* return if no left space */
    }

    kfifo_copy_in2(fifo, in, out, fifo->in);
    fifo->in += len;

    return len;
}

static void kfifo_copy_out(kfifo *fifo, void *dst, unsigned int len, unsigned int off) {
    unsigned int size = fifo->mask + 1;
    unsigned int l;

    off &= fifo->mask;
    l = min(len, size - off);

    memcpy(dst, fifo->data + off, l);
    memcpy(dst + l, fifo->data, len - l);
    /*
     * make sure that the data is copied before
     * incrementing the fifo->out index counter
     */
    barrier();
}

unsigned int kfifo_peek(kfifo *fifo, void *buf, unsigned int len) {
    unsigned int l;

    l = fifo->in - fifo->out;
    if (len > l) {
        len = l;
    }

    kfifo_copy_out(fifo, buf, len, fifo->out);
    return len;
}

unsigned int kfifo_pop(kfifo *fifo, void *buf, unsigned int len) {
    len = kfifo_peek(fifo, buf, len);
    fifo->out += len;
    return len;
}

unsigned int kfifo_peek_n(kfifo *fifo, kfifo_ele *ele, unsigned int len) {
    unsigned int off;
    unsigned int l;

    l = fifo->in - fifo->out;
    if (len > l) {
        len = l;
    }

    off = fifo->out & fifo->mask;
    l = min(len, fifo->mask + 1 - off);

    ele->buf[0].buf = fifo->data + off;
    ele->buf[0].len = l;

    ele->buf[1].len = len - l;
    if (ele->buf[1].len > 0) {
        ele->buf[1].buf = fifo->data;
    } else {
        ele->buf[1].buf = NULL;
    }

    return len;
}
