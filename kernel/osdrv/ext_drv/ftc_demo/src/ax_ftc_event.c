/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ax_ftc_priv.h"
#include "ax_sif_ftc_api.h"
#include "ax_sync_signal.h"
#include "ax_ftc_extern.h"
#include "ax_ftc_process.h"
#include "ax_ftc_event.h"


extern AX_U64 ax_sys_get_current_us(AX_VOID);
extern vin_ftc_node_priv_ctx_t *g_ftc_node_priv_ctx;

extern ftc_regs *g_ftc_regs;

static __inline AX_S32 isp_ife_sw_reload_0_set_cfg_set(regio_global_handle_t io_handle,
        sw_reload_0_cfg_s *sw_reload_0_set_cfg_ptr)
{
    SW_RELOAD_0_SET_U sw_reload_0_set = {0};
    sw_reload_0_set.reg32 = ISP_DRV_IO_RELOAD_READ32(io_handle, SW_RELOAD_0_SET);
    sw_reload_0_set.bits.isp_its_sw_reload_set_set = sw_reload_0_set_cfg_ptr->isp_its_sw_reload;
    ISP_DRV_IO_RELOAD_WRITE32(io_handle, SW_RELOAD_0_SET, sw_reload_0_set.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_ife_sw_reload_0_clr_cfg_set(regio_global_handle_t io_handle,
        sw_reload_0_cfg_s *sw_reload_0_clr_cfg_ptr)
{
    SW_RELOAD_0_CLR_U sw_reload_0_clr = {0};
    sw_reload_0_clr.reg32 = ISP_DRV_IO_RELOAD_READ32(io_handle, SW_RELOAD_0_CLR);
    sw_reload_0_clr.bits.isp_its_sw_reload_clr_clr = sw_reload_0_clr_cfg_ptr->isp_its_sw_reload;
    ISP_DRV_IO_RELOAD_WRITE32(io_handle, SW_RELOAD_0_CLR, sw_reload_0_clr.reg32);
    return AX_ISP_SUCCESS;
}

AX_S32 ax_ftc_reload_set_set(regio_global_handle_t io_handle, isp_reload_domain_e reload_domain)
{
    AX_S32 ret = AX_ISP_SUCCESS;
    sw_reload_0_cfg_s sw_reload_0_cfg_ptr = {0};

    VIN_CHECK_HANDLE(io_handle);

    switch (reload_domain) {
    case ISP_DOMAIN_ITS_RELOAD:
        sw_reload_0_cfg_ptr.isp_its_sw_reload = AX_TRUE;
        break;
    default:
        break;
    }

    ret |= isp_ife_sw_reload_0_set_cfg_set(io_handle, &sw_reload_0_cfg_ptr);

    return ret;
}

AX_S32 ax_ftc_reload_clr_set(regio_global_handle_t io_handle, isp_reload_domain_e reload_domain)
{
    AX_S32 ret = AX_ISP_SUCCESS;
    sw_reload_0_cfg_s sw_reload_0_cfg_ptr = {0};

    VIN_CHECK_HANDLE(io_handle);

    switch (reload_domain) {
    case ISP_DOMAIN_ITS_RELOAD:
        sw_reload_0_cfg_ptr.isp_its_sw_reload = AX_TRUE;
        break;
    default:
        break;
    }

    ret |= isp_ife_sw_reload_0_clr_cfg_set(io_handle, &sw_reload_0_cfg_ptr);

    return ret;
}

static AX_U32 __reg_endian_swap(AX_U32 reg, AX_U32 reg_bytes)
{
    int i = 0;
    AX_U8 temp = 0;
    AX_U8 temp1 = 0;
    AX_U8 temp2 = 0;

    if (reg_bytes == 1) {
        for (i = 7; i >= 0; i--) {
            temp = (reg >> i) & 1;
            temp1 += temp << (7 - i);
        }

        return temp1;
    } else {
        for (i = 7; i >= 0; i--) {
            temp = (reg >> i) & 1;
            temp1 += temp << (7 - i);
        }
        for (i = 7; i >= 0; i--) {
            temp = (reg >> (i + 8)) & 1;
            temp2 += temp << (7 - i);
        }
        return temp2 << 8 | temp1;
    }
}

static AX_VOID __vin_ftc_node_regmask_to_basebit(AX_U32 reg_mask, AX_U32 *pbase_bit)
{
    AX_U32 i = 0;

    for (i = 0; i < 32; i++) {
        if (reg_mask & (0x1 << i)) {
            break;
        }
    }

    *pbase_bit = i;
}

static AX_S32 vin_ftc_light_process(AX_VOID)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U32 nSyncIdx = 0, nLightIdx = 0;
    AX_U32 i = 0;
    AX_S32 LightDelayTime, LightDutyTime = 0;
    regio_global_handle_t io_handle, io_reload;
    vin_ftc_node_info_t *pftc_node_info = AX_NULL;
    ax_vin_sync_shutter_mode_e eShutterMode;

    AX_S16 FreqTolLeft;
    AX_S16 FreqTolRight;
    AX_U16 SyncTriggerFreq;

    AX_U32 default_hts = 0;

    AX_U32 elc_cycle_us = 0;
    AX_U32 sensor_vts = 0;
    //AX_U32 base_bit = 0;

    AX_U32 vsync_line_num = 0;
    AX_U32 base_bit = 0;

    pftc_node_info = &g_ftc_node_priv_ctx->ftc_info;
    VIN_CHECK_PTR(pftc_node_info);

    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);

    io_reload = g_ftc_regs->reload_reg;
    VIN_CHECK_PTR(io_reload);

    eShutterMode = VIN_SYNC_SAMPLE_PIPE_MODE_FLASH_SNAP;

    if (!g_ftc_node_priv_ctx->b_freeze_commbus_en) {
        if (pftc_node_info->t_sync_info.tFrmLengthRegInfo.bEnable
                && (AX_RISCV_Kernel_get_cycle_info(&elc_cycle_us) == 0)) {
            FreqTolLeft = pftc_node_info->t_power_attr.nFreqTolLeft;
            FreqTolRight = pftc_node_info->t_power_attr.nFreqTolRight;
            SyncTriggerFreq = pftc_node_info->t_power_attr.nSyncTriggerFreq;
            if ((elc_cycle_us > 1000000 / (SyncTriggerFreq - FreqTolLeft)) || (elc_cycle_us < 1000000 / (SyncTriggerFreq + FreqTolRight))) {
                isp_warn("elc_cycle_us = %d\n", elc_cycle_us);
                return retval;
            } else {
                default_hts = pftc_node_info->t_sync_info.nHts;
                sensor_vts = elc_cycle_us * 1000 / default_hts - ELC_MAX_HOP;
                vsync_line_num = elc_cycle_us * 1000 / default_hts;

                for (i = 0; i < AX_ISP_FRAME_LENGTH_REG_NUM; i++) {
                    if (g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegValueMask[i] & 0xFFFF) {
                        __vin_ftc_node_regmask_to_basebit(g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegValueMask[i], &base_bit);
                        AX_OSAL_DEV_spi_dev_write(g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.spi_bus,
                                        g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.spi_csn,
                                        g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.devid,
                                        __reg_endian_swap(g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegAddr[i] & 0xff, 1),
                                        __reg_endian_swap((g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegAddr[i] >> 8) & 0xff, 1),
                                        __reg_endian_swap(((sensor_vts & g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegValueMask[i]) >> base_bit), 1),
                                        4);
                    }
                }
            }
        }
    }


    ax_ftc_reload_set_set(io_reload, ISP_DOMAIN_ITS_RELOAD);
    ax_ftc_reload_clr_set(io_reload, ISP_DOMAIN_ITS_RELOAD);

    nSyncIdx = pftc_node_info->t_vsync_attr.nSyncIdx;

    isp_sif_sensor_expo_ctrl_mode(io_handle, 1);
    isp_sif_sensor_expo_v_set(io_handle, nSyncIdx, vsync_line_num, 0);
    isp_sif_sensor_vsync_out_en(io_handle, nSyncIdx, AX_FALSE);

    if ((eShutterMode == VIN_SYNC_SAMPLE_PIPE_MODE_VIDEO) || (eShutterMode == VIN_SYNC_SAMPLE_PIPE_MODE_PICTURE)) {
        for (i = 0; i < AX_VIN_LIGHT_FLASH_PIN_NUM; i++) {
            isp_sif_sensor_flash_en(io_handle, i, AX_FALSE);
        }
    } else if (eShutterMode == VIN_SYNC_SAMPLE_PIPE_MODE_FLASH_SNAP) {
        for (i = 0; i < AX_VIN_LIGHT_FLASH_PIN_NUM; i++) {
            nLightIdx = i;
            if ((g_ftc_node_priv_ctx->trigger_event[nLightIdx].trigger_en == AX_TRUE)
                && (pftc_node_info->t_light_flash_sync_attr[nLightIdx].bLightPinEnable == AX_TRUE)) {

                    isp_sif_sensor_flash_en(io_handle, nLightIdx, AX_TRUE);
                    isp_sif_sensor_flash_active_level_set(io_handle, nLightIdx, pftc_node_info->t_light_flash_sync_attr[nLightIdx].eLightSyncInv);

                    LightDelayTime = pftc_node_info->t_light_flash_sync_attr[nLightIdx].nLightDelayTime;
                    LightDutyTime = pftc_node_info->t_light_flash_sync_attr[nLightIdx].nLightDutyTime;

                    isp_sif_sensor_flash_active_time(io_handle, nLightIdx, LightDutyTime);
                    isp_sif_sensor_flash_delay_time(io_handle, nLightIdx, LightDelayTime);

                pftc_node_info->t_light_flash_sync_attr[nLightIdx].bLightTimingEnable = AX_FALSE;
                g_ftc_node_priv_ctx->trigger_event[nLightIdx].trigger_en = AX_FALSE;

            } else {
                isp_sif_sensor_flash_en(io_handle, nLightIdx, AX_FALSE);
            }
        }
    }

    isp_sif_sensor_shadow_en(io_handle, nSyncIdx, AX_TRUE);

    isp_sif_sensor_internal_trig(io_handle, AX_TRUE);

    isp_sif_sensor_expo_ctrl_mode(io_handle, 0);
    isp_sif_sensor_vsync_out_en(io_handle, nSyncIdx, AX_TRUE);
    isp_sif_sensor_expo_v_set(io_handle, nSyncIdx, vsync_line_num,
        (vsync_line_num) / 100 * pftc_node_info->t_vsync_attr.nSyncDutyRatio);

    isp_sif_sensor_shadow_en(io_handle, nSyncIdx, AX_TRUE);

    return retval;
}


AX_S32 vin_ftc_node_event_forward(AX_VOID)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    vin_dev_ftc_event_info_t ftc_event_info = {0};     /* must init to zero 0 */

    VIN_CHECK_PTR(g_ftc_node_priv_ctx);
    __vin_dev_ftc_node_trigger_event_find(&ftc_event_info);

    vin_ftc_light_process();


    return retval;
}
AX_S32 vin_ftc_node_event_backward(AX_VOID)
{
    AX_S32 retval = AX_VIN_SUCCESS;

    return retval;
}



