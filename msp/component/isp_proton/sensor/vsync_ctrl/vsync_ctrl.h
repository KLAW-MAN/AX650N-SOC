/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __VSYNC_CTRL_H__
#define __VSYNC_CTRL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define VSYNC_MAGIC 's'

#define AX_VSYNC_CTRL_IOC_INIT           _IOWR(VSYNC_MAGIC, 0x00, void*)
#define AX_VSYNC_CTRL_IOC_DEINIT         _IO(VSYNC_MAGIC,0x01)
#define AX_VSYNC_CTRL_IOC_START          _IO(VSYNC_MAGIC,0x02)
#define AX_VSYNC_CTRL_IOC_STOP           _IO(VSYNC_MAGIC,0x03)
#define AX_VSYNC_CTRL_IOC_SET_FPS        _IOWR(VSYNC_MAGIC,0x04, void*)

#define AX_VSYNC_CTRL_PIPE_NUM            8
#define AX_VSYNC_CTRL_SNS_REG_NUM         4

typedef struct _AX_VSYNC_SNS_REG_T{
    unsigned int nRegAddr;
    unsigned int nData;
} AX_VSYNC_SNS_REG_T;

typedef struct {
    unsigned char    nI2cNode;  /* sensor i2c node */
    unsigned char    nI2cAddr;  /* sensor device address */
    unsigned char    nDelayFrmNum;       /* Number of frames for register delay configuration */
    unsigned char    nIntPos;            /* Position of the register takes effect, only support AX_SNS_L_FSOF/AX_SNS_S_FSOF */
    unsigned char    nRegNum;
    unsigned int     nAddrByteNum;       /* Bit width of the register address */
    unsigned int     nDataByteNum;       /* Bit width of sensor register data */
    AX_VSYNC_SNS_REG_T tSnsReg[AX_VSYNC_CTRL_SNS_REG_NUM];

} AX_VSYNC_SNS_I2C_T;
typedef struct _AX_VSYNC_INFO_T {
    AX_VSYNC_SNS_I2C_T tI2cInfo[AX_VSYNC_CTRL_PIPE_NUM];
    int nFps;
    int nSnsCnt;
} AX_VSYNC_INFO_T;

int ax_vsync_ctrl_init(int fps);
int ax_vsync_ctrl_start(void);
int ax_vsync_ctrl_stop(void);
int ax_vsync_ctrl_set_fps(AX_VSYNC_INFO_T *tVsyncInfo);

#ifdef __cplusplus
}
#endif

#endif