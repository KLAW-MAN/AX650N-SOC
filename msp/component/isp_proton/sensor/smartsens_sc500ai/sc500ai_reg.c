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

#include "sc500ai.h"
#include "sc500ai_reg.h"
#include "sc500ai_ae_ctrl.h"
#include "sc500ai_settings.h"

AX_S32 sc500ai_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData)
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

AX_S32 sc500ai_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr)
{
    AX_U8 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd)
        return -1;

    i2c_read(sns_obj->sns_i2c_obj.sns_i2c_fd,
             sns_obj->sns_i2c_obj.slave_addr,
             addr,
             sns_obj->sns_i2c_obj.address_byte,
             (AX_U8 *)(&data),
             sns_obj->sns_i2c_obj.data_byte,
             sns_obj->sns_i2c_obj.swap_byte);

    return data;
}

AX_S32 sc500ai_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd) {
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    nRet = i2c_write(sns_obj->sns_i2c_obj.sns_i2c_fd, sns_obj->sns_i2c_obj.slave_addr, addr,
                     sns_obj->sns_i2c_obj.address_byte,
                     (AX_U8 *)(&data), sns_obj->sns_i2c_obj.data_byte, sns_obj->sns_i2c_obj.swap_byte);

    return nRet;
}

AX_U32 sc500ai_get_vts(ISP_PIPE_ID nPipeId)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U32 vts = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    vts_h = sc500ai_reg_read(nPipeId, SC500AI_VTS_H);
    vts_l = sc500ai_reg_read(nPipeId, SC500AI_VTS_L);

    vts = (vts_h & 0x7F) << 8 | vts_l;

    return vts;
}

AX_S32 sc500ai_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    vts_h = (vts & 0x7F00) >> 8;
    vts_l = vts & 0xFF;

    ret |= sc500ai_update_regidx_table(nPipeId, SC500AI_VTS_H_IDX, vts_h);
    ret |= sc500ai_update_regidx_table(nPipeId, SC500AI_VTS_L_IDX, vts_l);

    return ret;
}

AX_U32 sc500ai_set_vts_s(ISP_PIPE_ID nPipeId, AX_U32 vts_s)
{
    AX_U8 vts_s_h = 0;
    AX_U8 vts_s_l = 0;
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    vts_s_h = (vts_s & 0xFF00) >> 8;
    vts_s_l = vts_s & 0xFF;

    ret |= sc500ai_update_regidx_table(nPipeId, SC500AI_VTS_S_H_IDX, vts_s_h);
    ret |= sc500ai_update_regidx_table(nPipeId, SC500AI_VTS_S_L_IDX, vts_s_l);

    return ret;
}

AX_S32 sc500ai_set_int_t_l(ISP_PIPE_ID nPipeId, AX_U32 int_t)
{
    AX_S32 ret = 0;
    AX_U8 int_t_h = 0;
    AX_U8 int_t_m = 0;
    AX_U8 int_t_l = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    int_t_h = (int_t & 0xF000) >> 12;
    int_t_m = (int_t & 0xFF0) >> 4;
    int_t_l = (int_t & 0xF) << 4;

    sc500ai_update_regidx_table(nPipeId, SC500AI_EXP_L_H_IDX, int_t_h);
    sc500ai_update_regidx_table(nPipeId, SC500AI_EXP_L_M_IDX, int_t_m);
    sc500ai_update_regidx_table(nPipeId, SC500AI_EXP_L_L_IDX, int_t_l);

    return ret;
}

AX_S32 sc500ai_set_int_t_s(ISP_PIPE_ID nPipeId, AX_U32 int_t)
{
    AX_S32 ret = 0;
    AX_U8 int_t_h = 0;
    AX_U8 int_t_m = 0;
    AX_U8 int_t_l = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    int_t_h = (int_t & 0xF000) >> 12;
    int_t_m = (int_t & 0xFF0) >> 4;
    int_t_l = (int_t & 0xF) << 4;

    sc500ai_update_regidx_table(nPipeId, SC500AI_EXP_S_H_IDX, int_t_h);
    sc500ai_update_regidx_table(nPipeId, SC500AI_EXP_S_M_IDX, int_t_m);
    sc500ai_update_regidx_table(nPipeId, SC500AI_EXP_S_L_IDX, int_t_l);

    return ret;
}

AX_S32 sc500ai_set_again_l(ISP_PIPE_ID nPipeId, AX_U8 gain_in, AX_U8 gain_de)
{
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sc500ai_update_regidx_table(nPipeId, SC500AI_AGAIN_L_IDX, gain_in);
    sc500ai_update_regidx_table(nPipeId, SC500AI_AGAIN_L_FINE_IDX, gain_de);

    return ret;
}

AX_S32 sc500ai_set_again_s(ISP_PIPE_ID nPipeId, AX_U8 gain_in, AX_U8 gain_de)
{
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sc500ai_update_regidx_table(nPipeId, SC500AI_AGAIN_S_IDX, gain_in);
    sc500ai_update_regidx_table(nPipeId, SC500AI_AGAIN_S_FINE_IDX, gain_de);

    return ret;
}

AX_S32 sc500ai_set_dgain_l(ISP_PIPE_ID nPipeId, AX_U8 gain_in, AX_U8 gain_de)
{
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sc500ai_update_regidx_table(nPipeId, SC500AI_DGAIN_L_IDX, gain_in);
    sc500ai_update_regidx_table(nPipeId, SC500AI_DGAIN_L_FINE_IDX, gain_de);

    return ret;
}

AX_S32 sc500ai_set_dgain_s(ISP_PIPE_ID nPipeId, AX_U8 gain_in, AX_U8 gain_de)
{
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sc500ai_update_regidx_table(nPipeId, SC500AI_DGAIN_S_IDX, gain_in);
    sc500ai_update_regidx_table(nPipeId, SC500AI_DGAIN_S_FINE_IDX, gain_de);

    return ret;
}

AX_U32 sc500ai_get_hts(ISP_PIPE_ID nPipeId)
{
    AX_U8 hts_l = 0;
    AX_U8 hts_h = 0;
    AX_U32 hts = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    hts_h = sc500ai_reg_read(nPipeId, SC500AI_HTS_L_H);
    hts_l = sc500ai_reg_read(nPipeId, SC500AI_HTS_L_L);

    hts = hts_h << 8 | hts_l;

    return hts;
}

AX_F32 sc500ai_get_exp_offset(ISP_PIPE_ID nPipeId)
{
    AX_F32 offset = 0.0f;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    offset += (sc500ai_reg_read(nPipeId, 0x3301) & 0x3) * 2.0f;
    offset += sc500ai_reg_read(nPipeId, 0x3302);
    offset += (sc500ai_reg_read(nPipeId, 0x3303) & 0x1f);
    offset += sc500ai_reg_read(nPipeId, 0x3304);
    offset += ((sc500ai_reg_read(nPipeId, 0x3305) & 0xFF) << 8) |
              (sc500ai_reg_read(nPipeId, 0x3306) & 0xFF);
    offset += (sc500ai_reg_read(nPipeId, 0x3307) & 0x1f);
    offset += sc500ai_reg_read(nPipeId, 0x3308) * 2.0f;
    offset += sc500ai_reg_read(nPipeId, 0x330d);

    offset /= (AX_F32)sc500ai_get_hts(nPipeId);

    return offset;
}

AX_S32 sc500ai_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts)
{
    AX_U32 i = 0;
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U8 mask = 0;

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == SC500AI_VTS_H) {
            vts_h = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == SC500AI_VTS_L) {
            vts_l = (setting + i)->value;
            mask |= 1 << 1;
        }

        if(mask == 0x3) break;
    }

    //default value
    if((mask & 0x1) == 0) {
        vts_h = 0x6;
        mask |= 1;
    }
    if((mask & (1 << 1)) == 0) {
        vts_l = 0x72;
        mask |= 1 << 1;
    }

    if(mask != 0x3) {
        SNS_ERR("get setting vts fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *vts = (vts_h & 0x7F) << 8 | vts_l;

    return SNS_SUCCESS;
}

AX_S32 sc500ai_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    switch (sns_obj->eImgMode) {
        case E_SC500AI_MIPI_4LANE_24M_396MBPS_2880x1620_10BIT_SDR_30FPS:
            *setting = SC500AI_MIPI_4LANE_24M_396MBPS_2880x1620_10BIT_SDR_30FPS;
            *cnt = sizeof(SC500AI_MIPI_4LANE_24M_396MBPS_2880x1620_10BIT_SDR_30FPS) / sizeof(camera_i2c_reg_array);
            break;

        case E_SC500AI_MIPI_4LANE_24M_810MBPS_2880x1620_10BIT_HDR_30FPS:
            *setting = SC500AI_MIPI_4LANE_24M_810MBPS_2880x1620_10BIT_HDR_30FPS;
            *cnt = sizeof(SC500AI_MIPI_4LANE_24M_810MBPS_2880x1620_10BIT_HDR_30FPS) / sizeof(camera_i2c_reg_array);
            break;

        case E_SC500AI_MIPI_4LANE_24M_396MBPS_2880x1620_8BIT_SDR_30FPS:
            *setting = SC500AI_MIPI_4LANE_24M_396MBPS_2880x1620_8BIT_SDR_30FPS;
            *cnt = sizeof(SC500AI_MIPI_4LANE_24M_396MBPS_2880x1620_8BIT_SDR_30FPS) / sizeof(camera_i2c_reg_array);
            break;

        case E_SC500AI_MIPI_4LANE_24M_810MBPS_2880x1620_8BIT_HDR_30FPS:
            *setting = SC500AI_MIPI_4LANE_24M_810MBPS_2880x1620_8BIT_HDR_30FPS;
            *cnt = sizeof(SC500AI_MIPI_4LANE_24M_810MBPS_2880x1620_8BIT_HDR_30FPS) / sizeof(camera_i2c_reg_array);
            break;

        default:
            SNS_ERR("it's not supported. pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SNS_INFO("pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);

    return SNS_SUCCESS;
}

AX_S32 sc500ai_write_settings(ISP_PIPE_ID nPipeId)
{
    AX_U32 i = 0;
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;

    ret = sc500ai_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    for (i = 0; i < reg_cnt; i++) {
        sc500ai_write_register(nPipeId, (setting + i)->addr, ((setting + i)->value));
#ifdef SENSOR_DEBUG
        usleep(2 * 1000);
        AX_U8 val = sc500ai_reg_read(nPipeId, (default_setting + i)->addr);
        SNS_DBG(" addr: 0x%04x write:0x%02x read:0x%02x \n",
                (default_setting + i)->addr, (default_setting + i)->value, val);
#endif
    }

    return SNS_SUCCESS;
}