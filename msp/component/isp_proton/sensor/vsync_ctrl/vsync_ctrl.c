/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "vsync_ctrl.h"

#define VSYNC_CTRL_NODE "/dev/ax_vsync_ctrl"

int ax_vsync_ctrl_init(int fps)
{
    int ret;
    int fd = -1;

    fd = open(VSYNC_CTRL_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open ax_vsync_ctrl dev fail!\n");
        return -1;
    }

    ret = ioctl(fd, AX_VSYNC_CTRL_IOC_INIT, &fps);
    if (ret < 0) {
        printf("CMD AX_VSYNC_CTRL_IOC_INIT error!\n");
        close(fd);
        return ret;
    }

    close(fd);

    return ret;
}

int ax_vsync_ctrl_start(void)
{
    int ret;
    int fd = -1;

    fd = open(VSYNC_CTRL_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open ax_vsync_ctrl dev fail!\n");
        return -1;
    }

    ret = ioctl(fd, AX_VSYNC_CTRL_IOC_START);
    if (ret < 0) {
        printf("CMD AX_VSYNC_CTRL_IOC_START error!\n");
        close(fd);
        return ret;
    }

    close(fd);

    return ret;
}

int ax_vsync_ctrl_stop(void)
{
    int ret;
    int fd = -1;

    fd = open(VSYNC_CTRL_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open ax_vsync_ctrl dev fail!\n");
        return -1;
    }

    ret = ioctl(fd, AX_VSYNC_CTRL_IOC_STOP);
    if (ret < 0) {
        printf("CMD AX_VSYNC_CTRL_IOC_STOP error!\n");
        close(fd);
        return ret;
    }

    close(fd);

    return ret;
}

int ax_vsync_ctrl_set_fps(AX_VSYNC_INFO_T *tVsyncInfo)
{
    int ret;
    int fd = -1;

    fd = open(VSYNC_CTRL_NODE, O_RDWR);
    if (fd < 0) {
        printf("Open ax_vsync_ctrl dev fail!\n");
        return -1;
    }

    ret = ioctl(fd, AX_VSYNC_CTRL_IOC_SET_FPS, tVsyncInfo);
    if (ret < 0) {
        printf("CMD AX_VSYNC_CTRL_IOC_SET_FPS error!\n");
        close(fd);
        return ret;
    }

    close(fd);

    return ret;
}