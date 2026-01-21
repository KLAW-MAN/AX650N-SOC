#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/acpi.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include <linux/uaccess.h>

#include "osal_ax.h"
#include "osal_dev_ax.h"
#include "axdev.h"
#include "axdev_log.h"
#include "osal_lib_ax.h"

#define SPI_MAX_NUM 5
#define SPI_10M 10000000
#define SPI_07M 7000000

#ifdef CONFIG_SPI_SPIDEV

struct spi_master *master_bus[SPI_MAX_NUM];
struct spi_device *ax_spi[SPI_MAX_NUM][4];

AX_S32 AX_OSAL_DEV_spi_dev_read(u32 spi_bus, u32 spi_csn, u8 devid, u8 addrL, u8 addrH, u8 *data)
{
	struct spi_device *spi = ax_spi[spi_bus][spi_csn];
	struct spi_message m;

	unsigned char tx_buf[8];
	unsigned char rx_buf[8];

	struct spi_transfer t = {
		.rx_buf = rx_buf,
		.tx_buf = tx_buf,
		.len = 4,
		.cs_change = 1,
		.bits_per_word = 8,
		.speed_hz = SPI_07M,
	};

	if (IS_ERR_OR_NULL(spi))
		return -1;
	//spi->mode = SPI_MODE_3 | SPI_LSB_FIRST;
	spi->mode = SPI_MODE_0;
	memset(rx_buf, 0, sizeof(rx_buf));
	tx_buf[0] = 0xff;
	tx_buf[1] = addrL;
	tx_buf[2] = addrH;
	tx_buf[3] = 0x00;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	spi_sync(spi, &m);
	*data = rx_buf[3];

	return 0;
}

EXPORT_SYMBOL(AX_OSAL_DEV_spi_dev_read);

AX_S32 AX_OSAL_DEV_spi_dev_write(u32 spi_bus, u32 spi_csn, u8 devid, u8 addrL, u8 addrH, u8 data, u32 len)
{
	struct spi_device *spi = ax_spi[spi_bus][spi_csn];
	unsigned char tx_buf[64];
	struct spi_message m;
	struct spi_transfer t = {
		.tx_buf = tx_buf,
		.len = 4,
		.speed_hz = SPI_10M,
	};

	if (IS_ERR_OR_NULL(spi))
		return -1;

	if (spi_bus >= SPI_MAX_NUM) {
		printk("spi_bus: %d,spi bus num can't be larger than %d\n",spi_bus, SPI_MAX_NUM);
		return -1;
	}

	tx_buf[0] = devid;
	tx_buf[1] = addrL;
	tx_buf[2] = addrH;
	tx_buf[3] = data;

#if 0
	if ((0 == spi_bus) && (0 != spi_csn)) {
		printk("spi_bus: %d,spi_csn must be 0\n", spi_bus);
		return -1;
	}
#endif

	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	return spi_sync(spi, &m);
}

EXPORT_SYMBOL(AX_OSAL_DEV_spi_dev_write);

AX_S32 AX_OSAL_DEV_spi_dev_init(void)
{
	int chip_select;
	struct spi_master *master;
	char spi_name[128];
	int bus_num = 0;
	struct device *dev;

	for (bus_num = 0; bus_num < SPI_MAX_NUM; bus_num++) {
		master = spi_busnum_to_master(bus_num);
		if (master) {
			master_bus[bus_num] = master;
			for (chip_select = 0;
			     chip_select < master->num_chipselect;
			     chip_select++) {
				memset(spi_name, 0, sizeof(spi_name));
				snprintf(spi_name, sizeof(spi_name), "%s.%d",dev_name(&master->dev), chip_select);
				dev = bus_find_device_by_name(&spi_bus_type, NULL,spi_name);
				if (dev == NULL) {
					ax_spi[bus_num][chip_select] = NULL;
					printk("chipselect %d has not been used\n",
					     chip_select);
					continue;
				}

				ax_spi[bus_num][chip_select] = to_spi_device(dev);
				if (ax_spi[bus_num][chip_select] == NULL) {
					dev_err(dev,"to_spi_device() error!\n");
					return -ENXIO;
				}
				printk("already register bus_num %d chip_select %d\n",bus_num, chip_select);
			}
		} else {
			printk("spi busnum%d to_master warn,master no found\n", bus_num);
			master_bus[bus_num] = 0;
		}
	}
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_DEV_spi_dev_init);
#endif
