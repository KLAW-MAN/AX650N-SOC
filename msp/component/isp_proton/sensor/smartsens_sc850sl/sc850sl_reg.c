/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ax_base_type.h"
#include "ax_isp_common.h"

#include "i2c.h"
#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"

#include "sc850sl.h"
#include "sc850sl_reg.h"
#include "sc850sl_settings.h"


AX_S32 sc850sl_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData)
{
    AX_U8 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd)
        return -1;

    nRet = i2c_read(sns_obj->sns_i2c_obj.sns_i2c_fd,
             sns_obj->sns_i2c_obj.slave_addr,
             nAddr,
             sns_obj->sns_i2c_obj.address_byte,
             (AX_U8 *)(pData),
             sns_obj->sns_i2c_obj.data_byte,
             sns_obj->sns_i2c_obj.swap_byte);

    return nRet;
}

AX_S32 sc850sl_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr)
{
    AX_U8 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd)
        return SNS_ERR_CODE_FAILED;

    i2c_read(sns_obj->sns_i2c_obj.sns_i2c_fd,
             sns_obj->sns_i2c_obj.slave_addr,
             addr,
             sns_obj->sns_i2c_obj.address_byte,
             (AX_U8 *)(&data),
             sns_obj->sns_i2c_obj.data_byte,
             sns_obj->sns_i2c_obj.swap_byte);

    return data;
}

AX_S32 sc850sl_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd) {
        return -1;
    }

    nRet = i2c_write(sns_obj->sns_i2c_obj.sns_i2c_fd, sns_obj->sns_i2c_obj.slave_addr, addr,
                     sns_obj->sns_i2c_obj.address_byte,
                     (AX_U8 *)(&data), sns_obj->sns_i2c_obj.data_byte, sns_obj->sns_i2c_obj.swap_byte);

    return nRet;
}

AX_S32 sc850sl_update_regidx_table(ISP_PIPE_ID nPipeId, AX_U8 nRegIdx, AX_U8 nRegValue)
{
    AX_S32 i = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sns_obj->sztRegsInfo[0].sztData.sztI2cData[nRegIdx].nData = nRegValue;

    SNS_DBG("Idx = %d, reg addr 0x%x, reg data 0x%x\n",
        nRegIdx, sns_obj->sztRegsInfo[0].sztData.sztI2cData[nRegIdx].nRegAddr, nRegValue);

    return SNS_SUCCESS;
}

AX_S32 sc850sl_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts)
{
    AX_U8 vts_h;
    AX_U8 vts_l;
    AX_S32 result = 0;
    AX_U8 rb_rows_h;
    AX_U8 rb_rows_l;
    AX_U32 rb_rows = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    vts_l = vts & 0xFF;
    vts_h = (vts & 0xFF00) >> 8;

    result |= sc850sl_update_regidx_table(nPipeId, SC850SL_VTS_L_H_IDX, vts_h);
    result |= sc850sl_update_regidx_table(nPipeId, SC850SL_VTS_L_L_IDX, vts_l);

    if (sns_obj->sns_mode_obj.eHDRMode == AX_SNS_HDR_2X_MODE) {
        rb_rows = vts - 1;
        rb_rows_l = rb_rows & 0xFF;
        rb_rows_h = (rb_rows & 0xFF00) >> 8;
        result |= sc850sl_update_regidx_table(nPipeId, SC850SL_RB_ROWS_H_IDX, rb_rows_h);
        result |= sc850sl_update_regidx_table(nPipeId, SC850SL_RB_ROWS_L_IDX, rb_rows_l);
    }

    return result;
}

AX_S32 sc850sl_set_vts_s(ISP_PIPE_ID nPipeId, AX_U32 vts_s)
{
    AX_U8 vts_s_h = 0;
    AX_U8 vts_s_l = 0;
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    vts_s_h = (vts_s & 0xFF00) >> 8;
    vts_s_l = vts_s & 0xFF;

    result |= sc850sl_update_regidx_table(nPipeId, SC850SL_VTS_S_H_IDX, vts_s_h);
    result |= sc850sl_update_regidx_table(nPipeId, SC850SL_VTS_S_L_IDX, vts_s_l);

    return result;
}

AX_S32 sc850sl_get_vts(ISP_PIPE_ID nPipeId)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U32 vts = 0;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    vts_h = sc850sl_reg_read(nPipeId, SC850SL_VTS_L_H);
    vts_l = sc850sl_reg_read(nPipeId, SC850SL_VTS_L_L);

    vts = vts_h << 8 | vts_l;

    return vts;
}

AX_S32 sc850sl_get_vts_s(ISP_PIPE_ID nPipeId)
{
    AX_U8 vts_s_h = 0;
    AX_U8 vts_s_l = 0;
    AX_U32 vts_s = 0;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    vts_s_h = sc850sl_reg_read(nPipeId, SC850SL_VTS_S_H);
    vts_s_l = sc850sl_reg_read(nPipeId, SC850SL_VTS_S_L);

    vts_s = vts_s_h << 8 | vts_s_l;

    return vts_s;
}

AX_S32 sc850sl_set_int_t_l(ISP_PIPE_ID nPipeId, AX_U32 int_t)
{
    AX_S32 ret = 0;
    AX_U8 int_t_h = 0;
    AX_U8 int_t_m = 0;
    AX_U8 int_t_l = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    int_t_h = (int_t & 0xF000) >> 12;
    int_t_m = (int_t & 0xFF0) >> 4;
    int_t_l = (int_t & 0xF) << 4;

    ret |= sc850sl_update_regidx_table(nPipeId, SC850SL_EXPLINE_LONG_H_IDX, int_t_h);
    ret |= sc850sl_update_regidx_table(nPipeId, SC850SL_EXPLINE_LONG_M_IDX, int_t_m);
    ret |= sc850sl_update_regidx_table(nPipeId, SC850SL_EXPLINE_LONG_L_IDX, int_t_l);

    return ret;
}

AX_S32 sc850sl_set_int_t_s(ISP_PIPE_ID nPipeId, AX_U32 int_t)
{
    AX_U8 int_t_h;
    AX_U8 int_t_m;
    AX_U8 int_t_l;
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    int_t_h = (int_t & 0xF000) >> 12;
    int_t_m = (int_t & 0xFF0) >> 4;
    int_t_l = (int_t & 0xF) << 4;

    ret |= sc850sl_update_regidx_table(nPipeId, SC850SL_EXPLINE_SHORT_H_IDX, int_t_h);
    ret |= sc850sl_update_regidx_table(nPipeId, SC850SL_EXPLINE_SHORT_M_IDX, int_t_m);
    ret |= sc850sl_update_regidx_table(nPipeId, SC850SL_EXPLINE_SHORT_L_IDX, int_t_l);

    return ret;
}

AX_S32 sc850sl_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts)
{
    AX_U32 i = 0;
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U8 mask = 0;

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == SC850SL_VTS_L_H) {
            vts_h = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == SC850SL_VTS_L_L) {
            vts_l = (setting + i)->value;
            mask |= 1 << 1;
        }

        if(mask == 0x3) break;
    }

    //default value
    if((mask & 0x1) == 0) {
        vts_h = 0x8;
        mask |= 1;
    }
    if((mask & (1 << 1)) == 0) {
        vts_l = 0xca;
        mask |= 1 << 1;
    }

    if(mask != 0x3) {
        SNS_ERR("get setting vts fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *vts = vts_h << 8 | vts_l;

    return SNS_SUCCESS;
}

AX_S32 sc850sl_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    switch (sns_obj->eImgMode) {
        case E_SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_SDR_10BIT_60FPS_1404MBPS:
            *setting = SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_SDR_10BIT_60FPS_1404MBPS;
            *cnt = sizeof(SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_SDR_10BIT_60FPS_1404MBPS) / sizeof(camera_i2c_reg_array);
            break;

        case E_SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_HDR_10BIT_30FPS_1458MBPS:
            *setting = SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_HDR_10BIT_30FPS_1458MBPS;
            *cnt = sizeof(SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_HDR_10BIT_30FPS_1458MBPS) / sizeof(camera_i2c_reg_array);
            break;

        default:
            SNS_ERR("it's not supported. pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SNS_INFO("pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);

    return SNS_SUCCESS;
}

AX_S32 sc850sl_write_settings(ISP_PIPE_ID nPipeId)
{
    AX_U32 i = 0;
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;

    ret = sc850sl_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    for (i = 0; i < reg_cnt; i++) {
        sc850sl_write_register(nPipeId, (setting + i)->addr, ((setting + i)->value));
#ifdef SENSOR_DEBUG
        usleep(2 * 1000);
        AX_U8 val = sc850sl_reg_read(nPipeId, (default_setting + i)->addr);
        SNS_DBG(" addr: 0x%04x write:0x%02x read:0x%02x \n",
                (default_setting + i)->addr, (default_setting + i)->value, val);
#endif
    }

    return SNS_SUCCESS;
}
