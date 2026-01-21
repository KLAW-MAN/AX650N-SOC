/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SPI_H__
#define __SPI_H__
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include "ax_base_type.h"
#include "isp_sensor_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define	AX_SPI_CPHA		(1UL << 0)	/* clock phase */
#define	AX_SPI_CPOL		(1UL << 1)	/* clock polarity */

#define	AX_SPI_MODE_0		(0|0)		/* (original MicroWire) */
#define	AX_SPI_MODE_1		(0|AX_SPI_CPHA)
#define	AX_SPI_MODE_2		(AX_SPI_CPOL|0)
#define	AX_SPI_MODE_3		(AX_SPI_CPOL|AX_SPI_CPHA)
#define	AX_SPI_MODE_X_MASK		(AX_SPI_CPOL|AX_SPI_CPHA)

#define	AX_SPI_CS_HIGH		(1UL << 2)	/* chipselect active high? */
#define	AX_SPI_LSB_FIRST    (1UL << 3)	/* per-word bits-on-wire */
#define	AX_SPI_3WIRE		(1UL << 4)	/* SI/SO signals shared */
#define	AX_SPI_LOOP		    (1UL << 5)	/* loopback mode */
#define	AX_SPI_NO_CS		(1UL << 6)	/* 1 dev/bus, no chipselect */
#define	AX_SPI_READY		(1UL << 7)	/* slave pulls low to pause */
#define	AX_SPI_TX_DUAL		(1UL << 8)	/* transmit with 2 wires */
#define	AX_SPI_TX_QUAD		(1UL << 9)	/* transmit with 4 wires */
#define	AX_SPI_RX_DUAL		(1UL << 10)	/* receive with 2 wires */
#define	AX_SPI_RX_QUAD		(1UL << 11)	/* receive with 4 wires */
#define	AX_SPI_CS_WORD		(1UL << 12)	/* toggle cs after each word */
#define	AX_SPI_TX_OCTAL		(1UL << 13)	/* transmit with 8 wires */
#define	AX_SPI_RX_OCTAL		(1UL << 14)	/* receive with 8 wires */
#define	AX_SPI_3WIRE_HIZ    (1UL << 15)	/* high impedance turnaround */

/*
 * All the bits defined above should be covered by SPI_MODE_USER_MASK.
 * The SPI_MODE_USER_MASK has the SPI_MODE_KERNEL_MASK counterpart in
 * 'include/linux/spi/spi.h'. The bits defined here are from bit 0 upwards
 * while in SPI_MODE_KERNEL_MASK they are from the other end downwards.
 * These bits must not overlap. A static assert check should make sure of that.
 * If adding extra bits, make sure to increase the bit index below as well.
 */
#define AX_SPI_MODE_USER_MASK	(AX_BIT(16) - 1)


AX_S32 spi_write(int fd, unsigned int devaddr, unsigned int dev_addr_bytes,
                        unsigned int regaddr, unsigned int reg_addr_bytes,
                        unsigned int regdata, unsigned int data_bytes);
AX_S32 spi_read(int fd, unsigned int devaddr, unsigned int dev_addr_bytes,
                        unsigned int regaddr, unsigned int reg_addr_bytes,
                        unsigned int *regdata, unsigned int data_bytes);
int spi_init(AX_SNS_SPI_T *spi_attr);
int spi_exit(int spi_fd);
#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */
