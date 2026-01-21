/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OS08B10_REG_H__
#define __OS08B10_REG_H__

#include "ax_base_type.h"


#define OS08B10_SLAVE_ADDR      (0x36)    /**< i2c slave address of the OS08B10 camera sensor */
#define OS08B10_SENSOR_CHIP_ID  (0x530842)
#define OS08B10_ADDR_BYTE       (2)
#define OS08B10_DATA_BYTE       (1)
#define OS08B10_SWAP_BYTES      (1)
#define OS08B10_INCK_24M        (24)

/* Exposure control related registers */
#define     OS08B10_LONG_EXP_LINE_H     (0x3501)  /* bit[7:0], long frame exposure in unit of rows */
#define     OS08B10_LONG_EXP_LINE_L     (0x3502)  /* bit[7:0], long frame exposure in unit of rows */
#define     OS08B10_LONG_AGAIN_H        (0x3508)  /* bit[4:0], real gain[8:4] long frame */
#define     OS08B10_LONG_AGAIN_L        (0x3509)  /* bit[7:4], real gain[3:0] long frame */
#define     OS08B10_LONG_DGAIN_H        (0x350A)  /* bit[3:0], real gain[13:10] long frame */
#define     OS08B10_LONG_DGAIN_M        (0x350B)  /* bit[7:0], real gain[9:2] long frame */
#define     OS08B10_LONG_DGAIN_L        (0x350C)  /* bit[7:6], real gain[1:0] long frame */

#define     OS08B10_SHORT_EXP_LINE_H    (0x3541)  /* bit[7:0], short frame exposure in unit of rows */
#define     OS08B10_SHORT_EXP_LINE_L    (0x3542)  /* bit[7:0], short frame exposure in unit of rows */
#define     OS08B10_SHORT_AGAIN_H       (0x3548)  /* bit[4:0], real gain[8:4] short frame */
#define     OS08B10_SHORT_AGAIN_L       (0x3549)  /* bit[7:4], real gain[3:0] short frame */
#define     OS08B10_SHORT_DGAIN_H       (0x354A)  /* bit[3:0], real gain[13:10] short frame */
#define     OS08B10_SHORT_DGAIN_M       (0x354B)  /* bit[7:0], real gain[9:2] short frame */
#define     OS08B10_SHORT_DGAIN_L       (0x354C)  /* bit[7:6], real gain[1:0] short frame */

#define     OS08B10_VS_EXP_LINE_H       (0x3581)  /* bit[7:0], very short frame exposure in unit of rows */
#define     OS08B10_VS_EXP_LINE_L       (0x3582)  /* bit[7:0], very short frame exposure in unit of rows */
#define     OS08B10_VS_AGAIN_H          (0x3588)  /* bit[4:0], real gain[8:4] very short frame */
#define     OS08B10_VS_AGAIN_L          (0x3589)  /* bit[7:4], real gain[3:0] very short frame */
#define     OS08B10_VS_DGAIN_H          (0x358A)  /* bit[3:0], real gain[13:10] very short frame */
#define     OS08B10_VS_DGAIN_M          (0x358B)  /* bit[7:0], real gain[9:2] very short frame */
#define     OS08B10_VS_DGAIN_L          (0x358C)  /* bit[7:6], real gain[1:0] very short frame */

#define     OS08B10_VTS_H               (0x380E)  /* bit[15:8] */
#define     OS08B10_VTS_L               (0x380F)  /* bit[7:0] */

#define     OS08B10_GRP0_HLD_START      (0x3208)  /* 0x00: group 0 hold start */
#define     OS08B10_HCG_LCG             (0x376C)  /* bit 4 5 6 = L M S*/
#define     OS08B10_GRP0_HLD_END        (0x3208)  /* 0x10: group 0 hold end */
#define     OS08B10_GRP0_HLD_DELAY      (0x3208)  /* 0xA0: group 0 delay launch */

AX_S32 os08b10_sensor_i2c_init(ISP_PIPE_ID nPipeId);
AX_S32 os08b10_sensor_i2c_exit(ISP_PIPE_ID nPipeId);

AX_S32 os08b10_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio);

AX_S32 os08b10_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData);
AX_S32 os08b10_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr);
AX_S32 os08b10_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data);

AX_U32 os08b10_get_hts(ISP_PIPE_ID nPipeId);
AX_F32 os08b10_get_sclk(ISP_PIPE_ID nPipeId);
AX_F32 os08b10_get_vts(ISP_PIPE_ID nPipeId);
AX_U32 os08b10_get_vs_hts(ISP_PIPE_ID nPipeId);

AX_S32 os08b10_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts);
AX_S32 os08b10_get_hts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *hts);
AX_S32 os08b10_get_sclk_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_F32 *sclk);

AX_S32 os08b10_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt);
AX_S32 os08b10_write_settings(ISP_PIPE_ID nPipeId);

AX_S32 os08b10_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo);
AX_S32 os08b10_get_bus_num(ISP_PIPE_ID nPipeId);

#endif  //end __OS08B10_REG_H__
