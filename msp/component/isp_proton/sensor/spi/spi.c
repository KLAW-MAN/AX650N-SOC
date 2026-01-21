/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
//#include <linux/spi/spi.h>
#include "isp_sensor_types.h"
#include "spi.h"

AX_S32 spi_write(int fd, unsigned int devaddr, unsigned int dev_addr_bytes,
                        unsigned int regaddr, unsigned int reg_addr_bytes,
                        unsigned int regdata, unsigned int data_bytes)
{
    struct spi_ioc_transfer transfer;
    unsigned char buf[10] = {0};
    unsigned int dev_addr, reg_addr, data;
    unsigned int dev_width, reg_width, data_width;
    int index = 0;
    int ret = 0;

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
        buf[index++] = (dev_addr >> 8) & 0xff;
    }
    buf[index++] = dev_addr & 0xff;

    buf[index++] = reg_addr & 0xff;
    if (reg_width == 2) {
        buf[index++] = (reg_addr >> 8) & 0xff;
    }

    buf[index++] = data & 0xff;
    if (data_width == 2) {
        buf[index++] = (data >> 8) & 0xff;
    }

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
	if (ret < 1)
		SNS_ERR("can't send spi message");

    return ret;
}

AX_S32 spi_read(int fd, unsigned int devaddr, unsigned int dev_addr_bytes,
                        unsigned int regaddr, unsigned int reg_addr_bytes,
                        unsigned int *regdata, unsigned int data_bytes)
{
    struct spi_ioc_transfer transfer;
    unsigned char tx_buf[10] = {0};
    unsigned char rx_buf[10] = {0};
    unsigned int dev_addr, reg_addr, data;
    unsigned int dev_width, reg_width, data_width;
    int index = 0;
    int ret = 0;

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

    if (dev_width == 2) {
        tx_buf[index++] = (dev_addr >> 8) & 0xff;
    }
    tx_buf[index++] = dev_addr & 0xff;

    tx_buf[index++] = reg_addr & 0xff;
    if (reg_width == 2) {
        tx_buf[index++] = (reg_addr >> 8) & 0xff;
    }

    tx_buf[index++] = 0;

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &transfer);
	if (ret < 1)
		SNS_ERR("can't send spi message");

    if (data_width == 2)
        *regdata = (rx_buf[3] << 8) | rx_buf[4];
    else
        *regdata = rx_buf[3];

    return ret;
}

int spi_init(AX_SNS_SPI_T *spi_attr)
{
    int ret;
    int fd = -1;
    char device_name[64];

    sprintf(device_name, "/dev/spidev%d.%d", spi_attr->sns_spi_bnum, spi_attr->sns_spi_cs);

    SNS_DBG("spi device is %s \n", device_name);

    fd = open(device_name, O_RDWR);
    if (fd < 0) {
        SNS_ERR("Open %s error!\n", device_name);
        return -1;
    }

    /*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE32, &spi_attr->mode);
	if (ret == -1)
		SNS_ERR("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE32, &spi_attr->mode);
	if (ret == -1)
		SNS_ERR("can't get spi mode");

#if 0
    /*
	 * spi bit justification
	 */
	ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &spi_attr->lsb_first);
	if (ret == -1)
		SNS_ERR("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &spi_attr->lsb_first);
	if (ret == -1)
		SNS_ERR("can't get spi mode");
#endif
	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_attr->bits_per_word);
	if (ret == -1)
		SNS_ERR("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spi_attr->bits_per_word);
	if (ret == -1)
		SNS_ERR("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_attr->max_speed_hz);
	if (ret == -1)
		SNS_ERR("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_attr->max_speed_hz);
	if (ret == -1)
		SNS_ERR("can't get max speed hz");

    return fd;
}

int spi_exit(int spi_fd)
{
    if (spi_fd >= 0) {
        close(spi_fd);
        spi_fd = -1;
        return 0;
    }
    return -1;
}
