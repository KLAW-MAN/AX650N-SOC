/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include <QApplication>
#include <QWidget>
#include <QPushButton>

#include "ax_sys_api.h"
#include "ax_base_type.h"
#include "ax_vo_api.h"

#include "rasterwindow.h"

#ifndef SAMPLE_PRT
#define SAMPLE_PRT printf
#endif

static struct fb_bitfield g_r32 = {16, 8, 0};
static struct fb_bitfield g_g32 = {8, 8, 0};
static struct fb_bitfield g_b32 = {0, 8, 0};
static struct fb_bitfield g_a32 = {24, 8, 0};

static AX_S32 SAMPLE_VO_FB_INIT(AX_VO_PUB_ATTR_T *pstVoPubAttr)
{
    AX_S32 s32Fd, s32Ret = 0;
    AX_U32 u32Width, u32Height;
    struct fb_var_screeninfo var;

    u32Width = 1920;
    u32Height = 1080;

    /*1. open Framebuffer device overlay 0*/
    s32Fd = open("/dev/fb0", O_RDWR);
    if(s32Fd < 0) {
        SAMPLE_PRT("open fb0 failed!\n");
        goto exit0;
    }

    /*2. get the variable screen info*/
    s32Ret = ioctl(s32Fd, FBIOGET_VSCREENINFO, &var);
    if (s32Ret < 0) {
        SAMPLE_PRT("get variable screen info failed!\n");
        goto exit1;
    }

    /*3. modify the variable screen info
    the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
    the virtual screen size: IMAGE_WIDTH*(IMAGE_HEIGHT*2)
    the pixel format: ARGB8888
    */
    var.xres = var.xres_virtual = u32Width;
    var.yres = u32Height;
    var.yres_virtual = u32Height;
    var.transp= g_a32;
    var.red = g_r32;
    var.green = g_g32;
    var.blue = g_b32;
    var.bits_per_pixel = 32;

    /*4. set the variable screeninfo*/
    s32Ret = ioctl(s32Fd, FBIOPUT_VSCREENINFO, &var);
    if (s32Ret < 0) {
        SAMPLE_PRT("put variable screen info failed!\n");
        goto exit1;
    }

exit1:
    close(s32Fd);
exit0:
    SAMPLE_PRT("quit!\n");

    return s32Ret;
}

static AX_S32 SAMPLE_GUI(AX_S32 argc, AX_CHAR *argv[])
{
    QApplication QApp(argc, argv);
    QWidget QWin;
    QPushButton QPButton("Quit", &QWin);

    QPButton.setFont(QFont("Times", 18, QFont::Bold));
    QPButton.setGeometry(410, 250, 180, 40);
    QObject::connect(&QPButton, SIGNAL(clicked()), &QApp, SLOT(QPButton()));
    QWin.show();

    return QApp.exec();
}

AX_S32 main(AX_S32 argc, AX_CHAR *argv[])
{
    AX_S32 s32Ret = 0;
    VO_DEV VoDev = 0;
    GRAPHIC_LAYER GraphicLayer = 0;
    AX_VO_PUB_ATTR_T stVoPubAttr;

    memset(&stVoPubAttr, 0, sizeof(stVoPubAttr));
    stVoPubAttr.enIntfType = AX_VO_INTF_HDMI;
    stVoPubAttr.enIntfSync = AX_VO_OUTPUT_1080P60;

    SAMPLE_VO_FB_INIT(&stVoPubAttr);

    s32Ret = AX_SYS_Init();
    if (s32Ret) {
        SAMPLE_PRT("AX_SYS_Init failed, s32Ret = 0x%x\n", s32Ret);
        return -1;
    }

    s32Ret = AX_VO_Init();
    if (s32Ret) {
        SAMPLE_PRT("AX_VO_Init failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AX_VO_SetPubAttr(VoDev, &stVoPubAttr);
    if (s32Ret) {
        SAMPLE_PRT("AX_VO_SetPubAttr failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AX_VO_Enable(VoDev);
    if (s32Ret) {
        SAMPLE_PRT("AX_VO_Enable failed with %#x!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = AX_VO_BindGraphicLayer(GraphicLayer, VoDev);
    if (s32Ret)
        SAMPLE_PRT("AX_VO_BindGraphicLayer failed, s32Ret = 0x%x\n", s32Ret);

    SAMPLE_PRT("sample qt start show!!\n");

    return SAMPLE_GUI(argc, argv);
}
