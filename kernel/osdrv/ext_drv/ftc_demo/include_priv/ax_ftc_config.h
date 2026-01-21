/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_ISP_SIF_SNS_CONFIG_H__
#define __DEF_ISP_SIF_SNS_CONFIG_H__

#include "ax_sif_reg.h"
#include "ax_sif_priv.h"
#include "ax_ftc_extern.h"


static ISP_SIF_SNS_FLASH_REG_T *isp_sif_sns_flash_regmap = (ISP_SIF_SNS_FLASH_REG_T *)ISP_SIF_FLASH_REG_OFFSET;

static __inline AX_S32 isp_sif_sen_shadow_cfg_set(regio_global_handle_t io_handle, sen_shadow_cfg_s *sen_shadow_cfg_ptr)
{
    SEN_SHADOW_U sen_shadow = {0};

    sen_shadow.bits.sen_shadow_en0 = sen_shadow_cfg_ptr->sen_shadow_en0;
    sen_shadow.bits.sen_shadow_en1 = sen_shadow_cfg_ptr->sen_shadow_en1;
    sen_shadow.bits.sen_shadow_sts0 = sen_shadow_cfg_ptr->sen_shadow_sts0;
    sen_shadow.bits.sen_shadow_sts1 = sen_shadow_cfg_ptr->sen_shadow_sts1;
    sen_shadow.bits.sen_shadow_wr_id0 = sen_shadow_cfg_ptr->sen_shadow_wr_id0;
    sen_shadow.bits.sen_shadow_rd_id0 = sen_shadow_cfg_ptr->sen_shadow_rd_id0;
    sen_shadow.bits.sen_shadow_wr_id1 = sen_shadow_cfg_ptr->sen_shadow_wr_id1;
    sen_shadow.bits.sen_shadow_rd_id1 = sen_shadow_cfg_ptr->sen_shadow_rd_id1;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_SHADOW.reg32), sen_shadow.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_shadow_cfg_get(regio_global_handle_t io_handle, sen_shadow_cfg_s *sen_shadow_cfg_ptr)
{
    SEN_SHADOW_U sen_shadow = {0};
    sen_shadow.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_SHADOW.reg32));
    sen_shadow_cfg_ptr->sen_shadow_en0 = sen_shadow.bits.sen_shadow_en0;
    sen_shadow_cfg_ptr->sen_shadow_en1 = sen_shadow.bits.sen_shadow_en1;
    sen_shadow_cfg_ptr->sen_shadow_sts0 = sen_shadow.bits.sen_shadow_sts0;
    sen_shadow_cfg_ptr->sen_shadow_sts1 = sen_shadow.bits.sen_shadow_sts1;
    sen_shadow_cfg_ptr->sen_shadow_wr_id0 = sen_shadow.bits.sen_shadow_wr_id0;
    sen_shadow_cfg_ptr->sen_shadow_rd_id0 = sen_shadow.bits.sen_shadow_rd_id0;
    sen_shadow_cfg_ptr->sen_shadow_wr_id1 = sen_shadow.bits.sen_shadow_wr_id1;
    sen_shadow_cfg_ptr->sen_shadow_rd_id1 = sen_shadow.bits.sen_shadow_rd_id1;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_ctrl_cfg_set(regio_global_handle_t io_handle, sen_ctrl_cfg_s *sen_ctrl_cfg_ptr)
{
    SEN_CTRL_U sen_ctrl = {0};

    sen_ctrl.bits.sen_ctrl_en = sen_ctrl_cfg_ptr->sen_ctrl_en;
    sen_ctrl.bits.sen_ctrl_mode = sen_ctrl_cfg_ptr->sen_ctrl_mode;
    sen_ctrl.bits.sen_expo_v0_ctrl = sen_ctrl_cfg_ptr->sen_expo_v0_ctrl;
    sen_ctrl.bits.sen_expo_v1_ctrl = sen_ctrl_cfg_ptr->sen_expo_v1_ctrl;
    sen_ctrl.bits.sen_expo_v2_ctrl = sen_ctrl_cfg_ptr->sen_expo_v2_ctrl;
    sen_ctrl.bits.sen_expo_v3_ctrl = sen_ctrl_cfg_ptr->sen_expo_v3_ctrl;
    sen_ctrl.bits.sen_expo_h0_ctrl = sen_ctrl_cfg_ptr->sen_expo_h0_ctrl;
    sen_ctrl.bits.sen_expo_h1_ctrl = sen_ctrl_cfg_ptr->sen_expo_h1_ctrl;
    sen_ctrl.bits.sen_expo_h2_ctrl = sen_ctrl_cfg_ptr->sen_expo_h2_ctrl;
    sen_ctrl.bits.sen_expo_h3_ctrl = sen_ctrl_cfg_ptr->sen_expo_h3_ctrl;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_CTRL.reg32), sen_ctrl.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_ctrl_cfg_get(regio_global_handle_t io_handle, sen_ctrl_cfg_s *sen_ctrl_cfg_ptr)
{
    SEN_CTRL_U sen_ctrl = {0};
    sen_ctrl.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_CTRL.reg32));
    sen_ctrl_cfg_ptr->sen_ctrl_en = sen_ctrl.bits.sen_ctrl_en;
    sen_ctrl_cfg_ptr->sen_ctrl_mode = sen_ctrl.bits.sen_ctrl_mode;
    sen_ctrl_cfg_ptr->sen_expo_v0_ctrl = sen_ctrl.bits.sen_expo_v0_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_v1_ctrl = sen_ctrl.bits.sen_expo_v1_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_v2_ctrl = sen_ctrl.bits.sen_expo_v2_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_v3_ctrl = sen_ctrl.bits.sen_expo_v3_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_h0_ctrl = sen_ctrl.bits.sen_expo_h0_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_h1_ctrl = sen_ctrl.bits.sen_expo_h1_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_h2_ctrl = sen_ctrl.bits.sen_expo_h2_ctrl;
    sen_ctrl_cfg_ptr->sen_expo_h3_ctrl = sen_ctrl.bits.sen_expo_h3_ctrl;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_internal_trig_cfg_set(regio_global_handle_t io_handle, AX_U32 data)
{
    SEN_INTERNAL_TRIG_U sen_internal_trig = {0};

    sen_internal_trig.bits.sen_internal_trig = data;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_INTERNAL_TRIG.reg32),
                           sen_internal_trig.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_v_cfg_set(regio_global_handle_t io_handle, AX_U32 chn, AX_U32 sen_expo_v_num_total,
        AX_U32 sen_expo_v_num)
{
    SEN_EXPO_V_U sen_expo_v = {0};

    sen_expo_v.bits.sen_expo_v_num = sen_expo_v_num;
    sen_expo_v.bits.sen_expo_v_num_total = sen_expo_v_num_total;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO[chn].SEN_EXPO_V.reg32),
                           sen_expo_v.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_v_cfg_get(regio_global_handle_t io_handle, AX_U32 chn,
        AX_U32 *sen_expo_v_num_total, AX_U32 *sen_expo_v_num)
{
    SEN_EXPO_V_U sen_expo_v = {0};
    sen_expo_v.reg32 = ISP_DRV_IO_READ32(io_handle,
                       AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO[chn].SEN_EXPO_V.reg32));
    *sen_expo_v_num = sen_expo_v.bits.sen_expo_v_num;
    *sen_expo_v_num_total = sen_expo_v.bits.sen_expo_v_num_total;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_h_cfg_set(regio_global_handle_t io_handle, AX_U32 chn, AX_U32 sen_expo_h_num_total,
        AX_U32 sen_expo_h_num)
{
    SEN_EXPO_H_U sen_expo_h = {0};

    sen_expo_h.bits.sen_expo_h_num = sen_expo_h_num;
    sen_expo_h.bits.sen_expo_h_num_total = sen_expo_h_num_total;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO[chn].SEN_EXPO_H.reg32),
                           sen_expo_h.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_h_cfg_get(regio_global_handle_t io_handle, AX_U32 chn,
        AX_U32 *sen_expo_h_num_total, AX_U32 *sen_expo_h_num)
{
    SEN_EXPO_H_U sen_expo_h = {0};
    sen_expo_h.reg32 = ISP_DRV_IO_READ32(io_handle,
                       AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO[chn].SEN_EXPO_H.reg32));
    *sen_expo_h_num = sen_expo_h.bits.sen_expo_h_num;
    *sen_expo_h_num_total = sen_expo_h.bits.sen_expo_h_num_total;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_adjust0_cfg_set(regio_global_handle_t io_handle, AX_U32 sen_expo_v_dly_num)
{
    SEN_EXPO_ADJUST0_U sen_expo_adjust0 = {0};

    sen_expo_adjust0.bits.sen_expo_v_dly_num = sen_expo_v_dly_num;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO_ADJUST0.reg32), sen_expo_adjust0.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_adjust0_cfg_get(regio_global_handle_t io_handle, AX_U32 *sen_expo_v_dly_num)
{
    SEN_EXPO_ADJUST0_U sen_expo_adjust0 = {0};
    sen_expo_adjust0.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO_ADJUST0.reg32));
    *sen_expo_v_dly_num = sen_expo_adjust0.bits.sen_expo_v_dly_num;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_adjust1_cfg_set(regio_global_handle_t io_handle, AX_U32 sen_expo_adjust_line_start,
        AX_U32 sen_expo_adjust_line_num)
{
    SEN_EXPO_ADJUST1_U sen_expo_adjust1 = {0};

    sen_expo_adjust1.bits.sen_expo_adjust_line_num = sen_expo_adjust_line_num;
    sen_expo_adjust1.bits.sen_expo_adjust_line_start = sen_expo_adjust_line_start;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO_ADJUST1.reg32), sen_expo_adjust1.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_expo_adjust1_cfg_get(regio_global_handle_t io_handle,
        AX_U32 *sen_expo_adjust_line_start, AX_U32 *sen_expo_adjust_line_num)
{
    SEN_EXPO_ADJUST1_U sen_expo_adjust1 = {0};
    sen_expo_adjust1.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_EXPO_ADJUST1.reg32));
    *sen_expo_adjust_line_num = sen_expo_adjust1.bits.sen_expo_adjust_line_num;
    *sen_expo_adjust_line_start = sen_expo_adjust1.bits.sen_expo_adjust_line_start;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_flash_ctrl_cfg_set(regio_global_handle_t io_handle,
        sen_flash_ctrl_cfg_s *sen_flash_ctrl_cfg_ptr)
{
    SEN_FLASH_CTRL_U sen_flash_ctrl = {0};

    sen_flash_ctrl.bits.sen_flash0_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash0_ctrl;
    sen_flash_ctrl.bits.sen_flash1_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash1_ctrl;
    sen_flash_ctrl.bits.sen_flash2_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash2_ctrl;
    sen_flash_ctrl.bits.sen_flash3_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash3_ctrl;
    sen_flash_ctrl.bits.sen_flash4_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash4_ctrl;
    sen_flash_ctrl.bits.sen_flash5_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash5_ctrl;
    sen_flash_ctrl.bits.sen_flash6_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash6_ctrl;
    sen_flash_ctrl.bits.sen_flash7_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash7_ctrl;
    sen_flash_ctrl.bits.sen_flash8_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash8_ctrl;
    sen_flash_ctrl.bits.sen_flash9_ctrl = sen_flash_ctrl_cfg_ptr->sen_flash9_ctrl;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_FLASH_CTRL.reg32), sen_flash_ctrl.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_flash_ctrl_cfg_get(regio_global_handle_t io_handle,
        sen_flash_ctrl_cfg_s *sen_flash_ctrl_cfg_ptr)
{
    SEN_FLASH_CTRL_U sen_flash_ctrl = {0};
    sen_flash_ctrl.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_FLASH_CTRL.reg32));
    sen_flash_ctrl_cfg_ptr->sen_flash0_ctrl = sen_flash_ctrl.bits.sen_flash0_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash1_ctrl = sen_flash_ctrl.bits.sen_flash1_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash2_ctrl = sen_flash_ctrl.bits.sen_flash2_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash3_ctrl = sen_flash_ctrl.bits.sen_flash3_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash4_ctrl = sen_flash_ctrl.bits.sen_flash4_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash5_ctrl = sen_flash_ctrl.bits.sen_flash5_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash6_ctrl = sen_flash_ctrl.bits.sen_flash6_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash7_ctrl = sen_flash_ctrl.bits.sen_flash7_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash8_ctrl = sen_flash_ctrl.bits.sen_flash8_ctrl;
    sen_flash_ctrl_cfg_ptr->sen_flash9_ctrl = sen_flash_ctrl.bits.sen_flash9_ctrl;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_flash_cfg_set(regio_global_handle_t io_handle, AX_U8 dev_id, AX_U32 sen_flash_dly_num,
        AX_U32 sen_flash_on_num)
{
    SEN_FLASH_U sen_flash = {0};

    sen_flash.bits.sen_flash_on_num = sen_flash_on_num;
    sen_flash.bits.sen_flash_dly_num = sen_flash_dly_num;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_FLASH[dev_id].reg32), sen_flash.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_flash_cfg_get(regio_global_handle_t io_handle, AX_U8 dev_id, AX_U32 *sen_flash_dly_num,
        AX_U32 *sen_flash_on_num)
{
    SEN_FLASH_U sen_flash = {0};
    sen_flash.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_FLASH[dev_id].reg32));
    *sen_flash_on_num = sen_flash.bits.sen_flash_on_num;
    *sen_flash_dly_num = sen_flash.bits.sen_flash_dly_num;
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_debug_cfg_set(regio_global_handle_t io_handle, AX_U32 sen_debug_enable,
        AX_U32 sen_debug_status)
{
    SEN_DEBUG_U sen_debug = {0};
    sen_debug.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_DEBUG.reg32));
    sen_debug.bits.sen_debug_status = sen_debug_status;
    sen_debug.bits.sen_debug_enable = sen_debug_enable;
    ISP_DRV_IO_WRITE32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_DEBUG.reg32), sen_debug.reg32);
    return AX_ISP_SUCCESS;
}

static __inline AX_S32 isp_sif_sen_debug_cfg_get(regio_global_handle_t io_handle, AX_U32 *sen_debug_enable,
        AX_U32 *sen_debug_status)
{
    SEN_DEBUG_U sen_debug = {0};
    sen_debug.reg32 = ISP_DRV_IO_READ32(io_handle, AX_OFFSET(isp_sif_sns_flash_regmap->SEN_DEBUG.reg32));
    *sen_debug_status = sen_debug.bits.sen_debug_status;
    *sen_debug_enable = sen_debug.bits.sen_debug_enable;
    return AX_ISP_SUCCESS;
}


#endif // __DEF_ISP_SIF_SNS_CONFIG_H__
