/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

//#include "ax_base_type.h"
#include "ax_ftc_priv.h"
#include "ax_sync_signal.h"
#include "ax_sif_ftc_api.h"
#include "ax_sif_reg.h"
#include "ax_ftc_config.h"

static AX_U32 AX_FTC_ELEC_PLS_GPIO_NUM = 160;
static AX_U32 AX_FTC_VSYNC_GPIO_NUM[4] = {172, 183, 182, 181};
static AX_U32 AX_FTC_HSYNC_GPIO_NUM[4] = {171, 180, 179, 178};
static AX_U32 AX_FTC_FLASH_GPIO_NUM[AX_VIN_LIGHT_FLASH_PIN_NUM] = {170, 174, 173, 168, 167, 166, 165, 176, 175, 177};

extern AX_VOID cdphy_gpio_single_config_function(AX_U8 dev_num, AX_U8 pin_num, AX_U32 pin_mux);
extern AX_VOID cdphy_gpio_single_config_dirction(AX_U8 dev_num, AX_U8 pin_num, AX_U32 dirction);
extern AX_U64 ax_sys_get_current_us(AX_VOID);
extern vin_ftc_node_priv_ctx_t *g_ftc_node_priv_ctx;
extern ftc_regs *g_ftc_regs;
//extern AX_U64 ax_sys_get_current_us(AX_VOID);


static AX_S32 __vin_dev_ftc_pin_func(AX_U8 dev_num, AX_U8 pin_num,
        vin_ftc_phy_rx_pin_func_e pin_mux,
        vin_ftc_phy_rx_pin_direction_e dirction)
{
    printk("dev_num %d, pin_num %d, pin_mux %d, dirction %d\n",
            dev_num, pin_num, pin_mux, dirction);

    cdphy_gpio_single_config_function(dev_num, pin_num, pin_mux);
    cdphy_gpio_single_config_dirction(dev_num, pin_num, dirction);

    return 0;
}

static AX_S32 __vin_dev_ftc_flash_powerattr_to_riscv(ax_sync_power_attr_t *pftc_power_attr)
{
    ax_ftc_power_sync_attr_msg_t power_attr = {0};

    power_attr.power_trigger_mode = pftc_power_attr->eTriggerMode;
    power_attr.follow_cycle = pftc_power_attr->nFollowCycle;
    power_attr.tol_left_cyc = pftc_power_attr->nFreqTolLeft;
    power_attr.tol_right_cyc = pftc_power_attr->nFreqTolRight;
    power_attr.elec_freq = pftc_power_attr->nElecFreq;
    power_attr.sync_trigger_freq = pftc_power_attr->nSyncTriggerFreq;
    power_attr.elec_phase = pftc_power_attr->nSyncDelayElcUs;

    AX_RISCV_Kernel_set_power_attr(&power_attr);

    return 0;
}
# if 0
static AX_S32 __vin_dev_ftc_flash_pin_info_to_riscv(ax_sync_power_attr_t *pftc_power_attr)
{
    AX_U32 i = 0;
    ax_ftc_pin_ability_msg_t pin_cfg = {0};
    ax_ftc_pack_timing_attr_msg_t pack_attr = {0};

    pin_cfg.gpio_elec_in = pftc_power_attr->nGpioElecInPin;
    pin_cfg.gpio_sync_out = pftc_power_attr->nGpioSyncOutPin;
    for (i = 0; i < AX_VIN_LIGHT_GPIO_PIN_NUM; i++) {
        pin_cfg.gpio_num_blink[i] = pftc_power_attr->nStrobeGpioNum[i];
    }

    pin_cfg.gpio_num_pack = 0xFF;

    AX_RISCV_Kernel_set_pin_info(&pin_cfg);

    pack_attr.pack_enable = AX_FALSE;
    pack_attr.pack_polarity = 0; //high
    pack_attr.pack_freq_div = 1;
    pack_attr.pack_delay_us = 5000;
    pack_attr.pack_duty = 1;
    AX_RISCV_Kernel_set_pack_timing_attr(&pack_attr);

    return 0;
}
#endif
static AX_S32 __vin_dev_ftc_freeze_commbus(AX_VOID *param)
{
    vin_dev_freeze_commbus_t *freeze_commbus = AX_NULL;
    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);
    freeze_commbus = (vin_dev_freeze_commbus_t *)param;

    g_ftc_node_priv_ctx->b_freeze_commbus_en = freeze_commbus->b_freeze_commbus_en;

    return 0;
}

static AX_S32 __vin_dev_ftc_register_bus_callback(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    return retval;
}
static AX_S32 __vin_dev_ftc_flash_pin_info_to_riscv(ax_sync_power_attr_t *pftc_power_attr)
{
    AX_U32 i = 0;
    ax_ftc_pin_ability_msg_t pin_cfg = {0};
    ax_ftc_pack_timing_attr_msg_t pack_attr = {0};

    pin_cfg.gpio_elec_in = pftc_power_attr->nGpioElecInPin;
    pin_cfg.gpio_sync_out = pftc_power_attr->nGpioSyncOutPin;
    for (i = 0; i < AX_VIN_LIGHT_GPIO_PIN_NUM; i++) {
        pin_cfg.gpio_num_blink[i] = pftc_power_attr->nStrobeGpioNum[i];
    }

    pin_cfg.gpio_num_pack = 0xFF;

    AX_RISCV_Kernel_set_pin_info(&pin_cfg);

    pack_attr.pack_enable = AX_FALSE;
    pack_attr.pack_polarity = 0; //high
    pack_attr.pack_freq_div = 1;
    pack_attr.pack_delay_us = 5000;
    pack_attr.pack_duty = 1;
    AX_RISCV_Kernel_set_pack_timing_attr(&pack_attr);

    return 0;
}

static AX_S32 __vin_dev_ftc_flash_power_attr_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U8 dev_id = 0;
    regio_global_handle_t io_handle;
    AX_U32 eTriggerMode;
    ax_sync_power_attr_t *pftc_power_attr = AX_NULL;
    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);
    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);

    pftc_power_attr = (ax_sync_power_attr_t *)param;
    dev_id = g_ftc_node_priv_ctx->n_dev_id;
    eTriggerMode =  pftc_power_attr->eTriggerMode;
    /* config params to riscv driver */
    AX_RISCV_Kernel_setup_riscv();
    AX_RISCV_Kernel_send_version_info();
    AX_RISCV_Kernel_init();

    __vin_dev_ftc_flash_powerattr_to_riscv(pftc_power_attr);
    __vin_dev_ftc_flash_pin_info_to_riscv(pftc_power_attr);
    AX_RISCV_Kernel_start();



    g_ftc_node_priv_ctx->ftc_info.t_power_attr.eTriggerMode = pftc_power_attr->eTriggerMode;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nGpioElecInPin = pftc_power_attr->nGpioElecInPin;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nGpioSyncOutPin = pftc_power_attr->nGpioSyncOutPin;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nFollowCycle = pftc_power_attr->nFollowCycle;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nFreqTolLeft = pftc_power_attr->nFreqTolLeft;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nFreqTolRight = pftc_power_attr->nFreqTolRight;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nElecFreq = pftc_power_attr->nElecFreq;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nSyncTriggerFreq = pftc_power_attr->nSyncTriggerFreq;
    g_ftc_node_priv_ctx->ftc_info.t_power_attr.nSyncDelayElcUs = pftc_power_attr->nSyncDelayElcUs;
    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_power_attr.nStrobeGpioNum, &pftc_power_attr->nStrobeGpioNum,
        sizeof(g_ftc_node_priv_ctx->ftc_info.t_power_attr.nStrobeGpioNum));

    isp_sif_sensor_expo_ctrl_mode(io_handle, 0);

    isp_sif_sensor_expo_ctrl_enable(io_handle, AX_TRUE);

    isp_sif_sensor_shadow_en(io_handle, dev_id, 1);

    if (g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_elec_pls_func != FTC_PIN_FUNC_2) {
        __vin_dev_ftc_pin_func(7 - (AX_FTC_ELEC_PLS_GPIO_NUM - 160) / 6,
                (AX_FTC_ELEC_PLS_GPIO_NUM - 160) % 6, FTC_PIN_FUNC_2, FTC_PIN_DIRECTION_IN);
        g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_elec_pls_func = FTC_PIN_FUNC_2;
    }

    return retval;
}

static AX_S32 __vin_dev_ftc_sensor_vts_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_sensor_vts_t *pftc_sensor_vts = AX_NULL;
    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_sensor_vts = (ax_sensor_vts_t *)param;

    g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.spi_bus = pftc_sensor_vts->spi_bus;
    g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.spi_csn = pftc_sensor_vts->spi_csn;
    g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.devid = pftc_sensor_vts->devid;

    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegValueMask, &pftc_sensor_vts->nRegValueMask,
        sizeof(g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegValueMask));

    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegAddr, &pftc_sensor_vts->nRegAddr,
        sizeof(g_ftc_node_priv_ctx->ftc_info.t_sensor_vts.nRegAddr));

    return retval;
}

static AX_S32 __vin_dev_ftc_flash_power_attr_get(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_ftc_power_sync_attr_msg_t power_attr = {0};
    ax_ftc_pin_ability_msg_t pin_cfg = {0};
    ax_sync_power_attr_t *pftc_power_attr = AX_NULL;
    AX_U32 i = 0;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_power_attr = (ax_sync_power_attr_t *)param;



    AX_RISCV_Kernel_get_power_attr(&power_attr);
    AX_RISCV_Kernel_get_pin_info(&pin_cfg);

    pftc_power_attr->eTriggerMode = power_attr.power_trigger_mode;
    pftc_power_attr->nGpioElecInPin = pin_cfg.gpio_elec_in;
    pftc_power_attr->nGpioSyncOutPin =  pin_cfg.gpio_sync_out;
    pftc_power_attr->nElecFreq = power_attr.elec_freq;
    pftc_power_attr->nSyncTriggerFreq = power_attr.sync_trigger_freq;

    pftc_power_attr->nFreqTolLeft = power_attr.tol_left_cyc;
    pftc_power_attr->nFreqTolRight = power_attr.tol_right_cyc;
    pftc_power_attr->nFollowCycle = power_attr.follow_cycle;
    pftc_power_attr->nSyncDelayElcUs = power_attr.elec_phase;
    for (i = 0; i < AX_VIN_LIGHT_GPIO_PIN_NUM; i++) {
        pftc_power_attr->nStrobeGpioNum[i] = pin_cfg.gpio_num_blink[i];
    }

    return retval;
}


static AX_S32 __vin_dev_ftc_vsyncattr_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_sync_signal_t *pftc_vsync_attr = AX_NULL;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_vsync_attr = (ax_sync_signal_t *)param;

    g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx = pftc_vsync_attr->nSyncIdx;
    g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.eSyncInv = pftc_vsync_attr->eSyncInv;
    g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncFreq = pftc_vsync_attr->nSyncFreq;
    g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncDutyRatio = pftc_vsync_attr->nSyncDutyRatio;
    return retval;
}

static AX_S32 __vin_dev_ftc_vsyncattr_get(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_sync_signal_t *pftc_vsync_attr = AX_NULL;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_vsync_attr = (ax_sync_signal_t *)param;

    pftc_vsync_attr->nSyncIdx = g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx;
    pftc_vsync_attr->eSyncInv = g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.eSyncInv;
    pftc_vsync_attr->nSyncFreq = g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncFreq;
    pftc_vsync_attr->nSyncDutyRatio = g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncDutyRatio;

    return retval;
}

static AX_S32 __vin_dev_ftc_vsync_enable_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U8 dev_id = 0;
    regio_global_handle_t io_handle;
    AX_BOOL ftc_sync_enable;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);

    dev_id = g_ftc_node_priv_ctx->n_dev_id;
    ftc_sync_enable = *(AX_BOOL *)param;

    if (ftc_sync_enable) {
        isp_sif_sensor_vsync_active_level_set(io_handle,
            g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx, g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.eSyncInv);

        isp_sif_sensor_expo_v_set(io_handle,
            g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx, g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncFreq,
            g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncFreq / 100 * g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncDutyRatio);

        isp_sif_sensor_vsync_out_en(io_handle, g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx, AX_TRUE);

        isp_sif_sensor_shadow_en(io_handle, dev_id, 1);

        if (g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_vsync_func != FTC_PIN_FUNC_2) {
            __vin_dev_ftc_pin_func(7 - (AX_FTC_VSYNC_GPIO_NUM[g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx] - 160) / 6,
                    (AX_FTC_VSYNC_GPIO_NUM[g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx] - 160) % 6, FTC_PIN_FUNC_2, FTC_PIN_DIRECTION_OUT);
            g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_vsync_func = FTC_PIN_FUNC_2;
        }
    } else {

        isp_sif_sensor_vsync_out_en(io_handle, g_ftc_node_priv_ctx->ftc_info.t_vsync_attr.nSyncIdx, AX_FALSE);

        isp_sif_sensor_shadow_en(io_handle, dev_id, 1);
    }

    return retval;
}

static AX_S32 __vin_dev_ftc_hsyncattr_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_sync_signal_t *pftc_hsync_attr = AX_NULL;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_hsync_attr = (ax_sync_signal_t *)param;

    g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx = pftc_hsync_attr->nSyncIdx;
    g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.eSyncInv = pftc_hsync_attr->eSyncInv;
    g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncFreq = pftc_hsync_attr->nSyncFreq;
    g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncDutyRatio = pftc_hsync_attr->nSyncDutyRatio;

    return retval;
}

static AX_S32 __vin_dev_ftc_hsyncattr_get(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_sync_signal_t *pftc_hsync_attr = AX_NULL;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_hsync_attr = (ax_sync_signal_t *)param;

    pftc_hsync_attr->nSyncIdx = g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx;
    pftc_hsync_attr->eSyncInv = g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.eSyncInv;
    pftc_hsync_attr->nSyncFreq = g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncFreq;
    pftc_hsync_attr->nSyncDutyRatio = g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncDutyRatio;

    return retval;
}

static AX_S32 __vin_dev_ftc_hsync_enable_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U8 dev_id = 0;
    AX_BOOL ftc_sync_enable;
    regio_global_handle_t io_handle;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);

    dev_id = g_ftc_node_priv_ctx->n_dev_id;
    ftc_sync_enable = *(AX_BOOL *)param;

    if (ftc_sync_enable) {

        isp_sif_sensor_hsync_active_level_set(io_handle,
            g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx, g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.eSyncInv);

        isp_sif_sensor_expo_h_set(io_handle,
            g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx, g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncFreq / 10,
            g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncFreq / 100 * g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncDutyRatio);

        isp_sif_sensor_hsync_out_en(io_handle, g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx, AX_TRUE);

        isp_sif_sensor_shadow_en(io_handle, dev_id, 1);

        if (g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_hsync_func != FTC_PIN_FUNC_2) {
            __vin_dev_ftc_pin_func(7 - (AX_FTC_HSYNC_GPIO_NUM[g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx] - 160) / 6,
                    (AX_FTC_HSYNC_GPIO_NUM[g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx] - 160) % 6, FTC_PIN_FUNC_2, FTC_PIN_DIRECTION_OUT);
            g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_hsync_func = FTC_PIN_FUNC_2;
        }
    } else {

        isp_sif_sensor_hsync_out_en(io_handle, g_ftc_node_priv_ctx->ftc_info.t_hsync_attr.nSyncIdx, AX_FALSE);

        isp_sif_sensor_shadow_en(io_handle, dev_id, 1);

    }

    return retval;
}

static AX_S32 __vin_dev_ftc_lightsyncinfo_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U8 dev_id = 0;
    ax_sync_info_t *pftc_sync_info = AX_NULL;
    regio_global_handle_t io_handle;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);

    pftc_sync_info = (ax_sync_info_t *)param;

    g_ftc_node_priv_ctx->ftc_info.t_sync_info.nVts = pftc_sync_info->nVts;
    g_ftc_node_priv_ctx->ftc_info.t_sync_info.nHts = pftc_sync_info->nHts;
    g_ftc_node_priv_ctx->ftc_info.t_sync_info.nFrameRate = pftc_sync_info->nFrameRate;
    g_ftc_node_priv_ctx->ftc_info.t_sync_info.nElecToVsyncTime = pftc_sync_info->nElecToVsyncTime;
    g_ftc_node_priv_ctx->ftc_info.t_sync_info.nVbbTime = pftc_sync_info->nVbbTime;
    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_sync_info.nIntTime,
        &pftc_sync_info->nIntTime, sizeof(g_ftc_node_priv_ctx->ftc_info.t_sync_info.nIntTime));
    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_sync_info.tFrmLengthRegInfo,
        &pftc_sync_info->tFrmLengthRegInfo, sizeof(g_ftc_node_priv_ctx->ftc_info.t_sync_info.tFrmLengthRegInfo));
    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_sync_info.szShutterMap,
            &pftc_sync_info->szShutterMap, sizeof(g_ftc_node_priv_ctx->ftc_info.t_sync_info.szShutterMap));
    AX_OSAL_LIB_memcpy(&g_ftc_node_priv_ctx->ftc_info.t_sync_info.szShutterMode,
            &pftc_sync_info->szShutterMode, sizeof(g_ftc_node_priv_ctx->ftc_info.t_sync_info.szShutterMode));

    isp_sif_sensor_elec_puls_delay_to_vsync_adjust(io_handle, g_ftc_node_priv_ctx->ftc_info.t_sync_info.nElecToVsyncTime);

    isp_sif_sensor_elec_puls_start_line_to_trigger_adjust(io_handle, g_ftc_node_priv_ctx->ftc_info.t_sync_info.nVts - 0x100);

    isp_sif_sensor_elec_puls_line_num_adjust(io_handle, 0x80);

    isp_sif_sensor_shadow_en(io_handle, dev_id, 1);

    return retval;
}

static AX_S32 __vin_dev_ftc_lightsyncinfo_get(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_sync_info_t *pftc_sync_info = AX_NULL;

    VIN_CHECK_PTR(param);

    pftc_sync_info = (ax_sync_info_t *)param;

    pftc_sync_info->nVts = g_ftc_node_priv_ctx->ftc_info.t_sync_info.nVts;
    pftc_sync_info->nHts = g_ftc_node_priv_ctx->ftc_info.t_sync_info.nHts;
    pftc_sync_info->nFrameRate = g_ftc_node_priv_ctx->ftc_info.t_sync_info.nFrameRate;
    pftc_sync_info->nElecToVsyncTime = g_ftc_node_priv_ctx->ftc_info.t_sync_info.nElecToVsyncTime;
    pftc_sync_info->nVbbTime = g_ftc_node_priv_ctx->ftc_info.t_sync_info.nVbbTime;
    AX_OSAL_LIB_memcpy(&pftc_sync_info->nIntTime, &g_ftc_node_priv_ctx->ftc_info.t_sync_info.nIntTime,
        sizeof(pftc_sync_info->nIntTime));
    AX_OSAL_LIB_memcpy(&pftc_sync_info->tFrmLengthRegInfo, &g_ftc_node_priv_ctx->ftc_info.t_sync_info.tFrmLengthRegInfo,
            sizeof(pftc_sync_info->tFrmLengthRegInfo));
    AX_OSAL_LIB_memcpy(&pftc_sync_info->szShutterMap, &g_ftc_node_priv_ctx->ftc_info.t_sync_info.szShutterMap,
            sizeof(pftc_sync_info->szShutterMap));
    AX_OSAL_LIB_memcpy(&pftc_sync_info->szShutterMode, &g_ftc_node_priv_ctx->ftc_info.t_sync_info.szShutterMode,
            sizeof(pftc_sync_info->szShutterMode));

    return retval;
}

static AX_S32 __vin_dev_ftc_lightsyncattr_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_light_sync_signal_t *pftc_light_sync_attr = AX_NULL;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_light_sync_attr = (ax_light_sync_signal_t *)param;

    if (pftc_light_sync_attr->eLightType == VIN_SYNC_LIGHT_MODE_FLASH) {
        VIN_CHECK_VALUE_MAX(pftc_light_sync_attr->nLightIdx, AX_VIN_LIGHT_FLASH_PIN_NUM - 1);
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightIdx = pftc_light_sync_attr->nLightIdx;
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].bLightPinEnable = AX_TRUE;
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].eLightType = pftc_light_sync_attr->eLightType;
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].eLightSyncInv = pftc_light_sync_attr->eLightSyncInv;
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDelayTime = pftc_light_sync_attr->nLightDelayTime;
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDutyTime = pftc_light_sync_attr->nLightDutyTime;
        g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightFreqRatio = pftc_light_sync_attr->nLightFreqRatio;
    } else if (pftc_light_sync_attr->eLightType == VIN_SYNC_LIGHT_MODE_STROBE) {
        VIN_CHECK_VALUE_MAX(pftc_light_sync_attr->nLightIdx, AX_VIN_LIGHT_GPIO_PIN_NUM - 1);
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightIdx = pftc_light_sync_attr->nLightIdx;
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].bLightPinEnable = AX_TRUE;
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].eLightType = pftc_light_sync_attr->eLightType;
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].eLightSyncInv = pftc_light_sync_attr->eLightSyncInv;
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDelayTime = pftc_light_sync_attr->nLightDelayTime;
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDutyTime = pftc_light_sync_attr->nLightDutyTime;
        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightFreqRatio = pftc_light_sync_attr->nLightFreqRatio;
    }
    return retval;
}

static AX_S32 __vin_dev_ftc_lightsyncattr_get(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    ax_light_sync_signal_t *pftc_light_sync_attr = AX_NULL;

    VIN_CHECK_PTR(param);

    pftc_light_sync_attr = (ax_light_sync_signal_t *)param;

    if (pftc_light_sync_attr->eLightType == VIN_SYNC_LIGHT_MODE_FLASH) {
        VIN_CHECK_VALUE_MAX(pftc_light_sync_attr->nLightIdx, AX_VIN_LIGHT_FLASH_PIN_NUM - 1);
        pftc_light_sync_attr->bLightPinEnable = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].bLightPinEnable;
        pftc_light_sync_attr->bLightTimingEnable = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].bLightTimingEnable;
        pftc_light_sync_attr->eLightType = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].eLightType;
        pftc_light_sync_attr->eLightSyncInv = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].eLightSyncInv;
        pftc_light_sync_attr->nLightDutyTime = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDutyTime;
        pftc_light_sync_attr->nLightDelayTime = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDelayTime;
        pftc_light_sync_attr->nLightFreqRatio = g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[pftc_light_sync_attr->nLightIdx].nLightFreqRatio;
    } else if (pftc_light_sync_attr->eLightType == VIN_SYNC_LIGHT_MODE_STROBE) {
        VIN_CHECK_VALUE_MAX(pftc_light_sync_attr->nLightIdx, AX_VIN_LIGHT_GPIO_PIN_NUM - 1);
        pftc_light_sync_attr->bLightPinEnable = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].bLightPinEnable;
        pftc_light_sync_attr->bLightTimingEnable = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].bLightTimingEnable;
        pftc_light_sync_attr->eLightType = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].eLightType;
        pftc_light_sync_attr->eLightSyncInv = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].eLightSyncInv;
        pftc_light_sync_attr->nLightDutyTime = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDutyTime;
        pftc_light_sync_attr->nLightDelayTime = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightDelayTime;
        pftc_light_sync_attr->nLightFreqRatio = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[pftc_light_sync_attr->nLightIdx].nLightFreqRatio;
    }

    return retval;
}

static AX_S32 __vin_dev_ftc_light_enable_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U8 dev_id = 0;
    AX_U32 LightIdx = 0;
    AX_U32 gpio_num = 0;
    ax_light_sync_enable_t *pftc_light_enable = AX_NULL;
    regio_global_handle_t io_handle;
    ax_ftc_blink_timing_attr_msg_t blink_attr = {0};

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);
    dev_id = g_ftc_node_priv_ctx->n_dev_id;
    pftc_light_enable = (ax_light_sync_enable_t *)param;

    LightIdx = pftc_light_enable->nLightIdx;

    if (pftc_light_enable->eLightType == VIN_SYNC_LIGHT_MODE_FLASH) {
        VIN_CHECK_VALUE_MAX(pftc_light_enable->nLightIdx, AX_VIN_LIGHT_FLASH_PIN_NUM - 1);
        if (pftc_light_enable->bLightEnable) {
            //g_ftc_node_priv_ctx->ftc_status.event_note[LightIdx].dbg_pts0 = ax_sys_get_current_us();

            /* update params */
            g_ftc_node_priv_ctx->trigger_event[LightIdx].flash_id = LightIdx;
            g_ftc_node_priv_ctx->trigger_event[LightIdx].trigger_num = pftc_light_enable->tTriggerData.nTriggerNum;
            g_ftc_node_priv_ctx->trigger_event[LightIdx].interval_frm_num = pftc_light_enable->tTriggerData.nIntervalFrmNum;
            g_ftc_node_priv_ctx->trigger_event[LightIdx].trigger_select = pftc_light_enable->tTriggerData.eTriggerSelect;
            g_ftc_node_priv_ctx->trigger_event[LightIdx].user_data = pftc_light_enable->tTriggerData.nUserData;

            g_ftc_node_priv_ctx->trigger_event[LightIdx].trigger_fsm = FTC_TRIGGER_EVENT_FSM_UPDATE;
            isp_dbg("LightIdx:%d,flash_id:%d,trigger_num:%d,interval_frm_num:%d,trigger_select:%d,user_data:0x%llx,trigger_fsm:%d,nLightDelayTime:%d,nLightDutyTime:%d,eLightSyncInv:%d\n",
                    LightIdx,
                    g_ftc_node_priv_ctx->trigger_event[LightIdx].flash_id,
                    g_ftc_node_priv_ctx->trigger_event[LightIdx].trigger_num,
                    g_ftc_node_priv_ctx->trigger_event[LightIdx].interval_frm_num,
                    g_ftc_node_priv_ctx->trigger_event[LightIdx].trigger_select,
                    g_ftc_node_priv_ctx->trigger_event[LightIdx].user_data,
                    g_ftc_node_priv_ctx->trigger_event[LightIdx].trigger_fsm,
                    g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].nLightDelayTime,
                    g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].nLightDutyTime,
                    g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].eLightSyncInv);
            isp_sif_sensor_counter_unit_set(io_handle, LightIdx,
                USE_LINE_COUNTER_AS_COUNTER);

            isp_sif_sensor_flash_delay_time(io_handle, LightIdx,
                g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].nLightDelayTime);

            isp_sif_sensor_flash_active_time(io_handle, LightIdx,
                g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].nLightDutyTime);

            isp_sif_sensor_flash_active_level_set(io_handle, LightIdx,
                g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].eLightSyncInv);

            //isp_sif_sensor_flash_en(io_handle, dev_info.t_light_flash_sync_attr.nlightIdx, AX_TRUE);

            g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].bLightTimingEnable = AX_TRUE;
            isp_sif_sensor_shadow_en(io_handle, dev_id, 1);
        } else {
            isp_sif_sensor_flash_en(io_handle, LightIdx, AX_FALSE);
            g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[LightIdx].bLightTimingEnable = AX_FALSE;
            isp_sif_sensor_shadow_en(io_handle, dev_id, 1);
        }
    } else if (pftc_light_enable->eLightType == VIN_SYNC_LIGHT_MODE_STROBE) {
        VIN_CHECK_VALUE_MAX(pftc_light_enable->nLightIdx, AX_VIN_LIGHT_GPIO_PIN_NUM - 1);
        /*RISC-V*/

        blink_attr.blink_idx = pftc_light_enable->nLightIdx;
        blink_attr.blink_enable = pftc_light_enable->bLightEnable;
        blink_attr.blink_polarity = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[LightIdx].eLightSyncInv;
        blink_attr.blink_freq_div = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[LightIdx].nLightFreqRatio;
        blink_attr.blink_delay_duty = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[LightIdx].nLightDelayTime;
        blink_attr.blink_duty = g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[LightIdx].nLightDutyTime;
        AX_RISCV_Kernel_set_blink_timing_attr(&blink_attr);

        g_ftc_node_priv_ctx->ftc_info.t_light_gpio_sync_attr[LightIdx].bLightTimingEnable = pftc_light_enable->bLightEnable;

        if (pftc_light_enable->bLightEnable && (g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_strobe_func[LightIdx] != FTC_PIN_FUNC_6)) {
            gpio_num = g_ftc_node_priv_ctx->ftc_info.t_power_attr.nStrobeGpioNum[LightIdx];
            if (gpio_num >= 160 && gpio_num <= 183) {
                __vin_dev_ftc_pin_func(7 - (gpio_num - 160) / 6, (gpio_num - 160) % 6, FTC_PIN_FUNC_6, FTC_PIN_DIRECTION_OUT);
                g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_strobe_func[LightIdx] = FTC_PIN_FUNC_6;
            }
        }
    }

    return retval;
}

static AX_S32 __vin_dev_ftc_light_group_enable_set(AX_VOID *param)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U32 i = 0, LightIdxMax = 0;
    AX_U32 flags;
    ax_light_sync_enable_t (*pftc_light_enable)[] = AX_NULL;

    VIN_CHECK_PTR(param);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_light_enable = (ax_light_sync_enable_t (*)[])param;
    VIN_CHECK_PTR(*pftc_light_enable);

    LightIdxMax = AX_VIN_LIGHT_FLASH_PIN_NUM > AX_VIN_LIGHT_GPIO_PIN_NUM ? AX_VIN_LIGHT_FLASH_PIN_NUM : AX_VIN_LIGHT_GPIO_PIN_NUM;
    AX_OSAL_SYNC_spin_lock_irqsave(&g_ftc_node_priv_ctx->ftc_cfg_slock, &flags);
    for (i = 0; i < LightIdxMax; i++) {
        if ((((*pftc_light_enable)[i].eLightType == VIN_SYNC_LIGHT_MODE_FLASH) && ((*pftc_light_enable)[i].nLightIdx < AX_VIN_LIGHT_FLASH_PIN_NUM))
            || (((*pftc_light_enable)[i].eLightType == VIN_SYNC_LIGHT_MODE_STROBE) && ((*pftc_light_enable)[i].nLightIdx < AX_VIN_LIGHT_GPIO_PIN_NUM))) {
            retval = __vin_dev_ftc_light_enable_set(&((*pftc_light_enable)[i]));
            if (retval != AX_VIN_SUCCESS) {
                printk("nLightIdx %d failed \n", i);
                break;
            }
        } else {
            printk("LightIdx %d, eLightType %d\n", (*pftc_light_enable)[i].nLightIdx, (*pftc_light_enable)[i].eLightType);
        }
    }
    AX_OSAL_SYNC_spin_unlock_irqrestore(&g_ftc_node_priv_ctx->ftc_cfg_slock, &flags);

    return retval;
}

static AX_S32 __vin_dev_ftc_sns_info_set(AX_U32 *isp_sync_info)
{
    AX_S32 retval = AX_VIN_SUCCESS;

    return retval;
}


static AX_S32 __vin_dev_ftc_frame_info_set(AX_U32 *blk_id)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    return retval;
}

static AX_VOID __vin_ftc_node_expinfo_add_shuttermode(ax_vin_sync_shutter_mode_e eShutterMode, vin_dev_ftc_event_info_t *meta_ftc_info_out)
{
    ISP_META_FRAME_TYPE_E FrmType;

    switch (eShutterMode) {
    case VIN_SYNC_SAMPLE_PIPE_MODE_INVALID:
        FrmType = ISP_META_FRAME_TYPE_VIDEO;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_VIDEO:
        FrmType = ISP_META_FRAME_TYPE_VIDEO;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_PICTURE:
        FrmType = ISP_META_FRAME_TYPE_PICTURE;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_FLASH_SNAP:
        FrmType = ISP_META_FRAME_TYPE_FLASH_SNAP;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE0:
        FrmType = ISP_META_FRAME_TYPE_USER_DEFINED0;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE1:
        FrmType = ISP_META_FRAME_TYPE_USER_DEFINED1;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE2:
        FrmType = ISP_META_FRAME_TYPE_USER_DEFINED2;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE3:
        FrmType = ISP_META_FRAME_TYPE_USER_DEFINED3;
        break;
    case VIN_SYNC_SAMPLE_PIPE_MODE_USER_DEFINE4:
        FrmType = ISP_META_FRAME_TYPE_USER_DEFINED4;
        break;
    default:
        FrmType = ISP_META_FRAME_TYPE_VIDEO;
        break;
    }
    meta_ftc_info_out->meta_ftc_info.eFrmType = FrmType;

    return;
}

static AX_S32 __vin_dev_ftc_get_exp_pipeid(vin_dev_ftc_event_info_t *ftc_event_info)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U32 i = 0;
    vin_ftc_node_info_t *pftc_info = AX_NULL;
    ax_vin_sync_shutter_mode_e e_shutter_mode;

    pftc_info = &g_ftc_node_priv_ctx->ftc_info;

    for (i = 0; i < AX_ISP_SHUTTER_SEQ_NUM; i++) {

        pftc_info->n_cur_shutter_seq_num += 1;

        if (pftc_info->n_cur_shutter_seq_num >= AX_ISP_SHUTTER_SEQ_NUM) {
            pftc_info->n_cur_shutter_seq_num = 0;
        }

        if (pftc_info->n_cur_shutter_seq_num == 0) {
            AX_OSAL_LIB_memcpy(pftc_info->sz_cur_shutter_mode, pftc_info->t_sync_info.szShutterMode, sizeof(pftc_info->sz_cur_shutter_mode));
        }

        e_shutter_mode = pftc_info->sz_cur_shutter_mode[pftc_info->n_cur_shutter_seq_num];
        if (e_shutter_mode == VIN_SYNC_SAMPLE_PIPE_MODE_INVALID) {
            continue;
        } else {
            break;
        }
    }

    if (ftc_event_info == AX_NULL) {
        return retval;
    }

    for (i = 0; i < AX_ISP_SHUTTER_MAP_NUM; i++) {
        if (e_shutter_mode == pftc_info->t_sync_info.szShutterMap[i].eShutterMode) {
            ftc_event_info->pipe_id = pftc_info->t_sync_info.szShutterMap[i].nPipeId;
            __vin_ftc_node_expinfo_add_shuttermode(e_shutter_mode, ftc_event_info);
            break;
        }
    }

    if (i >= AX_ISP_SHUTTER_MAP_NUM) {
        ftc_event_info->pipe_id = 0;
    }

    return retval;
}

static __inline AX_S32 isp_sif_sen_frm_cfg_get(AX_U8 dev_id, AX_U32 *data)
{
    SIF_SEN_FRM_U sif_sen_frm = {0};
    regio_pipe_handle_t io_handle = g_ftc_regs->seq_reg;

    sif_sen_frm.reg32 = ISP_DRV_SEQ_READ32(io_handle, 0x4);
    *data = sif_sen_frm.bits.sif_frm_cnt;

    return AX_ISP_SUCCESS;
}

AX_S32 ax_isp_sif_sns_get_seq_num(AX_U8 dev_id, AX_U32 *p_seq_num)
{
    AX_S32 ret = AX_VIN_SUCCESS;
    AX_U32 seq_num = 0;

    ret = isp_sif_sen_frm_cfg_get(dev_id, &seq_num);

    *p_seq_num = seq_num;
    return ret;
}

static AX_S32 __vin_dev_ftc_find_next_seq(ax_vin_sync_shutter_mode_e e_shutter_mode, AX_U32 trigger_times)
{
    AX_U32 i = 0, j = 0;
    AX_U32 n_cur_shutter_seq_num = 0;
    AX_S32 add_seq = -1;
    AX_U32 seq_invalid_num = 0;
    vin_ftc_node_info_t *pftc_info = AX_NULL;

    VIN_CHECK_PTR(g_ftc_node_priv_ctx);

    pftc_info = &g_ftc_node_priv_ctx->ftc_info;
    n_cur_shutter_seq_num = pftc_info->n_cur_shutter_seq_num;

    for (j = 0; j < trigger_times; j++) {
        for (i = 1; i <= AX_ISP_SHUTTER_SEQ_NUM; i++) {
            if (n_cur_shutter_seq_num + i >= AX_ISP_SHUTTER_SEQ_NUM) {
                if (e_shutter_mode == pftc_info->sz_cur_shutter_mode[n_cur_shutter_seq_num + i - AX_ISP_SHUTTER_SEQ_NUM]) {
                    add_seq = i + j * AX_ISP_SHUTTER_SEQ_NUM - seq_invalid_num;
                    trigger_times--;
                } else if (VIN_SYNC_SAMPLE_PIPE_MODE_INVALID == pftc_info->sz_cur_shutter_mode[n_cur_shutter_seq_num + i - AX_ISP_SHUTTER_SEQ_NUM]) {
                    seq_invalid_num++;
                }

            } else {
                if (e_shutter_mode == pftc_info->sz_cur_shutter_mode[n_cur_shutter_seq_num + i]) {
                    add_seq = i + j * AX_ISP_SHUTTER_SEQ_NUM - seq_invalid_num;
                    trigger_times--;
                } else if (VIN_SYNC_SAMPLE_PIPE_MODE_INVALID == pftc_info->sz_cur_shutter_mode[n_cur_shutter_seq_num + i]) {
                    seq_invalid_num++;
                }
            }
            if (trigger_times <= 0) {
                isp_dbg("add_seq %d i %d j %d \n", add_seq, i, j);
                return add_seq;
            }
        }
    }

    isp_dbg("add_seq %d i %d j %d \n", add_seq, i, j);

    return add_seq;
}

AX_S32 __vin_dev_ftc_node_trigger_event_find(vin_dev_ftc_event_info_t *ftc_event_info)
{
    AX_S32 retval = AX_VIN_SUCCESS;
    AX_U32 i = 0, j = 0;
    AX_U32 seq_num = 0;
    AX_S32 add_seq_num = 0;
    AX_BOOL is_trigger = AX_FALSE;
    ax_sync_trigger_selection_e trigger_select;
    AX_U64 sof_pts = 0;
    AX_U32 flags;
    regio_global_handle_t io_handle;

    vin_ftc_flash_trigger_event_t *trg_event = AX_NULL;
    vin_ftc_node_info_t *pftc_info = AX_NULL;
    ax_light_sync_signal_t *pflash_attr = AX_NULL;

    sof_pts = ax_sys_get_current_us();

    VIN_CHECK_PTR(ftc_event_info);
    VIN_CHECK_PTR(g_ftc_node_priv_ctx);
    io_handle = g_ftc_regs->sen_reg;
    VIN_CHECK_PTR(io_handle);

    pftc_info = &g_ftc_node_priv_ctx->ftc_info;

    ax_isp_sif_sns_get_seq_num(0, &seq_num);

    AX_OSAL_SYNC_spin_lock_irqsave(&g_ftc_node_priv_ctx->ftc_cfg_slock, &flags);
    for (i = 0; i < AX_VIN_LIGHT_FLASH_PIN_NUM; i++) {
        trg_event = &g_ftc_node_priv_ctx->trigger_event[i];
        pflash_attr = &g_ftc_node_priv_ctx->ftc_info.t_light_flash_sync_attr[i];
        if (FTC_TRIGGER_EVENT_FSM_UPDATE == trg_event->trigger_fsm) {
            if (1 < trg_event->trigger_num) {
                if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_INSERT) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING;
                    for (j = 0; j < trg_event->trigger_num; j++) {
                        trg_event->trigger_queue[j] = seq_num + j * trg_event->interval_frm_num;
                    }
                } else if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_REPLACE_CURRENT) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING;
                    for (j = 0; j < trg_event->trigger_num; j++) {
                        trg_event->trigger_queue[j] = seq_num + j * trg_event->interval_frm_num;
                    }
                } else if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_REPLACE_VIDEO) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING;
                    for (j = 0; j < trg_event->trigger_num; j++) {
                        if (j == 0) {
                            add_seq_num = __vin_dev_ftc_find_next_seq(VIN_SYNC_SAMPLE_PIPE_MODE_VIDEO, 1);
                            if (add_seq_num < 0) {
                                printk("__vin_dev_ftc_find_next_seq fail\n");
                                trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_IDLE;
                                break;
                            }
                        }
                        trg_event->trigger_queue[j] = seq_num + add_seq_num + j * trg_event->interval_frm_num;
                    }
                } else if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_REPLACE_PICTURE) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING;
                    for (j = 0; j < trg_event->trigger_num; j++) {
                        if (j == 0) {
                            add_seq_num = __vin_dev_ftc_find_next_seq(VIN_SYNC_SAMPLE_PIPE_MODE_PICTURE, 1);
                            if (add_seq_num < 0) {
                                printk("__vin_dev_ftc_find_next_seq fail\n");
                                trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_IDLE;
                                break;
                            }
                        }
                        trg_event->trigger_queue[j] = seq_num + add_seq_num + j * trg_event->interval_frm_num;
                    }
                }
            } else {
               if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_INSERT) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONCE;
               } else if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_REPLACE_CURRENT) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONCE;
               } else if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_REPLACE_VIDEO) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING;
                    add_seq_num = __vin_dev_ftc_find_next_seq(VIN_SYNC_SAMPLE_PIPE_MODE_VIDEO, 1);
                    if (add_seq_num < 0) {
                        isp_err("__vin_dev_ftc_find_next_seq fail\n");
                        trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_IDLE;
                    }
                    trg_event->trigger_queue[0] = seq_num + add_seq_num;
               } else if (trg_event->trigger_select == VIN_SYNC_LIGHT_TRIGGER_REPLACE_PICTURE) {
                    trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING;
                    add_seq_num = __vin_dev_ftc_find_next_seq(VIN_SYNC_SAMPLE_PIPE_MODE_PICTURE, 1);
                    if (add_seq_num < 0) {
                        isp_err("__vin_dev_ftc_find_next_seq fail\n");
                        trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_IDLE;
                    }
                    trg_event->trigger_queue[0] = seq_num + add_seq_num;
               }
            }
        }
        switch (trg_event->trigger_fsm) {
        case FTC_TRIGGER_EVENT_FSM_IDLE:
            /* nothing to do */
            break;
        case FTC_TRIGGER_EVENT_FSM_SNAP_ONCE:
            is_trigger = AX_TRUE;
            trigger_select = trg_event->trigger_select;
            trg_event->trigger_en = 1;
            trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_IDLE;    /* State switching */

            /* fill ftc event meta info */
            ftc_event_info->meta_ftc_info.tFtcEventInfo[ftc_event_info->meta_ftc_info.nFlashActiveNum].nFlashId = trg_event->flash_id;
            ftc_event_info->meta_ftc_info.tFtcEventInfo[ftc_event_info->meta_ftc_info.nFlashActiveNum].nUserData = trg_event->user_data;
            ftc_event_info->meta_ftc_info.nFlashActiveNum++;

            break;
        case FTC_TRIGGER_EVENT_FSM_SNAP_ONGOING:
            for (j = 0; j < trg_event->trigger_num; j++) {
                if (seq_num + 1 == trg_event->trigger_queue[j]) {
                    trg_event->trigger_done_times++;
                    is_trigger = AX_TRUE;
                    trigger_select = trg_event->trigger_select;
                    trg_event->trigger_en = 1;
                    ftc_event_info->meta_ftc_info.tFtcEventInfo[ftc_event_info->meta_ftc_info.nFlashActiveNum].nFlashId = trg_event->flash_id;
                    ftc_event_info->meta_ftc_info.tFtcEventInfo[ftc_event_info->meta_ftc_info.nFlashActiveNum].nUserData = trg_event->user_data;
                    ftc_event_info->meta_ftc_info.nFlashActiveNum++;
                }
            }

            if (trg_event->trigger_done_times >= trg_event->trigger_num) {
                trg_event->trigger_done_times = 0;
                trg_event->trigger_fsm = FTC_TRIGGER_EVENT_FSM_IDLE;    /* State switching */
            }
            break;
        default:
            isp_err("Unknown trigger status:%d.\n", trg_event->trigger_fsm);
        }
        if (trg_event->trigger_en == 1) {
            if (pflash_attr->bLightPinEnable) {
                if (g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_flash_func[i] != FTC_PIN_FUNC_2) {
                    __vin_dev_ftc_pin_func(7 - (AX_FTC_FLASH_GPIO_NUM[i] - 160) / 6,
                            (AX_FTC_FLASH_GPIO_NUM[i] - 160) % 6, FTC_PIN_FUNC_2, FTC_PIN_DIRECTION_OUT);
                    g_ftc_node_priv_ctx->t_phy_rx_pin_func.e_flash_func[i] = FTC_PIN_FUNC_2;
                }
            }
        }
    }
    AX_OSAL_SYNC_spin_unlock_irqrestore(&g_ftc_node_priv_ctx->ftc_cfg_slock, &flags);

    /* find snap ppl pipe id */
    if (is_trigger) {
        for (i = 0; i < AX_ISP_SHUTTER_MAP_NUM; i++) {
            if (VIN_SYNC_SAMPLE_PIPE_MODE_FLASH_SNAP == pftc_info->t_sync_info.szShutterMap[i].eShutterMode) {
                ftc_event_info->pipe_id = pftc_info->t_sync_info.szShutterMap[i].nPipeId;
                break;
            }
        }
        if (trigger_select != VIN_SYNC_LIGHT_TRIGGER_INSERT) {
            __vin_dev_ftc_get_exp_pipeid(AX_NULL);
        }
    } else {
        __vin_dev_ftc_get_exp_pipeid(ftc_event_info);
    }

    for (i = 0; i < AX_ISP_SHUTTER_MAP_NUM; i++) {
        if (ftc_event_info->pipe_id == pftc_info->t_sync_info.szShutterMap[i].nPipeId) {
            //g_ftc_node_priv_ctx->shutter_mode = pftc_info->t_sync_info.szShutterMap[i].eShutterMode;
            //__vin_ftc_node_expinfo_add_shuttermode(g_ftc_node_priv_ctx->shutter_mode, ftc_event_info);
            break;
        }
    }

    return retval;
}


AX_S32 vin_ftc_node_set_data(AX_U32 cmd, AX_VOID *pdata)
{

    VIN_CHECK_PTR(pdata);
    switch (cmd) {
    case DEV_NODE_CMD_SYNC_POWERATTR_SET:
        return __vin_dev_ftc_flash_power_attr_set(pdata);
    case DEV_NODE_CMD_SENSOR_VTS_SET:
        return __vin_dev_ftc_sensor_vts_set(pdata);
    case DEV_NODE_CMD_VSYNCATTR_SET:
        return __vin_dev_ftc_vsyncattr_set(pdata);
    case DEV_NODE_CMD_VSYNC_ENABLE_SET: //15
        return __vin_dev_ftc_vsync_enable_set(pdata);

    case DEV_NODE_CMD_HSYNCATTR_SET:
        return __vin_dev_ftc_hsyncattr_set(pdata);
    case DEV_NODE_CMD_HSYNC_ENABLE_SET:
        return __vin_dev_ftc_hsync_enable_set(pdata);

    case DEV_NODE_CMD_LIGHTSYNCINFO_SET:
        return __vin_dev_ftc_lightsyncinfo_set(pdata);

    case DEV_NODE_CMD_LIGHTSYNCATTR_SET:
        return __vin_dev_ftc_lightsyncattr_set(pdata);
    case DEV_NODE_CMD_LIGHT_ENABLE_SET:
        return __vin_dev_ftc_light_enable_set(pdata);
    case DEV_NODE_CMD_LIGHT_GROUP_ENABLE_SET:
        return __vin_dev_ftc_light_group_enable_set(pdata);

    /* notify */
    case DEV_NODE_CMD_SYNC_EXP_PIPEID_GET:
        return __vin_dev_ftc_node_trigger_event_find(pdata);
    case DEV_NODE_CMD_SYNC_SNS_INFO_SET:
        return __vin_dev_ftc_sns_info_set(pdata);
    case DEV_NODE_CMD_SYNC_FRAME_INFO_SET:
        return __vin_dev_ftc_frame_info_set(pdata);
    case DEV_NODE_CMD_FREEZE_COMMBUS:
        return __vin_dev_ftc_freeze_commbus(pdata);
    case DEV_NODE_CMD_REGISTER_BUS_CALLBACK:
        return __vin_dev_ftc_register_bus_callback(pdata);

    default:
        isp_err("Unknown data_type(%d)\n", cmd);
        return AX_ERR_VIN_ILLEGAL_PARAM;
    }

    return 0;
}

AX_S32 vin_ftc_node_get_data(AX_U32 cmd, AX_VOID *pdata)
{
    //vin_node_context_t *pnode_ctx = AX_NULL;

    VIN_CHECK_PTR(pdata);
    //pnode_ctx = (vin_node_context_t *)handle;

    switch (cmd) {
    case DEV_NODE_CMD_SYNC_POWERATTR_GET: //12
        return __vin_dev_ftc_flash_power_attr_get(pdata);

    case DEV_NODE_CMD_VSYNCATTR_GET:  //14
        return __vin_dev_ftc_vsyncattr_get(pdata);

    case DEV_NODE_CMD_HSYNCATTR_GET:
        return __vin_dev_ftc_hsyncattr_get(pdata);

    case DEV_NODE_CMD_LIGHTSYNCINFO_GET:
        return __vin_dev_ftc_lightsyncinfo_get(pdata);

    case DEV_NODE_CMD_LIGHTSYNCATTR_GET:
        return __vin_dev_ftc_lightsyncattr_get(pdata);
    default:
        isp_err("Unknown data_type(%d)\n", cmd);
        return AX_ERR_VIN_ILLEGAL_PARAM;
    }

    return 0;
}
