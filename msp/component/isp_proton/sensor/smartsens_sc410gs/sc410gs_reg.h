/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC410GS_REG_H__
#define __SC410GS_REG_H__

#include "ax_base_type.h"


#define SC410GS_SLAVE_ADDR      (0x30)    /**< i2c slave address of the SC410GS camera sensor */
#define SC410GS_SENSOR_ID       (0xad19)
#define SC410GS_DEVADDR_BYTE    (1)
#define SC410GS_ADDR_BYTE       (2)
#define SC410GS_DATA_BYTE       (1)
#define SC410GS_SWAP_BYTES      (1)

#define SC410GS_DEVICEID 0XFF
#define SC410GS_SPI_WIRTE 0
#define SC410GS_SPI_READ  1

/* Exposure control related registers */
#define SC410GS_LONG_EXPLINE_H     (0x3e00)  /* bit[3:0] */
#define SC410GS_LONG_EXPLINE_M     (0x3e01)  /* bit[7:0] */
#define SC410GS_LONG_EXPLINE_L     (0x3e02)  /* bit[7:4] */
#define SC410GS_LONG_AGAIN_H       (0x3e08)  /* bit[7:0] */
#define SC410GS_LONG_AGAIN_L       (0x3e09)  /* bit[7:0] */
#define SC410GS_LONG_DGAIN_H       (0x3e06)  /* bit[1:0] */
#define SC410GS_LONG_DGAIN_L       (0x3e07)  /* bit[7:0] */

#define SC410GS_VTS_H              (0x320e)  /* bit[7:0] */
#define SC410GS_VTS_L              (0x320f)  /* bit[7:0] */

typedef enum _AX_SNS_AE_REG_IDX_E_ {
    SC410GS_LONG_EXPLINE_H_IDX = 0,
    SC410GS_LONG_EXPLINE_M_IDX,
    SC410GS_LONG_EXPLINE_L_IDX,
    SC410GS_LONG_AGAIN_H_IDX,
    SC410GS_LONG_AGAIN_L_IDX,
    SC410GS_LONG_DGAIN_H_IDX,
    SC410GS_LONG_DGAIN_L_IDX,
    SC410GS_VTS_H_IDX,
    SC410GS_VTS_L_IDX,
} AX_SNS_AE_REG_IDX_E;

AX_S32 sc410gs_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio);
AX_S32 sc410gs_i2c_init(ISP_PIPE_ID nPipeId);
AX_S32 sc410gs_i2c_exit(ISP_PIPE_ID nPipeId);
AX_S32 sc410gs_spi_init(ISP_PIPE_ID nPipeId);
AX_S32 sc410gs_spi_exit(ISP_PIPE_ID nPipeId);

AX_S32 sc410gs_reg_read(ISP_PIPE_ID nPipeId, AX_U32 nAddr);
AX_S32 sc410gs_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData);
AX_S32 sc410gs_write_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 nData);

AX_S32 sc410gs_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts);

AX_S32 sc410gs_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt);
AX_S32 sc410gs_write_settings(ISP_PIPE_ID nPipeId);

AX_S32 sc410gs_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo);
AX_S32 sc410gs_get_bus_num(ISP_PIPE_ID nPipeId);

#endif  //end __SC410GS_REG_H__
