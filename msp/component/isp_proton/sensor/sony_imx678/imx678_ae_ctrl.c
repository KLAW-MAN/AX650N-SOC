#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "ax_base_type.h"
#include "ax_isp_common.h"

#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"

#include "imx678_reg.h"
#include "imx678_ae_ctrl.h"

typedef struct SENSOR_IMX678_S {
    AX_U32 hmax;
    AX_U32 vmax;
    AX_F32 line_period;
    AX_F32 exp_ratio;
    AX_U32 rhs1_max;
} SNSIMX678_OBJ;

typedef struct _IMX678_GAIN_TABLE_T_ {
    float gain;
    AX_U8 again_in;
    AX_U8 again_de;
    AX_U8 dgain_in;
    AX_U8 dgain_de;
    AX_U8 dgain_de2;
} IMX678_GAIN_TABLE_T;

extern SNS_STATE_OBJ *g_szImx678Ctx[AX_VIN_MAX_PIPE_NUM];
#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szImx678Ctx[dev])

SNSIMX678_OBJ sns_imx678params[AX_VIN_MAX_PIPE_NUM];
AX_F32 nimx678AgainTable[SENSOR_MAX_GAIN_STEP];
AX_F32 nimx678DgainTable[SENSOR_MAX_GAIN_STEP];

static AX_F32 gFpsGear[] = {1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 10.00,
                            11.00, 12.00, 13.00, 14.00, 15.00, 16.00, 17.00, 18.00, 19.00, 20.00,
                            21.00, 22.00, 23.00, 24.00, 25.00, 26.00, 27.00, 28.00, 29.00, 30.00,
                            31.00, 32.00, 33.00, 34.00, 35.00, 36.00, 37.00, 38.00, 39.00, 40.00,
                            41.00, 42.00, 43.00, 44.00, 45.00, 46.00, 47.00, 48.00, 49.00, 50.00,
                            51.00, 52.00, 53.00, 54.00, 55.00, 56.00, 57.00, 58.00, 59.00, 60.00
                           };

static AX_SNS_DRV_DELAY_TABLE_T gimx678AeRegsTable[] = {
    /* regs index */          /* regs addr */                /*regs value*/        /*Delay Frame Num*/
    {0,   IMX678_SHR0_L,             0,                  0},
    {1,   IMX678_SHR0_M,             0,                  0},
    {2,   IMX678_SHR0_H,             0,                  0},
    {3,   IMX678_SHR1_L,             0,                  0},
    {4,   IMX678_SHR1_M,             0,                  0},
    {5,   IMX678_SHR1_H,             0,                  0},
    {6,   IMX678_RHS1_L,             0,                  0},
    {7,   IMX678_RHS1_M,             0,                  0},
    {8,   IMX678_RHS1_H,             0,                  0},

    {9,   IMX678_GAIN0_L,            0,                  0},
    {10, IMX678_GAIN0_H,            0,                  0},
    {11, IMX678_GAIN1_L,            0,                  0},
    {12, IMX678_GAIN1_H,            0,                  0},

    {13, IMX678_FDG_SEL0,           0,                  0},
    {14, IMX678_FDG_SEL1,           0,                  0},

    {15, IMX678_VMAX_L,             0,                  0},
    {16, IMX678_VMAX_M,             0,                  0},
    {17, IMX678_VMAX_H,             0,                  0},
};

AX_S32 imx678_get_gain_table(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_TABLE_T *params)
{
    AX_U32 i = 0;
    AX_S32 nRet = 0;
    int d_max = 0;
    int d_min = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SNS_CHECK_PTR_VALID(params);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    params->nAgainTableSize = 0;
    params->nDgainTableSize = 0;

    // value <=> dB:  dB = 20* log10(value),    value = 10 ^ (dB/20)
    // dB <=> d:      d = dB * (10/3) ,         dB = d * (3/10)
    // value <=>d :   d = (200/3) log10(value), value = 10 ^(d * (3/200))
    d_max = round(log10(sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX]) * (float)200 / 3);
    d_min = round(log10(sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX]) * (float)200 / 3);
    for (int i = d_min; i < (d_max + 1); i++) {
        if((i>133 && i<=165 && i%3 != 0) || (i>165 && i%5 != 0)) {     //i=165 gain = 298.538208, i=133 gain=98.855309
            continue;
        }
        nimx678AgainTable[params->nAgainTableSize] = pow(10, i * (float)3 / 200);
        params->nAgainTableSize ++;
    }

    d_max = round(log10(sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX]) * (float)200 / 3);
    d_min = round(log10(sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX]) * (float)200 / 3);
    params->nDgainTableSize = d_max - d_min + 1;
    for (int i = d_min; i < (d_max + 1); i++) {
        nimx678DgainTable[i] = pow(10, i * (float)3 / 200);
    }

    params->pAgainTable = nimx678AgainTable;
    params->pDgainTable = nimx678DgainTable;

    return nRet;
}

AX_S32 imx678_get_sensor_gain_table(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_TABLE_T *params)
{
    AX_S32 nRet = 0;
    SNS_CHECK_PTR_VALID(params);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    nRet = imx678_get_gain_table(nPipeId, params);

    return nRet;
}

AX_U32 imx678_get_shr0(ISP_PIPE_ID nPipeId)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = imx678_reg_read(nPipeId, IMX678_SHR0_L);
    m = imx678_reg_read(nPipeId, IMX678_SHR0_M);
    h = imx678_reg_read(nPipeId, IMX678_SHR0_H);

    val = (AX_U32)(((h & 0xF) << 16) | (AX_U32)(m << 8) | (AX_U32)(l << 0));

    return val;
}
AX_S32 imx678_set_shr0(ISP_PIPE_ID nPipeId, AX_U32 shr0)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = shr0 & 0xFF;
    m = (shr0 & 0xFF00) >> 8;
    h = (shr0 & 0xF0000) >> 16;

    result |= imx678_sns_update_regs_table(nPipeId, IMX678_SHR0_L, l);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_SHR0_M, m);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_SHR0_H, h);

    return result;
}

AX_U32 imx678_get_shr1(ISP_PIPE_ID nPipeId)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = imx678_reg_read(nPipeId, IMX678_SHR1_L);
    m = imx678_reg_read(nPipeId, IMX678_SHR1_M);
    h = imx678_reg_read(nPipeId, IMX678_SHR1_H);

    val = (AX_U32)(((h & 0xF) << 16) | (AX_U32)(m << 8) | (AX_U32)(l << 0));

    return val;
}

AX_S32 imx678_set_shr1(ISP_PIPE_ID nPipeId, AX_U32 shr1)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = shr1 & 0xFF;
    m = (shr1 & 0xFF00) >> 8;
    h = (shr1 & 0xF0000) >> 16;

    result |= imx678_sns_update_regs_table(nPipeId, IMX678_SHR1_L, l);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_SHR1_M, m);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_SHR1_H, h);

    return result;
}

AX_U32 imx678_get_rhs1(ISP_PIPE_ID nPipeId)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = imx678_reg_read(nPipeId, IMX678_RHS1_L);
    m = imx678_reg_read(nPipeId, IMX678_RHS1_M);
    h = imx678_reg_read(nPipeId, IMX678_RHS1_H);

    val = (AX_U32)(((h & 0xF) << 16) | (AX_U32)(m << 8) | (AX_U32)(l << 0));

    return val;
}
AX_S32 imx678_set_rhs1(ISP_PIPE_ID nPipeId, AX_U32 rhs1)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = rhs1 & 0xFF;
    m = (rhs1 & 0xFF00) >> 8;
    h = (rhs1 & 0xF0000) >> 16;

    result |= imx678_sns_update_regs_table(nPipeId, IMX678_RHS1_L, l);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_RHS1_M, m);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_RHS1_H, h);

    //SNS_DBG("set rhs1: 0x%x [0x%x, 0x%x, 0x%x]\n", rhs1, h, m, l);

    return result;
}

static AX_F32 imx678_get_line_period(ISP_PIPE_ID nPipeId)
{
    AX_F32 line_period = 0;
    AX_U32 hmax = 0;

    hmax = sns_imx678params[nPipeId].hmax;
    line_period = hmax / IMX678_INCLK_74POINT25M;

    return line_period;
}

static AX_SNS_HCGLCG_MODE_E imx678_get_dcg_mode(ISP_PIPE_ID nPipeId)
{
    AX_S32 value = 0;
    AX_U8 data = 0;

    value = imx678_reg_read(nPipeId, IMX678_FDG_SEL0);
    if (value == -1) {
        SNS_ERR("IMX678_FDG_SEL0 reg value:0x%x\n", value);
        return AX_LCG_NOTSUPPORT_MODE;
    }

    data = value & 0xff;

    if ((data & 0x01) == 0) {
        return AX_LCG_MODE;
    } else {
        return AX_HCG_MODE;
    }

    return AX_LCG_NOTSUPPORT_MODE;
}

static AX_S32 imx678_set_exp_limit(ISP_PIPE_ID nPipeId, AX_F32 fExpRatio)
{
    AX_U32 fsc = 0;
    AX_F32 line_period = 0;
    AX_U32 line_period_fp = 0;

    AX_U32 shr0_min = 0, shr0_max = 0;
    AX_U32 shr1_min = 0, shr1_max = 0;
    AX_U32 rhs1_min = 0, rhs1_max = 0;

    AX_U32 lef_max_fp = 0;
    AX_U32 sef1_max_fp = 0;
    AX_U32 exp_max_line = 0;
    AX_U32 lef_max_line = 0;
    AX_U32 sef1_max_line = 0;

    AX_F32 ratio = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = sns_imx678params[nPipeId].vmax;
        shr0_min = 3;
        shr0_max = fsc - 1;
    } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = 2 * sns_imx678params[nPipeId].vmax;
        if(fsc % 4 != 0){
            SNS_ERR("hdr fsc:%d is invalid!\n", fsc);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
        }

        /* SHR1_MIN */
        shr1_min = 5;
        exp_max_line = fsc - shr1_min - 5;

        /* SHR0_MIN */
        /* 1)ratio=lef/sef  2)shr0 > rhs1+9     3)2n */
        lef_max_line = exp_max_line * (fExpRatio / (1 + fExpRatio));
        SNS_DBG("lef_max_line:%d\n", lef_max_line);

        shr0_min = fsc - lef_max_line;
        SNS_DBG("limit0 shr0_min:0x%x\n", shr0_min);

        shr0_min = AXSNS_ALIGN_UP(shr0_min, 2);
        SNS_DBG("limitn shr0_min:0x%x\n", shr0_min);

        /* SHR0_MAX */
        shr0_max = AXSNS_ALIGN_DOWN(fsc - 2, 2);
        SNS_DBG("limitn shr0_max:0x%x\n", shr0_max);

        /* RHS1_MAX */
        /* 1)ratio=lef/sef  2)RHS1 ≤ (SHR0-5)   3)RHS1 < (2*BRL)  4)Within FSC: RHS1 < FSC-2*BRL  5)2n+1 */
        sef1_max_line = exp_max_line - lef_max_line;
        SNS_DBG("sef1_max_line:%d\n", sef1_max_line);

        rhs1_max = sef1_max_line + shr1_min;
        SNS_DBG("limit0 rhs1_max:0x%x\n", rhs1_max);

        rhs1_max = AXSNS_MIN(rhs1_max, shr0_max - 5);
        SNS_DBG("limit1 rhs1_max:0x%x\n", rhs1_max);

        rhs1_max = AXSNS_MIN(rhs1_max, 2 * IMX678_ALLPIXEL_BRL - 1);   // less than use max-1
        SNS_DBG("limit2 rhs1_max:0x%x\n", rhs1_max);

        if(0 == rhs1_max % 2){
            rhs1_max -= 1;
        }
        SNS_DBG("limitn rhs1_max:0x%x\n", rhs1_max);
        sns_imx678params[nPipeId].rhs1_max = rhs1_max;

        /* SHR1_MAX */
        /* 1)2n+1   2)shr1 <= rhs1-2 */
        shr1_max = AXSNS_ALIGN_DOWN(rhs1_max - 2, 2) + 1;
        SNS_DBG("limitn shr1_max:0x%x\n", shr1_max);
    } else {
        SNS_ERR("not support hdr mode:%d\n", sns_obj->sns_mode_obj.eHDRMode);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX] = (fsc - shr0_max);
    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = (fsc - shr0_min);
    sns_obj->ae_ctrl_param.sns_ae_param.fIntegrationTimeIncrement[HDR_LONG_FRAME_IDX] = 1;

    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_MEDIUM_FRAME_IDX] = (rhs1_max - shr1_max);
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX] = (rhs1_max - shr1_min);
        sns_obj->ae_ctrl_param.sns_ae_param.fIntegrationTimeIncrement[HDR_MEDIUM_FRAME_IDX] = 1;

        ratio = (float)sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] /
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX];
    }

    SNS_DBG("LIMIT expline:%.2f-%.2f-%.2f-%.2f, ratio:%.2f, line_period:%f\n",
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX],
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX],
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX],
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_MEDIUM_FRAME_IDX],
        ratio, sns_obj->ae_ctrl_param.fTimePerLine);

    return 0;
}

static AX_S32 imx678_get_params_from_setting(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;
    AX_U32 hmax = 0;
    AX_U32 vmax = 0;

    ret = imx678_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    ret = imx678_get_hmax_from_setting(nPipeId, setting, reg_cnt, &hmax);
    if(ret) {
        return ret;
    }

    ret = imx678_get_vmax_from_setting(nPipeId, setting, reg_cnt, &vmax);
    if(ret) {
        return ret;
    }

    sns_imx678params[nPipeId].hmax = hmax;
    sns_imx678params[nPipeId].vmax = vmax;

    SNS_DBG("pipe:%d, get setting params vts:0x%x, hts:0x%x\n", nPipeId, vmax, hmax);

    return SNS_SUCCESS;
}

AX_S32 imx678_cfg_aec_param(ISP_PIPE_ID nPipeId)
{
    AX_U32 shr0, shr1, rhs1, shr2, rhs2, vmax, fsc = 0;
    AX_U32 shr0_max = 0, shr0_min = 0, shr1_min = 0, rhs1_min = 0;
    AX_F32 line_period = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    imx678_get_params_from_setting(nPipeId);
    sns_obj->sns_mode_obj.nVts = sns_imx678params[nPipeId].vmax;

    shr0 = imx678_get_shr0(nPipeId);
    shr1 = imx678_get_shr1(nPipeId);
    rhs1 = imx678_get_rhs1(nPipeId);
    vmax = sns_imx678params[nPipeId].vmax;

    SNS_DBG("get shr0:0x%x, shr1:0x%x, rhs1:0x%x, vmax:0x%x, hmax:0x%x\n", shr0, shr1, rhs1,
            vmax, sns_imx678params[nPipeId].hmax);

    /* IntegrationTime limit */
    vmax = AXSNS_ALIGN_DOWN(vmax, 2);
    if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = AXSNS_ALIGN_DOWN(vmax, 4);
    } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = AXSNS_ALIGN_DOWN(2 * vmax, 4);
    } else {
        SNS_ERR("not support hdr mode:%d \n", sns_obj->sns_mode_obj.eHDRMode);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    sns_obj->ae_ctrl_param.sns_ae_limit.fMinRatio = IMX678_MIN_RATIO;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxRatio = IMX678_MAX_RATIO;

    line_period = imx678_get_line_period(nPipeId);
    sns_obj->ae_ctrl_param.fTimePerLine = line_period;
    sns_imx678params[nPipeId].line_period = line_period;

    sns_imx678params[nPipeId].exp_ratio = sns_obj->ae_ctrl_param.sns_ae_limit.fMaxRatio;
    imx678_set_exp_limit(nPipeId, sns_imx678params[nPipeId].exp_ratio);

    sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] = (fsc - shr0);
    sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_MEDIUM_FRAME_IDX] = (rhs1 - shr1);


    /* Gain limit */
    sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX] = pow(10, 0);
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX] = pow(10, (float)72 / 20);
    sns_obj->ae_ctrl_param.sns_ae_param.fAGainIncrement[HDR_LONG_FRAME_IDX] = pow(10, (float)0.3 / 20);

    sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX] = pow(10, 0);
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX] = pow(10, 0);
    sns_obj->ae_ctrl_param.sns_ae_param.fAGainIncrement[HDR_LONG_FRAME_IDX] = pow(10, (float)0.3 / 20);

    sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX] = 1.0f;
    sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX] = 1.0f;

    if (sns_obj->sns_mode_obj.eHDRMode == AX_SNS_HDR_2X_MODE) {
        sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_MEDIUM_FRAME_IDX] = pow(10, 0);
        sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_MEDIUM_FRAME_IDX] = pow(10, (float)72 / 20);
        sns_obj->ae_ctrl_param.sns_ae_param.fAGainIncrement[HDR_MEDIUM_FRAME_IDX] = pow(10, (float)0.3 / 20);

        sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_MEDIUM_FRAME_IDX] = pow(10, 0);
        sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_MEDIUM_FRAME_IDX] = pow(10, 0);
        sns_obj->ae_ctrl_param.sns_ae_param.fAGainIncrement[HDR_MEDIUM_FRAME_IDX] = pow(10, (float)0.3 / 20);

        sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_MEDIUM_FRAME_IDX] = 1.0f;
        sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_MEDIUM_FRAME_IDX] = 1.0f;
    }

    sns_obj->ae_ctrl_param.sns_ae_param.fIspDGainIncrement[HDR_LONG_FRAME_IDX] = 1/256.0;  //U10.8

    /* LCG2HCG config */
    sns_obj->ae_ctrl_param.fSnsHcgLcgRatio = 2.7f;
    sns_obj->ae_ctrl_param.eSnsHcgLcgMode = imx678_get_dcg_mode(nPipeId);

    /* current fps */
    sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = sns_obj->sns_mode_obj.fFrameRate;

    return 0;
}


AX_S32 imx678_set_again(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptAnalogGainTbl)
{
    AX_U8 uGain_h;
    AX_U8 uGain_l;
    AX_S32 nRet = 0;
    AX_F32 fGainFromUser = 0;
    AX_F32 fLocalNewGain = 0;
    AX_U32 uLocalNewGain = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SNS_CHECK_PTR_VALID(ptAnalogGainTbl);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    SNS_DBG("LIMIT Again:%f-%f-%f-%f\n",
            sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_MEDIUM_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_MEDIUM_FRAME_IDX]);

    SNS_DBG("userAgain:%f-%f\n", ptAnalogGainTbl->fGain[0], ptAnalogGainTbl->fGain[1]);

    /* long gain seting */
    fGainFromUser = ptAnalogGainTbl->fGain[HDR_LONG_FRAME_IDX];
    fGainFromUser = AXSNS_CLIP3(fGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX],
                                sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX]);

    fLocalNewGain = 20 * log10(fGainFromUser);
    fLocalNewGain = fLocalNewGain * (float)10 / 3;

    uGain_l = ((AX_U8)round(fLocalNewGain) & 0xFF);
    uGain_h = ((AX_U8)round(fLocalNewGain) & 0x700) >> 8;

    nRet |= imx678_sns_update_regs_table(nPipeId, IMX678_GAIN0_L, uGain_l);
    nRet |= imx678_sns_update_regs_table(nPipeId, IMX678_GAIN0_H, uGain_h);
    if (nRet != 0) {
        SNS_ERR("LEF userAgain:%f update fail nRet:%d.\n", fGainFromUser, nRet);
        return nRet;
    }
    sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX] = fGainFromUser;

    /* medium gain  seting */
    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fGainFromUser = AXSNS_CLIP3(fGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_MEDIUM_FRAME_IDX],
                                    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_MEDIUM_FRAME_IDX]);

        fLocalNewGain = 20 * log10(fGainFromUser);
        fLocalNewGain = fLocalNewGain * (float)10 / 3;

        uGain_l = ((AX_U8)round(fLocalNewGain) & 0xFF);
        uGain_h = ((AX_U8)round(fLocalNewGain) & 0x700) >> 8;

        nRet |= imx678_sns_update_regs_table(nPipeId, IMX678_GAIN1_L, uGain_l);
        nRet |= imx678_sns_update_regs_table(nPipeId, IMX678_GAIN1_H, uGain_h);
        if (nRet != 0) {
            SNS_ERR("SEF1 userAgain:%f update fail nRet:%d.\n", fGainFromUser, nRet);
            return nRet;
        }
        sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_MEDIUM_FRAME_IDX] = fGainFromUser;
    }

    return SNS_SUCCESS;
}

AX_S32 imx678_set_dgain(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptDigitalGainTbl)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_F32 fGainFromUser = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SNS_CHECK_PTR_VALID(ptDigitalGainTbl);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    SNS_DBG("LIMIT Dgain:%f-%f-%f-%f\n",
            sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_MEDIUM_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_MEDIUM_FRAME_IDX]);

    SNS_DBG("userDgain:%f-%f\n", ptDigitalGainTbl->fGain[0], ptDigitalGainTbl->fGain[1]);

    fGainFromUser = ptDigitalGainTbl->fGain[HDR_LONG_FRAME_IDX];
    fGainFromUser = AXSNS_CLIP3(fGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX],
                                sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX]);

    sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX] = fGainFromUser;

    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fGainFromUser = ptDigitalGainTbl->fGain[HDR_MEDIUM_FRAME_IDX];
        fGainFromUser = AXSNS_CLIP3(fGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_MEDIUM_FRAME_IDX],
                                    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_MEDIUM_FRAME_IDX]);

        sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_MEDIUM_FRAME_IDX] = fGainFromUser;
    }

    return SNS_SUCCESS;

}

AX_S32 imx678_get_integration_time_range(ISP_PIPE_ID nPipeId, AX_F32 fHdrRatio,
        AX_SNS_AE_INT_TIME_RANGE_T *ptIntTimeRange)
{
    AX_F32 ratio = 0.0f;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptIntTimeRange);

    if (fabs(fHdrRatio) < EPS) {
        SNS_ERR("hdr ratio is error \n");
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    ratio = fHdrRatio;
    ratio = AXSNS_CLIP3(ratio, sns_obj->ae_ctrl_param.sns_ae_limit.fMinRatio, sns_obj->ae_ctrl_param.sns_ae_limit.fMaxRatio);
    if (fabs(ratio) <= EPS) {
        SNS_ERR("hdr ratio is error \n");
    }
    imx678_set_exp_limit(nPipeId, ratio);

    if (AX_SNS_HDR_2X_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptIntTimeRange->fMaxIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX];
        ptIntTimeRange->fMinIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX];
        ptIntTimeRange->fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX];
        ptIntTimeRange->fMinIntegrationTime[HDR_MEDIUM_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_MEDIUM_FRAME_IDX];

    } else if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {

        ptIntTimeRange->fMaxIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX];
        ptIntTimeRange->fMinIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX];

    } else {
        // do nothing
    }

    return SNS_SUCCESS;
}


AX_S32 imx678_set_integration_time(ISP_PIPE_ID nPipeId, AX_SNS_AE_SHUTTER_CFG_T *ptIntTime)
{
    AX_U32 vmax = 0;
    AX_U32 hmax = 0;
    AX_U32 fsc = 0;
    AX_U32 shr0 = 0;
    AX_U32 shr1 = 0;
    AX_U32 rhs1 = 0;
    AX_S32 ret = 0;
    AX_F32 ratio = 0.0f;
    AX_U32 fIntTimeFromUser = 0;
    AX_U32 sef1 = 0;
    AX_U32 lef_line = 0;
    AX_U32 sef1_line = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptIntTime);

    vmax = imx678_get_vmax(nPipeId);
    hmax = imx678_get_hmax(nPipeId);
    shr0 = imx678_get_shr0(nPipeId);
    shr1 = imx678_get_shr1(nPipeId);
    rhs1 = imx678_get_rhs1(nPipeId);

    SNS_DBG("get vmax:0x%x, hmax:0x%x, shr0:0x%x, shr1:0x%x, rhs1:0x%x\n", vmax, hmax, shr0, shr1, rhs1);

    if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = sns_imx678params[nPipeId].vmax;
    } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = 2 * sns_imx678params[nPipeId].vmax;
    } else {
        SNS_ERR("not support hdr mode:%d\n", sns_obj->sns_mode_obj.eHDRMode);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    ratio = ptIntTime->fHdrRatio[HDR_LONG_FRAME_IDX];
    ratio = AXSNS_CLIP3(ratio, sns_obj->ae_ctrl_param.sns_ae_limit.fMinRatio, sns_obj->ae_ctrl_param.sns_ae_limit.fMaxRatio);
    sns_imx678params[nPipeId].exp_ratio = ratio;

    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fIntTimeFromUser = ptIntTime->fIntTime[HDR_MEDIUM_FRAME_IDX];
        fIntTimeFromUser = AXSNS_CLIP3(fIntTimeFromUser,
                                       sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_MEDIUM_FRAME_IDX],
                                       sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX]);

        SNS_DBG("SEF1 fIntTimeFromUser:%d\n", fIntTimeFromUser);

        sef1_line = fIntTimeFromUser;

        rhs1 = sns_imx678params[nPipeId].rhs1_max;
        SNS_DBG("limit0 rhs1:0x%x\n", rhs1);

        shr1 = rhs1 - sef1_line;
        SNS_DBG("limit0 shr1:0x%x\n", shr1);

        shr1 = AXSNS_MAX(shr1, 5);
        SNS_DBG("limit1 shr1:0x%x\n", shr1);

        /* shr1 2n+1 */
        shr1 = AXSNS_ALIGN_DOWN(shr1, 2) + 1;
        SNS_DBG("limitn shr1:0x%x\n", shr1);

        ret |= imx678_set_shr1(nPipeId, shr1);
        ret |= imx678_set_rhs1(nPipeId, rhs1);
        if (ret != 0) {
            SNS_ERR("%s: write shr1 rhs1 failed %d \n", __func__, ret);
            return ret;
        }
    }

    fIntTimeFromUser = ptIntTime->fIntTime[HDR_LONG_FRAME_IDX];
    fIntTimeFromUser = AXSNS_CLIP3(fIntTimeFromUser,
                                    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX],
                                    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX]);

    lef_line = fIntTimeFromUser;
    SNS_DBG("lef_line:%d\n", lef_line);

    shr0 = fsc - lef_line;
    SNS_DBG("limit0 shr0:0x%x\n", shr0);

    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        shr0 = AXSNS_ALIGN_DOWN(shr0, 2);
        SNS_DBG("limitn shr0:0x%x\n", shr0);
    }

    ret = imx678_set_shr0(nPipeId, shr0);
    if (ret != 0) {
        SNS_ERR("%s: write shr0 failed %d \n", __func__, ret);
        return ret;
    }

    sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] = (fsc - shr0);

    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        sef1 = (rhs1 - shr1);
        sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_MEDIUM_FRAME_IDX] = sef1;

        ratio = sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] /
                sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_MEDIUM_FRAME_IDX];
    }

    SNS_DBG("set vmax:0x%x, hmax:0x%x, shr0:0x%x, shr1:0x%x, rhs1:0x%x\n",
            sns_imx678params[nPipeId].vmax, sns_imx678params[nPipeId].hmax, shr0, shr1, rhs1);

    SNS_INFO("userExpline:%f-%f, userRatio:%.2f, curExpline:%.2f-%.2f(%.2f-%.2f), curRatio:%.2f\n",
            ptIntTime->fIntTime[0], ptIntTime->fIntTime[1], ptIntTime->fHdrRatio[0],
            sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_MEDIUM_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] * sns_obj->ae_ctrl_param.fTimePerLine,
            sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_MEDIUM_FRAME_IDX] * sns_obj->ae_ctrl_param.fTimePerLine,
            ratio);

    return 0;
}



AX_S32 imx678_get_hw_exposure_params(ISP_PIPE_ID nPipeId, AX_SNS_EXP_CTRL_PARAM_T *ptAeCtrlParam)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(ptAeCtrlParam);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    memcpy(ptAeCtrlParam, &sns_obj->ae_ctrl_param, sizeof(AX_SNS_EXP_CTRL_PARAM_T));
    memcpy(&ptAeCtrlParam->sns_dev_attr, &sns_obj->sns_attr_param, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

AX_U32 imx678_sns_update_regs_table(ISP_PIPE_ID nPipeId, AX_U32 nRegsAddr, AX_U8 nRegsValue)
{
    AX_S32 i = 0;
    AX_U32 nNum = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);

    nNum = sizeof(gimx678AeRegsTable) / sizeof(gimx678AeRegsTable[0]);

    for (i = 0; i < nNum; i++) {
        if (nRegsAddr == sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr) {
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData = nRegsValue;
            break;
        }
    }

    if (nNum <= i) {
        SNS_ERR(" reg addr 0x%x not find.\n", nRegsAddr);
        return SNS_ERR_CODE_INVALID_ADDRESS;
    }

    return SNS_SUCCESS;
}


AX_S32 imx678_hcglcg_ctrl(ISP_PIPE_ID nPipeId, AX_U32 nSnsHcgLcg)
{
    AX_U8 hcglcg_value = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (AX_LCG_MODE == nSnsHcgLcg) {
        hcglcg_value = 0;

        imx678_sns_update_regs_table(nPipeId, IMX678_FDG_SEL0, hcglcg_value);
        if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            imx678_sns_update_regs_table(nPipeId, IMX678_FDG_SEL1, hcglcg_value);
        }
        sns_obj->ae_ctrl_param.eSnsHcgLcgMode = AX_LCG_MODE;
    } else if (AX_HCG_MODE == nSnsHcgLcg) {
        hcglcg_value = 1;

        imx678_sns_update_regs_table(nPipeId, IMX678_FDG_SEL0, hcglcg_value);
        if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            imx678_sns_update_regs_table(nPipeId, IMX678_FDG_SEL1, hcglcg_value);
        }
        sns_obj->ae_ctrl_param.eSnsHcgLcgMode = AX_HCG_MODE;
    }

    return SNS_SUCCESS;
}


AX_U32 imx678_sns_refresh_all_regs_from_tbl(ISP_PIPE_ID nPipeId)
{
    AX_S32 i = 0;
    AX_U32 nNum = 0;
    AX_U32  nRegValue;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);

    nNum = sizeof(gimx678AeRegsTable) / sizeof(gimx678AeRegsTable[0]);

    for (i = 0; i < nNum; i++) {
        nRegValue = imx678_reg_read(nPipeId, gimx678AeRegsTable[i].nRegAddr);
        sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr = gimx678AeRegsTable[i].nRegAddr;
        sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData = nRegValue;

        SNS_DBG(" nRegAddr 0x%x, nRegValue 0x%x\n", gimx678AeRegsTable[i].nRegAddr, gimx678AeRegsTable[i].nRegValue);
    }

    return SNS_SUCCESS;
}

AX_S32 imx678_sns_update_init_exposure_reg(ISP_PIPE_ID nPipeId)
{
    int i = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
        imx678_write_register(nPipeId, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData);
        SNS_DBG("Idx = %d, reg addr 0x%x, reg data 0x%x\n",
            i, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData);
    }

    return SNS_SUCCESS;
}

AX_S32 imx678_ae_get_sensor_reg_info(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
{
    AX_S32 i = 0;
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_BOOL bUpdateReg = AX_FALSE;

    SNS_CHECK_PTR_VALID(ptSnsRegsInfo);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);

    if ((AX_FALSE == sns_obj->bSyncInit) || (AX_FALSE == ptSnsRegsInfo->bConfig)) {
        /* sync config */
        SNS_DBG(" bSyncInit %d, bConfig %d\n", sns_obj->bSyncInit, ptSnsRegsInfo->bConfig);
        sns_obj->sztRegsInfo[0].eSnsType = ISP_SNS_CONNECT_I2C_TYPE;
        sns_obj->sztRegsInfo[0].tComBus.I2cDev = imx678_get_bus_num(nPipeId);
        sns_obj->sztRegsInfo[0].nRegNum = sizeof(gimx678AeRegsTable) / sizeof(gimx678AeRegsTable[0]);
        sns_obj->sztRegsInfo[0].nCfg2ValidDelayMax = 1;

        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_TRUE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDevAddr = IMX678_SLAVE_ADDR;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nAddrByteNum = IMX678_ADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDataByteNum = IMX678_DATA_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDelayFrmNum = gimx678AeRegsTable[i].nDelayFrmNum;
            if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nIntPos = AX_SNS_L_FSOF;
            } else {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nIntPos = AX_SNS_S_FSOF;
            }
            SNS_DBG("[%2d] nRegAddr 0x%x, nRegValue 0x%x\n", i,
                    gimx678AeRegsTable[i].nRegAddr, gimx678AeRegsTable[i].nRegValue);
        }

        bUpdateReg = AX_TRUE;
        sns_obj->bSyncInit = AX_TRUE;
        imx678_sns_update_init_exposure_reg(nPipeId);
    } else {
        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            if (sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData == sns_obj->sztRegsInfo[1].sztData.sztI2cData[i].nData) {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_FALSE;
            } else {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_TRUE;
                SNS_DBG("[%2d] nRegAddr 0x%x, nRegValue 0x%x\n", i,
                        sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData);
            }
        }
    }

    if (AX_TRUE == bUpdateReg) {
        sns_obj->sztRegsInfo[0].bConfig = AX_FALSE;
    } else {
        sns_obj->sztRegsInfo[0].bConfig = AX_TRUE;
    }
    memcpy(ptSnsRegsInfo, &sns_obj->sztRegsInfo[0], sizeof(AX_SNS_REGS_CFG_TABLE_T));
    /* Save the current register table */
    memcpy(&sns_obj->sztRegsInfo[1], &sns_obj->sztRegsInfo[0], sizeof(AX_SNS_REGS_CFG_TABLE_T));

    return nRet;
}

static AX_U32 imx678_set_vmax(ISP_PIPE_ID nPipeId, AX_U32 vmax)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = vmax & 0xFF;
    m = (vmax & 0xFF00) >> 8;
    h = (vmax & 0xF0000) >> 16;

    result |= imx678_sns_update_regs_table(nPipeId, IMX678_VMAX_L, l);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_VMAX_M, m);
    result |= imx678_sns_update_regs_table(nPipeId, IMX678_VMAX_H, h);

    //SNS_DBG("set vmax: 0x%x [0x%x, 0x%x, 0x%x]\n", vmax, h, m, l);

    return result;
}


AX_S32 imx678_set_slow_fps(ISP_PIPE_ID nPipeId, AX_F32 nFps)
{
    AX_U32 vmax = 0;
    AX_U32 fsc = 0;
    AX_S32 nRet = 0;
    AX_F32 line_period = 0;     /* unit : U22.10 */
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (nFps < EPS) {
        SNS_ERR("userFps:%f < %f is Invalid.\n", nFps, EPS);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    line_period = sns_imx678params[nPipeId].line_period;

    if ((AX_U32)nFps >= sns_obj->sns_mode_obj.fFrameRate) {
        SNS_WRN("userFps:%d > fFrameRate:%f\n",
                (AX_U32)nFps, sns_obj->sns_mode_obj.fFrameRate);
        vmax = sns_obj->sns_mode_obj.nVts;
    } else {
        vmax = 1 * SNS_1_SECOND_UNIT_US / (line_period * nFps);
        if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            vmax = vmax / 2;
        }
    }

    if (vmax > IMX678_MAX_VMAX) {
        SNS_WRN("userFps:%.2f, line_period:%.2f, vmax:0x%x > max_vmax:0x%x, use max_vmax.\n",
                nFps, line_period, vmax, IMX678_MAX_VMAX);
        vmax = IMX678_MAX_VMAX;
    }

    if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        vmax = AXSNS_ALIGN_DOWN(vmax, 2);
    }
    nRet = imx678_set_vmax(nPipeId, vmax);
    if (nRet != 0) {
        SNS_ERR("%s: write vmax failed %d \n", __func__, nRet);
        return nRet;
    }

    sns_imx678params[nPipeId].vmax = vmax;

    //imx678_set_exp_limit(nPipeId, gImx678Params[nPipeId].exp_ratio);

    if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = vmax;
    } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        fsc = 2 * vmax;
    } else {
        SNS_ERR("not support hdr mode:%d\n", sns_obj->sns_mode_obj.eHDRMode);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = fsc - 7;
    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX] = fsc - 7;
    sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = 1 * SNS_1_SECOND_UNIT_US / (line_period * fsc);

    SNS_DBG("userFps:%.2f, nCurFps:%.2f, line_period:%.2f, vmax:0x%x\n",
            nFps, sns_obj->ae_ctrl_param.sns_ae_param.fCurFps, line_period, vmax);

    return SNS_SUCCESS;
}


AX_S32 imx678_ae_get_sensor_slow_shutter_param(ISP_PIPE_ID nPipeId,
        AX_SNS_AE_SLOW_SHUTTER_PARAM_T *ptSlowShutterParam)
{
    AX_F32 framerate = 30;
    AX_U32 nFps = 0;
    AX_U32 nVts = 0;
    AX_U32 shr0_min = 0;
    AX_U32 shr1_min = 0;
    AX_U32 fsc = 0;
    AX_F32 fSensorMinFps = 0;
    AX_U32 lef_max_fp = 0;
    AX_F32 line_period = 0.0;   /* unit : us */
    AX_F32 exp_ratio = 0;
    AX_U32 exp_max_line = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    framerate = sns_obj->sns_mode_obj.fFrameRate;
    if (framerate > SNS_MAX_FRAME_RATE) {
        SNS_ERR("framerate:%f out of range:%d\n", framerate, SNS_MAX_FRAME_RATE);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    line_period = sns_imx678params[nPipeId].line_period;

    if (0 == line_period) {
        SNS_ERR("line_period is zero !\n");
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SNS_INFO("pipe:%d, line_period:%f, max_ratio:%.2f\n", nPipeId, line_period, IMX678_MAX_RATIO);

    ptSlowShutterParam->nGroupNum = AXSNS_MIN((sizeof(gFpsGear) / sizeof(AX_F32)), framerate);
    //ax_sns_quick_sort_float(gFpsGear, ptSlowShutterParam->nGroupNum);
    fSensorMinFps = 1 * SNS_1_SECOND_UNIT_US / (line_period * IMX678_MAX_VMAX);
    ptSlowShutterParam->fMinFps = AXSNS_MAX(gFpsGear[0], fSensorMinFps);

    for (nFps = 0; nFps < ptSlowShutterParam->nGroupNum; nFps++) {
        if(gFpsGear[nFps] >= framerate) {
            SNS_WRN("userFps:%f >= framerate:%f\n", gFpsGear[nFps], framerate);
            nVts = sns_obj->sns_mode_obj.nVts;
        } else {
            nVts = 1 * SNS_1_SECOND_UNIT_US / (line_period * gFpsGear[nFps]);
            if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
                nVts = nVts / 2;
            }
        }

        if (nVts > IMX678_MAX_VMAX){
            SNS_WRN("userFps:%.2f, line_period:%.2f, nVts:0x%x > max_vmax:0x%x, use max_vmax.\n",
                gFpsGear[nFps], line_period, nVts, IMX678_MAX_VMAX);
            nVts = IMX678_MAX_VMAX;
        }

        if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            fsc = nVts;
            shr0_min = 3;
        } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            nVts = AXSNS_ALIGN_DOWN(nVts, 2);
            fsc = 2 * nVts;
            shr1_min = 5;

            /* SHR0_MIN */
            /* 1)ratio=lef/sef  2)shr0 > rhs1+5     3)2n */
            exp_max_line = fsc - shr1_min - 5;
            lef_max_fp = exp_max_line * line_period * (IMX678_MAX_RATIO / (1 + IMX678_MAX_RATIO));
            shr0_min = fsc - round((float)lef_max_fp / line_period);
            shr0_min = AXSNS_ALIGN_UP(shr0_min, 2);
        } else {
            SNS_ERR("not support hdr mode:%d\n", sns_obj->sns_mode_obj.eHDRMode);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
        }

        ptSlowShutterParam->tSlowShutterTbl[nFps].fMaxIntTime = (fsc - shr0_min);
        ptSlowShutterParam->tSlowShutterTbl[nFps].fRealFps = 1 * SNS_1_SECOND_UNIT_US / (line_period * fsc);
        ptSlowShutterParam->fMaxFps = ptSlowShutterParam->tSlowShutterTbl[nFps].fRealFps;

        SNS_INFO("fps:%f, max_expline:%.2f(%.2f), vts:0x%x\n",
            ptSlowShutterParam->tSlowShutterTbl[nFps].fRealFps,
            ptSlowShutterParam->tSlowShutterTbl[nFps].fMaxIntTime,
            ptSlowShutterParam->tSlowShutterTbl[nFps].fMaxIntTime * line_period,
            nVts);
    }

    SNS_DBG("nGroupNum:%d, fMinFps:%.2f, fMaxFps:%.2f, fSensorMinFps:%.2f, fFrameRate:%f\n",
        ptSlowShutterParam->nGroupNum, ptSlowShutterParam->fMinFps, ptSlowShutterParam->fMaxFps,
        fSensorMinFps, sns_obj->sns_mode_obj.fFrameRate);

    return SNS_SUCCESS;
}

AX_S32 imx678_get_fps(ISP_PIPE_ID nPipeId, AX_F32 *pFps)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(pFps);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    *pFps = sns_obj->ae_ctrl_param.sns_ae_param.fCurFps;

    return SNS_SUCCESS;
}

AX_S32 imx678_set_fps(ISP_PIPE_ID nPipeId, AX_F32 fFps)
{
    AX_U32 vmax = 0;
    AX_U32 fsc = 0;
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if(IS_SNS_FPS_EQUAL(fFps, 25.0) || IS_SNS_FPS_EQUAL(fFps, 50.0)) {  /* PAL */
        if(AX_SNS_LINEAR_MODE == sns_obj->sns_attr_param.eSnsMode) {
            if(AX_RT_RAW10 == sns_obj->sns_attr_param.eRawType) {
                vmax = IMX678_VMAX_10BIT_8M25_SDR;  /* SDR 10BIT 4LINE */
            }
            if(AX_RT_RAW12 == sns_obj->sns_attr_param.eRawType) {
                vmax = IMX678_VMAX_12BIT_8M25_SDR;  /* SDR 12BIT 4LINE */
            }
        }
        if(AX_SNS_HDR_2X_MODE == sns_obj->sns_attr_param.eSnsMode) {
            vmax = IMX678_VMAX_10BIT_8M25_HDR_2X;  /* HDR 10BIT 4LINE */
        }
    }

    if(IS_SNS_FPS_EQUAL(fFps, 30.0)) {  /* NTSC */
        if(AX_SNS_LINEAR_MODE == sns_obj->sns_attr_param.eSnsMode) {
            if(AX_RT_RAW10 == sns_obj->sns_attr_param.eRawType) {
                vmax = IMX678_VMAX_10BIT_8M30_SDR;  /* SDR 10BIT 4LINE */
            }
            if(AX_RT_RAW12 == sns_obj->sns_attr_param.eRawType) {
                vmax = IMX678_VMAX_12BIT_8M30_SDR;  /* SDR 12BIT 4LINE */
            }
        }
        if(AX_SNS_HDR_2X_MODE == sns_obj->sns_attr_param.eSnsMode) {
            vmax = IMX678_VMAX_10BIT_8M30_HDR_2X;  /* HDR 10BIT 4LINE */
        }
    }

    if(IS_SNS_FPS_EQUAL(fFps, 60.0)) {
        if(AX_SNS_LINEAR_MODE == sns_obj->sns_attr_param.eSnsMode) {
            if(AX_RT_RAW10 == sns_obj->sns_attr_param.eRawType) {
                vmax = IMX678_VMAX_10BIT_8M60_SDR;
            }
        }
    }

        nRet = imx678_set_vmax(nPipeId, vmax);
        if (nRet != 0) {
            SNS_ERR("%s: write vmax failed %d \n", __func__, nRet);
            return nRet;
        }

        sns_imx678params[nPipeId].vmax = vmax;

        if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            fsc = vmax;
        } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            fsc = 2 * vmax;
        } else {
            SNS_ERR("not support hdr mode:%d\n", sns_obj->sns_mode_obj.eHDRMode);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
        }

        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = fsc - 7;
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_MEDIUM_FRAME_IDX] = fsc - 7;
        sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = 1 * SNS_1_SECOND_UNIT_US / (sns_imx678params[nPipeId].line_period * fsc);

    SNS_DBG("userFps:%.2f, nCurFps:%.2f, line_period:%.2f, vmax:0x%x\n",
            fFps, sns_obj->ae_ctrl_param.sns_ae_param.fCurFps, sns_imx678params[nPipeId].line_period, vmax);

    return SNS_SUCCESS;
}
