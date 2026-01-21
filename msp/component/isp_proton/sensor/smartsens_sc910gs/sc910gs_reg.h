/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC910GS_REG_H__
#define __SC910GS_REG_H__

#include "ax_base_type.h"


#define SC910GS_SLAVE_ADDR      (0x30)    /**< i2c slave address of the SC910GS camera sensor */
#define SC910GS_SENSOR_CHIP_ID  (0xa22b)
#define SC910GS_DEVADDR_BYTE    (1)
#define SC910GS_ADDR_BYTE       (2)
#define SC910GS_DATA_BYTE       (1)
#define SC910GS_SWAP_BYTES      (1)
#define SC910GS_INCK_24M        (24)

#define SC910GS_DEVICEID 0XFF
#define SC910GS_SPI_WIRTE 0
#define SC910GS_SPI_READ  1

/* Exposure control related registers */
#define     SC910GS_LONG_EXP_LINE_H     (0x3e00)  /* bit[7:0], long frame exposure in unit of rows */
#define     SC910GS_LONG_EXP_LINE_M     (0x3e01)  /* bit[7:0], long frame exposure in unit of rows */
#define     SC910GS_LONG_EXP_LINE_L     (0x3e02)  /* bit[7:0], long frame exposure in unit of rows */
#define     SC910GS_LONG_AGAIN_H        (0x3e08)  /* bit[5:0], real gain[13:8] long frame */
#define     SC910GS_LONG_AGAIN_L        (0x3e09)  /* bit[7:0], real gain[7:0] long frame */
#define     SC910GS_LONG_DGAIN_H        (0x3e06)  /* bit[3:0], real gain[13:10] long frame */
#define     SC910GS_LONG_DGAIN_L        (0x3e07)  /* bit[7:0], real gain[9:2] long frame */

#define     SC910GS_SHORT_EXP_LINE_H    (0x0000)  /* bit[7:0], short frame exposure in unit of rows */
#define     SC910GS_SHORT_EXP_LINE_L    (0x0000)  /* bit[7:0], short frame exposure in unit of rows */
#define     SC910GS_SHORT_AGAIN_H       (0x0000)  /* bit[5:0], real gain[13:8] short frame */
#define     SC910GS_SHORT_AGAIN_L       (0x0000)  /* bit[7:0], real gain[7:0] short frame */
#define     SC910GS_SHORT_DGAIN_H       (0x0000)  /* bit[3:0], real gain[13:10] short frame */
#define     SC910GS_SHORT_DGAIN_L       (0x0000)  /* bit[7:0], real gain[9:2] short frame */

#define     SC910GS_VTS_H               (0x0000)  /* bit[7:0], vts[15:8] */
#define     SC910GS_VTS_L               (0x0000)  /* bit[7:0], vts[7:0] */

#define     SC910GS_REG_331f            (0x331f)  /* bit[7:0], exposure effect*/
#define     SC910GS_REG_3385            (0x3385)  /* bit[7:0], exposure effect*/

AX_S32 sc910gs_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio);
AX_S32 sc910gs_sensor_i2c_init(ISP_PIPE_ID nPipeId);
AX_S32 sc910gs_sensor_i2c_exit(ISP_PIPE_ID nPipeId);
AX_S32 sc910gs_sensor_spi_init(ISP_PIPE_ID nPipeId);
AX_S32 sc910gs_sensor_spi_exit(ISP_PIPE_ID nPipeId);
AX_S32 sc910gs_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData);
AX_S32 sc910gs_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr);
AX_S32 sc910gs_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data);

AX_U32 sc910gs_get_hts(ISP_PIPE_ID nPipeId);
AX_F32 sc910gs_get_sclk(ISP_PIPE_ID nPipeId);
AX_U32 sc910gs_get_vts(ISP_PIPE_ID nPipeId);
AX_U32 sc910gs_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts);
AX_U32 sc910gs_get_vs_hts(ISP_PIPE_ID nPipeId);

AX_S32 sc910gs_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts);

AX_S32 sc910gs_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt);
AX_S32 sc910gs_write_settings(ISP_PIPE_ID nPipeId);

AX_S32 sc910gs_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo);
AX_S32 sc910gs_get_bus_num(ISP_PIPE_ID nPipeId);

#endif  //end __SC910GS_REG_H__
