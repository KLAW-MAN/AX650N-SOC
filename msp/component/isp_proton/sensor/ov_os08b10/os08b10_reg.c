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
#include "ax_base_type.h"
#include "ax_isp_common.h"

#include "i2c.h"
#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"

#include "os08b10_reg.h"
#include "os08b10_settings.h"

extern SNS_STATE_OBJ *g_szOs08b10Ctx[AX_VIN_MAX_PIPE_NUM];
#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szOs08b10Ctx[dev])

static AX_SNS_COMMBUS_T gOs08b10BusInfo[AX_VIN_MAX_PIPE_NUM] = { {{1}}, {{2}},};
static AX_U32 SENSOR_RSTN[AX_VIN_MAX_PIPE_NUM] = {64, 65, 22, 135};


AX_S32 os08b10_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo)
{
    gOs08b10BusInfo[nPipeId].I2cDev = tSnsBusInfo.I2cDev;

    return SNS_SUCCESS;
}

AX_S32 os08b10_get_bus_num(ISP_PIPE_ID nPipeId)
{
    return gOs08b10BusInfo[nPipeId].I2cDev;
}


AX_S32 os08b10_sensor_i2c_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 snsId = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sns_obj->sns_i2c_obj.sns_i2c_fd = -1;
    sns_obj->sns_i2c_obj.slave_addr = OS08B10_SLAVE_ADDR;
    sns_obj->sns_i2c_obj.address_byte = OS08B10_ADDR_BYTE;
    sns_obj->sns_i2c_obj.data_byte = OS08B10_DATA_BYTE;
    sns_obj->sns_i2c_obj.swap_byte = OS08B10_SWAP_BYTES;

    sns_obj->sns_i2c_obj.sns_i2c_bnum = os08b10_get_bus_num(nPipeId);

    sns_obj->sns_i2c_obj.sns_i2c_fd = i2c_init(sns_obj->sns_i2c_obj.sns_i2c_bnum,
                                      sns_obj->sns_i2c_obj.slave_addr);

    SNS_DBG("os08b10 i2c init finish, i2c bus %d \n", sns_obj->sns_i2c_obj.sns_i2c_bnum);

    return SNS_SUCCESS;
}

AX_S32 os08b10_sensor_i2c_exit(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    ret = i2c_exit(sns_obj->sns_i2c_obj.sns_i2c_fd);

    return ret;
}

AX_S32 os08b10_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData)
{
    AX_U8 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd)
        return -1;

    nRet = i2c_read(sns_obj->sns_i2c_obj.sns_i2c_fd,
             sns_obj->sns_i2c_obj.slave_addr,
             nAddr,
             sns_obj->sns_i2c_obj.address_byte,
             (AX_U8 *)(pData),
             sns_obj->sns_i2c_obj.data_byte,
             sns_obj->sns_i2c_obj.swap_byte);

    return nRet;
}

AX_S32 os08b10_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr)
{
    AX_U8 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd)
        return -1;

    i2c_read(sns_obj->sns_i2c_obj.sns_i2c_fd,
             sns_obj->sns_i2c_obj.slave_addr,
             addr,
             sns_obj->sns_i2c_obj.address_byte,
             (AX_U8 *)(&data),
             sns_obj->sns_i2c_obj.data_byte,
             sns_obj->sns_i2c_obj.swap_byte);

    return data;
}

AX_S32 os08b10_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd) {
        return -1;
    }

    nRet = i2c_write(sns_obj->sns_i2c_obj.sns_i2c_fd, sns_obj->sns_i2c_obj.slave_addr, addr,
                     sns_obj->sns_i2c_obj.address_byte,
                     (AX_U8 *)(&data), sns_obj->sns_i2c_obj.data_byte, sns_obj->sns_i2c_obj.swap_byte);

    return nRet;
}


AX_S32 os08b10_hw_reset(unsigned int gpio_num, unsigned int gpio_out_val)
{
    FILE *fp = NULL;
    char file_name[50];
    char buf[10];

    sprintf(file_name, "/sys/class/gpio/gpio%d", gpio_num);
    if (0 != access(file_name, F_OK)) {
        sprintf(file_name, "/sys/class/gpio/export");
        fp = fopen(file_name, "w");
        if (fp == NULL) {
            SNS_ERR("Cannot open %s.\n", file_name);
            return -1;
        }
        fprintf(fp, "%d", gpio_num);
        fclose(fp);

        sprintf(file_name, "/sys/class/gpio/gpio%d/direction", gpio_num);
        fp = fopen(file_name, "w");
        if (fp == NULL) {
            SNS_ERR("Cannot open %s.\n", file_name);
            return -1;
        }
        fprintf(fp, "out");
        fclose(fp);
    }

    sprintf(file_name, "/sys/class/gpio/gpio%d/value", gpio_num);
    fp = fopen(file_name, "w");
    if (fp == NULL) {
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

AX_S32 os08b10_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio)
{
    AX_S32 result = 0;
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    result |= os08b10_hw_reset(nResetGpio, 0);
    usleep(5 * 1000);
    result |= os08b10_hw_reset(nResetGpio, 1);
    usleep(10 * 1000);

    return result;
}

AX_S32 os08b10_group_prepare(void)
{
    AX_S32 result = 0;

    result |= os08b10_write_register(0, 0x320D, 0x00);
    result |= os08b10_write_register(0, 0x3208, 0x00);
    result |= os08b10_write_register(0, 0x0808, 0x00);
    result |= os08b10_write_register(0, 0x3800, 0x11);
    result |= os08b10_write_register(0, 0x3911, 0x22);
    result |= os08b10_write_register(0, 0x3208, 0x10);

    return result;
}



AX_U32 os08b10_get_hts(ISP_PIPE_ID nPipeId)
{
    AX_U8 hts_h;
    AX_U8 hts_l;
    AX_U32 hts;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    hts_h = os08b10_reg_read(nPipeId, 0x380C);
    hts_l = os08b10_reg_read(nPipeId, 0x380D);

    hts = (AX_U32)(((hts_h & 0xF) << 8) | (AX_U32)(hts_l << 0));

    return hts;
}

AX_U32 os08b10_get_vs_hts(ISP_PIPE_ID nPipeId)
{
    AX_U8 hts_h;
    AX_U8 hts_l;
    AX_U32 hts;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    hts_h = os08b10_reg_read(nPipeId, 0x384C);
    hts_l = os08b10_reg_read(nPipeId, 0x384D);

    hts = (AX_U32)(((hts_h & 0xF) << 8) | (AX_U32)(hts_l << 0));

    return hts;
}

AX_U32 os08b10_set_hts(ISP_PIPE_ID nPipeId, AX_U32 hts)
{
    AX_U8 hts_h;
    AX_U8 hts_l;
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    hts_l = hts & 0xFF;
    hts_h = (hts & 0xFF00) >> 8;

    result |= os08b10_write_register(nPipeId, 0x380C, hts_h);
    result |= os08b10_write_register(nPipeId, 0x380D, hts_l);

    return result;
}

AX_F32 os08b10_get_vts(ISP_PIPE_ID nPipeId)
{
    AX_U8 vts_h;
    AX_U8 vts_l;
    AX_U32 vts;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    vts_h = os08b10_reg_read(nPipeId, 0x380E);
    vts_l = os08b10_reg_read(nPipeId, 0x380F);

    vts = (AX_U32)(((vts_h & 0xFF) << 8) | (AX_U32)(vts_l << 0));

    return vts;
}

AX_F32 os08b10_get_sclk(ISP_PIPE_ID nPipeId)
{
    AX_U8 pre_div0;
    AX_U8 pre_div;
    AX_U16 multiplier;
    AX_U8 post_div;
    //AX_U8 sram_div;
    AX_U8 st_div;
    AX_U8 t_div;
    float inck;
    float sclk;

    inck = OS08B10_INCK_24M;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    pre_div0 = (os08b10_reg_read(nPipeId, 0x0322) & 0x1) + 1;

    pre_div = os08b10_reg_read(nPipeId, 0x0323) & 0x7;
    if (pre_div == 0) {
        pre_div = 1;
    } else if (pre_div == 1) {
        pre_div = 1.5;
    } else if (pre_div == 2) {
        pre_div = 2;
    } else if (pre_div == 3) {
        pre_div = 2.5;
    } else if (pre_div == 4) {
        pre_div = 3;
    } else if (pre_div == 5) {
        pre_div = 4;
    } else if (pre_div == 6) {
        pre_div = 6;
    } else if (pre_div == 7) {
        pre_div = 8;
    } else {
    }

    multiplier = (os08b10_reg_read(nPipeId, 0x0324) & 0x3) << 8;
    multiplier = multiplier | ((os08b10_reg_read(nPipeId, 0x0325)) & 0xFF);

    post_div = (os08b10_reg_read(nPipeId, 0x032f) & 0xF) + 1;
    //sram_div = (os08b10_reg_read(nPipeId, 0x0327) & 0xF) + 1;
    st_div = (os08b10_reg_read(nPipeId, 0x0328) & 0xF) + 1;

    t_div = os08b10_reg_read(nPipeId, 0x032a) & 0x7;
    if (t_div == 0) {
        t_div = 1;
    } else if (t_div == 1) {
        t_div = 1.5;
    } else if (t_div == 2) {
        t_div = 2;
    } else if (t_div == 3) {
        t_div = 2.5;
    } else if (t_div == 4) {
        t_div = 3;
    } else if (t_div == 5) {
        t_div = 3.5;
    } else if (t_div == 6) {
        t_div = 4;
    } else if (t_div == 7) {
        t_div = 5;
    } else {
    }
    sclk = (((((((float)(inck * 1000 * 1000) / pre_div0) / pre_div) * multiplier) / post_div) / st_div) / t_div);
    /*SNS_DBG("%s pre_div0=0x%x, pre_div=0x%x, multiplier=0x%x, post_div=0x%x, sram_div=0x%x, st_div=0x%x, t_div=0x%x \n", \
          __func__, pre_div0, pre_div, multiplier, post_div, sram_div, st_div, t_div); */

    return sclk;
}

AX_S32 os08b10_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts)
{
    AX_U32 i = 0;
    AX_U8 high = 0;
    AX_U8 low = 0;
    AX_U8 mask = 0;

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == OS08B10_VTS_H) {
            high = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == OS08B10_VTS_L) {
            low = (setting + i)->value;
            mask |= 1 << 1;
        }

        if(mask == 0x3) break;
    }

    if(mask != 0x3) {
        SNS_ERR("get setting vts fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *vts = high << 8 | low;

    return SNS_SUCCESS;
}

AX_S32 os08b10_get_hts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *hts)
{
    AX_U32 i = 0;
    AX_U8 high = 0;
    AX_U8 low = 0;
    AX_U8 mask = 0;

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == 0x380C) {
            high = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == 0x380D) {
            low = (setting + i)->value;
            mask |= 1 << 1;
        }

        if(mask == 0x3) break;
    }

    if(mask != 0x3) {
        SNS_ERR("get setting hts fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *hts = high << 8 | low;

    return SNS_SUCCESS;
}

AX_S32 os08b10_get_sclk_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_F32 *sclk)
{
    AX_U8 pre_div0 = 0;
    AX_U8 pre_div = 0;
    AX_U16 multiplier = 0;
    AX_U8 multiplier_h = 0;
    AX_U8 multiplier_l = 0;
    AX_U8 post_div = 0;
    //AX_U8 sram_div;
    AX_U8 st_div = 0;
    AX_U8 t_div = 0;
    float inck = 1.0f;

    AX_U32 i = 0;
    AX_U8 mask = 0;

    inck = OS08B10_INCK_24M;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == 0x0322) {
            pre_div0 = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == 0x0323) {
            pre_div = (setting + i)->value;
            mask |= 1 << 1;
        } else if((setting + i)->addr == 0x0324) {
            multiplier_h = (setting + i)->value;
            mask |= 1 << 2;
        } else if((setting + i)->addr == 0x0325) {
            multiplier_l = (setting + i)->value;
            mask |= 1 << 3;
        } else if((setting + i)->addr == 0x032f) {
            post_div = (setting + i)->value;
            mask |= 1 << 4;
        } else if((setting + i)->addr == 0x0328) {
            st_div = (setting + i)->value;
            mask |= 1 << 5;
        } else if((setting + i)->addr == 0x032a) {
            t_div = (setting + i)->value;
            mask |= 1 << 6;
        }

        if(mask == 0x7f) break;
    }

    if(mask != 0x7f) {
        SNS_ERR("get setting sclk fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    pre_div0 = (pre_div0 & 0x1) + 1;

    pre_div = pre_div & 0x7;
    if (pre_div == 0) {
        pre_div = 1;
    } else if (pre_div == 1) {
        pre_div = 1.5;
    } else if (pre_div == 2) {
        pre_div = 2;
    } else if (pre_div == 3) {
        pre_div = 2.5;
    } else if (pre_div == 4) {
        pre_div = 3;
    } else if (pre_div == 5) {
        pre_div = 4;
    } else if (pre_div == 6) {
        pre_div = 6;
    } else if (pre_div == 7) {
        pre_div = 8;
    } else {
    }

    multiplier_h &= 0x3;
    multiplier_l &= 0xFF;
    multiplier = multiplier_h << 8 | multiplier_l;

    post_div = (post_div & 0xF) + 1;
    //sram_div = (os08b10_reg_read(nPipeId, 0x0327) & 0xF) + 1;
    st_div = (st_div & 0xF) + 1;

    t_div &= 0x7;
    if (t_div == 0) {
        t_div = 1;
    } else if (t_div == 1) {
        t_div = 1.5;
    } else if (t_div == 2) {
        t_div = 2;
    } else if (t_div == 3) {
        t_div = 2.5;
    } else if (t_div == 4) {
        t_div = 3;
    } else if (t_div == 5) {
        t_div = 3.5;
    } else if (t_div == 6) {
        t_div = 4;
    } else if (t_div == 7) {
        t_div = 5;
    } else {
    }
    *sclk = (((((((float)(inck * 1000 * 1000) / pre_div0) / pre_div) * multiplier) / post_div) / st_div) / t_div);
    /*SNS_DBG("%s pre_div0=0x%x, pre_div=0x%x, multiplier=0x%x, post_div=0x%x, sram_div=0x%x, st_div=0x%x, t_div=0x%x \n", \
          __func__, pre_div0, pre_div, multiplier, post_div, sram_div, st_div, t_div); */

    return SNS_SUCCESS;
}

AX_S32 os08b10_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    switch (sns_obj->eImgMode) {
    case e_OS08B10_4lane_3840x2160_Linear_10bit_25fps_1440Mbps:
        *setting = OS08B10_4lane_3840x2160_Linear_10bit_25fps_1440Mbps;
        *cnt = sizeof(OS08B10_4lane_3840x2160_Linear_10bit_25fps_1440Mbps) / sizeof(camera_i2c_reg_array);
        break;

    case e_OS08B10_4lane_3840x2160_Linear_10bit_30fps_1440Mbps:
        *setting = OS08B10_4lane_3840x2160_Linear_10bit_30fps_1440Mbps;
        *cnt = sizeof(OS08B10_4lane_3840x2160_Linear_10bit_30fps_1440Mbps) / sizeof(camera_i2c_reg_array);
        break;

    case e_OS08B10_4lane_3840x2160_2hdr_10bit_25fps_1440Mbps:
        *setting = OS08B10_4lane_3840x2160_2hdr_10bit_25fps_1440Mbps;
        *cnt = sizeof(OS08B10_4lane_3840x2160_2hdr_10bit_25fps_1440Mbps) / sizeof(camera_i2c_reg_array);
        break;

    case e_OS08B10_4lane_3840x2160_2hdr_10bit_30fps_1440Mbps:
        *setting = OS08B10_4lane_3840x2160_2hdr_10bit_30fps_1440Mbps;
        *cnt = sizeof(OS08B10_4lane_3840x2160_2hdr_10bit_30fps_1440Mbps) / sizeof(camera_i2c_reg_array);
        break;

    case e_OS08B10_4lane_3840x2160_Linear_10bit_50fps_1440Mbps:
        *setting = OS08B10_4lane_3840x2160_Linear_10bit_50fps_1440Mbps;
        *cnt = sizeof(OS08B10_4lane_3840x2160_Linear_10bit_50fps_1440Mbps) / sizeof(camera_i2c_reg_array);
        break;

    default:
        SNS_ERR("it's not supported. pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SNS_INFO("pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);

    return SNS_SUCCESS;
}

AX_S32 os08b10_write_settings(ISP_PIPE_ID nPipeId)
{
    AX_U32 i = 0;
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;

    ret = os08b10_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    for (i = 0; i < reg_cnt; i++) {
        os08b10_write_register(nPipeId, (setting + i)->addr, ((setting + i)->value));
#ifdef SENSOR_DEBUG
        usleep(2 * 1000);
        AX_U8 val = os08b10_reg_read(nPipeId, (default_setting + i)->addr);
        SNS_DBG(" addr: 0x%04x write:0x%02x read:0x%02x \n",
                (default_setting + i)->addr, (default_setting + i)->value, val);
#endif
    }

    return SNS_SUCCESS;
}
