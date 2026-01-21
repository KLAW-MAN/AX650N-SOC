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
#include <stdlib.h>

#include "ax_sensor_struct.h"
#include "ax_base_type.h"
#include "ax_isp_common.h"
#include "isp_sensor_internal.h"
#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"


#include "sc910gs_settings.h"
#include "sc910gs_reg.h"
#include "sc910gs_ae_ctrl.h"

/* default param */
#include "sc910gs_sdr.h"

#include "ax_module_version.h"
/****************************************************************************
 * golbal variables  and macro definition
 ****************************************************************************/

SNS_STATE_OBJ *g_szsc910gsCtx[AX_VIN_MAX_PIPE_NUM] = {NULL};

#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szsc910gsCtx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (g_szsc910gsCtx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (g_szsc910gsCtx[dev] = NULL)

const char axera_sns_sc910gs_version[] = AXERA_MODULE_VERSION;

extern AX_SNS_COMMBUS_T gsc910gsBusInfo[AX_VIN_MAX_PIPE_NUM];

/****************************************************************************
 * Internal function definition
 ****************************************************************************/
static AX_S32 sensor_ctx_init(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 ret = 0;

    SNS_DBG("sc910gs sensor_ctx_init. ret = %d\n", ret);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        sns_obj = (SNS_STATE_OBJ *)calloc(1, sizeof(SNS_STATE_OBJ));
        if (AX_NULL == sns_obj) {
            SNS_ERR("malloc g_szsc910gsCtx failed\n");
            return SNS_ERR_CODE_NOT_MEM;
        }
    }

    memset(sns_obj, 0, sizeof(SNS_STATE_OBJ));

    SENSOR_SET_CTX(nPipeId, sns_obj);

    return SNS_SUCCESS;
}

static AX_VOID sensor_ctx_exit(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SENSOR_GET_CTX(nPipeId, sns_obj);
    free(sns_obj);
    SENSOR_RESET_CTX(nPipeId);
}

/****************************************************************************
 * sensor control function
 ****************************************************************************/
static AX_S32 sc910gs_get_chipid(ISP_PIPE_ID nPipeId, AX_S32 *pSnsId)
{
    AX_U32 sensor_id = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sensor_id |= sc910gs_reg_read(nPipeId, 0x3107) << 8;
    sensor_id |= sc910gs_reg_read(nPipeId, 0x3108);

    SNS_DBG("%s: sensor sc910gs id: 0x%x\n", __func__, sensor_id);

    if (sensor_id != SC910GS_SENSOR_CHIP_ID) {
        SNS_ERR("%s: Failed to read sensor sc910gs id 0x%x\n", __func__, sensor_id);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    *pSnsId = sensor_id;

    return SNS_SUCCESS;
}

static void sc910gs_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 nRet = 0;
    AX_S32 nImagemode = 0;
    AX_S32 nSnsId = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    /* 1. contex init */
    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        /* contex init */
        nRet = sensor_ctx_init(nPipeId);
        if (0 != nRet) {
            SNS_ERR("sensor_ctx_init failed!\n");
            return;
        } else {
            SENSOR_GET_CTX(nPipeId, sns_obj);
        }
    }

    /* 2. i2c init */
    if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        sc910gs_sensor_i2c_init(nPipeId);
    } else {
        sc910gs_sensor_spi_init(nPipeId);
    }

    nRet = sc910gs_get_chipid(nPipeId, &nSnsId);
    if (nRet != SNS_SUCCESS) {
        SNS_ERR("can't find sc910gs sensor id.\n");
    } else {
        SNS_DBG("sc910gs check chip id success.\n");
    }

    /* 3. config settings  */
    sc910gs_write_settings(nPipeId);

    /* 4. refresh ae param */
    sc910gs_cfg_aec_param(nPipeId);

    /* 5. refresh ae regs table */
    sc910gs_sns_refresh_all_regs_from_tbl(nPipeId);

    sns_obj->bSyncInit = AX_FALSE;

    return;
}

static void sc910gs_exit(ISP_PIPE_ID nPipeId)
{
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        sc910gs_sensor_i2c_exit(nPipeId);
    } else {
        sc910gs_sensor_spi_exit(nPipeId);
    }
    sensor_ctx_exit(nPipeId);

    return;
}

static AX_S32 sc910gs_sensor_streaming_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    if (1 == on) {
        result = sc910gs_write_register(nPipeId, 0x0100, 0x01); // stream on
        SNS_DBG("sensor stream on!\n");
    } else {
        result = sc910gs_write_register(nPipeId, 0x0100, 0x00); // stream off
        SNS_DBG("sensor stream off!\n");
    }

    if (0 != result) {
        return -1;
    }

    return SNS_SUCCESS;
}

static AX_S32 sc910gs_sensor_set_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *sns_mode)
{
    AX_S32 width;
    AX_S32 height;
    AX_S32 hdrmode;
    AX_F32 framerate = 30;
    AX_RAW_TYPE_E   RawType;
    AX_SNS_MASTER_SLAVE_E   MasterSlaveSel;
    AX_S32 sns_setting_index = 0;
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(sns_mode);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        /* contex init */
        nRet = sensor_ctx_init(nPipeId);
        if (0 != nRet) {
            SNS_ERR("sensor_ctx_init failed!\n");
            return SNS_ERR_CODE_INIT_FAILD;
        } else {
            SENSOR_GET_CTX(nPipeId, sns_obj);
        }
    }

    width = sns_mode->nWidth;
    height = sns_mode->nHeight;
    framerate = sns_mode->fFrameRate;
    RawType = sns_mode->eRawType;
    MasterSlaveSel = sns_mode->eMasterSlaveSel;
    if ((AX_SNS_LINEAR_MODE != sns_mode->eSnsMode) &&
        (AX_SNS_HDR_2X_MODE != sns_mode->eSnsMode) &&
        (AX_SNS_HDR_3X_MODE != sns_mode->eSnsMode)) {
        hdrmode = AX_SNS_LINEAR_MODE;
    } else {
        hdrmode = sns_mode->eSnsMode;
    }

    if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW10 && IS_SNS_FPS_EQUAL(framerate, 25.0) && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_SC910GS_1x1C4D_3840x2336_LINEAR_10bit_25fps_4lane_450Mbps_MASTER;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW10 && IS_SNS_FPS_EQUAL(framerate, 30.0) && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_SC910GS_1x1C4D_3840x2336_LINEAR_10bit_30fps_4lane_450Mbps_MASTER;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW10 && IS_SNS_FPS_EQUAL(framerate, 50.0) && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_10bit_50fps_8lane_750Mbps_MASTER;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW12 && IS_SNS_FPS_EQUAL(framerate, 25.0) && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_25fps_8lane_540Mbps_MASTER;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW12 && IS_SNS_FPS_EQUAL(framerate, 30.0) && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_30fps_8lane_540Mbps_MASTER;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW10 && IS_SNS_FPS_EQUAL(framerate, 50.0) && MasterSlaveSel == AX_SNS_SLAVE) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_10bit_50fps_8lane_750Mbps_Trigger_Readout;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW12 && IS_SNS_FPS_EQUAL(framerate, 25.0) && MasterSlaveSel == AX_SNS_SLAVE) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_25fps_8lane_540Mbps_Trigger_Readout;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW12 && IS_SNS_FPS_EQUAL(framerate, 30.0) && MasterSlaveSel == AX_SNS_SLAVE) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_30fps_8lane_540Mbps_Trigger_Readout;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW10 && IS_SNS_FPS_EQUAL(framerate, 54.0) && MasterSlaveSel == AX_SNS_SLAVE) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_10bit_54fps_8lane_810Mbps_Trigger_Exposure;
    } else if (width == 3840 && height == 2336 && hdrmode == 1 && RawType == AX_RT_RAW12 && IS_SNS_FPS_EQUAL(framerate, 45.0) && MasterSlaveSel == AX_SNS_SLAVE) {
        sns_setting_index = e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_45fps_8lane_810Mbps_Trigger_Exposure;
    } else {
        SNS_ERR("%s it's not supported. [%dx%d mode=%d fps=%f] \n",
                __func__, width, height, hdrmode, framerate);
        return -1;
    }

    /* optional, Not Recommended. if nSettingIndex > 0 will take effect */
    if (sns_mode->nSettingIndex > 0) {
        sns_setting_index = sns_mode->nSettingIndex;
    }

    sns_obj->eImgMode = sns_setting_index;
    sns_obj->sns_mode_obj.eHDRMode = hdrmode;
    sns_obj->sns_mode_obj.nWidth = width;
    sns_obj->sns_mode_obj.nHeight = height;
    sns_obj->sns_mode_obj.fFrameRate = framerate;
    sns_obj->sns_mode_obj.eMasterSlaveSel = MasterSlaveSel;
    memcpy(&sns_obj->sns_attr_param, sns_mode, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

static AX_S32 sc910gs_sensor_get_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *pSnsMode)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(pSnsMode);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        /* contex init */
        nRet = sensor_ctx_init(nPipeId);
        if (0 != nRet) {
            SNS_ERR("sensor_ctx_init failed!\n");
            return -1;
        } else {
            SENSOR_GET_CTX(nPipeId, sns_obj);
        }
    }

    memcpy(pSnsMode, &sns_obj->sns_attr_param, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

static AX_S32 sc910gs_testpattern_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SNS_DBG("test pattern enable: %d\n", on);
    if (1 == on) {
        /* enable test-pattern */
        sc910gs_write_register(nPipeId, 0x4501, 0xbc);
        sc910gs_write_register(nPipeId, 0x3902, 0x81);
    } else {
        /* disable test-pattern */
        sc910gs_write_register(nPipeId, 0x4501, 0xb4);
        sc910gs_write_register(nPipeId, 0x3902, 0xc0);
    }

    return SNS_SUCCESS;
}

/****************************************************************************
 * get module default parameters function
 ****************************************************************************/
static AX_S32 sc910gs_get_isp_default_params(ISP_PIPE_ID nPipeId, AX_SENSOR_DEFAULT_PARAM_T *ptDftParam)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_SNS_HDR_MODE_E nHdrmode;

    SNS_CHECK_PTR_VALID(ptDftParam);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);


    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        /* contex init */
        nRet = sensor_ctx_init(nPipeId);
        if (0 != nRet) {
            SNS_ERR("sensor_ctx_init failed!\n");
            return SNS_ERR_CODE_INIT_FAILD;
        } else {
            SENSOR_GET_CTX(nPipeId, sns_obj);
        }
    }

    memset(ptDftParam, 0, sizeof(AX_SENSOR_DEFAULT_PARAM_T));

    nHdrmode = sns_obj->sns_mode_obj.eHDRMode;

    SNS_DBG(" current hdr mode %d \n", nHdrmode);

    switch (nHdrmode) {
    case AX_SNS_LINEAR_MODE:
        /* TODO: Users configure their own default parameters */
        ptDftParam->ptDpc           = &dpc_param_sdr;
        ptDftParam->ptBlc           = &blc_param_sdr;
        ptDftParam->ptFpn           = &fpn_param_sdr;
        ptDftParam->ptDarkshading   = &darkshading_param_sdr;
        ptDftParam->ptCsc           = &csc_param_sdr;
        ptDftParam->ptDemosaic      = &demosaic_param_sdr;
        ptDftParam->ptGic           = &gic_param_sdr;
        ptDftParam->ptGamma         = &gamma_param_sdr;
        ptDftParam->ptCc            = &cc_param_sdr;
        ptDftParam->ptMde           = &mde_param_sdr;
        ptDftParam->ptAYnr          = &aynr_param_sdr;
        ptDftParam->ptACnr          = &acnr_param_sdr;
        ptDftParam->ptRltm          = &rltm_param_sdr;
        ptDftParam->ptRaw3dnr       = &raw3dnr_param_sdr;
        ptDftParam->ptDepurple      = &depurple_param_sdr;
        ptDftParam->ptLsc           = &lsc_param_sdr;
        ptDftParam->ptSharpen       = &sharpen_param_sdr;
        ptDftParam->ptScm           = &scm_param_sdr;
        ptDftParam->ptYnr           = &ynr_param_sdr;
        ptDftParam->ptCnr           = &cnr_param_sdr;
        ptDftParam->ptCcmp          = &ccmp_param_sdr;
        ptDftParam->ptYcproc        = &ycproc_param_sdr;
        ptDftParam->ptYcrt          = &ycrt_param_sdr;
        ptDftParam->ptCa            = &ca_param_sdr;
        ptDftParam->ptAinr          = &ainr_param_sdr;
        ptDftParam->ptAwbDftParam   = &awb_param_sdr;
        ptDftParam->ptAeDftParam    = &ae_param_sdr;
        ptDftParam->pt3Dlut         = &isp_3dlut_param_sdr;
        ptDftParam->ptAice          = &aice_param_sdr;
        ptDftParam->ptDehaze        = &dehaze_param_sdr;
        ptDftParam->ptScene         = &scene_param_sdr;
        ptDftParam->ptLdc           = &ldc_param_sdr;
        ptDftParam->ptDis           = &dis_param_sdr;
        ptDftParam->ptMe            = &me_param_sdr;
        break;

    case AX_SNS_HDR_2X_MODE:
        /*
        * TODO: Users configure their own default parameters
        * FIXME: Need change to 2x struct
         */
        break;

    case AX_SNS_HDR_3X_MODE:
        /* TODO: Users configure their own default parameters */
        break;

    case AX_SNS_HDR_4X_MODE:
        /* TODO: Users configure their own default parameters */
        break;
    default:
        break;
    }

    return SNS_SUCCESS;
}

static AX_S32 sc910gs_get_isp_black_level(ISP_PIPE_ID nPipeId, AX_SNS_BLACK_LEVEL_T *ptBlackLevel)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(ptBlackLevel);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    /* black level of linear mode */
    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptBlackLevel->nBlackLevel[0] = 1040;  /*sc910 sdr 10bit 260/4 => u8.6 260*4 */
        ptBlackLevel->nBlackLevel[1] = 1040;
        ptBlackLevel->nBlackLevel[2] = 1040;
        ptBlackLevel->nBlackLevel[3] = 1040;
    } else {
    /**/
    }

    return SNS_SUCCESS;
}


AX_SYS_API_PUBLIC AX_SENSOR_REGISTER_FUNC_T gSnssc910gsObj = {

    /* sensor ctrl */
    .pfn_sensor_chipid                      = sc910gs_get_chipid,
    .pfn_sensor_init                        = sc910gs_init,
    .pfn_sensor_exit                        = sc910gs_exit,
    .pfn_sensor_reset                       = sc910gs_reset,
    .pfn_sensor_streaming_ctrl              = sc910gs_sensor_streaming_ctrl,
    .pfn_sensor_testpattern                 = sc910gs_testpattern_ctrl,

    .pfn_sensor_set_mode                    = sc910gs_sensor_set_mode,
    .pfn_sensor_get_mode                    = sc910gs_sensor_get_mode,

    /* communication : register read/write */
    .pfn_sensor_set_bus_info                = sc910gs_set_bus_info,
    .pfn_sensor_write_register              = sc910gs_write_register,
    .pfn_sensor_read_register               = sc910gs_read_register,

    /* default param */
    .pfn_sensor_get_default_params          = sc910gs_get_isp_default_params,
    .pfn_sensor_get_black_level             = sc910gs_get_isp_black_level,

    /* ae ctrl */
    .pfn_sensor_get_hw_exposure_params     = sc910gs_get_hw_exposure_params,
    .pfn_sensor_get_gain_table              = sc910gs_get_sensor_gain_table,
    .pfn_sensor_set_again                   = sc910gs_set_again,
    .pfn_sensor_set_dgain                   = sc910gs_set_dgain,
    .pfn_sensor_hcglcg_ctrl                 = AX_NULL,

    .pfn_sensor_set_integration_time        = sc910gs_set_integration_time,
    .pfn_sensor_get_integration_time_range  = sc910gs_get_integration_time_range,
    .pfn_sensor_set_slow_fps                = sc910gs_set_fps,
    .pfn_sensor_get_slow_shutter_param      = AX_NULL,
    .pfn_sensor_get_sns_reg_info            = sc910gs_ae_get_sensor_reg_info,
    .pfn_sensor_set_wbgain                  = AX_NULL,
};
