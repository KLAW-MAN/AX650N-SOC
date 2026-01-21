/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ax_ftc_config.h"
#include "ax_ftc_priv.h"

AX_S32 isp_sif_sensor_shadow_en(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_BOOL enable)
{
    sen_shadow_cfg_s sen_shadow_cfg = {0};

    isp_sif_sen_shadow_cfg_get(io_handle, &sen_shadow_cfg);
    if (sensor_id == 0) {
        sen_shadow_cfg.sen_shadow_en0 = enable;
        sen_shadow_cfg.sen_shadow_wr_id0 += 1;
        sen_shadow_cfg.sen_shadow_en1 = enable;
    } else if (sensor_id == 1) {
        sen_shadow_cfg.sen_shadow_en1 = enable;
        sen_shadow_cfg.sen_shadow_wr_id1 += 1;
    }
    isp_sif_sen_shadow_cfg_set(io_handle, &sen_shadow_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_internal_trig(regio_global_handle_t io_handle, AX_BOOL enable)
{

    isp_sif_sen_internal_trig_cfg_set(io_handle, enable);

    return 0;
}

AX_S32 isp_sif_sensor_expo_ctrl_enable(regio_global_handle_t io_handle, AX_BOOL enable)
{
    sen_ctrl_cfg_s sen_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_ctrl_cfg_get(io_handle, &sen_ctrl_cfg);
    sen_ctrl_cfg.sen_ctrl_en = enable;
    isp_sif_sen_ctrl_cfg_set(io_handle, &sen_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_expo_ctrl_mode(regio_global_handle_t io_handle, ax_vin_sync_trigger_mode_e  trigger_mode)
{
    sen_ctrl_cfg_s sen_ctrl_cfg = {0};
    AX_S32 ret = 0;
    ret = isp_sif_sen_ctrl_cfg_get(io_handle, &sen_ctrl_cfg);
    sen_ctrl_cfg.sen_ctrl_mode = trigger_mode;
    isp_sif_sen_ctrl_cfg_set(io_handle, &sen_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_vsync_active_level_set(regio_global_handle_t io_handle, AX_U8 sensor_id,
                                                ax_sync_polariiy_e  vsync_active_level)
{
    sen_ctrl_cfg_s sen_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_ctrl_cfg_get(io_handle, &sen_ctrl_cfg);
    switch (sensor_id) {
    case 0:
        sen_ctrl_cfg.sen_expo_v0_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_v0_ctrl  |= vsync_active_level << 1;
        sen_ctrl_cfg.sen_expo_v2_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_v2_ctrl  |= vsync_active_level << 1;
        break;
    case 1:
        sen_ctrl_cfg.sen_expo_v1_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_v1_ctrl  |= vsync_active_level << 1;
        sen_ctrl_cfg.sen_expo_v3_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_v3_ctrl  |= vsync_active_level << 1;
        break;
    }
    isp_sif_sen_ctrl_cfg_set(io_handle, &sen_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_hsync_active_level_set(regio_global_handle_t io_handle, AX_U8 sensor_id,
                                                ax_sync_polariiy_e hsync_active_level)
{
    sen_ctrl_cfg_s sen_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_ctrl_cfg_get(io_handle, &sen_ctrl_cfg);
    switch (sensor_id) {
    case 0:
        sen_ctrl_cfg.sen_expo_h0_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_h0_ctrl  |= hsync_active_level << 1;
        sen_ctrl_cfg.sen_expo_h2_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_h2_ctrl  |= hsync_active_level << 1;
        break;
    case 1:
        sen_ctrl_cfg.sen_expo_h1_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_h1_ctrl  |= hsync_active_level << 1;
        sen_ctrl_cfg.sen_expo_h3_ctrl  &= 1;
        sen_ctrl_cfg.sen_expo_h3_ctrl  |= hsync_active_level << 1;
        break;
    }
    isp_sif_sen_ctrl_cfg_set(io_handle, &sen_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_counter_unit_set(regio_global_handle_t io_handle, AX_U8 flash_id, sensor_counter_unit_e counter_unit)
{
    sen_flash_ctrl_cfg_s sen_flash_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_flash_ctrl_cfg_get(io_handle, &sen_flash_ctrl_cfg);
    switch (flash_id) {
    case 0:
        sen_flash_ctrl_cfg.sen_flash0_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash0_ctrl |= counter_unit << 2;
    break;
    case 1:
        sen_flash_ctrl_cfg.sen_flash1_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash1_ctrl |= counter_unit << 2;
    break;
    case 2:
        sen_flash_ctrl_cfg.sen_flash2_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash2_ctrl |= counter_unit << 2;
    break;
    case 3:
        sen_flash_ctrl_cfg.sen_flash3_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash3_ctrl |= counter_unit << 2;
    break;
    case 4:
        sen_flash_ctrl_cfg.sen_flash4_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash4_ctrl |= counter_unit << 2;
    break;
    case 5:
        sen_flash_ctrl_cfg.sen_flash5_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash5_ctrl |= counter_unit << 2;
    break;
    case 6:
        sen_flash_ctrl_cfg.sen_flash6_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash6_ctrl |= counter_unit << 2;
    break;
    case 7:
        sen_flash_ctrl_cfg.sen_flash7_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash7_ctrl |= counter_unit << 2;
    break;
    case 8:
        sen_flash_ctrl_cfg.sen_flash8_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash8_ctrl |= counter_unit << 2;
    break;
    case 9:
        sen_flash_ctrl_cfg.sen_flash9_ctrl &= 3;
        sen_flash_ctrl_cfg.sen_flash9_ctrl |= counter_unit << 2;
    break;
    }

    ret = isp_sif_sen_flash_ctrl_cfg_set(io_handle, &sen_flash_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_flash_en(regio_global_handle_t io_handle, AX_U8 flash_id, AX_BOOL enable)
{
    sen_flash_ctrl_cfg_s sen_flash_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_flash_ctrl_cfg_get(io_handle, &sen_flash_ctrl_cfg);
    switch (flash_id) {
    case 0:
        sen_flash_ctrl_cfg.sen_flash0_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash0_ctrl |= enable;
    break;
    case 1:
        sen_flash_ctrl_cfg.sen_flash1_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash1_ctrl |= enable;
    break;
    case 2:
        sen_flash_ctrl_cfg.sen_flash2_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash2_ctrl |= enable;
    break;
    case 3:
        sen_flash_ctrl_cfg.sen_flash3_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash3_ctrl |= enable;
    break;
    case 4:
        sen_flash_ctrl_cfg.sen_flash4_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash4_ctrl |= enable;
    break;
    case 5:
        sen_flash_ctrl_cfg.sen_flash5_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash5_ctrl |= enable;
    break;
    case 6:
        sen_flash_ctrl_cfg.sen_flash6_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash6_ctrl |= enable;
    break;
    case 7:
        sen_flash_ctrl_cfg.sen_flash7_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash7_ctrl |= enable;
    break;
    case 8:
        sen_flash_ctrl_cfg.sen_flash8_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash8_ctrl |= enable;
    break;
    case 9:
        sen_flash_ctrl_cfg.sen_flash9_ctrl &= 6;
        sen_flash_ctrl_cfg.sen_flash9_ctrl |= enable;
    break;
    }

    ret = isp_sif_sen_flash_ctrl_cfg_set(io_handle, &sen_flash_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_flash_active_level_set(regio_global_handle_t io_handle, AX_U8 flash_id, ax_sync_polariiy_e eInv)
{
    sen_flash_ctrl_cfg_s sen_flash_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_flash_ctrl_cfg_get(io_handle, &sen_flash_ctrl_cfg);
    switch (flash_id) {
    case 0:
        sen_flash_ctrl_cfg.sen_flash0_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash0_ctrl |= eInv << 1;
    break;
    case 1:
        sen_flash_ctrl_cfg.sen_flash1_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash1_ctrl |= eInv << 1;
    break;
    case 2:
        sen_flash_ctrl_cfg.sen_flash2_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash2_ctrl |= eInv << 1;
    break;
    case 3:
        sen_flash_ctrl_cfg.sen_flash3_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash3_ctrl |= eInv << 1;
    break;
    case 4:
        sen_flash_ctrl_cfg.sen_flash4_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash4_ctrl |= eInv << 1;
    break;
    case 5:
        sen_flash_ctrl_cfg.sen_flash5_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash5_ctrl |= eInv << 1;
    break;
    case 6:
        sen_flash_ctrl_cfg.sen_flash6_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash6_ctrl |= eInv << 1;
    break;
    case 7:
        sen_flash_ctrl_cfg.sen_flash7_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash7_ctrl |= eInv << 1;
    break;
    case 8:
        sen_flash_ctrl_cfg.sen_flash8_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash8_ctrl |= eInv << 1;
    break;
    case 9:
        sen_flash_ctrl_cfg.sen_flash9_ctrl &= 5;
        sen_flash_ctrl_cfg.sen_flash9_ctrl |= eInv << 1;
    break;
    }

    ret = isp_sif_sen_flash_ctrl_cfg_set(io_handle, &sen_flash_ctrl_cfg);

    return 0;
}

AX_S32 isp_sif_sensor_vsync_out_en(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_BOOL enable)
{
    sen_ctrl_cfg_s sen_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_ctrl_cfg_get(io_handle, &sen_ctrl_cfg);
    switch (sensor_id) {
    case 0:
        sen_ctrl_cfg.sen_expo_v0_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_v0_ctrl  |= enable;
        sen_ctrl_cfg.sen_expo_v2_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_v2_ctrl  |= enable;
        break;
    case 1:
        sen_ctrl_cfg.sen_expo_v1_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_v1_ctrl  |= enable;
        sen_ctrl_cfg.sen_expo_v3_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_v3_ctrl  |= enable;
        break;
    }
    isp_sif_sen_ctrl_cfg_set(io_handle, &sen_ctrl_cfg);

    return ret;
}

AX_S32 isp_sif_sensor_hsync_out_en(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_BOOL enable)
{
    sen_ctrl_cfg_s sen_ctrl_cfg = {0};
    AX_S32 ret = 0;

    ret = isp_sif_sen_ctrl_cfg_get(io_handle, &sen_ctrl_cfg);
    switch (sensor_id) {
    case 0:
        sen_ctrl_cfg.sen_expo_h0_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_h0_ctrl  |= enable;
        sen_ctrl_cfg.sen_expo_h2_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_h2_ctrl  |= enable;
        break;
    case 1:
        sen_ctrl_cfg.sen_expo_h1_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_h1_ctrl  |= enable;
        sen_ctrl_cfg.sen_expo_h3_ctrl  &= 2;
        sen_ctrl_cfg.sen_expo_h3_ctrl  |= enable;
        break;
    }
    isp_sif_sen_ctrl_cfg_set(io_handle, &sen_ctrl_cfg);

    return ret;
}

AX_S32 isp_sif_sensor_expo_v_set(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_U32 v_num_total, AX_U32 v_num)
{
    if (v_num_total > 0x3fff) {
        isp_err("v_num_total should < 0x4000 ,now is 0x%x\n", v_num_total);
    }

    if (v_num > 0xfff) {
        isp_err("v_num should < 0x1000 ,now is 0x%x\n", v_num);
    }

    isp_sif_sen_expo_v_cfg_set(io_handle, sensor_id, v_num_total & 0x3fff, v_num & 0xfff);

    return 0;
}

AX_S32 isp_sif_sensor_expo_h_set(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_U32 h_num_total, AX_U32 h_num)
{
    if (h_num_total > 0x3fff) {
        isp_err("h_num_total should < 0x4000 ,now is 0x%x\n", h_num_total);
    }

    if (h_num > 0xfff) {
        isp_err("h_num should < 0x1000 ,now is 0x%x\n", h_num);
    }

    isp_sif_sen_expo_h_cfg_set(io_handle, sensor_id, h_num_total & 0x3fff, h_num & 0xfff);

    return 0;
}


AX_S32 isp_sif_sensor_elec_puls_delay_to_vsync_adjust(regio_global_handle_t io_handle, AX_U32 delay_line)
{
    if (delay_line > 0x3fff) {
        isp_err("sensor_elec_puls_delay_to_vsync_adjust should < 0x4000, now is 0x%x\n", delay_line);
    }

    isp_sif_sen_expo_adjust0_cfg_set(io_handle, delay_line & 0x3fff);

    return 0;
}

AX_S32 isp_sif_sensor_elec_puls_start_line_to_trigger_adjust(regio_global_handle_t io_handle, AX_U32 line_start)
{
    AX_U32 sen_expo_adjust_line_start = 0;
    AX_U32 sen_expo_adjust_line_num = 0;

    if (line_start > 0x3fff) {
        isp_err("sen_expo_adjust_line_num should < 0x4000, now is 0x%x\n", line_start);
    }

    isp_sif_sen_expo_adjust1_cfg_get(io_handle, &sen_expo_adjust_line_start, &sen_expo_adjust_line_num);
    sen_expo_adjust_line_start = line_start & 0x3fff;
    isp_sif_sen_expo_adjust1_cfg_set(io_handle, sen_expo_adjust_line_start, sen_expo_adjust_line_num);

    return 0;
}

AX_S32 isp_sif_sensor_elec_puls_line_num_adjust(regio_global_handle_t io_handle, AX_U32 line_num)
{
    AX_U32 sen_expo_adjust_line_start = 0;
    AX_U32 sen_expo_adjust_line_num = 0;

    if (line_num > 0xfff) {
        isp_err("sen_expo_adjust_line_num should < 0x1000, now is 0x%x\n", line_num);
    }
    isp_sif_sen_expo_adjust1_cfg_get(io_handle, &sen_expo_adjust_line_start, &sen_expo_adjust_line_num);
    sen_expo_adjust_line_num = line_num & 0xfff;
    isp_sif_sen_expo_adjust1_cfg_set(io_handle, sen_expo_adjust_line_start, sen_expo_adjust_line_num);

    return 0;
}



AX_S32 isp_sif_sensor_flash_delay_time(regio_global_handle_t io_handle, AX_U8 dev_id, AX_U32 delay_line)
{
    AX_U32 sen_flash_dly_num = 0;
    AX_U32 sen_flash_on_num = 0;

    if (delay_line > 0x3fff) {
        isp_err("isp_sif_sensor_flash_delay_time should < 0x3fff, now is 0x%x\n", delay_line);
    }
    isp_sif_sen_flash_cfg_get(io_handle, dev_id, &sen_flash_dly_num, &sen_flash_on_num);
    sen_flash_dly_num = delay_line & 0x3fff;
    isp_sif_sen_flash_cfg_set(io_handle, dev_id, sen_flash_dly_num, sen_flash_on_num);

    return 0;
}


AX_S32 isp_sif_sensor_flash_active_time(regio_global_handle_t io_handle, AX_U8 dev_id, AX_U32 active_time)
{
    AX_U32 sen_flash_dly_num = 0;
    AX_U32 sen_flash_on_num = 0;

    if (active_time > 0x3ffff) {
        isp_err("_sensor_flash_active_time should < 0x3ffff, now is 0x%x\n", active_time);
    }

    isp_sif_sen_flash_cfg_get(io_handle, dev_id, &sen_flash_dly_num, &sen_flash_on_num);
    sen_flash_on_num = active_time & 0x3ffff;
    isp_sif_sen_flash_cfg_set(io_handle, dev_id, sen_flash_dly_num, sen_flash_on_num);

    return 0;
}
