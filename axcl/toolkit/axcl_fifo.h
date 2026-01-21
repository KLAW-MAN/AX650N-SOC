/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_FIFO_H__
#define __AXCL_FIFO_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned int in;
    unsigned int out;
    unsigned int mask;
    void *data;
} axcl_fifo;

typedef struct {
    struct {
        void *buf;
        unsigned int len;
    } buf[2]; /* [0]: tail [1]: rewind */
} axcl_fifo_element;

int          axcl_fifo_alloc(axcl_fifo *fifo, unsigned int size);
void         axcl_fifo_free(axcl_fifo *fifo);
void         axcl_fifo_reset(axcl_fifo *fifo);

unsigned int axcl_fifo_put(axcl_fifo *fifo, const void *buf, unsigned int len);
unsigned int axcl_fifo_put_element(axcl_fifo *fifo, const axcl_fifo_element *ele, unsigned int len);
unsigned int axcl_fifo_peek(axcl_fifo *fifo, void *buf, unsigned int len);
unsigned int axcl_fifo_pop(axcl_fifo *fifo, void *buf, unsigned int len);
unsigned int axcl_fifo_peek_element(axcl_fifo *fifo, axcl_fifo_element *ele, unsigned int len);
void         axcl_fifo_skip(axcl_fifo *fifo, unsigned int len);

#define axcl_fifo_size(fifo)              ((fifo)->mask + 1)
#define axcl_fifo_len(fifo)               ((fifo)->in - (fifo)->out)
#define axcl_fifo_is_empty(fifo)          ((fifo)->in == (fifo)->out)
#define axcl_fifo_is_full(fifo)           (axcl_fifo_len(fifo) > (fifo)->mask)
#define axcl_fifo_avail(fifo)             (axcl_fifo_size(fifo) - axcl_fifo_len(fifo))

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_FIFO_H__ */