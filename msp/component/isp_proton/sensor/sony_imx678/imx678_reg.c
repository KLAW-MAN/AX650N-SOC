#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ax_base_type.h"
#include "ax_isp_common.h"

#include "i2c.h"
#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"

#include "imx678_reg.h"
#include "imx678_settings.h"


extern SNS_STATE_OBJ *g_szImx678Ctx[AX_VIN_MAX_PIPE_NUM];
#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szImx678Ctx[dev])

static AX_SNS_COMMBUS_T gImx678BusInfo[AX_VIN_MAX_PIPE_NUM] = { {{1}}, {{2}},};
static AX_U32 SENSOR_RSTN[AX_VIN_MAX_DEV_NUM] = {64, 65, 22, 135};

AX_S32 imx678_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo)
{
    gImx678BusInfo[nPipeId].I2cDev = tSnsBusInfo.I2cDev;

    return SNS_SUCCESS;
}

AX_S32 imx678_get_bus_num(ISP_PIPE_ID nPipeId)
{
    return gImx678BusInfo[nPipeId].I2cDev;
}

AX_S32 imx678_sensor_i2c_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 snsId = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);

    SNS_CHECK_PTR_VALID(sns_obj);

    sns_obj->sns_i2c_obj.sns_i2c_fd = -1;
    sns_obj->sns_i2c_obj.slave_addr = IMX678_SLAVE_ADDR;
    sns_obj->sns_i2c_obj.address_byte = IMX678_ADDR_BYTE;
    sns_obj->sns_i2c_obj.data_byte = IMX678_DATA_BYTE;
    sns_obj->sns_i2c_obj.swap_byte = IMX678_SWAP_BYTES;

    sns_obj->sns_i2c_obj.sns_i2c_bnum = imx678_get_bus_num(nPipeId);

    sns_obj->sns_i2c_obj.sns_i2c_fd = i2c_init(sns_obj->sns_i2c_obj.sns_i2c_bnum,
                                      sns_obj->sns_i2c_obj.slave_addr);

    SNS_DBG("imx678 i2c init finish, i2c bus %d \n", sns_obj->sns_i2c_obj.sns_i2c_bnum);

    return SNS_SUCCESS;
}

AX_S32 imx678_sensor_i2c_exit(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    ret = i2c_exit(sns_obj->sns_i2c_obj.sns_i2c_fd);

    return ret;
}

AX_S32 imx678_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData)
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

AX_S32 imx678_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr)
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

AX_S32 imx678_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data)
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

AX_U32 imx678_get_vmax(ISP_PIPE_ID nPipeId)
{
    AX_U8 h = 0;
    AX_U8 m = 0;
    AX_U8 l = 0;
    AX_U32 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = imx678_reg_read(nPipeId, IMX678_VMAX_L);
    m = imx678_reg_read(nPipeId, IMX678_VMAX_M);
    h = imx678_reg_read(nPipeId, IMX678_VMAX_H);

    val = (AX_U32)(((h & 0xF) << 16) | (AX_U32)(m << 8) | (AX_U32)(l << 0));

    return val;
}

AX_U32 imx678_get_hmax(ISP_PIPE_ID nPipeId)
{
    AX_U8 h = 0;
    AX_U8 l = 0;
    AX_U32 val = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    l = imx678_reg_read(nPipeId, IMX678_HMAX_L);
    h = imx678_reg_read(nPipeId, IMX678_HMAX_H);

    val = (AX_U32)((AX_U32)(h << 8) | (AX_U32)(l << 0));

    return val;
}

AX_S32 imx678_hw_reset(unsigned int gpio_num, unsigned int gpio_out_val)
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


AX_S32 imx678_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio)
{
    AX_S32 result = 0;
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    result |= imx678_hw_reset(nResetGpio, 0);
    usleep(5 * 1000);
    result |= imx678_hw_reset(nResetGpio, 1);
    usleep(10 * 1000);

    return result;
}

AX_S32 imx678_get_hmax_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *hmax)
{
    AX_U32 i = 0;
    AX_U8 hmax_h = 0;
    AX_U8 hmax_l = 0;
    AX_U8 mask = 0;

    SNS_CHECK_PTR_VALID(setting);
    SNS_CHECK_PTR_VALID(hmax);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == IMX678_HMAX_H) {
            hmax_h = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == IMX678_HMAX_L) {
            hmax_l = (setting + i)->value;
            mask |= 1 << 1;
        }

        if(mask == 0x3) break;
    }

    if(mask != 0x3) {
        SNS_ERR("get setting hmax fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *hmax = hmax_h << 8 | hmax_l;

    return SNS_SUCCESS;
}

AX_S32 imx678_get_vmax_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vmax)
{
    AX_U32 i = 0;
    AX_U8 vmax_h = 0;
    AX_U8 vmax_m = 0;
    AX_U8 vmax_l = 0;
    AX_U8 mask = 0;

    SNS_CHECK_PTR_VALID(setting);
    SNS_CHECK_PTR_VALID(vmax);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == IMX678_VMAX_H) {
            vmax_h = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == IMX678_VMAX_M) {
            vmax_m = (setting + i)->value;
            mask |= 1 << 1;
        } else if((setting + i)->addr == IMX678_VMAX_L) {
            vmax_l = (setting + i)->value;
            mask |= 1 << 2;
        }
        if(mask == 0x7) break;
    }

    if(mask != 0x7) {
        SNS_ERR("get setting vmax fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *vmax = ((vmax_h & 0xF) << 16) | (vmax_m << 8) | vmax_l;

    return SNS_SUCCESS;
}

AX_S32 imx678_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    switch (sns_obj->eImgMode) {
        case e_IMX678_4LANE_24MHZ_891MBPS_3840x2160_RGGB_10BIT_LINEAR_30FPS:
            *setting = IMX678_4LANE_24MHZ_891MBPS_3840x2160_RGGB_10BIT_LINEAR_30FPS;
            *cnt = sizeof(IMX678_4LANE_24MHZ_891MBPS_3840x2160_RGGB_10BIT_LINEAR_30FPS) / sizeof(camera_i2c_reg_array);
            break;
        case e_IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_LINEAR_60FPS:
            *setting = IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_LINEAR_60FPS;
            *cnt = sizeof(IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_LINEAR_60FPS) / sizeof(camera_i2c_reg_array);
            break;
        case e_IMX678_4LANE_24MHZ_1188MBPS_3840x2160_RGGB_12BIT_LINEAR_30FPS:
            *setting = IMX678_4LANE_24MHZ_1188MBPS_3840x2160_RGGB_12BIT_LINEAR_30FPS;
            *cnt = sizeof(IMX678_4LANE_24MHZ_1188MBPS_3840x2160_RGGB_12BIT_LINEAR_30FPS) / sizeof(camera_i2c_reg_array);
            break;
        case e_IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_HDR2DOL_30FPS:
            *setting = IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_HDR2DOL_30FPS;
            *cnt = sizeof(IMX678_4LANE_24MHZ_1440MBPS_3840x2160_RGGB_10BIT_HDR2DOL_30FPS) / sizeof(camera_i2c_reg_array);
            break;

        default:
            SNS_ERR("it's not supported. pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);
            return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SNS_INFO("pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);

    return SNS_SUCCESS;
}

AX_S32 imx678_write_settings(ISP_PIPE_ID nPipeId)
{
    AX_U32 i = 0;
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    ret = imx678_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    for (i = 0; i < reg_cnt; i++) {
        imx678_write_register(nPipeId, (setting + i)->addr, ((setting + i)->value));
#ifdef SENSOR_DEBUG
        usleep(2 * 1000);
        AX_U8 val = imx678_reg_read(nPipeId, (setting + i)->addr);
        SNS_DBG(" addr: 0x%04x write:0x%02x read:0x%02x \n",
                (setting + i)->addr, (setting + i)->value, val);
#endif
    }

    return SNS_SUCCESS;
}
