/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
#ifndef _SPI_AXERA_RAW_H
#define _SPI_AXERA_RAW_H

#include <linux/const.h>

typedef struct spi_data {
	int tx_size;
	int rx_size;
	char *rx_buf;
	char *tx_buf;
} spi_data_t;

#define SPI_SLV_IOC_MAGIC 'B'
#define SET_SPI_TX  _IOW(SPI_SLV_IOC_MAGIC, 0x82, char)
#define SET_SPI_RX _IOR(SPI_SLV_IOC_MAGIC, 0x83, char)
#define GET_SPI_INPUT_KFIFO_SIZE _IOR(SPI_SLV_IOC_MAGIC, 0x84, int)

#define	SPI_TX_OCTAL		_BITUL(13)	/* transmit with 8 wires */
#define	SPI_RX_OCTAL		_BITUL(14)	/* receive with 8 wires */

#endif /* _SPI_AXERA_RAW_H */
