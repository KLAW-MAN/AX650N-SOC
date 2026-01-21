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


#include "sc410gs_settings.h"
#include "sc410gs_reg.h"
#include "sc410gs_ae_ctrl.h"

/* default param */
#include "sc410gs_sdr.h"

#include "ax_module_version.h"

/****************************************************************************
 * golbal variables  and macro definition
 ****************************************************************************/

SNS_STATE_OBJ *g_szsc410gsCtx[AX_VIN_MAX_PIPE_NUM] = {NULL};

#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szsc410gsCtx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (g_szsc410gsCtx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (g_szsc410gsCtx[dev] = NULL)

const char axera_sns_sc410gs_version[] = AXERA_MODULE_VERSION;

extern AX_SNS_COMMBUS_T gsc410gsBusInfo[AX_VIN_MAX_PIPE_NUM];

/****************************************************************************
 * Internal function definition
 ****************************************************************************/
static AX_S32 sensor_ctx_init(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        sns_obj = (SNS_STATE_OBJ *)calloc(1, sizeof(SNS_STATE_OBJ));
        if (AX_NULL == sns_obj) {
            SNS_ERR("malloc g_szsc410gsCtx failed\n");
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
static AX_S32 sc410gs_get_chipid(ISP_PIPE_ID nPipeId, AX_S32 *pSnsId)
{
    AX_U8 sensor_id_h = 0;
    AX_U8 sensor_id_l = 0;
    AX_U16 sensor_id = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sensor_id_h = sc410gs_reg_read(nPipeId, 0x3107);
    sensor_id_l = sc410gs_reg_read(nPipeId, 0x3108);

    sensor_id = sensor_id_h << 8 | sensor_id_l;

    SNS_DBG("pipe:%d, sc410gs sensor_id: 0x%x\n", nPipeId, sensor_id);

    if (sensor_id != SC410GS_SENSOR_ID) {
        SNS_ERR("Failed to read sc410gs sensor_id 0x%x\n", sensor_id);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    *pSnsId = sensor_id;

    return SNS_SUCCESS;
}

static void sc410gs_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 nRet = 0;
    AX_S32 nImageMode = 0;
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
    if (gsc410gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        sc410gs_i2c_init(nPipeId);
    } else {
        sc410gs_spi_init(nPipeId);
    }

    sc410gs_get_chipid(nPipeId, &nSnsId);

    /* 3. config settings  */
    sc410gs_write_settings(nPipeId);

    /* 4. refresh ae param */
    sc410gs_cfg_aec_param(nPipeId);

    /* 5. refresh ae regs table */
    sc410gs_refresh_all_regs_from_tbl(nPipeId);

    sns_obj->bSyncInit = AX_FALSE;

    return;
}

static void sc410gs_exit(ISP_PIPE_ID nPipeId)
{
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    if (gsc410gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        sc410gs_i2c_exit(nPipeId);
    } else {
        sc410gs_spi_exit(nPipeId);
    }
    sensor_ctx_exit(nPipeId);

    return;
}

static AX_S32 sc410gs_streaming_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    if (1 == on) {
        result = sc410gs_write_register(nPipeId, 0x0100, 0x01); // stream on
        SNS_DBG("sensor stream on!\n");
    } else {
        result = sc410gs_write_register(nPipeId, 0x0100, 0x00); // stream off
        SNS_DBG("sensor stream off!\n");
    }

    if (0 != result) {
        return -1;
    }

    return SNS_SUCCESS;
}

static AX_S32 sc410gs_set_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *sns_mode)
{
    AX_S32 nRet = 0;
    AX_F32 fSettingFps = 50;
    AX_S32 nSettingIndex = 0;
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

    if (sns_mode->nWidth == 1744                    &&
        sns_mode->nHeight == 2336                   &&
        sns_mode->eRawType == AX_RT_RAW10           &&
        sns_mode->eSnsMode == AX_SNS_LINEAR_MODE    &&
        sns_mode->eMasterSlaveSel == AX_SNS_SLAVE)
    {
        nSettingIndex = e_SC410GS_mipi_4line_1744x2336_linear_10bit_50fps_704Mbps_trigger;
        fSettingFps = 50;

    } else {
        SNS_ERR("it's not supported. pipe:%d, mode:%d_%d_%d_%d_%f\n",
                nPipeId, sns_mode->eSnsMode, sns_mode->nWidth, sns_mode->nHeight,
                sns_mode->eRawType, sns_mode->fFrameRate);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }


    /* optional, Not Recommended. if nSettingIndex > 0 will take effect */
    if (sns_mode->nSettingIndex > 0) {
        nSettingIndex = sns_mode->nSettingIndex;
    }

    SNS_INFO("pipe:%d, nSettingIndex:%d, mode:%d_%d_%d_%d_%f\n",
            nPipeId, nSettingIndex, sns_mode->eSnsMode, sns_mode->nWidth,
            sns_mode->nHeight, sns_mode->eRawType, fSettingFps);

    if(AXSNS_CAMPARE_FLOAT(sns_mode->fFrameRate, fSettingFps)){
        SNS_WRN("it's not recommended, pipe:%d, userFps:%f > fSettingFps:%f\n",
            nPipeId, sns_mode->fFrameRate, fSettingFps);
    }

    sns_obj->eImgMode = nSettingIndex;
    sns_obj->sns_mode_obj.eHDRMode = sns_mode->eSnsMode;
    sns_obj->sns_mode_obj.nWidth = sns_mode->nWidth;
    sns_obj->sns_mode_obj.nHeight = sns_mode->nHeight;
    sns_obj->sns_mode_obj.fFrameRate = fSettingFps;
    sns_obj->sns_mode_obj.eMasterSlaveSel = sns_mode->eMasterSlaveSel;
    memcpy(&sns_obj->sns_attr_param, sns_mode, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

static AX_S32 sc410gs_get_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *pSnsMode)
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

static AX_S32 sc410gs_testpattern_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SNS_DBG("test pattern enable: %d\n", on);
    if (1 == on) {
        /* enable test-pattern */
        sc410gs_write_register(nPipeId, 0x4501, 0xcc);
        sc410gs_write_register(nPipeId, 0x3902, 0x85);
    } else {
        /* disable test-pattern */
        sc410gs_write_register(nPipeId, 0x4501, 0xc4);
        sc410gs_write_register(nPipeId, 0x3902, 0xc5);
    }

    return SNS_SUCCESS;
}

static AX_S32 sc410gs_mirror_flip(ISP_PIPE_ID nPipeId, AX_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
    switch (eSnsMirrorFlip) {
    default:
    case AX_SNS_MF_NORMAL:
        sc410gs_write_register(nPipeId, 0x3221, 0x00);
        break;

    case AX_SNS_MF_MIRROR:
        sc410gs_write_register(nPipeId, 0x3221, 0x06);    //bit[2:1]
        break;

    case AX_SNS_MF_FLIP:
        sc410gs_write_register(nPipeId, 0x3221, 0x60);    //bit[7:5]
        break;

    case AX_SNS_MF_MIRROR_FLIP:
        sc410gs_write_register(nPipeId, 0x3221, 0x66);
        break;
    }
    return SNS_SUCCESS;
}

/****************************************************************************
 * get module default parameters function
 ****************************************************************************/
static AX_S32 sc410gs_get_default_params(ISP_PIPE_ID nPipeId, AX_SENSOR_DEFAULT_PARAM_T *ptDftParam)
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

    SNS_DBG("pipe:%d, current hdr mode %d \n", nPipeId, nHdrmode);

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

static AX_S32 sc410gs_get_black_level(ISP_PIPE_ID nPipeId, AX_SNS_BLACK_LEVEL_T *ptBlackLevel)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(ptBlackLevel);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    /* {0x3907，0x3908}=0x211=529(U12)=2116(U8.6) */
    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptBlackLevel->nBlackLevel[0] = 2116;
        ptBlackLevel->nBlackLevel[1] = 2116;
        ptBlackLevel->nBlackLevel[2] = 2116;
        ptBlackLevel->nBlackLevel[3] = 2116;
    } else {
    /**/
    }

    return SNS_SUCCESS;
}


AX_SYS_API_PUBLIC AX_SENSOR_REGISTER_FUNC_T gSnssc410gsObj = {

    /* sensor ctrl */
    .pfn_sensor_chipid                      = sc410gs_get_chipid,
    .pfn_sensor_init                        = sc410gs_init,
    .pfn_sensor_exit                        = sc410gs_exit,
    .pfn_sensor_reset                       = sc410gs_reset,
    .pfn_sensor_streaming_ctrl              = sc410gs_streaming_ctrl,
    .pfn_sensor_testpattern                 = sc410gs_testpattern_ctrl,
    .pfn_sensor_mirror_flip                 = sc410gs_mirror_flip,

    .pfn_sensor_set_mode                    = sc410gs_set_mode,
    .pfn_sensor_get_mode                    = sc410gs_get_mode,

    .pfn_sensor_set_fps                     = sc410gs_set_fps,

    /* communication : register read/write */
    .pfn_sensor_set_bus_info                = sc410gs_set_bus_info,
    .pfn_sensor_write_register              = sc410gs_write_register,
    .pfn_sensor_read_register               = sc410gs_read_register,

    /* default param */
    .pfn_sensor_get_default_params          = sc410gs_get_default_params,
    .pfn_sensor_get_black_level             = sc410gs_get_black_level,

    /* ae ctrl */
    .pfn_sensor_get_hw_exposure_params      = sc410gs_get_hw_exposure_params,
    .pfn_sensor_get_gain_table              = sc410gs_get_gain_table,
    .pfn_sensor_set_again                   = sc410gs_set_again,
    .pfn_sensor_set_dgain                   = sc410gs_set_dgain,
    .pfn_sensor_hcglcg_ctrl                 = AX_NULL,

    .pfn_sensor_set_integration_time        = sc410gs_set_integration_time,
    .pfn_sensor_get_integration_time_range  = sc410gs_get_integration_time_range,
    // .pfn_sensor_set_slow_fps                = sc410gs_set_fps,
    // .pfn_sensor_get_slow_shutter_param      = sc410gs_get_slow_shutter_param,
    .pfn_sensor_get_sns_reg_info            = sc410gs_ae_get_reg_info,
    .pfn_sensor_set_wbgain                  = AX_NULL,
};
