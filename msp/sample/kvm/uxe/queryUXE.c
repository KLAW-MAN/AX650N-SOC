/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "queryUXE.h"
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define UXE_I2C_BUSID 0
#define UXE_DEV_ADDR 0x2B /* 0x56 >> 1 */

#define CHECK_RETVAL(_statement)   \
    do {                           \
        AX_S32 ret = (_statement); \
        if (0 != ret) {            \
            return ret;            \
        }                          \
    } while (0)

#define MACRO_LOG_BLACK  "\033[1;30;30m"
#define MACRO_LOG_RED    "\033[1;30;31m"
#define MACRO_LOG_GREEN  "\033[1;30;32m"
#define MACRO_LOG_YELLOW "\033[1;30;33m"
#define MACRO_LOG_BLUE   "\033[1;30;34m"
#define MACRO_LOG_PURPLE "\033[1;30;35m"
#define MACRO_LOG_WHITE  "\033[1;30;37m"
#define MACRO_LOG_END    "\033[0m\n"

#define LOG_MM_E(tag, fmt, ...) printf(MACRO_LOG_RED    "E %s <%s>.%d: " fmt MACRO_LOG_END, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_W(tag, fmt, ...) printf(MACRO_LOG_YELLOW "W %s <%s>.%d: " fmt MACRO_LOG_END, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_I(tag, fmt, ...) printf(MACRO_LOG_GREEN  "I %s <%s>.%d: " fmt MACRO_LOG_END, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_D(tag, fmt, ...) printf(MACRO_LOG_WHITE  "D %s <%s>.%d: " fmt MACRO_LOG_END, tag, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_MM_N(tag, fmt, ...) printf(MACRO_LOG_PURPLE "N %s <%s>.%d: " fmt MACRO_LOG_END, tag, __func__, __LINE__, ##__VA_ARGS__)

#define TAG "UXE"

#define MAKEU16(b, a) ((AX_U16)((AX_U8)((a) & 0xFF)) | ((AX_U16)((AX_U8)((b) & 0xFF) << 8)))
#define MAKEU32(c, b, a) ((AX_U32)((AX_U8)((a) & 0xFF)) | ((AX_U32)((AX_U8)((b) & 0xFF) << 8)) | ((AX_U32)((AX_U8)((c) & 0xFF) << 16)))

typedef struct {
    AX_S32 fd;
    AX_U16 devAddr;
} I2C_CONTEXT_T;

static AX_S32 I2C_Open(I2C_CONTEXT_T *ctx, AX_U16 busId, AX_U16 devAddr);
static AX_S32 I2C_Close(const I2C_CONTEXT_T *ctx);
static AX_S32 I2C_WriteRegU8(const I2C_CONTEXT_T *ctx, AX_U8 addr, AX_U8 val);
static AX_S32 I2C_ReadRegU8(const I2C_CONTEXT_T *ctx, AX_U8 addr, AX_U8 *val);

AX_S32 AX_UXE_GetChip(AX_U16 *chipId) {
    I2C_CONTEXT_T ctx;
    CHECK_RETVAL(I2C_Open(&ctx, UXE_I2C_BUSID, UXE_DEV_ADDR));

    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xFF, 0xE0));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xEE, 0x01));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xFF, 0xE1));

    AX_U8 val[2] = {0, 0};
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x00, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x01, &val[1]));

    *chipId = MAKEU16(val[0], val[1]);
    LOG_MM_I(TAG, "chip id = 0x%x", *chipId);

    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xFF, 0xE0));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xEE, 0x00));

    I2C_Close(&ctx);
    return 0;
}

AX_S32 AX_UXE_GetSignalInfo(UXE_SIGNAL_INFO_T *info) {
    I2C_CONTEXT_T ctx;
    CHECK_RETVAL(I2C_Open(&ctx, UXE_I2C_BUSID, UXE_DEV_ADDR));

    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xFF, 0xE0));

    AX_U8 val[4];
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x84, &val[0]));
    info->intType = val[0];

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x85, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x86, &val[1]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x87, &val[2]));
    info->pixelClk = MAKEU32(val[0], val[1], val[2]) * 2;

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x88, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x89, &val[1]));
    info->HTotal = MAKEU16(val[0], val[1]) * 2;

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x8A, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x8B, &val[1]));
    info->VTotal = MAKEU16(val[0], val[1]);

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x8C, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x8D, &val[1]));
    info->HActive = MAKEU16(val[0], val[1]) * 2;

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x8E, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x8F, &val[1]));
    info->VActive = MAKEU16(val[0], val[1]);

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x90, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x91, &val[1]));
    info->AudioFS = MAKEU16(val[0], val[1]);

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x92, &val[0]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x93, &val[1]));
    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x94, &val[2]));
    info->byteClk = MAKEU32(val[0], val[1], val[2]);

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x95, &val[0]));
    info->laneNum = val[0];

    CHECK_RETVAL(I2C_ReadRegU8(&ctx, 0x96, &val[0]));
    info->mipiFmt = val[0];

    LOG_MM_I(TAG, "interrupt type: %d", info->intType);
    LOG_MM_I(TAG, "pixel clock: %d KHz, byte clock: %d KHz, mipi clock rate: %d MHz, mipi data rate: %d MHz", info->pixelClk, info->byteClk,
             info->byteClk * 4 / 1000, info->byteClk * 8 / 1000);
    LOG_MM_I(TAG, "HTotal: %d VTotal: %d, HActive: %d VActive: %d, fps = %.2f", info->HTotal, info->VTotal, info->HActive, info->VActive,
             info->pixelClk * 1000.0 / (info->HTotal * info->VTotal));
    LOG_MM_I(TAG, "Audio sampling frequency: %d KHz", info->AudioFS);
    LOG_MM_I(TAG, "lane num: %d", info->laneNum);
    LOG_MM_I(TAG, "mipi format: %d", info->mipiFmt);

    I2C_Close(&ctx);
    return 0;
}

static AX_S32 I2C_Open(I2C_CONTEXT_T *ctx, AX_U16 busId, AX_U16 devAddr) {
    AX_CHAR devName[32];
    sprintf(devName, "/dev/i2c-%u", busId);
    AX_S32 fd = open(devName, O_RDWR);
    if (-1 == fd) {
        LOG_MM_E(TAG, "open %s fail", devName);
        return -1;
    }

    AX_S32 ret = ioctl(fd, I2C_SLAVE_FORCE, devAddr);
    if (ret < 0) {
        close(fd);
        LOG_MM_E(TAG, "set I2C %d slave addr to %d fail", busId, devAddr);
        return ret;
    }

    ctx->fd = fd;
    ctx->devAddr = devAddr;
    return 0;
}

static AX_S32 I2C_Close(const I2C_CONTEXT_T *ctx) {
    return close(ctx->fd);
}

static AX_S32 I2C_WriteRegU8(const I2C_CONTEXT_T *ctx, AX_U8 addr, AX_U8 val) {
    AX_U8 msgBuf[2];
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg msgs;

    msgBuf[0] = addr;
    msgBuf[1] = val;

    msgs.addr = ctx->devAddr;
    msgs.flags = 0;
    msgs.len = 2;
    msgs.buf = msgBuf;

    data.msgs = &msgs;
    data.nmsgs = 1;

    AX_S32 ret = ioctl(ctx->fd, I2C_RDWR, &data);
    if (ret < 0) {
        LOG_MM_E(TAG, "write reg 0x%x to 0x%x fail", addr, val);
        return ret;
    }

    return 0;
}

static AX_S32 I2C_ReadRegU8(const I2C_CONTEXT_T *ctx, AX_U8 addr, AX_U8 *val) {
    AX_U8 msgBuf[2];
    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg msgs[2];

    msgBuf[0] = addr;

    msgs[0].addr = ctx->devAddr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = msgBuf;

    msgs[1].addr = ctx->devAddr;
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 1;
    msgs[1].buf = msgBuf;

    data.msgs = &msgs[0];
    data.nmsgs = 2;

    AX_S32 ret = ioctl(ctx->fd, I2C_RDWR, &data);
    if (ret < 0) {
        LOG_MM_E(TAG, "read reg %x fail", addr);
        return ret;
    }

    *val = msgBuf[0];
    return 0;
}

AX_S32 AX_UXE_EnableMipiTx(AX_VOID) {
    I2C_CONTEXT_T ctx;
    CHECK_RETVAL(I2C_Open(&ctx, UXE_I2C_BUSID, UXE_DEV_ADDR));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xFF, 0xE0));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xB0, 0x01));
    I2C_Close(&ctx);
    LOG_MM_I(TAG, "enable mipi tx");
    return 0;
}

AX_S32 AX_UXE_DisableMipiTx(AX_VOID) {
    I2C_CONTEXT_T ctx;
    CHECK_RETVAL(I2C_Open(&ctx, UXE_I2C_BUSID, UXE_DEV_ADDR));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xFF, 0xE0));
    CHECK_RETVAL(I2C_WriteRegU8(&ctx, 0xB0, 0x00));
    I2C_Close(&ctx);
    LOG_MM_I(TAG, "disable mipi tx");
    return 0;
}
