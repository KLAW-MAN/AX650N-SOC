/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SENSOR_IMX678_REG_H__
#define __SENSOR_IMX678_REG_H__

#include "ax_base_type.h"

#define IMX678_SLAVE_ADDR      (0x1a)    /**< i2c slave address of the OS08a20 camera sensor */
#define IMX678_SENSOR_CHIP_ID  (678)
#define IMX678_ADDR_BYTE       (2)
#define IMX678_DATA_BYTE       (1)
#define IMX678_SWAP_BYTES      (1)
#define IMX678_INCK_24M        (24)

#define IMX678_INCLK_74POINT25M         (74.25)
#define IMX678_ALLPIXEL_BRL             (2200)
#define IMX678_10BIT_T_OFFSET           (0)
#define IMX678_MAX_VMAX                 (0xFFFFF)

/* Exposure control */
#define IMX678_SHR0_L               (0x3050)  /* [7:0] */
#define IMX678_SHR0_M               (0x3051)  /* [7:0] */
#define IMX678_SHR0_H               (0x3052)  /* [3:0] */

#define IMX678_SHR1_L               (0x3054)  /* [7:0] */
#define IMX678_SHR1_M               (0x3055)  /* [7:0] */
#define IMX678_SHR1_H               (0x3056)  /* [3:0] */

#define IMX678_RHS1_L               (0x3060)  /* [7:0] */
#define IMX678_RHS1_M               (0x3061)  /* [7:0] */
#define IMX678_RHS1_H               (0x3062)  /* [3:0] */

/* gain control */
#define IMX678_GAIN_PGC_FIDMD       (0x3400)  /* [7:0]  0:Valid, 1:Invalid */
#define IMX678_GAIN0_L              (0x3070)  /* [7:0] */
#define IMX678_GAIN0_H              (0x3071)  /* [2:0] */
#define IMX678_GAIN1_L              (0x3072)  /* [7:0] */
#define IMX678_GAIN1_H              (0x3073)  /* [2:0] */

#define IMX678_VMAX_L               (0x3028)  /* [7:0] */
#define IMX678_VMAX_M               (0x3029)  /* [7:0] */
#define IMX678_VMAX_H               (0x302A)  /* [3:0] */

#define IMX678_HMAX_L               (0x302C)  /* [7:0] */
#define IMX678_HMAX_H               (0x302D)  /* [7:0] */

/* LCG/HCG control */
#define IMX678_FDG_SEL0             (0x3030)  /* [1:0]  1:HCG,  0:LCG */
#define IMX678_FDG_SEL1             (0x3031)  /* [1:0]  1:HCG,  0:LCG */

/* readout direction */
#define IMX678_HREVERSE             (0x3020)  /* [0:0]  1:Inverted,  0:Normal */
#define IMX678_VREVERSE             (0x3021)  /* [0:0]  1:Inverted,  0:Normal */

/* Sensor information */
#define IMX678_SEENSOR_ID_L         (0x4D1C)  /* [7:0] */
#define IMX678_SEENSOR_ID_H         (0x4D1D)  /* [1:0] */

/* test pattern */
#define IMX678_TPG_EN_DUOUT         (0x30E0)  /* [0:0] */
#define IMX678_TPG_PATSEL_DUOUT     (0x30E2)  /* [4:0] */

#define IMX678_MAX_RATIO       (32.0f)
#define IMX678_MIN_RATIO       (1.0f)

/* VTS 4LINE */
#define IMX678_VMAX_10BIT_8M30_SDR             (0x0008CA)
#define IMX678_VMAX_10BIT_8M25_SDR             (0x000A8C)

#define IMX678_VMAX_10BIT_8M60_SDR             (0x0008CA)

#define IMX678_VMAX_12BIT_8M30_SDR             (0x0008CA)
#define IMX678_VMAX_12BIT_8M25_SDR             (0x000A8C)

#define IMX678_VMAX_10BIT_8M30_HDR_2X          (0x0008CA)
#define IMX678_VMAX_10BIT_8M25_HDR_2X          (0x000A8C)

AX_S32 imx678_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio);
AX_S32 imx678_sensor_i2c_init(ISP_PIPE_ID nPipeId);
AX_S32 imx678_sensor_i2c_exit(ISP_PIPE_ID nPipeId);
AX_S32 imx678_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData);
AX_S32 imx678_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr);
AX_S32 imx678_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data);

AX_U32 imx678_get_vmax(ISP_PIPE_ID nPipeId);
AX_U32 imx678_get_hmax(ISP_PIPE_ID nPipeId);

AX_S32 imx678_get_hmax_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *hmax);
AX_S32 imx678_get_vmax_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vmax);
AX_S32 imx678_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt);
AX_S32 imx678_write_settings(ISP_PIPE_ID nPipeId);

AX_S32 imx678_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo);
AX_S32 imx678_get_bus_num(ISP_PIPE_ID nPipeId);

#endif

