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

#include "os04a10_settings.h"
#include "os04a10_reg.h"
#include "os04a10_ae_ctrl.h"
#include "os04a10_ae_params.h"

/* default param */
#include "os04a10_sdr.h"
#include "os04a10_hdr_2x.h"

#include "ax_module_version.h"
/****************************************************************************
 * golbal variables  and macro definition                                   *
 ****************************************************************************/

SNS_STATE_OBJ *g_szOs04a10Ctx[AX_VIN_MAX_PIPE_NUM] = {AX_NULL};
SNSOS04A10_OBJ_T sns_os04a10params[AX_VIN_MAX_PIPE_NUM];
extern AX_U8 gOs04a10SlaveMode[AX_VIN_MAX_PIPE_NUM];


#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szOs04a10Ctx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (g_szOs04a10Ctx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (g_szOs04a10Ctx[dev] = AX_NULL)

const char axera_sns_os04a10_version[] = AXERA_MODULE_VERSION;

/****************************************************************************
 * Internal function definition
 ****************************************************************************/
static AX_S32 sensor_ctx_init(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 ret = 0;

    SNS_DBG("os04a10 sensor_ctx_init. ret = %d\n", ret);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);

    if (AX_NULL == sns_obj) {
        sns_obj = (SNS_STATE_OBJ *)calloc(1, sizeof(SNS_STATE_OBJ));
        if (AX_NULL == sns_obj) {
            SNS_ERR("malloc g_szOs04a10Ctx failed\n");
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
static AX_S32 os04a10_get_chipid(ISP_PIPE_ID nPipeId, AX_S32 *pSnsId)
{
    AX_U32 sensor_id = 0;
    AX_U16 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sensor_id |= os04a10_reg_read(nPipeId, 0x300A) << 16;
    sensor_id |= os04a10_reg_read(nPipeId, 0x300B) << 8;
    sensor_id |= os04a10_reg_read(nPipeId, 0x300C);

    SNS_DBG("%s: sensor os04a10 id: 0x%x\n", __func__, sensor_id);

    if (sensor_id != OS04A10_SENSOR_CHIP_ID) {
        SNS_ERR("%s: Failed to read sensor os04a10 id\n", __func__);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    *pSnsId = sensor_id;

    return SNS_SUCCESS;
}

static void os04a10_init(ISP_PIPE_ID nPipeId)
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

    /* 2. i2c init */
    os04a10_sensor_i2c_init(nPipeId);

    nRet = os04a10_get_chipid(nPipeId, &nSnsId);
    if (nRet != SNS_SUCCESS) {
        SNS_ERR("can't find os04a10 sensor id.\n");
    } else {
        SNS_DBG("os04a10 check chip id success.\n");
    }

    /* 3. config settings  */
    os04a10_write_settings(nPipeId);

    /* 4. refresh ae param */
    os04a10_cfg_aec_param(nPipeId);

    /* 5. refresh ae regs table */
    os04a10_sns_refresh_all_regs_from_tbl(nPipeId);

    sns_obj->bSyncInit = AX_FALSE;

    return;
}

static void os04a10_exit(ISP_PIPE_ID nPipeId)
{
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    os04a10_sensor_i2c_exit(nPipeId);
    sensor_ctx_exit(nPipeId);

    return;
}


static AX_S32 os04a10_sensor_streaming_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    AX_S32 result = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (1 == on) {
        result = os04a10_write_register(nPipeId, 0x0100, 0x01); // stream on
        SNS_DBG("sensor stream on!\n");
    } else {
        result = os04a10_write_register(nPipeId, 0x0100, 0x00); // stream off
        SNS_DBG("sensor stream off!\n");
    }
    if (0 != result) {
        return -1;
    }

    return SNS_SUCCESS;
}

static AX_S32 os04a10_sensor_set_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *sns_mode)
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

    if (width == 2688 && height == 1520 && hdrmode == AX_SNS_LINEAR_MODE && MasterSlaveSel == AX_SNS_MASTER && (framerate > 30)) {
        sns_setting_index = e_OS04A10_4lane_2688x1520_10bit_Linear_60fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_LINEAR_MODE && nRawType == 10 && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_OS04A10_4lane_2688x1520_10bit_Linear_30fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_LINEAR_MODE && nRawType == 12 && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_OS04A10_4lane_2688x1520_12bit_Linear_30fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_LINEAR_MODE && nRawType == 10 && MasterSlaveSel == AX_SNS_SYNC_MASTER) {
        sns_setting_index = e_OS04A10_4lane_SyncMaster_2688x1520_10bit_Linear_30fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_LINEAR_MODE && nRawType == 10 && MasterSlaveSel == AX_SNS_SYNC_SLAVE) {
        sns_setting_index = e_OS04A10_4lane_SyncSlave_2688x1520_10bit_Linear_30fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_HDR_2X_MODE && nRawType == 10 && MasterSlaveSel == AX_SNS_MASTER) {
        sns_setting_index = e_OS04A10_4lane_2688x1520_10bit_2Stagger_HDR_30fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_HDR_2X_MODE && MasterSlaveSel == AX_SNS_SYNC_MASTER) {
        sns_setting_index = e_OS04A10_4lane_SyncMaster_2688x1520_10bit_2Stagger_HDR_30fps;
    } else if (width == 2688 && height == 1520 && hdrmode == AX_SNS_HDR_2X_MODE && MasterSlaveSel == AX_SNS_SYNC_SLAVE) {
        sns_setting_index = e_OS04A10_4lane_SyncSlave_2688x1520_10bit_2Stagger_HDR_30fps;
    } else {
        SNS_ERR("it's not supported. [%dx%d mode=%d fps=%d raw_type=%d] \n",
                width, height, hdrmode, framerate, nRawType);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    /* optional, Not Recommended. if nSettingIndex > 0 will take effect */
    if (sns_mode->nSettingIndex > 0) {
        sns_setting_index = sns_mode->nSettingIndex;
    }

    SNS_DBG("nPipeId=%d, sns_setting_index %d!\n", nPipeId, sns_setting_index);
    sns_obj->eImgMode = sns_setting_index;
    sns_obj->sns_mode_obj.eHDRMode = hdrmode;
    sns_obj->sns_mode_obj.nWidth = width;
    sns_obj->sns_mode_obj.nHeight = height;
    sns_obj->sns_mode_obj.fFrameRate = framerate;
    sns_obj->sns_mode_obj.eMasterSlaveSel = MasterSlaveSel;

    if(sns_obj->sns_mode_obj.eMasterSlaveSel == AX_SNS_SYNC_SLAVE) {
        gOs04a10SlaveMode[nPipeId] = AX_TRUE;
    }
    memcpy(&sns_obj->sns_attr_param, sns_mode, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

static AX_S32 os04a10_sensor_get_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *pSnsMode)
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


static AX_S32 os04a10_testpattern_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SNS_DBG("test pattern enable: %d\n", on);
    if (1 == on) {
        /* enable test-pattern */
        os04a10_write_register(nPipeId, 0x5080, 0x80); /* long  frame*/
        os04a10_write_register(nPipeId, 0x50c0, 0x80); /* short frame*/
    } else {
        /* disable test-pattern */
        os04a10_write_register(nPipeId, 0x5080, 0x00); /* long  frame*/
        os04a10_write_register(nPipeId, 0x50c0, 0x00); /* short frame*/
    }

    return SNS_SUCCESS;
}

static AX_S32 os04a10_mirror_flip(ISP_PIPE_ID nPipeId, AX_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
    AX_S32 result = -1;
    AX_SNS_ATTR_T snsModeAttr = {0};

    result = os04a10_sensor_get_mode(nPipeId, &snsModeAttr);
    if (0 != result) {
        SNS_ERR("os04a10_sensor_get_mode failed!\n");
        return -1;
    }

    switch (eSnsMirrorFlip)
    {
        default:
        case AX_SNS_MF_NORMAL:
            if (snsModeAttr.eSnsMode == AX_SNS_LINEAR_MODE || snsModeAttr.eSnsOutputMode == AX_SNS_DCG_HDR) {
                /* sdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x0);
            } else {
                /* hdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x1);
            }
            break;

        case AX_SNS_MF_MIRROR:
            if (snsModeAttr.eSnsMode == AX_SNS_LINEAR_MODE || snsModeAttr.eSnsOutputMode == AX_SNS_DCG_HDR) {
                /* sdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x02);
            } else {
                /* hdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x03);
            }
            break;

        case AX_SNS_MF_FLIP:
            if (snsModeAttr.eSnsMode == AX_SNS_LINEAR_MODE || snsModeAttr.eSnsOutputMode == AX_SNS_DCG_HDR) {
                /* sdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x04);
            } else {
                /* hdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x05);
            }
            break;

        case AX_SNS_MF_MIRROR_FLIP:
            if (snsModeAttr.eSnsMode == AX_SNS_LINEAR_MODE || snsModeAttr.eSnsOutputMode == AX_SNS_DCG_HDR) {
                /* sdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x06);
            } else {
                /* hdr */
                result = os04a10_write_register(nPipeId, OS04A10_MF_V, 0x07);
            }
            break;
    }

    if (result != 0) {
        SNS_ERR("write mirror_flip hw failed %d \n", result);
        return result;
    }

    return SNS_SUCCESS;
}

/****************************************************************************
 * get module default parameters function
 ****************************************************************************/
static AX_S32 os04a10_get_isp_default_params(ISP_PIPE_ID nPipeId, AX_SENSOR_DEFAULT_PARAM_T *ptDftParam)
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

static AX_S32 os04a10_get_black_level(ISP_PIPE_ID nPipeId, AX_SNS_BLACK_LEVEL_T *ptBlackLevel)
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


AX_SYS_API_PUBLIC AX_SENSOR_REGISTER_FUNC_T gSnsos04a10Obj = {

    /* sensor ctrl */
    .pfn_sensor_reset                       = os04a10_reset,
    .pfn_sensor_chipid                      = os04a10_get_chipid,
    .pfn_sensor_init                        = os04a10_init,
    .pfn_sensor_exit                        = os04a10_exit,
    .pfn_sensor_streaming_ctrl              = os04a10_sensor_streaming_ctrl,
    .pfn_sensor_testpattern                 = os04a10_testpattern_ctrl,
    .pfn_sensor_mirror_flip                 = os04a10_mirror_flip,

    .pfn_sensor_set_mode                    = os04a10_sensor_set_mode,
    .pfn_sensor_get_mode                    = os04a10_sensor_get_mode,

    .pfn_sensor_set_fps                     = os04a10_set_fps,
    .pfn_sensor_get_fps                     = os04a10_get_fps,
    .pfn_sensor_set_slaveaddr               = os04a10_set_slaveaddr,

    /* communication : register read/write */
    .pfn_sensor_set_bus_info                = os04a10_set_bus_info,
    .pfn_sensor_write_register              = os04a10_write_register,
    .pfn_sensor_read_register               = os04a10_read_register,

    /* default param */
    .pfn_sensor_get_default_params          = os04a10_get_isp_default_params,
    .pfn_sensor_get_black_level             = os04a10_get_black_level,

    /* ae ctrl */
    .pfn_sensor_get_hw_exposure_params      = os04a10_get_hw_exposure_params,
    .pfn_sensor_get_gain_table              = os04a10_get_sensor_gain_table,
    .pfn_sensor_set_again                   = os04a10_set_again,
    .pfn_sensor_set_dgain                   = os04a10_set_dgain,
    .pfn_sensor_hcglcg_ctrl                 = os04a10_hcglcg_ctrl,

    .pfn_sensor_set_integration_time        = os04a10_set_integration_time,
    .pfn_sensor_get_integration_time_range  = os04a10_get_integration_time_range,
    .pfn_sensor_set_slow_fps                = os04a10_set_slow_fps,
    .pfn_sensor_get_slow_shutter_param      = os04a10_get_slow_shutter_param,
    .pfn_sensor_get_sns_reg_info            = os04a10_ae_get_sensor_reg_info,
    .pfn_sensor_set_wbgain                  = AX_NULL,
};

