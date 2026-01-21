/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _ISP_SIF_FTC_API_H_
#define _ISP_SIF_FTC_API_H_
#include "ax_global_type.h"
AX_S32 isp_sif_sensor_shadow_en(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_BOOL enable);
AX_S32 isp_sif_sensor_internal_trig(regio_global_handle_t io_handle, AX_BOOL enable);
AX_S32 isp_sif_sensor_expo_ctrl_enable(regio_global_handle_t io_handle, AX_BOOL enable);
AX_S32 isp_sif_sensor_expo_ctrl_mode(regio_global_handle_t io_handle, ax_vin_sync_trigger_mode_e  trigger_mode);
AX_S32 isp_sif_sensor_vsync_active_level_set(regio_global_handle_t io_handle, AX_U8 sensor_id,
                                                ax_sync_polariiy_e  vsync_active_level);
AX_S32 isp_sif_sensor_hsync_active_level_set(regio_global_handle_t io_handle, AX_U8 sensor_id,
                                                ax_sync_polariiy_e hsync_active_level);
AX_S32 isp_sif_sensor_counter_unit_set(regio_global_handle_t io_handle, AX_U8 flash_id, AX_U8 counter_unit);
AX_S32 isp_sif_sensor_flash_en(regio_global_handle_t io_handle, AX_U8 flash_id, AX_BOOL enable);
AX_S32 isp_sif_sensor_flash_active_level_set(regio_global_handle_t io_handle, AX_U8 flash_id, ax_sync_polariiy_e eInv);
AX_S32 isp_sif_sensor_vsync_out_en(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_BOOL enable);
AX_S32 isp_sif_sensor_hsync_out_en(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_BOOL enable);
AX_S32 isp_sif_sensor_expo_v_set(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_U32 v_num_total, AX_U32 v_num);
AX_S32 isp_sif_sensor_expo_h_set(regio_global_handle_t io_handle, AX_U8 sensor_id, AX_U32 h_num_total, AX_U32 h_num);
AX_S32 isp_sif_sensor_elec_puls_delay_to_vsync_adjust(regio_global_handle_t io_handle, AX_U32 delay_line);
AX_S32 isp_sif_sensor_elec_puls_start_line_to_trigger_adjust(regio_global_handle_t io_handle, AX_U32 line_start);
AX_S32 isp_sif_sensor_elec_puls_line_num_adjust(regio_global_handle_t io_handle, AX_U32 line_num);
AX_S32 isp_sif_sensor_flash_delay_time(regio_global_handle_t io_handle, AX_U8 dev_id, AX_U32 delay_line);
AX_S32 isp_sif_sensor_flash_active_time(regio_global_handle_t io_handle, AX_U8 dev_id, AX_U32 active_time);

#endif // _ISP_SIF_FTC_API_H_