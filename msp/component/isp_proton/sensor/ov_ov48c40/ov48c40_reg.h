/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OV48C40_REG_H__
#define __OV48C40_REG_H__

#include "ax_base_type.h"

#define OV48C40_SLAVE_ADDR1      (0x36)    /**< i2c slave address of the OS04a10 camera sensor */
#define OV48C40_SLAVE_ADDR2      (0x10)    /**< i2c slave address of the OS04a10 camera sensor */
#define OV48C40_SENSOR_CHIP_ID   (0x564843)
#define OV48C40_ADDR_BYTE        (2)
#define OV48C40_DATA_BYTE        (1)
#define OV48C40_SWAP_BYTES       (1)
#define OV48C40_INCK_24M         (24)

/* Exposure control related registers */
#define OV48C40_LONG_EXP_LINE_H     (0x3500)  /* bit[7:0], long frame exposure [23:16] in unit of rows */
#define OV48C40_LONG_EXP_LINE_M     (0x3501)  /* bit[7:0], long frame exposure [15:8] in unit of rows */
#define OV48C40_LONG_EXP_LINE_L     (0x3502)  /* bit[7:0], long frame exposure [7:0] in unit of rows */
#define OV48C40_LONG_AGAIN_H        (0x3508)  /* bit[6:0], real gain coarse [6:0] long frame */
#define OV48C40_LONG_AGAIN_L        (0x3509)  /* bit[7:1], real gain fine [6:0] long frame */
#define OV48C40_LONG_DGAIN_H        (0x350A)  /* bit[3:0], real gain coarse [3:0] long frame */
#define OV48C40_LONG_DGAIN_M        (0x350B)  /* bit[7:0], real gain digital [9:2] long frame */
#define OV48C40_LONG_DGAIN_L        (0x350C)  /* bit[7:6], real gain digital [1:0] long frame */

#define OV48C40_SHORT_EXP_LINE_H    (0x3540)  /* bit[7:0], short frame exposure [23:16] in unit of rows */
#define OV48C40_SHORT_EXP_LINE_M    (0x3541)  /* bit[7:0], short frame exposure [15:8] in unit of rows */
#define OV48C40_SHORT_EXP_LINE_L    (0x3542)  /* bit[7:0], short frame exposure [7:0] in unit of rows */
#define OV48C40_SHORT_AGAIN_H       (0x3548)  /* bit[6:0], real gain coarse[6:0] long frame */
#define OV48C40_SHORT_AGAIN_L       (0x3549)  /* bit[7:1], real gain fine[6:0] long frame */
#define OV48C40_SHORT_DGAIN_H       (0x354A)  /* bit[3:0], real gain coarse [3:0] short frame */
#define OV48C40_SHORT_DGAIN_M       (0x354B)  /* bit[7:0], real gain digital [9:2] short frame */
#define OV48C40_SHORT_DGAIN_L       (0x354C)  /* bit[7:6], real gain digital [1:0] short frame */

#define OV48C40_VTS_H               (0x380E)  /* bit[7:0], vts[15:8] */
#define OV48C40_VTS_L               (0x380F)  /* bit[7:0], vts[7:0] */

#define OV48C40_GROUP_ACCESS1       (0x3208)  /* */
#define OV48C40_HCG_LCG_REG0        (0x37A0)  /* bit[2:0] = b000, LCG; bit[2:0] = b111, HCG */
#define OV48C40_GROUP_ACCESS2       (0x3208)  /* */
#define OV48C40_GROUP_ACCESS3       (0x3208)  /* */

#define OV48C40_VTS_10BIT_4M30_SDR  (0x1A0A)
#define OV48C40_VTS_10BIT_4M25_SDR  (0x1F40)

#define OV48C40_STREAM_CTRL_REG     (0X0100)

AX_S32 ov48c40_sensor_i2c_init(ISP_PIPE_ID nPipeId);
AX_S32 ov48c40_sensor_i2c_exit(ISP_PIPE_ID nPipeId);

AX_S32 ov48c40_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio);

AX_S32 ov48c40_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData);
AX_S32 ov48c40_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr);
AX_S32 ov48c40_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data);

AX_U32 ov48c40_get_hts(ISP_PIPE_ID nPipeId);
AX_F32 ov48c40_get_sclk(ISP_PIPE_ID nPipeId);
AX_U32 ov48c40_get_vts(ISP_PIPE_ID nPipeId);
AX_U32 ov48c40_get_vs_hts(ISP_PIPE_ID nPipeId);

AX_S32 ov48c40_write_settings(ISP_PIPE_ID nPipeId, AX_U32 setindex);

AX_S32 ov48c40_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo);
AX_S32 ov48c40_get_bus_num(ISP_PIPE_ID nPipeId);
AX_S32 ov48c40_set_slaveaddr(ISP_PIPE_ID nPipeId, AX_U8 nslaveaddr);

#endif  //end __OV48C40_REG_H__
