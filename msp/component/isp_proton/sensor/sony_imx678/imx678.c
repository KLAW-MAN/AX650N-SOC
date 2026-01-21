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

#include "imx678_settings.h"
#include "imx678_reg.h"
#include "imx678_ae_ctrl.h"

/* default param */
#include "imx678_sdr.h"
#include "imx678_hdr_2x.h"

#include "ax_module_version.h"

/****************************************************************************
 * golbal variables  and macro definition
 ****************************************************************************/
SNS_STATE_OBJ *g_szImx678Ctx[AX_VIN_MAX_PIPE_NUM] = {NULL};

#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szImx678Ctx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (g_szImx678Ctx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (g_szImx678Ctx[dev] = NULL)

const char axera_sns_imx678_version[] = AXERA_MODULE_VERSION;

/****************************************************************************
 * Internal function definition
 ****************************************************************************/
static AX_S32 sensor_ctx_init(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 ret = 0;

    SNS_DBG("imx678 sensor_ctx_init. ret = %d\n", ret);

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SENSOR_GET_CTX(nPipeId, sns_obj);

    if (AX_NULL == sns_obj) {
        sns_obj = (SNS_STATE_OBJ *)calloc(1, sizeof(SNS_STATE_OBJ));
        if (AX_NULL == sns_obj) {
            SNS_ERR("malloc sns_ctx failed\n");
            return -1;
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
static AX_S32 imx678_get_chipid(ISP_PIPE_ID nPipeId, AX_S32 *pSnsId)
{
    AX_U16 sensor_id = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sensor_id |= imx678_reg_read(nPipeId, 0x4d1d) << 8;
    sensor_id |= imx678_reg_read(nPipeId, 0x4d1c);

    SNS_DBG("%s: sensor imx678 id: 0x%x\n", __func__, sensor_id);

    *pSnsId = IMX678_SENSOR_CHIP_ID;


    return SNS_SUCCESS;

}

static void imx678_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 nRet = 0;
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
    SENSOR_GET_CTX(nPipeId, sns_obj);

    /* 2. i2c init */
    imx678_sensor_i2c_init(nPipeId);
    nRet = imx678_get_chipid(nPipeId, &nSnsId);
    if (nRet != SNS_SUCCESS) {
        SNS_ERR("can't find imx678 sensor id.\n");
    } else {
        SNS_DBG("imx678 check chip id success.\n");
    }

    /* 3. config settings  */
    imx678_write_settings(nPipeId);

    /* 4. refresh ae param */
    imx678_cfg_aec_param(nPipeId);

    /* 5. refresh ae regs table */
    imx678_sns_refresh_all_regs_from_tbl(nPipeId);

    sns_obj->bSyncInit = AX_FALSE;

    return;
}

static void imx678_exit(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    imx678_sensor_i2c_exit(nPipeId);
    sensor_ctx_exit(nPipeId);

    return;
}

static AX_S32 imx678_sensor_streaming_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    if (1 == on) {
        result |= imx678_write_register(nPipeId, 0x3002, 0x00); // stream on
        result |= imx678_write_register(nPipeId, 0x3000, 0x00); // stream on
        SNS_DBG("sensor stream on!\n");
    } else {
        result |= imx678_write_register(nPipeId, 0x3000, 0x01); // stream off
        result |= imx678_write_register(nPipeId, 0x3002, 0x01); // stream off
        SNS_DBG("sensor stream off!\n");
    }
    if (0 != result) {
        return -1;
    }

    return SNS_SUCCESS;
}

static AX_S32 imx678_sensor_set_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *sns_mode)
{
    AX_S32 width;
    AX_S32 height;
    AX_S32 hdrmode;
    AX_RAW_TYPE_E RawType;
    AX_S32 framerate = 30; // init value to 30fps, void null fps gives.
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
            return -1;
        } else {
            SENSOR_GET_CTX(nPipeId, sns_obj);
        }
    }

    width = sns_mode->nWidth;
    height = sns_mode->nHeight;
    framerate = sns_mode->fFrameRate;
    RawType = sns_mode->eRawType;
    if ((1 != sns_mode->eSnsMode) &&
        (2 != sns_mode->eSnsMode) &&
        (3 != sns_mode->eSnsMode)) {
        hdrmode = 1;
    } else {
        hdrmode = sns_mode->eSnsMode;
    }

    if (width == 3840 && height == 2160 && hdrmode == 1 && RawType == AX_RT_RAW10 && framerate == 60) {
        sns_setting_index = e_IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_LINEAR_60FPS;
    } else if (width == 3840 && height == 2160 && hdrmode == 1 && RawType == AX_RT_RAW10) {
        sns_setting_index = e_IMX678_4LANE_24MHZ_891MBPS_3840x2160_RGGB_10BIT_LINEAR_30FPS;
    } else if (width == 3840 && height == 2160 && hdrmode == 1 && RawType == AX_RT_RAW12) {
        sns_setting_index = e_IMX678_4LANE_24MHZ_1188MBPS_3840x2160_RGGB_12BIT_LINEAR_30FPS;
    } else if (width == 3840 && height == 2160 && hdrmode == 2 && RawType == AX_RT_RAW10) {
        sns_setting_index = e_IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_HDR2DOL_30FPS;
    } else {
        SNS_ERR("%s it's not supported. [%dx%d mode=%d fps=%d] \n",
                __func__, width, height, hdrmode, framerate);
        return -1;
    }

    /* optional, Not Recommended. if nSettingIndex > 0 will take effect */
    if (sns_mode->nSettingIndex > 0) {
        sns_setting_index = sns_mode->nSettingIndex;
    }

    SNS_DBG("sns_setting_index %d!\n", sns_setting_index);
    sns_obj->eImgMode = sns_setting_index;
    sns_obj->sns_mode_obj.eHDRMode = hdrmode;
    sns_obj->sns_mode_obj.nWidth = width;
    sns_obj->sns_mode_obj.nHeight = height;
    sns_obj->sns_mode_obj.fFrameRate = framerate;
    memcpy(&sns_obj->sns_attr_param, sns_mode, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

static AX_S32 imx678_sensor_get_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *pSnsMode)
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

static AX_S32 imx678_testpattern_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SNS_DBG("test pattern enable: %d\n", on);
    /* Set the pattern generator via sensor standby */
    if (1 == on) {
        imx678_write_register(nPipeId, 0x30e0, 0x01);
        imx678_write_register(nPipeId, 0x30e2, 0x0A); /* Horizontal color bars */
    } else {
        imx678_write_register(nPipeId, 0x30e0, 0x00);
    }


    return SNS_SUCCESS;
}

/****************************************************************************
 * get module default parameters function
 ****************************************************************************/
static AX_S32 imx678_get_isp_default_params(ISP_PIPE_ID nPipeId, AX_SENSOR_DEFAULT_PARAM_T *ptDftParam)
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
        ptDftParam->ptAice          = &aice_param_sdr;
        ptDftParam->pt3Dlut         = &isp_3dlut_param_sdr;
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
        ptDftParam->ptHdr           = &hdr_param_hdr_2x;
        ptDftParam->ptDpc           = &dpc_param_hdr_2x;
        ptDftParam->ptFpn           = &fpn_param_sdr;
        ptDftParam->ptDarkshading   = &darkshading_param_hdr_2x;
        ptDftParam->ptBlc           = &blc_param_hdr_2x;
        ptDftParam->ptCsc           = &csc_param_hdr_2x;
        ptDftParam->ptDemosaic      = &demosaic_param_hdr_2x;
        ptDftParam->ptGic           = &gic_param_hdr_2x;
        ptDftParam->ptGamma         = &gamma_param_hdr_2x;
        ptDftParam->ptCc            = &cc_param_hdr_2x;
        ptDftParam->ptMde           = &mde_param_hdr_2x;
        ptDftParam->ptAYnr          = &aynr_param_hdr_2x;
        ptDftParam->ptACnr          = &acnr_param_hdr_2x;
        ptDftParam->ptRltm          = &rltm_param_hdr_2x;
        ptDftParam->ptRaw3dnr       = &raw3dnr_param_hdr_2x;
        ptDftParam->ptDepurple      = &depurple_param_hdr_2x;
        ptDftParam->ptLsc           = &lsc_param_hdr_2x;
        ptDftParam->ptSharpen       = &sharpen_param_hdr_2x;
        ptDftParam->ptScm           = &scm_param_hdr_2x;
        ptDftParam->ptYnr           = &ynr_param_hdr_2x;
        ptDftParam->ptCnr           = &cnr_param_hdr_2x;
        ptDftParam->ptCcmp          = &ccmp_param_hdr_2x;
        ptDftParam->ptYcproc        = &ycproc_param_hdr_2x;
        ptDftParam->ptYcrt          = &ycrt_param_hdr_2x;
        ptDftParam->ptCa            = &ca_param_hdr_2x;
        ptDftParam->ptAinr          = &ainr_param_hdr_2x;
        ptDftParam->ptAwbDftParam   = &awb_param_hdr_2x;
        ptDftParam->ptAeDftParam    = &ae_param_hdr_2x;
        ptDftParam->ptAice          = &aice_param_hdr_2x;
        ptDftParam->pt3Dlut         = &isp_3dlut_param_hdr_2x;
        ptDftParam->ptDehaze        = &dehaze_param_hdr_2x;
        ptDftParam->ptScene         = &scene_param_hdr_2x;
        ptDftParam->ptLdc           = &ldc_param_hdr_2x;
        ptDftParam->ptDis           = &dis_param_hdr_2x;
        ptDftParam->ptMe            = &me_param_hdr_2x;
        break;

    case AX_SNS_HDR_3X_MODE:
        /* TODO: Users configure their own default parameters */
        break;

    case AX_SNS_HDR_4X_MODE:
        /* TODO: Users configure their own default parameters */
        break;
    default:
        SNS_ERR(" hdr mode %d error\n", nHdrmode);
        break;
    }


    return SNS_SUCCESS;
}

static AX_S32 imx678_get_isp_black_level(ISP_PIPE_ID nPipeId, AX_SNS_BLACK_LEVEL_T *ptBlackLevel)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_U32 BlackLevel = 0;

    SNS_CHECK_PTR_VALID(ptBlackLevel);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    /* black level of linear mode */
    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        if (AX_RT_RAW10 == sns_obj->sns_attr_param.eRawType){
            BlackLevel = 800;  /* 10bit 50d U10 -->  800 U8.6 */
        } else if (AX_RT_RAW12 == sns_obj->sns_attr_param.eRawType) {
            BlackLevel = 800;  /* 12bit 200d U12 -->  800 U8.6 */
        }
        ptBlackLevel->nBlackLevel[0] = BlackLevel;
        ptBlackLevel->nBlackLevel[1] = BlackLevel;
        ptBlackLevel->nBlackLevel[2] = BlackLevel;
        ptBlackLevel->nBlackLevel[3] = BlackLevel;
    } else {
        BlackLevel = 800;  /* 10bit 50d U10 -->  800 U8.6 */
        ptBlackLevel->nBlackLevel[0] = BlackLevel;
        ptBlackLevel->nBlackLevel[1] = BlackLevel;
        ptBlackLevel->nBlackLevel[2] = BlackLevel;
        ptBlackLevel->nBlackLevel[3] = BlackLevel;
    }

    return SNS_SUCCESS;
}

AX_SYS_API_PUBLIC AX_SENSOR_REGISTER_FUNC_T gSnsimx678Obj = {

    /* sensor ctrl */
    .pfn_sensor_chipid                  = imx678_get_chipid,
    .pfn_sensor_reset                   = imx678_reset,
    .pfn_sensor_init                    = imx678_init,
    .pfn_sensor_exit                    = imx678_exit,
    .pfn_sensor_streaming_ctrl          = imx678_sensor_streaming_ctrl,
    .pfn_sensor_testpattern             = imx678_testpattern_ctrl,

    .pfn_sensor_set_mode                = imx678_sensor_set_mode,
    .pfn_sensor_get_mode                = imx678_sensor_get_mode,

    .pfn_sensor_set_fps                 = imx678_set_fps,
    .pfn_sensor_get_fps                 = imx678_get_fps,

    /* communication : register read/write */
    .pfn_sensor_set_bus_info            = imx678_set_bus_info,
    .pfn_sensor_write_register          = imx678_write_register,
    .pfn_sensor_read_register           = imx678_read_register,

    /* default param */
    .pfn_sensor_get_default_params      = imx678_get_isp_default_params,
    .pfn_sensor_get_black_level         = imx678_get_isp_black_level,

    /* ae ctrl */
    .pfn_sensor_get_hw_exposure_params      = imx678_get_hw_exposure_params,
    .pfn_sensor_get_gain_table              = imx678_get_sensor_gain_table,
    .pfn_sensor_set_again                   = imx678_set_again,
    .pfn_sensor_set_dgain                   = imx678_set_dgain,
    .pfn_sensor_hcglcg_ctrl                 = imx678_hcglcg_ctrl,

    .pfn_sensor_set_integration_time        = imx678_set_integration_time,
    .pfn_sensor_get_integration_time_range  = imx678_get_integration_time_range,
    .pfn_sensor_set_slow_fps                = imx678_set_slow_fps,
    .pfn_sensor_get_slow_shutter_param      = imx678_ae_get_sensor_slow_shutter_param,
    .pfn_sensor_get_sns_reg_info            = imx678_ae_get_sensor_reg_info,
    .pfn_sensor_set_wbgain                  = AX_NULL,
};
