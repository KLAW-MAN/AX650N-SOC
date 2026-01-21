/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __KFIFO_H_FF8A19BD_FE4E_419C_B962_1C2E8077E2B7__
#define __KFIFO_H_FF8A19BD_FE4E_419C_B962_1C2E8077E2B7__

typedef struct {
    unsigned int in;
    unsigned int out;
    unsigned int mask;
    void *data;
} kfifo;

typedef struct {
    struct {
        void *buf;
        unsigned int len;
    } buf[2]; /* [0]: tail [1]: rewind */
} kfifo_ele;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * kfifo_alloc - dynamically allocates a new fifo buffer
 * @fifo: pointer to the fifo
 * @size: the number of elements in the fifo, this must be a power of 2
 *
 * The fifo will be release with kfifo_free().
 * Return 0 if no error, otherwise an error code.
 */
extern int kfifo_alloc(kfifo *fifo, unsigned int size);

/**
 * kfifo_free - frees the fifo
 * @fifo: the fifo to be freed
 */
extern void kfifo_free(kfifo *fifo);

/**
 * kfifo_put - put data into the fifo
 * @fifo: address of the fifo to be used
 * @buf: address of the data to put
 * @len: data len
 *
 * This macro copies the given value into the fifo.
 * It returns 0 if the fifo was full. Otherwise it returns the number
 * processed elements.
 */
extern unsigned int kfifo_put(kfifo *fifo, const void *buf, unsigned int len);

extern unsigned int kfifo_put2(kfifo *fifo, const kfifo_ele *in, kfifo_ele *out);
/**
 * kfifo_get - get data from the fifo
 * @fifo: address of the fifo to be used
 * @buf: address where to store the data
 * @len: data len
 *
 * This macro reads the data from the fifo.
 * It returns 0 if the fifo was empty. Otherwise it returns the number
 * processed elements.
 */
extern unsigned int kfifo_pop(kfifo *fifo, void *buf, unsigned int len);

/**
 * kfifo_peek - get data from the fifo without removing
 * @fifo: address of the fifo to be used
 * @buf: address where to store the data
 * @len: peek length
 *
 * This reads the data from the fifo without removing it from the fifo.
 * It returns 0 if the fifo was empty. Otherwise it returns the number
 * processed elements.
 */
extern unsigned int kfifo_peek(kfifo *fifo, void *buf, unsigned int len);
extern unsigned int kfifo_peek_n(kfifo *fifo, kfifo_ele *ele, unsigned int len);

/**
 * kfifo_size - returns the size of the fifo in elements
 * @fifo: address of the fifo to be used
 */
#define kfifo_size(fifo) ((fifo)->mask + 1)

/**
 * kfifo_len - returns the number of used elements in the fifo
 * @fifo: address of the fifo to be used
 */
#define kfifo_len(fifo) ({ (fifo)->in - (fifo)->out; })

/**
 * kfifo_is_empty - returns true if the fifo is empty
 * @fifo: address of the fifo to be used
 */
#define kfifo_is_empty(fifo) ({ (fifo)->in == (fifo)->out; })

/**
 * kfifo_is_full - returns true if the fifo is full
 * @fifo: address of the fifo to be used
 */
#define kfifo_is_full(fifo) ({ kfifo_len(fifo) > (fifo)->mask; })

/**
 * kfifo_avail - returns the number of unused elements in the fifo
 * @fifo: address of the fifo to be used
 */
#define kfifo_avail(fifo) ({ kfifo_size(fifo) - kfifo_len(fifo); })

/**
 * kfifo_skip - skip output data
 * @fifo: address of the fifo to be used
 * @len: len to skip
 */
#define kfifo_skip(fifo, len) (void)({ (fifo)->out += (len); })

/**
 * kfifo_reset - removes the entire fifo content
 * @fifo: address of the fifo to be used
 *
 * Note: usage of kfifo_reset() is dangerous. It should be only called when the
 * fifo is exclusived locked or when it is secured that no other thread is
 * accessing the fifo.
 */
#define kfifo_reset(fifo) (void)({ (fifo)->in = (fifo)->out = 0; })

#ifdef __cplusplus
}
#endif
#endif /* __KFIFO_H_FF8A19BD_FE4E_419C_B962_1C2E8077E2B7__ */