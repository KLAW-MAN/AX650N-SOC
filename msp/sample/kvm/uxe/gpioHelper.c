/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "gpioHelper.h"
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define GPIO_CONTEXT_MAGIC (0x4750494F)
#define CHECK_GPIO_CONTEXT(h)                                                   \
    do {                                                                        \
        if (!(h) || ((h)->magic != GPIO_CONTEXT_MAGIC)) {                       \
            printf("%s %d: invalid gpio handle %p\n", __func__, __LINE__, (h)); \
            return -1;                                                          \
        }                                                                       \
    } while (0)

typedef struct {
    AX_U32 magic;
    AX_U32 gpio;
    pthread_t tid;
    GpioPollingCallback cb;
    AX_VOID *userdata;
    AX_U32 state; /* polling thread state, 0: stop, 1: running */
} GPIO_CONTEXT_T;

static AX_S32 WriteGpio(const char *fname, const char *value) {
    AX_S32 fd = open(fname, O_WRONLY);
    if (fd < 0) {
        printf("open %s fail, %s\n", fname, strerror(errno));
        return -1;
    }

    size_t sz1 = strlen(value);
    size_t sz2 = write(fd, value, sz1);
    if (sz1 != sz2) {
        printf("write %s to %s, but only %lu is written\n", value, fname, sz2);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static size_t ReadGpio(const char *fname, void *value, size_t sz) {
    AX_S32 fd = open(fname, O_RDONLY);
    if (fd < 0) {
        printf("open %s fail, %s\n", fname, strerror(errno));
        return -1;
    }

    size_t sz1 = read(fd, value, sz);
    if (sz1 != sz) {
        printf("read %lu from %s, but only %lu is read\n", sz, fname, sz1);
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static void *polling(void *arg) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)arg;

    char fname[64] = {0};
    sprintf(fname, "/sys/class/gpio/gpio%d/value", ctx->gpio);
    AX_S32 fd = open(fname, O_RDONLY);
    if (fd < 0) {
        printf("open %s fail, %s\n", fname, strerror(errno));
        return (void *)0;
    }

    AX_U8 value[2];
    read(fd, value, 1);
    lseek(fd, 0, SEEK_SET);

    struct pollfd pollfds[1];
    ctx->state = 1;
    int ret;
    while (ctx->state) {
        pollfds[0].fd = fd;
        pollfds[0].events = POLLPRI;

        ret = poll(pollfds, 1, 100);
        if (-1 == ret) {
            if (EINTR == errno) {
                continue;
            }

            printf("polling GPIO%d fail, %s", ctx->gpio, strerror(errno));
            break;
        } else if (0 == ret) {
            /* timeout */
            continue;
        } else {
            if (pollfds[0].revents & POLLPRI) {
                read(fd, value, 1);

                if (ctx->cb) {
                    ctx->cb(ctx->gpio, value[0], ctx->userdata);
                }

                pollfds[0].revents = 0;
            }
        }
    }

    close(fd);
    return (void *)0;
}

GPIO_HANDLE CreateGpioHelper(AX_U32 gpio, GPIO_DIRECTION_E direction, GPIO_EDGE_E edge) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)calloc(1, sizeof(GPIO_CONTEXT_T));
    if (!ctx) {
        return INVALID_GPIO_HANDLE;
    }

    ctx->magic = GPIO_CONTEXT_MAGIC;
    ctx->gpio = gpio;

    AX_S32 ret;
    char fname[64] = {0};
    char value[32] = {0};
    sprintf(value, "%d", gpio);
    if (0 != WriteGpio("/sys/class/gpio/export", value)) {
        goto __FAIL;
    }

    sprintf(fname, "/sys/class/gpio/gpio%d/direction", gpio);
    switch (direction) {
        case GPIO_IN:
            ret = WriteGpio(fname, "in");
            break;
        case GPIO_OUT:
            ret = WriteGpio(fname, "out");
            break;
        default:
            ret = -1;
            break;
    }

    if (0 != ret) {
        goto __FAIL;
    }

    sprintf(fname, "/sys/class/gpio/gpio%d/edge", gpio);
    switch (edge) {
        case GPIO_EDGE_NONE:
            ret = WriteGpio(fname, "none");
            break;
        case GPIO_EDGE_RISING:
            ret = WriteGpio(fname, "rising");
            break;
        case GPIO_EDGE_FALLING:
            ret = WriteGpio(fname, "falling");
            break;
        case GPIO_EDGE_BOTH:
            ret = WriteGpio(fname, "both");
            break;
        default:
            ret = -1;
            break;
    }

    if (0 != ret) {
        goto __FAIL;
    }

    return (GPIO_HANDLE)ctx;

__FAIL:
    free(ctx);
    return INVALID_GPIO_HANDLE;
}

AX_S32 SetGpioValue(GPIO_HANDLE handle, AX_U8 value) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)handle;
    CHECK_GPIO_CONTEXT(ctx);

    char szVal[32] = {0};
    sprintf(szVal, "%d", value);
    char fname[64] = {0};
    sprintf(fname, "/sys/class/gpio/gpio%d/value", ctx->gpio);
    return WriteGpio(fname, szVal);
}

AX_S32 GetGpioValue(GPIO_HANDLE handle, AX_U8 *value) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)handle;
    CHECK_GPIO_CONTEXT(ctx);

    char fname[64] = {0};
    sprintf(fname, "/sys/class/gpio/gpio%d/value", ctx->gpio);
    return ReadGpio(fname, value, 1);
}

AX_S32 StartGpioPolling(GPIO_HANDLE handle, GpioPollingCallback cb, AX_VOID *userdata) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)handle;
    CHECK_GPIO_CONTEXT(ctx);

    ctx->cb = cb;
    ctx->userdata = userdata;

    if (0 != pthread_create(&ctx->tid, NULL, polling, (void *)ctx)) {
        printf("create polling thread fail, %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

AX_S32 StopGpioPolling(GPIO_HANDLE handle) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)handle;
    CHECK_GPIO_CONTEXT(ctx);
    ctx->state = 0;
    pthread_join(ctx->tid, NULL);
    return 0;
}

AX_S32 DestoryGpioHelper(GPIO_HANDLE handle) {
    GPIO_CONTEXT_T *ctx = (GPIO_CONTEXT_T *)handle;
    CHECK_GPIO_CONTEXT(ctx);

    char value[32] = {0};
    sprintf(value, "%d", ctx->gpio);
    if (0 != WriteGpio("/sys/class/gpio/unexport", value)) {
        return -1;
    }

    free(ctx);
    return 0;
}
