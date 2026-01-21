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
#include <math.h>
#include "i2c.h"
#include "ax_sensor_struct.h"
#include "ax_base_type.h"

#include "ax_isp_common.h"
#include "isp_sensor_internal.h"
#include "isp_sensor_types.h"

#include "ov48c40_settings.h"
#include "ov48c40_reg.h"
#include "ov48c40_ae_ctrl.h"

/* default param */
#include "ov48c40_sdr.h"
#include "ov48c40_hdr_2x.h"

#include "ax_module_version.h"
/****************************************************************************
 * golbal variables  and macro definition                                   *
 ****************************************************************************/

SNS_STATE_OBJ *g_szOv48c40Ctx[AX_VIN_MAX_PIPE_NUM] = {AX_NULL};


#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szOv48c40Ctx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (g_szOv48c40Ctx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (g_szOv48c40Ctx[dev] = AX_NULL)

const char axera_sns_ov48c40_version[] = AXERA_MODULE_VERSION;

/****************************************************************************
 * Internal function definition
 ****************************************************************************/
static AX_S32 sensor_ctx_init(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 ret = 0;

    SNS_DBG("ov48c40 sensor_ctx_init. ret = %d\n", ret);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);

    if (AX_NULL == sns_obj) {
        sns_obj = (SNS_STATE_OBJ *)calloc(1, sizeof(SNS_STATE_OBJ));
        if (AX_NULL == sns_obj) {
            SNS_ERR("malloc g_szOv48c40Ctx failed\n");
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
static AX_S32 ov48c40_get_chipid(ISP_PIPE_ID nPipeId, AX_S32 *pSnsId)
{
    AX_U32 sensor_id = 0;
    AX_U16 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sensor_id |= ov48c40_reg_read(nPipeId, 0x300A) << 16;
    sensor_id |= ov48c40_reg_read(nPipeId, 0x300B) << 8;
    sensor_id |= ov48c40_reg_read(nPipeId, 0x300C);

    SNS_DBG("%s: sensor ov48c40 id: 0x%x\n", __func__, sensor_id);

    if (sensor_id != OV48C40_SENSOR_CHIP_ID) {
        SNS_ERR("%s: Failed to read sensor ov48c40 id\n", __func__);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    *pSnsId = sensor_id;

    return SNS_SUCCESS;
}

static void ov48c40_init(ISP_PIPE_ID nPipeId)
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
    ov48c40_sensor_i2c_init(nPipeId);

    nRet = ov48c40_get_chipid(nPipeId, &nSnsId);
    if (nRet != SNS_SUCCESS) {
        SNS_ERR("can't find ov48c40 sensor id.\n");
    } else {
        SNS_DBG("ov48c40 check chip id success.\n");
    }

    /* 3. config settings  */
    nImagemode = sns_obj->eImgMode;
    ov48c40_write_settings(nPipeId, nImagemode);

    /* 4. refresh ae param */
    ov48c40_cfg_aec_param(nPipeId);

    /* 5. refresh ae regs table */
    ov48c40_sns_refresh_all_regs_from_tbl(nPipeId);
    sns_obj->bSyncInit = AX_FALSE;

    sns_obj->sns_mode_obj.nVts = ov48c40_get_vts(nPipeId);

    return;
}

static void ov48c40_exit(ISP_PIPE_ID nPipeId)
{
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    ov48c40_sensor_i2c_exit(nPipeId);
    sensor_ctx_exit(nPipeId);

    return;
}


static AX_S32 ov48c40_sensor_streaming_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    AX_S32 result = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (1 == on) {
        result = ov48c40_write_register(nPipeId, OV48C40_STREAM_CTRL_REG, 0x01); // stream on
        SNS_DBG("sensor stream on!\n");
    } else {
        result = ov48c40_write_register(nPipeId, OV48C40_STREAM_CTRL_REG, 0x00); // stream off
        SNS_DBG("sensor stream off!\n");
    }
    if (0 != result) {
        return -1;
    }

    return SNS_SUCCESS;
}

static AX_S32 ov48c40_sensor_set_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *sns_mode)
{
    AX_S32 width = 0;
    AX_S32 height = 0;
    AX_S32 hdrmode = 0;
    AX_S32 nRawType = 0;
    AX_S32 framerate = 25; // init value to 30fps, void null fps gives.
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

    sns_obj->bSyncInit = AX_FALSE;
    width = sns_mode->nWidth;
    height = sns_mode->nHeight;
    framerate = sns_mode->fFrameRate;
    nRawType = sns_mode->eRawType;
    MasterSlaveSel = sns_mode->eMasterSlaveSel;

    if ((AX_SNS_LINEAR_MODE != sns_mode->eSnsMode) &&
        (AX_SNS_HDR_2X_MODE != sns_mode->eSnsMode) &&
        (AX_SNS_HDR_3X_MODE != sns_mode->eSnsMode)) {
        hdrmode = AX_SNS_LINEAR_MODE;
    } else {
        hdrmode = sns_mode->eSnsMode;
    }

    memcpy(&sns_obj->sns_attr_param, sns_mode, sizeof(AX_SNS_ATTR_T));

    /* optional, Not Recommended. if nSettingIndex > 0 will take effect */
    if (sns_mode->nSettingIndex > 0) {
        sns_setting_index = sns_mode->nSettingIndex;
    } else {
        if (width == 8000 && height == 6000 && hdrmode == AX_SNS_LINEAR_MODE && MasterSlaveSel == AX_SNS_MASTER && framerate == 15 && nRawType == 10) {
            sns_setting_index = e_OV48C_mclk24M_8000x6000_4lane_2496Mbps_Linear_Raw10_15fps;
        } else if (width == 8000 && height == 6000 && hdrmode == AX_SNS_LINEAR_MODE && MasterSlaveSel == AX_SNS_MASTER && framerate == 8 && nRawType == 10) {
            sns_setting_index = e_OV48C_mclk24M_8000x6000_4lane_1459Mbps_Linear_Raw10_8fps;
        } else if (width == 4000 && height == 3000 && hdrmode == AX_SNS_LINEAR_MODE && MasterSlaveSel == AX_SNS_MASTER && (framerate == 30 || framerate == 25) && nRawType == 10) {
            sns_setting_index = e_OV48C_mclk24M_4000x3000_4lane_2496Mbps_Linear_Raw10_30fps;
            framerate = 30;
            sns_obj->sns_attr_param.fFrameRate = 30;
        } else if (width == 4000 && height == 3000 && hdrmode == AX_SNS_LINEAR_MODE && MasterSlaveSel == AX_SNS_MASTER && framerate == 12 && nRawType == 10) {
            sns_setting_index = e_OV48C_mclk24M_4000x3000_4lane_1459Mbps_Linear_Raw10_12fps;
        } else if (width == 3840 && height == 2160 && hdrmode == AX_SNS_LINEAR_MODE && MasterSlaveSel == AX_SNS_MASTER && (framerate == 30 || framerate == 25) && nRawType == 10) {
            sns_setting_index = e_OV48C_mclk24M_3840x2160_4lane_2496Mbps_Linear_Raw10_30fps;
            framerate = 30;
            sns_obj->sns_attr_param.fFrameRate = 30;
        } else {
            SNS_ERR("it's not supported. [%dx%d mode=%d fps=%d raw_type=%d] \n",
                    width, height, hdrmode, framerate, nRawType);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
        }
    }

    SNS_DBG("nPipeId=%d, sns_setting_index %d!\n", nPipeId, sns_setting_index);
    sns_obj->eImgMode = sns_setting_index;
    sns_obj->sns_mode_obj.eHDRMode = hdrmode;
    sns_obj->sns_mode_obj.nWidth = width;
    sns_obj->sns_mode_obj.nHeight = height;
    sns_obj->sns_mode_obj.fFrameRate = framerate;
    sns_obj->sns_mode_obj.eMasterSlaveSel = MasterSlaveSel;

    return SNS_SUCCESS;
}

static AX_S32 ov48c40_sensor_get_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *pSnsMode)
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


static AX_S32 ov48c40_testpattern_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SNS_DBG("test pattern enable: %d\n", on);
    if (1 == on) {
        /* enable test-pattern */

    } else {
        /* disable test-pattern */

    }

    return SNS_SUCCESS;
}

/****************************************************************************
 * get module default parameters function
 ****************************************************************************/
static AX_S32 ov48c40_get_isp_default_params(ISP_PIPE_ID nPipeId, AX_SENSOR_DEFAULT_PARAM_T *ptDftParam)
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
        ptDftParam->ptAice          = &aice_param_sdr;
        ptDftParam->ptAwbDftParam   = &awb_param_sdr;
        ptDftParam->ptAeDftParam    = &ae_param_sdr;
        ptDftParam->pt3Dlut         = &isp_3dlut_param_sdr;
        ptDftParam->ptDehaze        = &dehaze_param_sdr;
        ptDftParam->ptScene         = &scene_param_sdr;
        ptDftParam->ptLdc           = &ldc_param_sdr;
        ptDftParam->ptDis           = &dis_param_sdr;
        ptDftParam->ptMe            = &me_param_sdr;
        break;

    case AX_SNS_HDR_2X_MODE:
        /* TODO: Users configure their own default parameters */
        ptDftParam->ptHdr           = &hdr_param_hdr_2x;
        ptDftParam->ptDpc           = &dpc_param_hdr_2x;
        ptDftParam->ptBlc           = &blc_param_hdr_2x;
        ptDftParam->ptFpn           = &fpn_param_sdr;
        ptDftParam->ptDarkshading   = &darkshading_param_hdr_2x;
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
        ptDftParam->ptAice          = &aice_param_hdr_2x;
        ptDftParam->ptAwbDftParam   = &awb_param_hdr_2x;
        ptDftParam->ptAeDftParam    = &ae_param_hdr_2x;
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

static AX_S32 ov48c40_get_black_level(ISP_PIPE_ID nPipeId, AX_SNS_BLACK_LEVEL_T *ptBlackLevel)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(ptBlackLevel);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    /* black level of linear mode */
    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptBlackLevel->nBlackLevel[0] = 1024;    /* linear mode 10bit sensor default blc 64(U10.0) --> 1024(U8.6) */
        ptBlackLevel->nBlackLevel[1] = 1024;
        ptBlackLevel->nBlackLevel[2] = 1024;
        ptBlackLevel->nBlackLevel[3] = 1024;
    } else {
        ptBlackLevel->nBlackLevel[0] = 1024;
        ptBlackLevel->nBlackLevel[1] = 1024;
        ptBlackLevel->nBlackLevel[2] = 1024;
        ptBlackLevel->nBlackLevel[3] = 1024;
    }

    return SNS_SUCCESS;
}


AX_SYS_API_PUBLIC AX_SENSOR_REGISTER_FUNC_T gSnsov48c40Obj = {

    /* sensor ctrl */
    .pfn_sensor_reset                       = ov48c40_reset,
    .pfn_sensor_chipid                      = ov48c40_get_chipid,
    .pfn_sensor_init                        = ov48c40_init,
    .pfn_sensor_exit                        = ov48c40_exit,
    .pfn_sensor_streaming_ctrl              = ov48c40_sensor_streaming_ctrl,
    .pfn_sensor_testpattern                 = ov48c40_testpattern_ctrl,

    .pfn_sensor_set_mode                    = ov48c40_sensor_set_mode,
    .pfn_sensor_get_mode                    = ov48c40_sensor_get_mode,

    .pfn_sensor_set_fps                     = ov48c40_set_fps,
    .pfn_sensor_get_fps                     = ov48c40_get_fps,
    //.pfn_sensor_set_slaveaddr               = ov48c40_set_slaveaddr,

    /* communication : register read/write */
    .pfn_sensor_set_bus_info                = ov48c40_set_bus_info,
    .pfn_sensor_write_register              = ov48c40_write_register,
    .pfn_sensor_read_register               = ov48c40_read_register,

    /* default param */
    .pfn_sensor_get_default_params          = ov48c40_get_isp_default_params,
    .pfn_sensor_get_black_level             = ov48c40_get_black_level,

    /* ae ctrl */
    .pfn_sensor_get_hw_exposure_params      = ov48c40_get_hw_exposure_params,
    .pfn_sensor_get_gain_table              = ov48c40_get_sensor_gain_table,
    .pfn_sensor_set_again                   = ov48c40_set_again,
    .pfn_sensor_set_dgain                   = ov48c40_set_dgain,
    .pfn_sensor_hcglcg_ctrl                 = ov48c40_hcglcg_ctrl,

    .pfn_sensor_set_integration_time        = ov48c40_set_integration_time,
    .pfn_sensor_get_integration_time_range  = ov48c40_get_integration_time_range,
    .pfn_sensor_set_slow_fps                = ov48c40_set_slow_fps,
    .pfn_sensor_get_slow_shutter_param      = ov48c40_get_slow_shutter_param,
    .pfn_sensor_get_sns_reg_info            = ov48c40_ae_get_sensor_reg_info,
    .pfn_sensor_set_wbgain                  = AX_NULL,
};

