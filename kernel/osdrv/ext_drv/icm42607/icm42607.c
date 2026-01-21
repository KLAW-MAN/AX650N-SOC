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

#include "icm42607.h"

#define ICM42607_CNT	1
#define ICM42607_NAME	"icm42607"

static const struct regmap_config icm42607_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x4FFF,
};

/* chip initial default configuration */
static const struct icm42607_conf icm42607_default_conf = {
	.gyro = {
		.mode = ICM42607_SENSOR_MODE_LOW_NOISE,
		.fs = ICM42607_GYRO_FS_2000DPS,
		.odr = ICM42607_ODR_800HZ,
	},
	.accel = {
		.mode = ICM42607_SENSOR_MODE_LOW_NOISE,
		.fs = ICM42607_ACCEL_FS_16G,
		.odr = ICM42607_ODR_800HZ,
	},
	.temp_en = false,
};

static DECLARE_WAIT_QUEUE_HEAD(data_waitq);
static DECLARE_WAIT_QUEUE_HEAD(read_waitq);
extern AX_U64 ax_sys_get_pts(void);
extern AX_U64 ax_sys_get_current_us(void);
static struct icm42607_state *icm42607_dev_filp;
static struct task_struct *imu_fifo_data_thr;

static int icm42607_read_mreg(struct icm42607_state *st, unsigned int reg,
				unsigned int bank, unsigned int *buf)
{
	int ret;

	ret = regmap_write(st->map, ICM42607_REG_BLK_SEL_R, bank);
	if (ret) {
		return ret;
	}

	ret = regmap_write(st->map, ICM42607_REG_MADDR_R, reg);
	if (ret) {
		return ret;
	}

	regmap_read(st->map, ICM42607_REG_M_R, buf);
	if (ret) {
		return ret;
	}

	return 0;
}

static int icm42607_write_mreg(struct icm42607_state *st, unsigned int reg,
				unsigned int bank, unsigned int buf)
{
	int ret;

	ret = regmap_write(st->map, ICM42607_REG_BLK_SEL_W, bank);
	if (ret) {
		return ret;
	}

	ret = regmap_write(st->map, ICM42607_REG_MADDR_W, reg);
	if (ret) {
		return ret;
	}

	ret = regmap_write(st->map, ICM42607_REG_M_W, buf);
	if (ret) {
		return ret;
	}

	return 0;
}

static int icm42607_single_merg_read(struct icm42607_state *st, uint16_t reg,
				unsigned int *buf, size_t len)
{
	int ret = 0;
	unsigned int bank = FIELD_GET(ICM42607_REG_BANK_MASK, reg);
	unsigned int address = FIELD_GET(ICM42607_REG_ADDRESS_MASK, reg);

	ret = icm42607_read_mreg(st, address, bank, buf);
	if (ret)
        return ret;

	return 0;
}

static int icm42607_single_mreg_write(struct icm42607_state *st, uint16_t reg,
				unsigned int buf)
{
	int ret = 0;
	unsigned int bank = FIELD_GET(ICM42607_REG_BANK_MASK, reg);
	unsigned int address = FIELD_GET(ICM42607_REG_ADDRESS_MASK, reg);

	ret = icm42607_write_mreg(st, address, bank, buf);
	if (ret)
        return ret;

    return 0;
}

static int icm42607_setup(struct icm42607_state *st)
{
	unsigned int val = 0;
	int ret = 0;
	int retry = 3;

	while (retry--) {
		regmap_read(st->map, ICM42607_REG_WHOAMI, &val);
		AX_DEBUG("invalid whoami %#02x expected %#02x (%s)\n",
                val, ICM42607_WHOAMI_ICM42607, ICM42607_NAME);
		if (val == 0x60) {
			break;
		}
		if ((val != 0x60) && (retry == 1)) {
			AX_DEBUG("ICM42607 ID error\r\n");
			return -ENODEV;
		}
	}

	/* reset to make sure previous state are not there */
	ret = regmap_write(st->map, ICM42607_REG_SIGNAL_PATH_RESET,
			ICM42607_DEVICE_CONFIG_SOFT_RESET);
	if (ret) {
		AX_DEBUG("write reset register error\n");
	}

	/* soft reset need 1ms to be effective */
	msleep(1);

	/* Read INT register status after reset */
	ret = regmap_read(st->map, ICM42607_REG_INT_STATUS, &val);
	if (ret) {
		AX_DEBUG("Read INT register failed\n");
	}

	if (!(val & ICM42607_INT_STATUS_RESET_DONE)) {
		AX_DEBUG("reset error, reset done bit not set\n");
	}

	return 0;
}

static int icm42607_reg_init(struct icm42607_state *st)
{
	unsigned int val;
	int ret;

	/* gyro/accel  register init */
	/* set PWR_MGMT0 register (gyro & accel sensor mode, temp enabled) */
	val = ICM42607_PWR_MGMT0_GYRO(icm42607_default_conf.gyro.mode) |
	      ICM42607_PWR_MGMT0_ACCEL(icm42607_default_conf.accel.mode);
	ret = regmap_write(st->map, ICM42607_REG_PWR_MGMT0, val);
	if (ret)
		return ret;

	/* set GYRO_CONFIG0 register (gyro fullscale & odr) */
	val = ICM42607_GYRO_CONFIG0_FS(icm42607_default_conf.gyro.fs) |
	      ICM42607_GYRO_CONFIG0_ODR(icm42607_default_conf.gyro.odr);
	ret = regmap_write(st->map, ICM42607_REG_GYRO_CONFIG0, val);
	if (ret)
		return ret;

	/* set ACCEL_CONFIG0 register (accel fullscale & odr) */
	val = ICM42607_ACCEL_CONFIG0_FS(icm42607_default_conf.accel.fs) |
	      ICM42607_ACCEL_CONFIG0_ODR(icm42607_default_conf.accel.odr);
	ret = regmap_write(st->map, ICM42607_REG_ACCEL_CONFIG0, val);
	if (ret)
		return ret;

	/* FIFO buffer init */
	/* stop on full mode */
	val = ICM42607_FIFO_CONFIG_STOP_ON_FULL;
	ret = regmap_update_bits(st->map, ICM42607_REG_FIFO_CONFIG1,
			GENMASK(1, 0), val);
	if (ret)
		return ret;

	/* FIFO count in bytes / FIFO count and sensor data in big endian */
	val = ICM42607_INTF_CONFIG0_FIFO_COUNT_ENDIAN |
	      ICM42607_INTF_CONFIG0_SENSOR_DATA_ENDIAN;
	ret = regmap_update_bits(st->map, ICM42607_REG_INTF_CONFIG0,
			GENMASK(5, 4), val);
	if (ret)
		return ret;

	/* Enable FIFO EN bits. */
	val = ICM42607_FIFO_CONFIG5_GYRO_EN | ICM42607_FIFO_CONFIG5_ACCEL_EN;
	ret = icm42607_single_mreg_write(st, ICM42607_REG_FIFO_CONFIG5, val);
	if (ret)
		return ret;

	/* timestamp init*/
	val = ICM42607_TMST_CONFIG1_TMST_TO_REGS_EN | ICM42607_TMST_CONFIG1_TMST_EN;
	ret = icm42607_single_mreg_write(st, ICM42607_REG_TMST_CONFIG1, val);
	if (ret)
		return ret;

	return 0;
}

static int icm42607_buffer_fifo_read(struct icm42607_state *st)
{
	int ret;
	size_t fifo_count;
	__be16 *be_fifo_count;

	be_fifo_count = (__be16 *)st->buffer;
	ret = regmap_bulk_read(st->map, ICM42607_REG_FIFO_COUNT,
			be_fifo_count, sizeof(*be_fifo_count));
	if (ret)
		return ret;

	fifo_count = be16_to_cpup(be_fifo_count);
	if (fifo_count <= MAX_FIFO_COUNT) {
		st->fifo.pkg_cnt =  fifo_count / ICM42607_FIFO_PACKET_SIZE;
		st->fifo.pts_us = ax_sys_get_current_us();
		ret = regmap_noinc_read(st->map, ICM42607_REG_FIFO_DATA,
				st->fifo.data, fifo_count);
		st->fifo.read_time = ax_sys_get_current_us() - st->fifo.pts_us;
		// AX_DEBUG("pkg_cnt = %lld, pts_us = %lld, read_time = %lld\n",
		// 	st->fifo.pkg_cnt, st->fifo.pts_us, st->fifo.read_time);
		if (ret) {
			AX_DEBUG("FIFO read failed ret = %d\n", ret);
			return ret;
		}
	} else {
            printk("Get IMU Data Exception\n ");
	}

	return 0;
}

static int icm42607_open(struct inode *inode, struct file *filp)
{
	filp->private_data = icm42607_dev_filp;
	return 0;
}

static ssize_t icm42607_read(struct file *filp, char __user *buf, size_t cnt, loff_t *off)
{
	long ret = 0;
	size_t data_size = 0;
	struct icm42607_state *st = (struct icm42607_state *)filp->private_data;
	wait_event_interruptible(data_waitq, atomic_read(&st->read_ready));
	data_size = sizeof(st->fifo.pkg_cnt) +
		sizeof(st->fifo.pts_us) +
		sizeof(st->fifo.read_time) +
		sizeof(uint8_t)*st->fifo.pkg_cnt*ICM42607_FIFO_PACKET_SIZE;

	ret = copy_to_user(buf, &st->fifo, data_size);
	if (ret)
		ret = -EFAULT;

	atomic_inc(&st->data_ready);
	atomic_dec(&st->read_ready);
	wake_up_interruptible(&read_waitq);

	return ret;
}

static int icm42607_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static __poll_t icm42607_poll(struct file *filp, struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	struct icm42607_state *st = (struct icm42607_state *)filp->private_data;
	poll_wait(filp, &data_waitq, wait);

	if (!atomic_read(&st->data_ready))
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static const struct file_operations icm42607_ops = {
	.owner = THIS_MODULE,
	.open = icm42607_open,
	.read = icm42607_read,
	.release = icm42607_release,
	.poll = icm42607_poll,
};

static ssize_t icm42607_reg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct icm42607_state *st;
	struct icm42607_fifo_2sensors_packet *pack;
	size_t len;
	int ret, i;

	st = dev_get_drvdata(dev);

	ret = icm42607_buffer_fifo_read(st);
	if (ret) {
		AX_DEBUG("read fifo data error\n");
        return ret;
	}

	for (i = 0; i < st->fifo.pkg_cnt*ICM42607_FIFO_PACKET_SIZE;) {
        pack = (struct icm42607_fifo_2sensors_packet *)&st->fifo.data[i];
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

		AX_DEBUG("fifo group %d: \n %s\n", i / 16, buf);
		i += sizeof(*pack);
 	}

	return len;
}

static DEVICE_ATTR(icm42607_reg, 0664, icm42607_reg_show, NULL);
static struct attribute *icm42607_attributes[] = {
	&dev_attr_icm42607_reg.attr,
	NULL
};

static struct attribute_group icm42607_attribute_group = {
	.attrs = icm42607_attributes
};

static int imu_fifo_thr(void *data)
{
	struct icm42607_state *st = (struct icm42607_state *)data;
	while (!kthread_should_stop()) {
		wait_event_interruptible(read_waitq, atomic_read(&st->data_ready));
		icm42607_buffer_fifo_read(st);
		atomic_dec(&st->data_ready);
		atomic_inc(&st->read_ready);
		wake_up_interruptible(&data_waitq);
	}

	return 0;
}
static __init int thm_kthread_init(struct icm42607_state *st)
{
	imu_fifo_data_thr = kthread_run(imu_fifo_thr, st, "kthread-imu");
	if (!imu_fifo_data_thr) {
		printk("%s: kthread_run fail", __func__);
		return -ECHILD;
	}
	return 0;
}

static int icm42607_probe(struct i2c_client *i2c,
			const struct i2c_device_id *id)
{
	int ret;
	struct regmap *regmap;
	struct icm42607_state *st;

	st = devm_kzalloc(&i2c->dev, sizeof(*st), GFP_KERNEL);
	if (!st) {
		AX_DEBUG("devm kzalloc failed\n");
		return -ENOMEM;
	}
	dev_set_drvdata(&i2c->dev, st);
	mutex_init(&st->lock);

	regmap = devm_regmap_init_i2c(i2c, &icm42607_regmap_config);
	if (IS_ERR(regmap)) {
		AX_DEBUG("regmap init spi error\n");
		return PTR_ERR(regmap);
	}
	st->map = regmap;

	/* check chip id, soft reset chip*/
	ret = icm42607_setup(st);
	if (ret) {
		AX_DEBUG("failed to setup icm42607 %d\n", ret);
		return ret;
	}

	ret = icm42607_reg_init(st);
	if (ret) {
		AX_DEBUG("icm42607 reg init error\n");
	}

	/* create character devices for application access */
	icm42607_dev_filp = st;
	if (st->major) {
		st->devid = MKDEV(st->major, 0);
		register_chrdev_region(st->devid, ICM42607_CNT, ICM42607_NAME);
	} else {
		alloc_chrdev_region(&st->devid, 0, ICM42607_CNT, ICM42607_NAME);
		st->major = MAJOR(st->devid);
	}

	cdev_init(&st->cdev, &icm42607_ops);
	cdev_add(&st->cdev, st->devid, ICM42607_CNT);

	st->class = class_create(THIS_MODULE, ICM42607_NAME);
	if (IS_ERR(st->class)) {
		return PTR_ERR(st->class);
	}

	st->device = device_create(st->class, NULL, st->devid, NULL, ICM42607_NAME);
	if (IS_ERR(st->device)) {
		return PTR_ERR(st->device);
	}

	ret = sysfs_create_group(&i2c->dev.kobj, &icm42607_attribute_group);
	if (ret) {
		AX_DEBUG("icm42607_create_sysfs error ret = %d\n", ret);
	}

	/* FIFO data read thread init */
	mdelay(5);
	atomic_set(&st->data_ready, 1);
	atomic_set(&st->read_ready, 0);
	thm_kthread_init(st);

	return 0;
}

static int icm42607_remove(struct i2c_client *i2c)
{
	if (imu_fifo_data_thr) {
        atomic_inc(&icm42607_dev_filp->data_ready);
        wake_up_interruptible(&read_waitq);
		kthread_stop(imu_fifo_data_thr);
		imu_fifo_data_thr = NULL;
	}

	sysfs_remove_group(&i2c->dev.kobj, &icm42607_attribute_group);
	device_destroy(icm42607_dev_filp->class, icm42607_dev_filp->devid);
	class_destroy(icm42607_dev_filp->class);
	cdev_del(&icm42607_dev_filp->cdev);
	unregister_chrdev_region(icm42607_dev_filp->devid, ICM42607_CNT);

	return 0;
}

static const struct of_device_id icm42607_of_matches[] = {
    {
        .compatible = "axera,icm42607",
	},
	{}
};
MODULE_DEVICE_TABLE(of, icm42607_of_matches);

static struct i2c_driver icm42607_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "axera-icm42607",
		.of_match_table = icm42607_of_matches,
	},
	.probe = icm42607_probe,
	.remove = icm42607_remove,
};

static int __init icm42607_init(void)
{
	AX_DEBUG("enter icm42607_init\r\n");
	return i2c_add_driver(&icm42607_driver);
}

static void __exit icm42607_exit(void)
{
	i2c_del_driver(&icm42607_driver);
}

module_init(icm42607_init);
module_exit(icm42607_exit);

MODULE_AUTHOR("ningtiantong@axera-tech.com");
MODULE_LICENSE("GPL");
