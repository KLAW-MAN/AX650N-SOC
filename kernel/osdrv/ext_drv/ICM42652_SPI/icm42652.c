/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/regulator/consumer.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/of_gpio.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/poll.h>
#include <asm/uaccess.h>

#include "icm42652.h"
#include "msensor.h"

#define ICM42652_CNT	1
#define ICM42652_NAME	"icm42652"

static const struct regmap_config icm42652_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x4FFF,
};

/* chip initial default configuration */
static const struct icm42652_conf icm42652_default_conf = {
	.gyro = {
		.mode = ICM42652_SENSOR_MODE_LOW_NOISE,
		.fs = ICM42652_GYRO_FS_2000DPS,
		.odr = ICM42652_ODR_1000HZ,
	},
	.accel = {
		.mode = ICM42652_SENSOR_MODE_LOW_NOISE,
		.fs = ICM42652_ACCEL_FS_16G,
		.odr = ICM42652_ODR_1000HZ,
	},
	.temp_en = false,
};

static DECLARE_WAIT_QUEUE_HEAD(data_waitq);
static DECLARE_WAIT_QUEUE_HEAD(read_waitq);
extern AX_U64 ax_sys_get_pts(void);
extern AX_U64 ax_sys_get_current_us(void);
static struct icm42652_state *g_icm42652_dev_filp;
static struct task_struct *imu_fifo_data_thr;

static int icm42652_setup(struct icm42652_state *st)
{
	unsigned int val = 0;
	int ret = 0;
	int retry = 3;
	ret = regmap_write(st->map, ICM42652_REG_PWR_MGMT0, 0);
	if (ret)
		return ret;
	mdelay(2);
	while (retry--) {
		regmap_read(st->map, ICM42652_REG_WHOAMI, &val);
		AX_DEBUG("invalid whoami %#02x expected %#02x (%s)\n",
                val, ICM42652_WHOAMI_ICM42652, ICM42652_NAME);
		if (val == ICM42652_WHOAMI_ICM42652) {
			break;
		}
		if ((val != ICM42652_WHOAMI_ICM42652) && (retry == 1)) {
			AX_DEBUG("ICM42652 ID error\r\n");
			return -ENODEV;
		}
	}

	/* reset to make sure previous state are not there */
	ret = regmap_write(st->map, ICM42652_REG_SIGNAL_PATH_RESET,
			ICM42652_DEVICE_CONFIG_ABORT_ADN_RESET);
	if (ret) {
		AX_DEBUG("write reset register error\n");
	}

	ret = regmap_write(st->map, ICM42652_REG_DEVICE_CONFIG, 0x1);
	if (ret) {
		AX_DEBUG("write reset register error\n");
	}
	/* soft reset need 1ms to be effective */
	msleep(1);

	/* Read INT register status after reset */
	ret = regmap_read(st->map, ICM42652_REG_INT_STATUS, &val);
	if (ret) {
		AX_DEBUG("Read INT register failed\n");
	}
	AX_DEBUG("INT register status:val=0x%x\n", val);
	if (!(val & ICM42652_INT_STATUS_RESET_DONE)) {
		AX_DEBUG("reset error, reset done bit not set\n");
	}

	return 0;
}

static int icm42652_reg_init(struct icm42652_state *st)
{
	unsigned int val;
	int ret;

	/* set slew rates for SPI */
	val = ICM42652_DRIVE_CONFIG_SPI(ICM42652_SLEW_RATE_INF_2NS);
	ret = regmap_update_bits(st->map, ICM42652_REG_DRIVE_CONFIG,
				ICM42652_DRIVE_CONFIG_SPI_MASK, val);
	if (ret)
		return ret;

	/* gyro/accel  register init */
	/* set PWR_MGMT0 register (gyro & accel sensor mode, temp enabled) */
	val = ICM42652_PWR_MGMT0_GYRO(icm42652_default_conf.gyro.mode) |
	      ICM42652_PWR_MGMT0_ACCEL(icm42652_default_conf.accel.mode);
	ret = regmap_write(st->map, ICM42652_REG_PWR_MGMT0, val);
	if (ret)
		return ret;

	//NOT INT
	ret = regmap_write(st->map, MPUREG_INT_SOURCE0, 0x0);
	if (ret)
		return ret;

	ret = regmap_write(st->map, MPUREG_INT_CONFIG, 0x0);
	if (ret)
		return ret;


	/* FIFO buffer init */
	/* stop on full mode */
	val = ICM42652_FIFO_CONFIG_STOP_ON_FULL;
	// val = ICM42652_FIFO_CONFIG_STREAM;
	ret = regmap_write(st->map, ICM42652_REG_FIFO_CONFIG, val);
	if (ret)
		return ret;

	/* FIFO count in bytes / FIFO count and sensor data in big endian */
	val = ICM42652_INTF_CONFIG0_FIFO_COUNT_ENDIAN |
	      ICM42652_INTF_CONFIG0_SENSOR_DATA_ENDIAN;
	ret = regmap_write(st->map, ICM42652_REG_INTF_CONFIG0, val);
	if (ret)
		return ret;

	/* set GYRO_CONFIG0 register (gyro fullscale & odr) */
	val = ICM42652_GYRO_CONFIG0_FS(icm42652_default_conf.gyro.fs) |
	      ICM42652_GYRO_CONFIG0_ODR(icm42652_default_conf.gyro.odr);
	ret = regmap_write(st->map, ICM42652_REG_GYRO_CONFIG0, val);
	if (ret)
		return ret;

	/* set ACCEL_CONFIG0 register (accel fullscale & odr) */
	val = ICM42652_ACCEL_CONFIG0_FS(icm42652_default_conf.accel.fs) |
	      ICM42652_ACCEL_CONFIG0_ODR(icm42652_default_conf.accel.odr);
	ret = regmap_write(st->map, ICM42652_REG_ACCEL_CONFIG0, val);
	if (ret)
		return ret;

	/* Enable FIFO EN bits. */
	val = ICM42652_FIFO_CONFIG1_GYRO_EN | ICM42652_FIFO_CONFIG1_ACCEL_EN | ICM42652_FIFO_CONFIG1_TEMP_EN | ICM42652_FIFO_CONFIG1_TMST_FSYNC_EN ;
	ret = regmap_write(st->map, ICM42652_REG_FIFO_CONFIG1, val);
	if (ret)
		return ret;

	//NOT INT
	ret = regmap_write(st->map, MPUREG_INT_SOURCE0, 0x0);
	if (ret)
		return ret;
	//FIFO will get flushed
	ret = regmap_write(st->map, MPUREG_SIGNAL_PATH_RESET, 0x2);
	if (ret)
		return ret;

	return 0;
}

static int icm42652_buffer_fifo_read(struct icm42652_state *st)
{
	int ret, value;
	// int i;
	size_t fifo_count;
	__be16 *be_fifo_count;

	be_fifo_count = (__be16 *)st->buffer;
	ret = regmap_bulk_read(st->map, ICM42652_REG_FIFO_COUNT,
			be_fifo_count, sizeof(*be_fifo_count));
	if (ret)
		return ret;

	fifo_count = be16_to_cpup(be_fifo_count);

	/* Ensure data ready status bit is set */
	ret = regmap_read(st->map,  MPUREG_INT_STATUS, &value);
	if (ret)
		return ret;

	if (fifo_count <= MAX_FIFO_COUNT) {
		st->fifo.pkg_cnt =  fifo_count / ICM42652_FIFO_PACKET_SIZE;
		st->fifo.pts_us = ax_sys_get_current_us();
		ret = regmap_noinc_read(st->map, ICM42652_REG_FIFO_DATA,
				st->fifo.data, fifo_count);
		st->fifo.read_time = ax_sys_get_current_us() - st->fifo.pts_us;
		// AX_DEBUG("pkg_cnt = %lld, pts_us = %lld, read_time = %lld, fifo_count:%d\n",
		// 	st->fifo.pkg_cnt, st->fifo.pts_us, st->fifo.read_time, fifo_count);

		if (ret) {
			printk("FIFO read failed ret = %d\n", ret);
			return ret;
		}
	} else {
            printk("Get IMU Data Exception\n ");
	}

	return 0;
}

static int icm42652_open(struct inode *inode, struct file *filp)
{
	filp->private_data = g_icm42652_dev_filp;
	return 0;
}

static ssize_t icm42652_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
	long ret = 0;
	size_t data_size = 0;
	struct icm42652_state *st = (struct icm42652_state *)filp->private_data;
	wait_event_interruptible(data_waitq, atomic_read(&st->read_ready));
	data_size = sizeof(st->fifo.pkg_cnt) +
		sizeof(st->fifo.pts_us) +
		sizeof(st->fifo.read_time) +
		sizeof(uint8_t)*st->fifo.pkg_cnt*ICM42652_FIFO_PACKET_SIZE;

	ret = copy_to_user(buf, &st->fifo, data_size);
	if (ret)
		ret = -EFAULT;

	atomic_inc(&st->data_ready);
	atomic_dec(&st->read_ready);
	wake_up_interruptible(&read_waitq);

	return ret;
}

static AX_S32 icm42652_save_data(ax_msensor_fifo_data_t *pdata, struct icm42652_state *st)
{
	int i = 0;
	pdata->pkg_cnt = st->fifo.pkg_cnt;
	pdata->pts_us = st->fifo.pts_us;
	pdata->read_time = st->fifo.read_time;

	for (i = 0; i < st->fifo.pkg_cnt; i++) {
		pdata->data.acc_data[i].x = ((AX_U32)(st->fifo.data[i].accel.xh) << 8 | (AX_U32)st->fifo.data[i].accel.xl);
		pdata->data.acc_data[i].y = ((AX_U32)(st->fifo.data[i].accel.yh) << 8 | (AX_U32)st->fifo.data[i].accel.yl);
		pdata->data.acc_data[i].z = ((AX_U32)(st->fifo.data[i].accel.zh) << 8 | (AX_U32)st->fifo.data[i].accel.zl);
		pdata->data.gyro_data[i].x = ((AX_U32)(st->fifo.data[i].gyro.xh) << 8 | (AX_U32)st->fifo.data[i].gyro.xl);
		pdata->data.gyro_data[i].y = ((AX_U32)(st->fifo.data[i].gyro.yh) << 8 | (AX_U32)st->fifo.data[i].gyro.yl);
		pdata->data.gyro_data[i].z = ((AX_U32)(st->fifo.data[i].gyro.zh) << 8 | (AX_U32)st->fifo.data[i].gyro.zl);

		pdata->data.temperature[i] = (AX_U32)st->fifo.data[i].temp;
		pdata->data.timestamp[i] = ((AX_U32)st->fifo.data[i].timestamph << 8 | (AX_U32)st->fifo.data[i].timestampl);
	}
	return 0;
}

AX_S32 icm42652_get_data(ax_msensor_fifo_data_t *pdata)
{
	long ret = 0;

	struct icm42652_state *st = g_icm42652_dev_filp;
	atomic_inc(&st->data_ready);
	wake_up_interruptible(&read_waitq);
	wait_event_interruptible(data_waitq, atomic_read(&st->read_ready));
	icm42652_save_data(pdata, st);
	atomic_dec(&st->read_ready);

	return ret;
}
EXPORT_SYMBOL(icm42652_get_data);

static int icm42652_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static __poll_t icm42652_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	struct icm42652_state *st = (struct icm42652_state *)filp->private_data;
	poll_wait(filp, &data_waitq, wait);

	if (!atomic_read(&st->data_ready))
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static const struct file_operations icm42652_ops = {
	.owner = THIS_MODULE,
	.open = icm42652_open,
	.read = icm42652_read,
	.release = icm42652_release,
	.poll = icm42652_poll,
};

static ssize_t icm42652_reg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct icm42652_state *st;
	struct icm42652_fifo_2sensors_packet *pack;
	size_t len;
	int ret, i;

	st = dev_get_drvdata(dev);

	ret = icm42652_buffer_fifo_read(st);
	if (ret) {
		AX_DEBUG("read fifo data error\n");
		return ret;
	}

	for (i = 0; i < (st->fifo.pkg_cnt); ) {
		pack = (struct icm42652_fifo_2sensors_packet *)&st->fifo.data[i];
		len = snprintf(buf, PAGE_SIZE,
				"\
				header = 0x%x\n \
				accel_x_adc = 0x%x, accel_y_adc = 0x%x, accel_z_adc = 0x%x,\n \
				gyro_x_adc = 0x%x, gyro_y_adc = 0x%x, gyro_z_adc = 0x%x,\n \
				temp_adc = 0x%x, timestamp1 = 0x%x\n",
				pack->header,
				pack->accel.xh << 8 | pack->accel.xl,
				pack->accel.yh << 8 | pack->accel.yl,
				pack->accel.zh << 8 | pack->accel.zl,
				pack->gyro.xh << 8 | pack->gyro.xl,
				pack->gyro.yh << 8 | pack->gyro.yl,
				pack->gyro.zh << 8 | pack->gyro.zl,
				pack->temp,
				pack->timestamph << 8 | pack->timestampl);

		printk("fifo group %d: \n %s\n", i / 16, buf);
		i += sizeof(*pack);
 	}

	return len;
}

static ssize_t icm42652_dump_reg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct icm42652_state *st;
	int value;
	size_t len = 0;
	int i;

	st = dev_get_drvdata(dev);
	printk("======== dump bank 0 reg ========\n");
	regmap_write(st->map, ICM42652_REG_BANK_SEL, 0);
	regmap_read(st->map, 0x11, &value);
	printk("0x11 = 0x%x\n", value);
	regmap_read(st->map, 0x13, &value);
	printk("0x13 = 0x%x\n", value);
	regmap_read(st->map, 0x14, &value);
	printk("0x14 = 0x%x\n", value);
	regmap_read(st->map, 0x16, &value);
	printk("0x16 = 0x%x\n", value);
	for (i = 0x1D; i < 0x6A; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}
	regmap_read(st->map, 0x6C, &value);
	printk("0x6C = 0x%x\n", value);
	regmap_read(st->map, 0x6D, &value);
	printk("0x6D = 0x%x\n", value);

	regmap_read(st->map, 0x70, &value);
	printk("0x6D = 0x%x\n", value);

	regmap_read(st->map, 0x75, &value);
	printk("0x75 = 0x%x\n", value);

	regmap_read(st->map, 0x76, &value);
	printk("0x76 = 0x%x\n", value);


	printk("======== dump bank 1 reg ========\n");
	regmap_write(st->map, ICM42652_REG_BANK_SEL, 1);
	regmap_read(st->map, 0x3, &value);
	printk("0x3 = 0x%x\n", value);
	for (i = 0xB; i < 0x14; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}
	for (i = 0x5F; i < 0x65; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}
	for (i = 0x7A; i < 0x7D; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}

	printk("======== dump bank 2 reg ========\n");
	regmap_write(st->map, ICM42652_REG_BANK_SEL, 2);
	for (i = 0x3; i < 0x6; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}
	for (i = 0x3B; i < 0x3F; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}
	printk("======== dump bank 3 reg ========\n");
	regmap_write(st->map, ICM42652_REG_BANK_SEL, 3);

	regmap_read(st->map, 0x6, &value);
	printk("0x%x = 0x%x\n", 0x6, value);
	regmap_read(st->map, 0xe, &value);
	printk("0x%x = 0x%x\n", 0xe, value);

	printk("======== dump bank 4 reg ========\n");
	regmap_read(st->map, 0x9, &value);
	printk("0x9 = 0x%x\n", value);
	regmap_write(st->map, ICM42652_REG_BANK_SEL, 4);
	for (i = 0x40; i < 0x52; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}
	for (i = 0x77; i < 0x80; i++) {
		regmap_read(st->map, i, &value);
		printk("0x%x = 0x%x\n", i, value);
	}

	regmap_write(st->map, ICM42652_REG_BANK_SEL, 0);
	return len;
}


static DEVICE_ATTR(icm42652_reg, 0664, icm42652_reg_show, NULL);
static DEVICE_ATTR(icm42652_dump_reg, 0664, icm42652_dump_reg_show, NULL);
static struct attribute *icm42652_attributes[] = {
	&dev_attr_icm42652_reg.attr,
	&dev_attr_icm42652_dump_reg.attr,
	NULL
};

static struct attribute_group icm42652_attribute_group = {
	.attrs = icm42652_attributes
};

static int imu_fifo_thr(void *data)
{
	struct icm42652_state *st = (struct icm42652_state *)data;
	while (!kthread_should_stop()) {
		wait_event_interruptible(read_waitq, atomic_read(&st->data_ready));
		icm42652_buffer_fifo_read(st);
		atomic_dec(&st->data_ready);
		atomic_inc(&st->read_ready);
		wake_up_interruptible(&data_waitq);
	}

	return 0;
}
static __init int thm_kthread_init(struct icm42652_state *st)
{
	imu_fifo_data_thr = kthread_run(imu_fifo_thr, st, "kthread-imu");
	if (!imu_fifo_data_thr) {
		printk("%s: kthread_run fail", __func__);
		return -ECHILD;
	}
	return 0;
}

static ax_msensor_callback_t icm42652_callback_ops = {
	.ax_msensor_read_data = icm42652_get_data,
};

#define PAD_ADDR         0x04251C00
#define PAD_SPI_M0_MOSI_OFFSET  (0xC)
#define PAD_GPIO3_A12_OFFSET    (0x30)
#define PAD_SPI_M0_MISO_OFFSET  (0x3C)
#define PAD_SPI_M0_CLK_OFFSET   (0x48)

#define SDIO_PHY_ADDR     0x28001300
#define PHY_CNFG_OFFSET          (0x0)     //32bits
#define PHY_CMDPAD_CNFG_OFFSET   (0x4)     //16bits
#define PHY_CLKPAD_CNFG_OFFSET   (0x8)     //16bits
#define PHY_RSTNPAD_CNFG_OFFSET  (0xC)

static int spi0_m0_pad_map(void)
{
	int ret;
	void __iomem *pad_addr = NULL;
	void __iomem *phy_addr = NULL;
	pad_addr = ioremap(PAD_ADDR, 0x50);
	if (!pad_addr) {
		pr_err("%s:ioremap(pinmux) failed\n", __func__);
		ret = -ENOMEM;
	}
	phy_addr = ioremap(SDIO_PHY_ADDR, 0x10);
	if (!phy_addr) {
		pr_err("%s:ioremap(pinmux) failed\n", __func__);
		ret = -ENOMEM;
	}
	/* set pin func */
	writel(0x00010084, pad_addr + PAD_SPI_M0_MOSI_OFFSET);
	writel(0x00060084, pad_addr + PAD_GPIO3_A12_OFFSET);
	writel(0x00010004, pad_addr + PAD_SPI_M0_MISO_OFFSET);
	writel(0x00010004, pad_addr + PAD_SPI_M0_CLK_OFFSET);

	/* set pad vol */
	writel(0x00cc0002, phy_addr + PHY_CNFG_OFFSET);
	writel(0xCD0000, phy_addr + PHY_CNFG_OFFSET);
	writel(0x04420442, phy_addr + PHY_CMDPAD_CNFG_OFFSET);
	writel(0x04420442, phy_addr + PHY_CLKPAD_CNFG_OFFSET);
	writel(0x0442, phy_addr + PHY_RSTNPAD_CNFG_OFFSET);
	writel(0xCD0003, phy_addr + PHY_CNFG_OFFSET);
	iounmap(pad_addr);
	iounmap(phy_addr);

	return 0;
}

static int icm42652_probe(struct spi_device *spi)
{
	int ret, rc;
	u32 value;
	struct regmap *regmap;
	struct icm42652_state *st;

	st = devm_kzalloc(&spi->dev, sizeof(*st), GFP_KERNEL);
	if (!st) {
		AX_DEBUG("devm kzalloc failed\n");
		return -ENOMEM;
	}
	dev_set_drvdata(&spi->dev, st);
	mutex_init(&st->lock);
	ret = spi0_m0_pad_map();
	if (ret) {
		AX_DEBUG("spi0_m0_pad_map failed\n");
		return -ENOMEM;
	}
	/* Device address */
	rc = of_property_read_u32(spi->dev.of_node, "reg", &value);
	if (rc) {
		dev_err(&spi->dev, "%pOF has no valid 'reg' property (%d)\n",
			spi->dev.of_node, rc);
		return rc;
	}
	// spi->chip_select = value;
	printk("icm42652_probe:  spi->chip_select:%d\n", spi->chip_select);
	spi->mode = SPI_MODE_3;
	spi_setup(spi);

	regmap = devm_regmap_init_spi(spi, &icm42652_regmap_config);
	if (IS_ERR(regmap)) {
		AX_DEBUG("regmap init spi error\n");
		return PTR_ERR(regmap);
	}
	st->map = regmap;

	/* check chip id, soft reset chip*/
	ret = icm42652_setup(st);
	if (ret) {
		AX_DEBUG("failed to setup icm42652 %d\n", ret);
		return ret;
	}

	ret = icm42652_reg_init(st);
	if (ret) {
		AX_DEBUG("icm42652 reg init error\n");
	}

	/* create character devices for application access */
	g_icm42652_dev_filp = st;
	if (st->major) {
		st->devid = MKDEV(st->major, 0);
		register_chrdev_region(st->devid, ICM42652_CNT, ICM42652_NAME);
	} else {
		alloc_chrdev_region(&st->devid, 0, ICM42652_CNT, ICM42652_NAME);
		st->major = MAJOR(st->devid);
	}

	cdev_init(&st->cdev, &icm42652_ops);
	cdev_add(&st->cdev, st->devid, ICM42652_CNT);

	st->class = class_create(THIS_MODULE, ICM42652_NAME);
	if (IS_ERR(st->class)) {
		return PTR_ERR(st->class);
	}

	st->device = device_create(st->class, NULL, st->devid, NULL, ICM42652_NAME);
	if (IS_ERR(st->device)) {
		return PTR_ERR(st->device);
	}

	ret = sysfs_create_group(&spi->dev.kobj, &icm42652_attribute_group);
	if (ret) {
		AX_DEBUG("icm42652_create_sysfs error ret = %d\n", ret);
	}

	/* FIFO data read thread init */
	mdelay(5);
	atomic_set(&st->data_ready, 0);
	atomic_set(&st->read_ready, 0);
	thm_kthread_init(st);

	ax_msensor_register_callback(AX_MSENSOR_ICM42652, &icm42652_callback_ops);
	return 0;
}

static int icm42652_remove(struct spi_device *spi)
{
	if (imu_fifo_data_thr) {
        atomic_inc(&g_icm42652_dev_filp->data_ready);
        wake_up_interruptible(&read_waitq);
		kthread_stop(imu_fifo_data_thr);
		imu_fifo_data_thr = NULL;
	}

	sysfs_remove_group(&spi->dev.kobj, &icm42652_attribute_group);
	device_destroy(g_icm42652_dev_filp->class, g_icm42652_dev_filp->devid);
	class_destroy(g_icm42652_dev_filp->class);
	cdev_del(&g_icm42652_dev_filp->cdev);
	unregister_chrdev_region(g_icm42652_dev_filp->devid, ICM42652_CNT);

	return 0;
}

static const struct of_device_id icm42652_of_matches[] = {
	{
		.compatible = "axera,icm42652_spi",
	},
	{}
};
MODULE_DEVICE_TABLE(of, icm42652_of_matches);

static struct spi_driver icm42652_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "axera-icm42652",
		.of_match_table = icm42652_of_matches,
	},
	.probe = icm42652_probe,
	.remove = icm42652_remove,
};

static int __init icm42652_init(void)
{
	AX_DEBUG("enter icm42652_init\r\n");
	return spi_register_driver(&icm42652_driver);
}

static void __exit icm42652_exit(void)
{
	spi_unregister_driver(&icm42652_driver);
}

module_init(icm42652_init);
module_exit(icm42652_exit);

MODULE_AUTHOR("ningtiantong@axera-tech.com");
MODULE_LICENSE("GPL");
