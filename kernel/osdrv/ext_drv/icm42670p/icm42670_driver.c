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

#include "imu/inv_imu_apex.h"

#include "ax_vin_kernel_ext.h"

extern AX_U64 ax_sys_get_current_us(void);

#if 0
	#define ICM42670_SELF_TEST
	#define ICM42670_DEBUG
#endif

#ifdef ICM42670_DEBUG
#define ICM_DEBUG(fmt, args...) \
	do {	\
		pr_info("[%s:%d] " fmt, __func__, __LINE__, ##args); \
	} while (0)
#else
#define ICM_DEBUG(fmt, args...)
#endif

#define ICM_INFO(fmt, args...) \
	do {	\
		pr_info("[%s:%d] " fmt, __func__, __LINE__, ##args); \
	} while (0)

#define ICM_ERROR(fmt, args...) \
	do {	\
		pr_err("[%s:%d] " fmt, __func__, __LINE__, ##args); \
	} while (0)

struct icm42670_device {
	u16 accel_freq;
	u16 accel_fsr_g;
	u16 gyro_freq;
	u16 gyro_fsr_dps;

	bool is_high_resolution;

	struct inv_imu_device icm_dev;

	struct regmap *map;

	void *data;

#ifdef ICM42670_SELF_TEST
	struct task_struct *task;
#endif
};

struct icm42670_device *g_icm42670_dev;

static void icm42670_event_callback(inv_imu_sensor_event_t *event)
{
	int index, max_pkg_cnt;
	struct icm42670_device *icm42670_dev = g_icm42670_dev;
	ax_msensor_fifo_data_t *pdata = (ax_msensor_fifo_data_t *)icm42670_dev->data;

	if ((event->sensor_mask & (1 << INV_SENSOR_ACCEL)) &&
	    (event->sensor_mask & (1 << INV_SENSOR_GYRO))) {
		index = pdata->pkg_cnt;
		max_pkg_cnt = sizeof(pdata->data.acc_data) / sizeof(pdata->data.acc_data[0]);
		if (index >= max_pkg_cnt) {
			ICM_ERROR("packet overflow, index:%d, max_pkg_cnt:%d\n", index, max_pkg_cnt);
			return;
		}

		if (!icm42670_dev->is_high_resolution) {
			memset(event->accel_high_res, 0, sizeof(event->accel_high_res));
			memset(event->gyro_high_res, 0, sizeof(event->gyro_high_res));
		}

		ICM_DEBUG("index:%d AccelX:[%d,%d], AccelY:[%d,%d], AccelZ:[%d,%d]\n", index,
		          event->accel[0], event->accel_high_res[0],
		          event->accel[1], event->accel_high_res[1],
		          event->accel[2], event->accel_high_res[2]);
		ICM_DEBUG("index:%d GyroX:[%d,%d], GyroY:[%d,%d], GyroZ:[%d,%d]\n", index,
		          event->gyro[0], event->gyro_high_res[0],
		          event->gyro[1], event->gyro_high_res[1],
		          event->gyro[2], event->gyro_high_res[2]);
		ICM_DEBUG("index:%d Temperature:%d\n", index, event->temperature);

		pdata->data.acc_data[index].x = ((uint32_t)(event->accel_high_res[0]) << 16 | (uint32_t)event->accel[0]);
		pdata->data.acc_data[index].y = ((uint32_t)(event->accel_high_res[1]) << 16 | (uint32_t)event->accel[1]);
		pdata->data.acc_data[index].z = ((uint32_t)(event->accel_high_res[2]) << 16 | (uint32_t)event->accel[2]);
		pdata->data.gyro_data[index].x = ((uint32_t)(event->gyro_high_res[0]) << 16 | (uint32_t)event->gyro[0]);
		pdata->data.gyro_data[index].y = ((uint32_t)(event->gyro_high_res[1]) << 16 | (uint32_t)event->gyro[1]);
		pdata->data.gyro_data[index].z = ((uint32_t)(event->gyro_high_res[2]) << 16 | (uint32_t)event->gyro[2]);

		pdata->data.temperature[index] = (uint32_t)event->temperature;
		pdata->data.timestamp[index] = (uint32_t)event->timestamp_fsync;
		pdata->pkg_cnt += 1;
	}
}

static int icm42670_capture_data(ax_msensor_fifo_data_t *pdata)
{
	int ret;
	struct icm42670_device *icm42670_dev = g_icm42670_dev;
	struct inv_imu_device *icm_dev;

	if (!icm42670_dev) {
		ICM_ERROR("no icm42670 dev\n");
		return -ENODEV;
	}

	pdata->pkg_cnt = 0;
	pdata->pts_us = ax_sys_get_current_us();

	icm42670_dev->data = pdata;

	icm_dev = &icm42670_dev->icm_dev;
	icm_dev->sensor_event_cb = icm42670_event_callback;

	ICM_DEBUG("capture data start\n");
	ret = inv_imu_get_data_from_fifo(icm_dev);
	if (ret == 0) {
		ICM_ERROR("no valid data\n");
		ret = -EAGAIN;
	} else if (ret < 0) {
		ICM_ERROR("get icm42670 data err(%d)\n", ret);
		ret = -EIO;
	} else {
		pdata->read_time = ax_sys_get_current_us() - pdata->pts_us;
		ICM_DEBUG("capture data end, packet count:%d, time-consume:%lldus\n", ret,
		          pdata->read_time);
		ret = 0;
	}

	icm42670_dev->data = NULL;

	return ret;
}

static AX_VIN_PYRO_CALLBACK_T capture_data_callback = {
	.pfn_vin_pyro_read_data = icm42670_capture_data,
};

static const struct regmap_config icm42670_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = 0x4FFF,
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
	int ret = 0;
	void __iomem *pad_addr = NULL;
	void __iomem *phy_addr = NULL;

	pad_addr = ioremap(PAD_ADDR, 0x50);
	if (!pad_addr) {
		ICM_ERROR("ioremap(pinmux) failed\n");
		ret = -ENOMEM;
		goto exit;
	}

	phy_addr = ioremap(SDIO_PHY_ADDR, 0x10);
	if (!phy_addr) {
		pr_err("ioremap(phy) failed\n");
		ret = -ENOMEM;
		goto exit;
	}

	ICM_DEBUG("enter\n");

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

exit:
	return ret;
}

static int icm42670_spi_read(struct inv_imu_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len)
{
	struct icm42670_device *icm42670_dev = serif->context;

	/* ICM_DEBUG("reg:%2x, len:%d\n", reg, len); */

	if (reg == (FIFO_DATA & 0xFF))
		return regmap_noinc_read(icm42670_dev->map, reg, buf, len);

	return regmap_bulk_read(icm42670_dev->map, reg, buf, len);
}

static int icm42670_spi_write(struct inv_imu_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len)
{
	struct icm42670_device *icm42670_dev = serif->context;

	/* ICM_DEBUG("reg:%2x, len:%d\n", reg, len); */

	return regmap_bulk_write(icm42670_dev->map, reg, buf, len);
}

static ACCEL_CONFIG0_FS_SEL_t accel_fsr_g_to_param(uint16_t accel_fsr_g)
{
	ACCEL_CONFIG0_FS_SEL_t ret = ACCEL_CONFIG0_FS_SEL_16g;

	switch (accel_fsr_g) {
	case 2:
		ret = ACCEL_CONFIG0_FS_SEL_2g;
		break;
	case 4:
		ret = ACCEL_CONFIG0_FS_SEL_4g;
		break;
	case 8:
		ret = ACCEL_CONFIG0_FS_SEL_8g;
		break;
	case 16:
		ret = ACCEL_CONFIG0_FS_SEL_16g;
		break;
	default:
		/* Unknown accel FSR. Set to default 16G */
		break;
	}

	return ret;
}

static GYRO_CONFIG0_FS_SEL_t gyro_fsr_dps_to_param(uint16_t gyro_fsr_dps)
{
	GYRO_CONFIG0_FS_SEL_t ret = GYRO_CONFIG0_FS_SEL_2000dps;

	switch (gyro_fsr_dps) {
	case 250:
		ret = GYRO_CONFIG0_FS_SEL_250dps;
		break;
	case 500:
		ret = GYRO_CONFIG0_FS_SEL_500dps;
		break;
	case 1000:
		ret = GYRO_CONFIG0_FS_SEL_1000dps;
		break;
	case 2000:
		ret = GYRO_CONFIG0_FS_SEL_2000dps;
		break;
	default:
		/* Unknown gyro FSR. Set to default 2000dps" */
		break;
	}

	return ret;
}

static ACCEL_CONFIG0_ODR_t accel_freq_to_param(uint16_t accel_freq_hz)
{
	ACCEL_CONFIG0_ODR_t ret = ACCEL_CONFIG0_ODR_100_HZ;

	switch (accel_freq_hz) {
	case 12:
		ret = ACCEL_CONFIG0_ODR_12_5_HZ;
		break;
	case 25:
		ret = ACCEL_CONFIG0_ODR_25_HZ;
		break;
	case 50:
		ret = ACCEL_CONFIG0_ODR_50_HZ;
		break;
	case 100:
		ret = ACCEL_CONFIG0_ODR_100_HZ;
		break;
	case 200:
		ret = ACCEL_CONFIG0_ODR_200_HZ;
		break;
	case 400:
		ret = ACCEL_CONFIG0_ODR_400_HZ;
		break;
	case 800:
		ret = ACCEL_CONFIG0_ODR_800_HZ;
		break;
	case 1600:
		ret = ACCEL_CONFIG0_ODR_1600_HZ;
		break;
	default:
		/* Unknown accel frequency. Set to default 100Hz */
		break;
	}

	return ret;
}

static GYRO_CONFIG0_ODR_t gyro_freq_to_param(uint16_t gyro_freq_hz)
{
	GYRO_CONFIG0_ODR_t ret = GYRO_CONFIG0_ODR_100_HZ;

	switch (gyro_freq_hz) {
	case 12:
		ret = GYRO_CONFIG0_ODR_12_5_HZ;
		break;
	case 25:
		ret = GYRO_CONFIG0_ODR_25_HZ;
		break;
	case 50:
		ret = GYRO_CONFIG0_ODR_50_HZ;
		break;
	case 100:
		ret = GYRO_CONFIG0_ODR_100_HZ;
		break;
	case 200:
		ret = GYRO_CONFIG0_ODR_200_HZ;
		break;
	case 400:
		ret = GYRO_CONFIG0_ODR_400_HZ;
		break;
	case 800:
		ret = GYRO_CONFIG0_ODR_800_HZ;
		break;
	case 1600:
		ret = GYRO_CONFIG0_ODR_1600_HZ;
		break;
	default:
		/* Unknown gyro ODR. Set to default 100Hz */
		break;
	}

	return ret;
}

static int icm42670_accel_enable(struct icm42670_device *icm42670_dev, uint16_t odr, uint16_t fsr)
{
	int ret = 0;

	ret |= inv_imu_set_accel_fsr(&icm42670_dev->icm_dev, accel_fsr_g_to_param(fsr));
	ret |= inv_imu_set_accel_frequency(&icm42670_dev->icm_dev, accel_freq_to_param(odr));
	ret |= inv_imu_enable_accel_low_noise_mode(&icm42670_dev->icm_dev);

	return ret;
}

static int icm42670_gyro_enable(struct icm42670_device *icm42670_dev, uint16_t odr, uint16_t fsr)
{
	int ret = 0;

	ret |= inv_imu_set_gyro_fsr(&icm42670_dev->icm_dev, gyro_fsr_dps_to_param(fsr));
	ret |= inv_imu_set_gyro_frequency(&icm42670_dev->icm_dev, gyro_freq_to_param(odr));
	ret |= inv_imu_enable_gyro_low_noise_mode(&icm42670_dev->icm_dev);

	return ret;
}


#ifdef ICM42670_SELF_TEST
static int icm42670_data_capture_thread(void *data)
{
	int ret = 0;
	ax_msensor_fifo_data_t *pdata;

	ICM_INFO("enter\n");

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		ICM_ERROR("kzalloc pdata failed\n");
		return -ENOMEM;
	}

	while (!kthread_should_stop()) {
		ret = icm42670_capture_data(pdata);
		if (ret)
			break;

		msleep(10);
	}

	if (pdata)
		kfree(pdata);

	ICM_INFO("exit\n");

	return ret;
}
#endif

static int icm42670_setup(struct icm42670_device *icm42670_dev)
{
	int ret;
	uint8_t who_am_i = 0;
	struct inv_imu_serif icm_serif = {0,};

	icm_serif.context   = (void *)icm42670_dev;
	icm_serif.max_read  = 2560; /* maximum number of bytes allowed per serial read */
	icm_serif.max_write = 2560; /* maximum number of bytes allowed per serial write */

	icm_serif.serif_type = UI_SPI4;
	icm_serif.read_reg  = icm42670_spi_read;
	icm_serif.write_reg = icm42670_spi_write;

	ret = inv_imu_init(&icm42670_dev->icm_dev, &icm_serif, NULL);
	if (ret) {
		ICM_ERROR("imu init failed, ret:%d\n", ret);
		goto err0;
	}

	/* Check WHOAMI */
	ret = inv_imu_get_who_am_i(&icm42670_dev->icm_dev, &who_am_i);
	if (ret) {
		ICM_ERROR("get who_am_i failed, ret:%d\n", ret);
		goto err0;
	}

	if (who_am_i != INV_IMU_WHOAMI) {
		ICM_ERROR("who_am_i:%x is not %x\n", who_am_i, INV_IMU_WHOAMI);
		ret = -ENODEV;
		goto err0;
	}

	if (!icm42670_dev->is_high_resolution)
		ret = inv_imu_disable_high_resolution_fifo(&icm42670_dev->icm_dev);
	else
		ret = inv_imu_enable_high_resolution_fifo(&icm42670_dev->icm_dev);
	if (ret) {
		ICM_ERROR("%s high resolution failed, ret:%d\n", icm42670_dev->is_high_resolution ? "enable" : "disable", ret);
		goto err0;
	}

	ret = icm42670_accel_enable(icm42670_dev, icm42670_dev->accel_freq, icm42670_dev->accel_fsr_g);
	if (ret) {
		ICM_ERROR("enable accel failed, ret:%d\n", ret);
		goto err0;
	}

	ret = icm42670_gyro_enable(icm42670_dev, icm42670_dev->gyro_freq, icm42670_dev->gyro_fsr_dps);
	if (ret) {
		ICM_ERROR("enable gyro failed, ret:%d\n", ret);
		goto err0;
	}

	usleep_range(100000, 150000);

	ICM_INFO("success!!\n");

	return 0;

err0:
	return ret;
}

static void icm42670_reset(struct icm42670_device *icm42670_dev)
{
	inv_imu_disable_accel(&icm42670_dev->icm_dev);
	inv_imu_disable_gyro(&icm42670_dev->icm_dev);
	inv_imu_device_reset(&icm42670_dev->icm_dev);

	ICM_INFO("success!!\n");
}

static void icm42670_conf_check(struct icm42670_device *icm42670_dev)
{
	switch (icm42670_dev->accel_freq) {
	case 12:
	case 25:
	case 50:
	case 100:
	case 200:
	case 400:
	case 800:
	case 1600:
		break;
	default:
		ICM_INFO("unknown accel_freq(%d) and set to default 100Hz\n", icm42670_dev->accel_freq);
		icm42670_dev->accel_freq = 100;
		break;
	}

	switch (icm42670_dev->accel_fsr_g) {
	case 2:
	case 4:
	case 8:
	case 16:
		break;
	default:
		ICM_INFO("unknown accel_fsr_g(%d) and set to default 16g\n", icm42670_dev->accel_fsr_g);
		icm42670_dev->accel_fsr_g = 16;
		break;
	}

	switch (icm42670_dev->gyro_freq) {
	case 12:
	case 25:
	case 50:
	case 100:
	case 200:
	case 400:
	case 800:
	case 1600:
		break;
	default:
		ICM_INFO("unknown gyro_freq(%d) and set to default 100Hz\n", icm42670_dev->gyro_freq);
		icm42670_dev->gyro_freq = 100;
		break;
	}

	switch (icm42670_dev->gyro_fsr_dps) {
	case 250:
	case 500:
	case 1000:
	case 2000:
		break;
	default:
		ICM_INFO("unknown gyro_fsr_dps(%d) and set to default 2000dps\n", icm42670_dev->gyro_fsr_dps);
		icm42670_dev->gyro_fsr_dps = 2000;
		break;
	}

}

static int icm42670_probe(struct spi_device *spi)
{
	int ret = 0;
	u16 values[2];
	struct icm42670_device *icm42670_dev;

	icm42670_dev = devm_kzalloc(&spi->dev, sizeof(*icm42670_dev), GFP_KERNEL);
	if (!icm42670_dev) {
		ICM_ERROR("devm kzalloc failed\n");
		ret = -ENOMEM;
		goto err0;
	}

	ret = spi0_m0_pad_map();
	if (ret)
		goto err0;

	spi->mode = SPI_MODE_3;
	ret = spi_setup(spi);
	if (ret) {
		ICM_ERROR("spi setup failed\n");
		goto err0;
	}

	icm42670_dev->map = devm_regmap_init_spi(spi, &icm42670_regmap_config);
	if (IS_ERR(icm42670_dev->map)) {
		ICM_ERROR("regmap init spi error\n");
		ret = PTR_ERR(icm42670_dev->map);
		goto err0;
	}

	/* Accel ODR = 100 Hz and Full Scale Range = 16G */
	icm42670_dev->accel_freq = 100;
	icm42670_dev->accel_fsr_g = 16;
	if (!of_property_read_u16_array(spi->dev.of_node, "accel-param", values, 2)) {
		icm42670_dev->accel_freq = values[0];
		icm42670_dev->accel_fsr_g = values[1];
	}

	/* Gyro ODR = 100 Hz and Full Scale Range = 2000 dps */
	icm42670_dev->gyro_freq = 100;
	icm42670_dev->gyro_fsr_dps = 2000;
	if (!of_property_read_u16_array(spi->dev.of_node, "gyro-param", values, 2)) {
		icm42670_dev->gyro_freq = values[0];
		icm42670_dev->gyro_fsr_dps = values[1];
	}

	icm42670_dev->is_high_resolution = of_property_read_bool(spi->dev.of_node, "high-resolution-enable");

	icm42670_conf_check(icm42670_dev);

	ICM_INFO("accel_freq:%dHz, accel_fsr_g:%dg, gyro_freq:%dHz, gyro_fsr_dps:%ddps, high_resolution:%s\n",
	         icm42670_dev->accel_freq, icm42670_dev->accel_fsr_g,
	         icm42670_dev->gyro_freq, icm42670_dev->gyro_fsr_dps,
	         icm42670_dev->is_high_resolution ? "y" : "n");

	ret = icm42670_setup(icm42670_dev);
	if (ret)
		goto err0;

	dev_set_drvdata(&spi->dev, icm42670_dev);

	g_icm42670_dev = icm42670_dev;

	CB_VIN_PRYO_RegisterCallback(&capture_data_callback);

#ifdef ICM42670_SELF_TEST
	icm42670_dev->task = kthread_run(icm42670_data_capture_thread, icm42670_dev, "kthread-imu");
	if (!icm42670_dev->task) {
		ICM_ERROR("run data capture thread failed\n");
		ret = -ECHILD;
		goto err0;
	}
#endif

	ICM_INFO("success!!\n");

	return 0;

err0:
	return ret;
}

static int icm42670_remove(struct spi_device *spi)
{
	struct icm42670_device *icm42670_dev = (struct icm42670_device *)dev_get_drvdata(&spi->dev);

#ifdef ICM42670_SELF_TEST
	kthread_stop(icm42670_dev->task);
#endif

	icm42670_reset(icm42670_dev);

	return 0;
}

static const struct of_device_id icm42670_of_matches[] = {
	{
		.compatible = "axera,icm42670_spi",
	},
	{}
};
MODULE_DEVICE_TABLE(of, icm42670_of_matches);

static struct spi_driver icm42670_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "axera-icm42670",
		.of_match_table = icm42670_of_matches,
	},

	.probe = icm42670_probe,
	.remove = icm42670_remove,
};

static int __init icm42670_init(void)
{
	return spi_register_driver(&icm42670_driver);
}

static void __exit icm42670_exit(void)
{
	spi_unregister_driver(&icm42670_driver);
}

module_init(icm42670_init);
module_exit(icm42670_exit);

MODULE_LICENSE("GPL");
