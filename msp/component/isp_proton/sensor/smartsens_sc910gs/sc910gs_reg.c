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
#include "spi.h"
#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"

#include "sc910gs_reg.h"
#include "sc910gs_settings.h"


extern SNS_STATE_OBJ *g_szsc910gsCtx[AX_VIN_MAX_PIPE_NUM];
#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szsc910gsCtx[dev])

AX_SNS_COMMBUS_T gsc910gsBusInfo[AX_VIN_MAX_PIPE_NUM] = {{{1}}, {{2}}};


AX_S32 sc910gs_set_bus_info(ISP_PIPE_ID nPipeId, AX_SNS_COMMBUS_T tSnsBusInfo)
{
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    if (tSnsBusInfo.busType == ISP_SNS_CONNECT_I2C_TYPE) {
        gsc910gsBusInfo[nPipeId].I2cDev = tSnsBusInfo.I2cDev;
    } else {
        gsc910gsBusInfo[nPipeId].SpiDev.bit4SpiDev = tSnsBusInfo.SpiDev.bit4SpiDev;
        gsc910gsBusInfo[nPipeId].SpiDev.bit4SpiCs = tSnsBusInfo.SpiDev.bit4SpiCs;
    }

    gsc910gsBusInfo[nPipeId].busType = tSnsBusInfo.busType;

    return SNS_SUCCESS;
}

AX_S32 sc910gs_get_bus_num(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        return gsc910gsBusInfo[nPipeId].I2cDev;
    } else {
        return  gsc910gsBusInfo[nPipeId].SpiDev.bit4SpiDev;
    }
}


AX_S32 sc910gs_sensor_i2c_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 snsId = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sns_obj->sns_i2c_obj.sns_i2c_fd = -1;
    sns_obj->sns_i2c_obj.slave_addr = SC910GS_SLAVE_ADDR;
    sns_obj->sns_i2c_obj.address_byte = SC910GS_ADDR_BYTE;
    sns_obj->sns_i2c_obj.data_byte = SC910GS_DATA_BYTE;
    sns_obj->sns_i2c_obj.swap_byte = SC910GS_SWAP_BYTES;

    sns_obj->sns_i2c_obj.sns_i2c_bnum = sc910gs_get_bus_num(nPipeId);

    sns_obj->sns_i2c_obj.sns_i2c_fd = i2c_init(sns_obj->sns_i2c_obj.sns_i2c_bnum,
                                      sns_obj->sns_i2c_obj.slave_addr);

    SNS_DBG("sc910gs i2c init finish, i2c bus %d \n", sns_obj->sns_i2c_obj.sns_i2c_bnum);

    return SNS_SUCCESS;
}

AX_S32 sc910gs_sensor_spi_init(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 snsId = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sns_obj->sns_spi_obj.sns_spi_bnum = gsc910gsBusInfo[nPipeId].SpiDev.bit4SpiDev;
    sns_obj->sns_spi_obj.sns_spi_cs = gsc910gsBusInfo[nPipeId].SpiDev.bit4SpiCs;
    sns_obj->sns_spi_obj.mode = AX_SPI_MODE_3;
    sns_obj->sns_spi_obj.lsb_first = 0; //no use
    sns_obj->sns_spi_obj.bits_per_word = 8;
    sns_obj->sns_spi_obj.max_speed_hz = 1000000;
    sns_obj->sns_spi_obj.sns_spi_fd = spi_init(&sns_obj->sns_spi_obj);

    SNS_DBG("sc910gs spi init finish, spi bus %d \n", sns_obj->sns_spi_obj.sns_spi_bnum);

    return SNS_SUCCESS;
}

AX_S32 sc910gs_sensor_i2c_exit(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    ret = i2c_exit(sns_obj->sns_i2c_obj.sns_i2c_fd);

    return ret;
}

AX_S32 sc910gs_sensor_spi_exit(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    ret = spi_exit(sns_obj->sns_spi_obj.sns_spi_fd);

    return ret;
}

static AX_U8 reg_endian_swap(AX_U8 reg)
{
    int i = 0;
    AX_U8 temp =0;
    AX_U8 temp1 = 0;

    for (i = 7; i >= 0; i--) {
        temp = (reg >> i) & 1;
        temp1 += temp << (7 - i);
    }

    return temp1;
}

static AX_S32 sc910gs_spi_write(AX_S32 fd, AX_U32 devaddr, AX_U32 dev_addr_bytes,
                        AX_U32 regaddr, AX_U32 reg_addr_bytes,
                        AX_U32 regdata, AX_U32 data_bytes)
{
    struct spi_ioc_transfer transfer;
    AX_U8 buf[10] = {0};
    AX_U32 dev_addr, reg_addr, data;
    AX_U32 dev_width, reg_width, data_width;
    AX_U32 index = 0;
    AX_S32 ret = 0;

    memset(&transfer, 0, sizeof(transfer));

    dev_addr = devaddr;
    reg_addr = regaddr;
    data = regdata;
    dev_width = dev_addr_bytes;
    reg_width = reg_addr_bytes;
    data_width = data_bytes;

    transfer.tx_buf = (__u64)buf;
    transfer.rx_buf = 0;
    transfer.len = dev_width + reg_width + data_width;
    transfer.cs_change = 1;

    if (dev_width == 2) {
        buf[index++] = reg_endian_swap((dev_addr >> 8) & 0xff);
    }
    buf[index++] = reg_endian_swap(dev_addr & 0xff);

    buf[index++] = reg_endian_swap(reg_addr & 0xff);
    if (reg_width == 2) {
        buf[index++] = reg_endian_swap((reg_addr >> 8) & 0xff);
    }

    buf[index++] = reg_endian_swap(data & 0xff);
    if (data_width == 2) {
        buf[index++] = reg_endian_swap((data >> 8) & 0xff);
    }

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
	if (ret < 1)
		SNS_ERR("can't send spi message");

    return ret;
}

static AX_S32 sc910gs_spi_read(int fd, AX_U32 devaddr, AX_U32 dev_addr_bytes,
                        AX_U32 regaddr, AX_U32 reg_addr_bytes,
                        AX_U32 *regdata, AX_U32 data_bytes)
{
    struct spi_ioc_transfer transfer;
    AX_U8 tx_buf[10] = {0};
    AX_U8 rx_buf[10] = {0};
    AX_U32 dev_addr, reg_addr, data;
    AX_U32 dev_width, reg_width, data_width;
    AX_S32 index = 0;
    AX_S32 ret = 0;

    memset(&transfer, 0, sizeof(transfer));

    dev_addr = devaddr;
    reg_addr = regaddr;
    dev_width = dev_addr_bytes;
    reg_width = reg_addr_bytes;
    data_width = data_bytes;

    transfer.tx_buf = (__u64)tx_buf;
    transfer.rx_buf = (__u64)rx_buf;
    transfer.len = dev_width + reg_width + 1;
    transfer.cs_change = 1;
    transfer.speed_hz = 7000000; //as kernel will set 10M, but 10M read is invalid

    if (dev_width == 2) {
        tx_buf[index++] = reg_endian_swap((dev_addr >> 8) & 0xff);
    }
    tx_buf[index++] = reg_endian_swap(dev_addr & 0xff);

    tx_buf[index++] = reg_endian_swap(reg_addr & 0xff);
    if (reg_width == 2) {
        tx_buf[index++] = reg_endian_swap((reg_addr >> 8) & 0xff);
    }

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
	if (ret < 1)
		SNS_ERR("can't send spi message");

    if (data_width == 2)
        *regdata = reg_endian_swap((rx_buf[3] << 8)) | reg_endian_swap(rx_buf[4]);
    else
        *regdata = reg_endian_swap(rx_buf[3]);

    return ret;
}

AX_S32 sc910gs_read_register(ISP_PIPE_ID nPipeId, AX_U32 nAddr, AX_U32 *pData)
{
    AX_U8 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;
    AX_U32 dev_addr, devaddr_bytes, regaddr_bytes, data_bytes;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
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
    } else {
        if (-1 == sns_obj->sns_spi_obj.sns_spi_fd) {
            return -1;
        }
        dev_addr = ((SC910GS_DEVICEID << 1) | SC910GS_SPI_READ) & 0XFF;
        devaddr_bytes = 1;
        regaddr_bytes = 2;
        data_bytes = 1;
        nRet = sc910gs_spi_read(sns_obj->sns_spi_obj.sns_spi_fd,
                        dev_addr, devaddr_bytes,
                        nAddr, regaddr_bytes,
                        pData, data_bytes);
         if (nRet < 1) {
            SNS_ERR("read sensor reg 0x%x fail\n", nAddr);
            return nRet;
         } else {
            return 0;
         }
    }
}


AX_S32 sc910gs_reg_read(ISP_PIPE_ID nPipeId, AX_U32 addr)
{
    AX_U32 data;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_S32 nRet = 0;
    AX_U32 dev_addr, devaddr_bytes, regaddr_bytes, data_bytes;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
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
    } else {
        if (-1 == sns_obj->sns_spi_obj.sns_spi_fd) {
            return -1;
        }
        dev_addr = ((SC910GS_DEVICEID << 1) | SC910GS_SPI_READ) & 0XFF;
        devaddr_bytes = 1;
        regaddr_bytes = 2;
        data_bytes = 1;
        nRet = sc910gs_spi_read(sns_obj->sns_spi_obj.sns_spi_fd,
                        dev_addr, devaddr_bytes,
                        addr, regaddr_bytes,
                        &data, data_bytes);
         if (nRet < 1) {
            SNS_ERR("read sensor reg 0x%x fail\n", addr);
            return nRet;
         } else {
            return data;
         }

    }
}

AX_S32 sc910gs_write_register(ISP_PIPE_ID nPipeId, AX_U32 addr, AX_U32 data)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_U32 dev_addr, devaddr_bytes, regaddr_bytes, data_bytes;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
   if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        if (-1 == sns_obj->sns_i2c_obj.sns_i2c_fd) {
            return -1;
        }

        nRet = i2c_write(sns_obj->sns_i2c_obj.sns_i2c_fd, sns_obj->sns_i2c_obj.slave_addr, addr,
                        sns_obj->sns_i2c_obj.address_byte,
                        (AX_U8 *)(&data), sns_obj->sns_i2c_obj.data_byte, sns_obj->sns_i2c_obj.swap_byte);

        return nRet;
   } else {
        if (-1 == sns_obj->sns_spi_obj.sns_spi_fd) {
            return -1;
        }
        dev_addr = ((SC910GS_DEVICEID << 1) | SC910GS_SPI_WIRTE) & 0xFF;
        devaddr_bytes = 1;
        regaddr_bytes = 2;
        data_bytes = 1;
        nRet = sc910gs_spi_write(sns_obj->sns_spi_obj.sns_spi_fd,
                        dev_addr, devaddr_bytes,
                        addr, regaddr_bytes,
                        data, data_bytes);
        if (nRet < 0)
            return -1;

        return 0;
   }
}


AX_S32 sc910gs_hw_reset(AX_U32 gpio_num, AX_U32 gpio_out_val)
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

AX_S32 sc910gs_group_prepare(ISP_PIPE_ID nPipeId)
{
    AX_S32 result = 0;

    return result;
}

AX_S32 sc910gs_reset(ISP_PIPE_ID nPipeId, AX_U32 nResetGpio)
{
    AX_S32 result = 0;
    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    result |= sc910gs_hw_reset(nResetGpio, 0);
    usleep(5 * 1000);
    result |= sc910gs_hw_reset(nResetGpio, 1);
    usleep(10 * 1000);

    return result;
}

AX_U32 sc910gs_get_hts(ISP_PIPE_ID nPipeId)
{
    AX_U8 frame_length_l = 0;
    AX_U8 frame_length_h = 0;
    AX_U16 frame_length = 0;
    AX_U32 hts = 0;
    AX_U32 fps = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    // one line time = 1 / (fps * frame_length)
    frame_length_h = sc910gs_reg_read(nPipeId, 0x320e);
    frame_length_l = sc910gs_reg_read(nPipeId, 0x320f);
    frame_length = (frame_length_h << 8) | frame_length_l;
    fps = (AX_U32)sns_obj->sns_mode_obj.fFrameRate;

    hts = 1000000 / (fps * frame_length);

    return hts;
}

AX_U32 sc910gs_get_vs_hts(ISP_PIPE_ID nPipeId)
{
    AX_U8 frame_length_l = 0;
    AX_U8 frame_length_h = 0;
    AX_U16 frame_length = 0;
    AX_U32 hts = 0;
    AX_U32 fps = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    // one line time = 1 / (fps * frame_length)
    frame_length_h = sc910gs_reg_read(nPipeId, 0x320e);
    frame_length_l = sc910gs_reg_read(nPipeId, 0x320f);
    frame_length = (frame_length_h << 8) | frame_length_l;
    fps = (AX_U32)sns_obj->sns_mode_obj.fFrameRate;

    hts = 1000000 / (fps * frame_length);

    return hts;
}

AX_U32 sc910gs_set_hts(ISP_PIPE_ID nPipeId, AX_U32 hts)
{
    AX_U8 frame_length_l = 0;
    AX_U8 frame_length_h = 0;
    AX_U16 frame_length = 0;
    AX_U32 fps = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    // one line time = 1 / (fps * frame_length)
    fps = (AX_U32)sns_obj->sns_mode_obj.fFrameRate;
    frame_length = 1000000 / (fps * hts);

    frame_length_h = (frame_length >> 8) & 0xff;
    frame_length_l = frame_length & 0xff;
    sc910gs_write_register(nPipeId, 0x320e, frame_length_h);
    sc910gs_write_register(nPipeId, 0x320f, frame_length_l);

    return 0;
}

AX_U32 sc910gs_get_vts(ISP_PIPE_ID nPipeId)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U32 vts = 0;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    vts_h = sc910gs_reg_read(nPipeId, 0x320e);
    vts_l = sc910gs_reg_read(nPipeId, 0x320f);
    vts = (AX_U32)(((vts_h & 0xFF) << 8) | (AX_U32)(vts_l & 0xFF));

    return vts;
}

AX_U32 sc910gs_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_S32 result = 0;

    if (nPipeId < 0 || (nPipeId >= AX_VIN_MAX_PIPE_NUM))
        return -1;

    vts_h = (vts >> 8) & 0xff;
    vts_l = vts & 0xff;
    result |= sc910gs_write_register(nPipeId, 0x320e, vts_h);
    result |= sc910gs_write_register(nPipeId, 0x320f, vts_l);


    return result;
}
#if 0
AX_U32 sc910gs_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts)
{
    AX_U8 hts_h;
    AX_U8 hts_m;
    AX_U8 hts_l;
    AX_U8 fix_row = 0;
    AX_U32 max_raw = 0;
    AX_U8 max_h = 0;
    AX_U8 max_l = 0;
    AX_U8 tmp;
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    max_h = sc910gs_reg_read(nPipeId, 0x320e);
    max_l = sc910gs_reg_read(nPipeId, 0x320f);
    max_raw = (max_h << 8) | max_l;
    if (vts > (max_raw / 2 - 4))
        vts = max_raw / 2 - 4;
    fix_row = sc910gs_reg_read(nPipeId, 0x323b);
    hts_l = sc910gs_reg_read(nPipeId, 0x3e02);
    hts_h = sc910gs_reg_read(nPipeId, 0x3e00);
    tmp = vts - fix_row;
    hts_l &= 0x0f;
    hts_l |= (vts & 0xF0);
    hts_m = (vts >> 4) & 0xff;
    hts_h &= 0xf0;
    hts_h |= (vts >> 12) & 0xf;

    result |= sc910gs_write_register(nPipeId, 0x3e00, hts_h);
    result |= sc910gs_write_register(nPipeId, 0x3e01, hts_m);
    result |= sc910gs_write_register(nPipeId, 0x3e02, hts_l);

    return result;
}
#endif

AX_F32 sc910gs_get_sclk(ISP_PIPE_ID nPipeId)
{
    float inck;

    inck = SC910GS_INCK_24M;

    return inck;
}

AX_S32 sc910gs_get_vts_from_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array *setting, AX_U32 reg_cnt, AX_U32 *vts)
{
    AX_U32 i = 0;
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U8 mask = 0;

    for (i = 0; i < reg_cnt; i++) {
        if((setting + i)->addr == 0x320e) {
            vts_h = (setting + i)->value;
            mask |= 1;
        } else if((setting + i)->addr == 0x320f) {
            vts_l = (setting + i)->value;
            mask |= 1 << 1;
        }

        if(mask == 0x3) break;
    }

    //default value
    if((mask & 0x1) == 0) {
        vts_h = 0x9;
        mask |= 1;
    }
    if((mask & (1 << 1)) == 0) {
        vts_l = 0xc4;
        mask |= 1 << 1;
    }

    if(mask != 0x3) {
        SNS_ERR("get setting vts fail, mask:0x%x\n", mask);
        return SNS_ERR_CODE_FAILED;
    }

    *vts = vts_h << 8 | vts_l;

    return SNS_SUCCESS;
}

AX_S32 sc910gs_select_setting(ISP_PIPE_ID nPipeId, camera_i2c_reg_array **setting, AX_U32 *cnt)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    switch (sns_obj->eImgMode) {
    case e_SC910GS_1x1C4D_3840x2336_LINEAR_10bit_25fps_4lane_450Mbps_MASTER:
        *setting = &SC910GS_LVDS_24Minput_1x1C4D_900Mbps_10bit_3840x2336_25fps_MASTER[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_1x1C4D_900Mbps_10bit_3840x2336_25fps_MASTER) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_1x1C4D_3840x2336_LINEAR_10bit_30fps_4lane_450Mbps_MASTER:
        *setting = &SC910GS_LVDS_24Minput_1x1C4D_900Mbps_10bit_3840x2336_30fps_MASTER[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_1x1C4D_900Mbps_10bit_3840x2336_30fps_MASTER) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_10bit_50fps_8lane_750Mbps_MASTER:
        *setting = &SC910GS_LVDS_24Minput_2x1C4D_750Mbps_10bit_3840x2336_50fps_MASTER[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_2x1C4D_750Mbps_10bit_3840x2336_50fps_MASTER) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_25fps_8lane_540Mbps_MASTER:
        *setting = &SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_25fps_MASTER[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_25fps_MASTER) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_30fps_8lane_540Mbps_MASTER:
        *setting = &SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_30fps_MASTER[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_30fps_MASTER) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_10bit_50fps_8lane_750Mbps_Trigger_Readout:
        *setting = &SC910GS_LVDS_24Minput_2x1C4D_750Mbps_10bit_3840x2336_50fps_Trigger_Readout[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_2x1C4D_750Mbps_10bit_3840x2336_50fps_Trigger_Readout) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_25fps_8lane_540Mbps_Trigger_Readout:
        *setting = &SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_25fps_Trigger_Readout[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_25fps_Trigger_Readout) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_30fps_8lane_540Mbps_Trigger_Readout:
        *setting = &SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_30fps_Trigger_Readout[0];
        *cnt = sizeof(SC910GS_LVDS_24Minput_2x1C4D_540Mbps_12bit_3840x2336_30fps_Trigger_Readout) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_10bit_54fps_8lane_810Mbps_Trigger_Exposure:
        *setting = &SC910GS_2x1C4D_3840x2336_LINEAR_10bit_54fps_8lane_810Mbps_Trigger_Exposure[0];
        *cnt = sizeof(SC910GS_2x1C4D_3840x2336_LINEAR_10bit_54fps_8lane_810Mbps_Trigger_Exposure) / sizeof(camera_i2c_reg_array);
        break;

    case e_SC910GS_2x1C4D_3840x2336_LINEAR_12bit_45fps_8lane_810Mbps_Trigger_Exposure:
        *setting = &SC910GS_2x1C4D_3840x2336_LINEAR_12bit_45fps_8lane_810Mbps_Trigger_Exposure[0];
        *cnt = sizeof(SC910GS_2x1C4D_3840x2336_LINEAR_12bit_45fps_8lane_810Mbps_Trigger_Exposure) / sizeof(camera_i2c_reg_array);
        break;

    default:
        SNS_ERR("it's not supported. pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    SNS_INFO("pipe=%d, setting mode=%d\n", nPipeId, sns_obj->eImgMode);

    return SNS_SUCCESS;
}

AX_S32 sc910gs_write_settings(ISP_PIPE_ID nPipeId)
{
    AX_U32 i = 0;
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;

    ret = sc910gs_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    for (i = 0; i < reg_cnt; i++) {
        sc910gs_write_register(nPipeId, (setting + i)->addr, ((setting + i)->value));
#ifdef SENSOR_DEBUG
        usleep(2 * 1000);
        AX_U8 val = sc910gs_reg_read(nPipeId, (default_setting + i)->addr);
        SNS_DBG(" addr: 0x%04x write:0x%02x read:0x%02x \n",
                (default_setting + i)->addr, (default_setting + i)->value, val);
#endif
    }

    return SNS_SUCCESS;
}