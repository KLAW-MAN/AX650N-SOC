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
#include "isp_sensor_types.h"
#include "i2c.h"

#include "sc850sl.h"
#include "sc850sl_settings.h"
#include "sc850sl_reg.h"
#include "sc850sl_ae_ctrl.h"

/* default param */
#include "sc850sl_sdr.h"
#include "sc850sl_hdr_2x.h"

#include "ax_module_version.h"

SNS_STATE_OBJ *gSc850slCtx[AX_VIN_MAX_PIPE_NUM] = {NULL};
AX_U8 gSc850slSlaveAddr[AX_VIN_MAX_PIPE_NUM] = {0x30, 0x30};

static AX_SNS_COMMBUS_T gSc850slBusInfo[AX_VIN_MAX_PIPE_NUM] = { {{0}}, {{2}},};

const char axera_sns_sc850sl_version[] = AXERA_MODULE_VERSION;

/****************************************************************************
 * Internal function definition
 ****************************************************************************/
AX_S32 sc850sl_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo)
{
    gSc850slBusInfo[nPipeId].I2cDev = tSnsBusInfo.I2cDev;

    return SNS_SUCCESS;
}

AX_S32 sc850sl_get_bus_num(ISP_PIPE_ID nPipeId)
{
    return gSc850slBusInfo[nPipeId].I2cDev;
}

AX_S32 sc850sl_i2c_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 snsId = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sns_obj->sns_i2c_obj.sns_i2c_fd = -1;
    sns_obj->sns_i2c_obj.slave_addr = gSc850slSlaveAddr[nPipeId];
    sns_obj->sns_i2c_obj.address_byte = SC850SL_ADDR_BYTE;
    sns_obj->sns_i2c_obj.data_byte = SC850SL_DATA_BYTE;
    sns_obj->sns_i2c_obj.swap_byte = SC850SL_SWAP_BYTES;

    sns_obj->sns_i2c_obj.sns_i2c_bnum = sc850sl_get_bus_num(nPipeId);

    sns_obj->sns_i2c_obj.sns_i2c_fd = i2c_init(sns_obj->sns_i2c_obj.sns_i2c_bnum,
                                      sns_obj->sns_i2c_obj.slave_addr);

    SNS_INFO("sc850sl i2c init finish, i2c bus %d \n", sns_obj->sns_i2c_obj.sns_i2c_bnum);

    return SNS_SUCCESS;
}

AX_S32 sc850sl_i2c_exit(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    ret = i2c_exit(sns_obj->sns_i2c_obj.sns_i2c_fd);

    return ret;
}


static AX_S32 sensor_ctx_init(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 ret = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    if (AX_NULL == sns_obj) {
        sns_obj = (SNS_STATE_OBJ *)calloc(1, sizeof(SNS_STATE_OBJ));
        if (AX_NULL == sns_obj) {
            SNS_ERR("malloc gSc850slCtx failed\n");
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

#if 0
/****************************************************************************
 * sensor control function
 ****************************************************************************/
static AX_S32 sc850sl_get_version(ISP_PIPE_ID nPipeId, AX_CHAR * ptSnsVerInfo)
{
    SNS_CHECK_PTR_VALID(ptSnsVerInfo);

    memcpy(ptSnsVerInfo, axera_module_version, sizeof(axera_module_version));

    return 0;
}
#endif

AX_S32 sc850sl_hw_reset(unsigned int gpio_num, unsigned int gpio_out_val)
{
    FILE *fp = AX_NULL;
    char file_name[50];
    char buf[10];

    sprintf(file_name, "/sys/class/gpio/gpio%d", gpio_num);
    if (0 != access(file_name, F_OK)) {
        sprintf(file_name, "/sys/class/gpio/export");
        fp = fopen(file_name, "w");
        if (fp == AX_NULL) {
            SNS_ERR("Cannot open %s.\n", file_name);
            return -1;
        }
        fprintf(fp, "%d", gpio_num);
        fclose(fp);

        sprintf(file_name, "/sys/class/gpio/gpio%d/direction", gpio_num);
        fp = fopen(file_name, "w");
        if (fp == AX_NULL) {
            SNS_ERR("Cannot open %s.\n", file_name);
            return -1;
        }
        fprintf(fp, "out");
        fclose(fp);
    }

    sprintf(file_name, "/sys/class/gpio/gpio%d/value", gpio_num);
    fp = fopen(file_name, "w");
    if (fp == AX_NULL) {
        SNS_ERR("Cannot open %s.\n", file_name);
        return -1;
    }
    if (gpio_out_val) {
        strcpy(buf, "1");
    } else {
        strcpy(buf, "0");
    }
    fprintf(fp, "%s", buf);
    fclose(fp);

    return 0;
}

AX_S32 sc850sl_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio)
{
    AX_S32 result = 0;
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    result |= sc850sl_hw_reset(nResetGpio, 0);
    usleep(5 * 1000);
    result |= sc850sl_hw_reset(nResetGpio, 1);
    usleep(10 * 1000);

    return result;
}

static AX_S32 sc850sl_get_chipid(ISP_PIPE_ID nPipeId, AX_S32 *pSnsId)
{
    AX_U32 sensor_id = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    sensor_id |= sc850sl_reg_read(nPipeId, 0x3107) << 8;
    sensor_id |= sc850sl_reg_read(nPipeId, 0x3108);

    SNS_INFO("%s: sensor sc850sl id: 0x%x\n", __func__, sensor_id);

    if (sensor_id != SC850SL_SENSOR_ID) {
        SNS_ERR("%s: Failed to read sensor sc850sl id 0x%x\n", __func__, sensor_id);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    *pSnsId = sensor_id;

    return SNS_SUCCESS;
}

static void sc850sl_init(ISP_PIPE_ID nPipeId)
{
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    /* 1. i2c init */
    sc850sl_i2c_init(nPipeId);

    /* 2. config settings  */
    sc850sl_write_settings(nPipeId);

    /* 3. refresh ae param */
    sc850sl_cfg_aec_param(nPipeId);

    /* 4. refresh ae regs table */
    sc850sl_refresh_all_regs_from_tbl(nPipeId);

    return;
}

static void sc850sl_exit(ISP_PIPE_ID nPipeId)
{
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM)) {
        return;
    }

    sc850sl_i2c_exit(nPipeId);
    sensor_ctx_exit(nPipeId);

    return;
}

static AX_S32 sc850sl_streaming_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    if (1 == on) {
        result = sc850sl_write_register(nPipeId, 0x0100, 0x01); // stream on
        SNS_DBG("sensor stream on!\n");
    } else {
        result = sc850sl_write_register(nPipeId, 0x0100, 0x00); // stream off
        SNS_DBG("sensor stream off!\n");
    }
    if (0 != result) {
        return -1;
    }

    return SNS_SUCCESS;
}

static AX_S32 sc850sl_set_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *sns_mode)
{
    AX_S32 nRet = 0;
    AX_S32 setting_index = 0;
    AX_F32 setting_fps = 30.0;
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

    if (sns_mode->nWidth == 3840            &&
        sns_mode->nHeight == 2160           &&
        sns_mode->eRawType == AX_RT_RAW10   &&
        sns_mode->eSnsMode == AX_SNS_LINEAR_MODE)
    {
        setting_index = E_SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_SDR_10BIT_60FPS_1404MBPS;
        setting_fps = 60.0;

    } else if (sns_mode->nWidth == 3840     &&
        sns_mode->nHeight == 2160           &&
        sns_mode->eRawType == AX_RT_RAW10   &&
        sns_mode->eSnsMode == AX_SNS_HDR_2X_MODE)
    {
        setting_index = E_SC850SL_MIPI_4LANE_24M_RGGB_3840X2160_HDR_10BIT_30FPS_1458MBPS;
        setting_fps = 30.0;

    } else {
        SNS_ERR("it's not supported. pipe=%d, mode=%d_%d_%d_%d_%.2f\n",
                nPipeId, sns_mode->eSnsMode, sns_mode->nWidth, sns_mode->nHeight,
                sns_mode->eRawType, setting_fps);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    if (sns_mode->nSettingIndex > 0) {
        setting_index = sns_mode->nSettingIndex;
        setting_fps = sns_mode->fFrameRate;
    }

    SNS_INFO("pipe=%d, setting_index:%d, mode=%d_%d_%d_%d_%.2f\n",
            nPipeId, setting_index, sns_mode->eSnsMode, sns_mode->nWidth,
            sns_mode->nHeight, sns_mode->eRawType, setting_fps);

    sns_obj->eImgMode = setting_index;
    sns_obj->sns_mode_obj.eHDRMode = sns_mode->eSnsMode;
    sns_obj->sns_mode_obj.nWidth = sns_mode->nWidth;
    sns_obj->sns_mode_obj.nHeight = sns_mode->nHeight;
    sns_obj->sns_mode_obj.fFrameRate = setting_fps;
    memcpy(&sns_obj->sns_attr_param, sns_mode, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

static AX_S32 sc850sl_get_mode(ISP_PIPE_ID nPipeId, AX_SNS_ATTR_T *pSnsMode)
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

static AX_S32 sc850sl_testpattern_ctrl(ISP_PIPE_ID nPipeId, AX_U32 on)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SNS_DBG("test pattern enable: %d\n", on);
    if (1 == on) {
        /* enable test-pattern */
        sc850sl_write_register(nPipeId, 0x4501, 0xb5);
    } else {
        /* disable test-pattern */
        sc850sl_write_register(nPipeId, 0x4501, 0xb4);
    }

    return SNS_SUCCESS;
}

static AX_S32 sc850sl_mirror_flip(ISP_PIPE_ID nPipeId, AX_SNS_MIRRORFLIP_TYPE_E eSnsMirrorFlip)
{
    AX_S32 value = 0;

    value = sc850sl_reg_read(nPipeId, 0x3221);

    value &= 0x99;

    switch (eSnsMirrorFlip) {
    default:
    case AX_SNS_MF_NORMAL:
        sc850sl_write_register(nPipeId, 0x3221, 0x00 | value);
        break;

    case AX_SNS_MF_MIRROR:
        sc850sl_write_register(nPipeId, 0x3221, 0x06 | value);    //bit[2:1]
        break;

    case AX_SNS_MF_FLIP:
        sc850sl_write_register(nPipeId, 0x3221, 0x60 | value);    //bit[6:5]
        break;

    case AX_SNS_MF_MIRROR_FLIP:
        sc850sl_write_register(nPipeId, 0x3221, 0x66 | value);
        break;
    }
    return SNS_SUCCESS;
}


/****************************************************************************
 * get module default parameters function
 ****************************************************************************/
static AX_S32 sc850sl_get_isp_default_params(ISP_PIPE_ID nPipeId, AX_SENSOR_DEFAULT_PARAM_T *ptDftParam)
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

static AX_S32 sc850sl_get_black_level(ISP_PIPE_ID nPipeId, AX_SNS_BLACK_LEVEL_T *ptBlackLevel)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(ptBlackLevel);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    //{0x3907/0x3908}=0x111=273(U12.0)=1092(U8.6) for sdr/hdr 10bit/12bit
    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptBlackLevel->nBlackLevel[0] = 1092;
        ptBlackLevel->nBlackLevel[1] = 1092;
        ptBlackLevel->nBlackLevel[2] = 1092;
        ptBlackLevel->nBlackLevel[3] = 1092;
    } else {
        ptBlackLevel->nBlackLevel[0] = 1092;
        ptBlackLevel->nBlackLevel[1] = 1092;
        ptBlackLevel->nBlackLevel[2] = 1092;
        ptBlackLevel->nBlackLevel[3] = 1092;
    }

    return SNS_SUCCESS;
}


AX_SYS_API_PUBLIC AX_SENSOR_REGISTER_FUNC_T gSnssc850slObj = {

    /* sensor ctrl */
    .pfn_sensor_chipid                      = sc850sl_get_chipid,
    .pfn_sensor_init                        = sc850sl_init,
    .pfn_sensor_exit                        = sc850sl_exit,
    .pfn_sensor_reset                       = sc850sl_reset,
    .pfn_sensor_streaming_ctrl              = sc850sl_streaming_ctrl,
    .pfn_sensor_testpattern                 = sc850sl_testpattern_ctrl,
    .pfn_sensor_mirror_flip                 = sc850sl_mirror_flip,

    .pfn_sensor_set_mode                    = sc850sl_set_mode,
    .pfn_sensor_get_mode                    = sc850sl_get_mode,

    .pfn_sensor_set_fps                     = sc850sl_set_fps,
    .pfn_sensor_get_fps                     = sc850sl_get_fps,

    /* communication : register read/write */
    .pfn_sensor_set_bus_info                = sc850sl_set_bus_info,
    .pfn_sensor_write_register              = sc850sl_write_register,
    .pfn_sensor_read_register               = sc850sl_read_register,

    /* default param */
    .pfn_sensor_get_default_params          = sc850sl_get_isp_default_params,
    .pfn_sensor_get_black_level             = sc850sl_get_black_level,

    /* ae ctrl */
    .pfn_sensor_get_hw_exposure_params      = sc850sl_get_hw_exposure_params,
    .pfn_sensor_get_gain_table              = sc850sl_get_gain_table,
    .pfn_sensor_set_again                   = sc850sl_set_again,
    .pfn_sensor_set_dgain                   = sc850sl_set_dgain,
    .pfn_sensor_hcglcg_ctrl                 = AX_NULL,

    .pfn_sensor_set_integration_time        = sc850sl_set_integration_time,
    .pfn_sensor_get_integration_time_range  = sc850sl_get_integration_time_range,
    .pfn_sensor_set_slow_fps                = sc850sl_set_slow_fps,
    .pfn_sensor_get_slow_shutter_param      = sc850sl_get_slow_shutter_param,
    .pfn_sensor_get_sns_reg_info            = sc850sl_ae_get_sensor_reg_info,
};
