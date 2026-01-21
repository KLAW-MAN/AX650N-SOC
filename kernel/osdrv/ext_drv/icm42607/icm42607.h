/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef ICM42607_H_
#define ICM42607_H_

#include <linux/bits.h>
#include <linux/bitfield.h>
#include <linux/regmap.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <linux/pm.h>

#define ICM42607_DEBUG		1
#if (defined (ICM42607_DEBUG)  &&  (ICM42607_DEBUG != 0))
#define AX_DEBUG(fmt, args...) \
	do {	\
		pr_err("%s(%d): " fmt, __func__, __LINE__, ##args); \
	} while (0)
#else
#define AX_DEBUG(fmt, ...) \
	do {	\
	} while (0)
#endif

#define MAX_FIFO_COUNT              1056
typedef unsigned long long int      AX_U64;

/* serial bus slew rates */
enum icm42607_slew_rate {
	ICM42607_SLEW_RATE_20_60NS,
	ICM42607_SLEW_RATE_12_36NS,
	ICM42607_SLEW_RATE_6_18NS,
	ICM42607_SLEW_RATE_4_12NS,
	ICM42607_SLEW_RATE_2_6NS,
	ICM42607_SLEW_RATE_INF_2NS,
};

enum icm42607_sensor_mode {
	ICM42607_SENSOR_MODE_OFF,
	ICM42607_SENSOR_MODE_STANDBY,
	ICM42607_SENSOR_MODE_LOW_POWER,
	ICM42607_SENSOR_MODE_LOW_NOISE,
	ICM42607_SENSOR_MODE_NB,
};

/* gyroscope fullscale values */
enum icm42607_gyro_fs {
	ICM42607_GYRO_FS_2000DPS,
	ICM42607_GYRO_FS_1000DPS,
	ICM42607_GYRO_FS_500DPS,
	ICM42607_GYRO_FS_250DPS,
	ICM42607_GYRO_FS_NB,
};

/* accelerometer fullscale values */
enum icm42607_accel_fs {
	ICM42607_ACCEL_FS_16G,
	ICM42607_ACCEL_FS_8G,
	ICM42607_ACCEL_FS_4G,
	ICM42607_ACCEL_FS_2G,
	ICM42607_ACCEL_FS_NB,
};

/* ODR suffixed by LN or LP are Low-Noise or Low-Power mode only */
enum icm42607_odr {
	ICM42607_ODR_1600HZ = 5,
	ICM42607_ODR_800HZ,
	ICM42607_ODR_400HZ,
	ICM42607_ODR_200HZ,
	ICM42607_ODR_100HZ,
	ICM42607_ODR_50HZ,
	ICM42607_ODR_25HZ,
	ICM42607_ODR_12_5HZ,
	ICM42607_ODR_NB,
};

struct icm42607_sensor_conf {
	int mode;
	int fs;
	int odr;
	int filter;
};

struct icm42607_conf {
	struct icm42607_sensor_conf gyro;
	struct icm42607_sensor_conf accel;
	bool temp_en;
};

struct icm42607_fifo {
	AX_U64 pkg_cnt;
	AX_U64 pts_us;
	AX_U64 read_time;
	uint8_t data[MAX_FIFO_COUNT]; // ____cacheline_aligned;
};

struct icm42607_state {
	dev_t devid;
	struct mutex lock;
	atomic_t read_ready;
	atomic_t data_ready;
	const char *name;
	struct regmap *map;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	int major;
	int cs_gpio;
	uint8_t buffer[2] ____cacheline_aligned;
	struct icm42607_fifo fifo;
};

struct icm42607_fifo_sensor_data {
	uint8_t xh;
	uint8_t xl;
	uint8_t yh;
	uint8_t yl;
	uint8_t zh;
	uint8_t zl;
} __packed;

/* FIFO data packet */
struct icm42607_fifo_2sensors_packet {
	uint8_t header;
	struct icm42607_fifo_sensor_data accel;
	struct icm42607_fifo_sensor_data gyro;
	uint8_t temp;
	uint8_t timestamph;
	uint8_t timestampl;
} __packed;
#define ICM42607_FIFO_PACKET_SIZE		16

/* Virtual register addresses: @bank on MSB (4 upper bits), @address on LSB */

/* Bank selection register, available in all banks */

#define ICM42607_REG_BLK_SEL_W                   0x79
#define ICM42607_REG_MADDR_W                     0x7A
#define ICM42607_REG_M_W                         0x7B
#define ICM42607_REG_BLK_SEL_R                   0x7C
#define ICM42607_REG_MADDR_R                     0x7D
#define ICM42607_REG_M_R                         0x7E
#define ICM42607_REG_BANK_SEL                    0x76
#define ICM42607_BANK_SEL_MASK                   GENMASK(2, 0)
#define ICM42607_REG_BANK_MASK                   GENMASK(15, 8)
#define ICM42607_REG_ADDRESS_MASK                GENMASK(7, 0)

/* User bank 0 (MSB 0x00) */
#define ICM42607_REG_WHOAMI                      0x75
#define ICM42607_WHOAMI_ICM42607                 0x60

#define ICM42607_REG_DEVICE_CONFIG               0x0001

#define ICM42607_REG_DRIVE_CONFIG                0x0005
#define ICM42607_DRIVE_CONFIG_I2C_MASK           GENMASK(5, 3)
#define ICM42607_DRIVE_CONFIG_I2C(_rate)         \
		FIELD_PREP(ICM42607_DRIVE_CONFIG_I2C_MASK, (_rate))
#define ICM42607_DRIVE_CONFIG_SPI_MASK           GENMASK(2, 0)
#define ICM42607_DRIVE_CONFIG_SPI(_rate)         \
		FIELD_PREP(ICM42607_DRIVE_CONFIG_SPI_MASK, (_rate))

#define ICM42607_REG_INT_CONFIG                  0x0006
#define ICM42607_INT_CONFIG_INT2_LATCHED         BIT(5)
#define ICM42607_INT_CONFIG_INT2_PUSH_PULL       BIT(4)
#define ICM42607_INT_CONFIG_INT2_ACTIVE_HIGH     BIT(3)
#define ICM42607_INT_CONFIG_INT2_ACTIVE_LOW      0x00
#define ICM42607_INT_CONFIG_INT1_LATCHED         BIT(2)
#define ICM42607_INT_CONFIG_INT1_PUSH_PULL       BIT(1)
#define ICM42607_INT_CONFIG_INT1_ACTIVE_HIGH     BIT(0)
#define ICM42607_INT_CONFIG_INT1_ACTIVE_LOW      0x00

#define ICM42607_REG_FIFO_CONFIG1                0x0028
#define ICM42607_FIFO_CONFIG_MASK                GENMASK(1, 0)
#define ICM42607_FIFO_CONFIG_STREAM              \
		FIELD_PREP(ICM42607_FIFO_CONFIG_MASK, 0)
#define ICM42607_FIFO_CONFIG_BYPASS              \
		FIELD_PREP(ICM42607_FIFO_CONFIG_MASK, 1)
#define ICM42607_FIFO_CONFIG_STOP_ON_FULL        \
		FIELD_PREP(ICM42607_FIFO_CONFIG_MASK, 2)

/* all sensor data are 16 bits (2 registers wide) in big-endian */
#define ICM42607_REG_TEMP_DATA                   0x0009
#define ICM42607_REG_ACCEL_DATA_X                0x000B
#define ICM42607_REG_ACCEL_DATA_Y                0x000D
#define ICM42607_REG_ACCEL_DATA_Z                0x000F
#define ICM42607_REG_GYRO_DATA_X                 0x0011
#define ICM42607_REG_GYRO_DATA_Y                 0x0013
#define ICM42607_REG_GYRO_DATA_Z                 0x0015
#define ICM42607_DATA_INVALID                    -32768

#define ICM42607_REG_INT_STATUS                  0x003A
#define ICM42607_INT_STATUS_UI_FSYNC             BIT(6)
#define ICM42607_INT_STATUS_PLL_RDY              BIT(5)
#define ICM42607_INT_STATUS_RESET_DONE           BIT(4)
//#define ICM42607_INT_STATUS_DATA_RDY           BIT(3)
#define ICM42607_INT_STATUS_FIFO_THS             BIT(2)
#define ICM42607_INT_STATUS_FIFO_FULL            BIT(1)
#define ICM42607_INT_STATUS_AGC_RDY              BIT(0)

/*
 * FIFO access registers
 * FIFO count is 16 bits (2 registers) big-endian
 * FIFO data is a continuous read register to read FIFO content
 */
#define ICM42607_REG_FIFO_COUNT                  0x003D
#define ICM42607_REG_FIFO_DATA                   0x003F

#define ICM42607_REG_SIGNAL_PATH_RESET           0x0002
#define ICM42607_DEVICE_CONFIG_SOFT_RESET        BIT(4)
#define ICM42607_SIGNAL_PATH_RESET_FIFO_FLUSH    BIT(2)

/* default configuration: all data big-endian and fifo count in bytes */
#define ICM42607_REG_INTF_CONFIG0                0x0035
#define ICM42607_INTF_CONFIG0_FIFO_COUNT_REC     BIT(6)
#define ICM42607_INTF_CONFIG0_FIFO_COUNT_ENDIAN  BIT(5)
#define ICM42607_INTF_CONFIG0_SENSOR_DATA_ENDIAN BIT(4)

#define ICM42607_REG_INTF_CONFIG1                0x0036

#define ICM42607_REG_PWR_MGMT0                   0x001F
#define ICM42607_PWR_MGMT0_IDLE                  BIT(4)
#define ICM42607_PWR_MGMT0_GYRO(_mode)           \
		FIELD_PREP(GENMASK(3, 2), (_mode))
#define ICM42607_PWR_MGMT0_ACCEL(_mode)          \
		FIELD_PREP(GENMASK(1, 0), (_mode))

#define ICM42607_REG_GYRO_CONFIG0                0x0020
#define ICM42607_GYRO_CONFIG0_FS(_fs)            \
		FIELD_PREP(GENMASK(6, 5), (_fs))
#define ICM42607_GYRO_CONFIG0_ODR(_odr)          \
		FIELD_PREP(GENMASK(3, 0), (_odr))

#define ICM42607_REG_ACCEL_CONFIG0               0x0021
#define ICM42607_ACCEL_CONFIG0_FS(_fs)           \
		FIELD_PREP(GENMASK(6, 5), (_fs))
#define ICM42607_ACCEL_CONFIG0_ODR(_odr)         \
		FIELD_PREP(GENMASK(3, 0), (_odr))

#define ICM42607_REG_GYRO_CONFIG1                0x0023
#define ICM42607_GYRO_CONFIG1_GYRO_FILT(_f)      \
		FIELD_PREP(GENMASK(2, 0), (_f))

#define ICM42607_REG_ACCEL_CONFIG1               0x0024
#define ICM42607_ACCEL_CONFIG1_ACCEL_FILT(_f)    \
		FIELD_PREP(GENMASK(2, 0), (_f))

/* FIFO watermark is 16 bits (2 registers wide) in little-endian */
#define ICM42607_REG_FIFO_WATERMARK              0x0029
#define ICM42607_FIFO_WATERMARK_VAL(_wm)         \
		cpu_to_le16((_wm) & GENMASK(11, 0))
/* FIFO is 2048 bytes, let 12 samples for reading latency */
#define ICM42607_FIFO_WATERMARK_MAX	             (2048 - 12 * 16)

#define ICM42607_REG_INT_SOURCE0                 0x002B
#define ICM42607_INT_SOURCE0_FSYNC_INT1_EN       BIT(6)
#define ICM42607_INT_SOURCE0_PLL_RDY_INT1_EN     BIT(5)
#define ICM42607_INT_SOURCE0_RESET_DONE_INT1_EN  BIT(4)
#define ICM42607_INT_SOURCE0_DRDY_INT1_EN        BIT(3)
#define ICM42607_INT_SOURCE0_FIFO_THS_INT1_EN    BIT(2)
#define ICM42607_INT_SOURCE0_FIFO_FULL_INT1_EN   BIT(1)
#define ICM42607_INT_SOURCE0_AGC_RDY_INT1_EN     BIT(0)


/* User bank 1 (MSB 0x10) */
#define ICM42607_REG_TMST_CONFIG1                0x00
#define ICM42607_TMST_CONFIG1_MASK               GENMASK(4, 0)
#define ICM42607_TMST_CONFIG1_TMST_TO_REGS_EN    BIT(4)
#define ICM42607_TMST_CONFIG1_TMST_RES_16US      BIT(3)
#define ICM42607_TMST_CONFIG1_TMST_DELTA_EN      BIT(2)
#define ICM42607_TMST_CONFIG1_TMST_FSYNC_EN      BIT(1)
#define ICM42607_TMST_CONFIG1_TMST_EN            BIT(0)

#define ICM42607_REG_FIFO_CONFIG5                0x01
#define ICM42607_FIFO_CONFIG5_WM_GT_TH           BIT(5)
#define ICM42607_FIFO_CONFIG5_RESUME_PARTIAL_RD  BIT(4)
#define ICM42607_FIFO_CONFIG5_TMST_FSYNC_EN      BIT(2)
#define ICM42607_FIFO_CONFIG5_GYRO_EN            BIT(1)
#define ICM42607_FIFO_CONFIG5_ACCEL_EN           BIT(0)

#define ICM42607_REG_INT_CONFIG1                 0x05
#define ICM42607_INT_CONFIG1_TPULSE_DURATION     BIT(6)
#define ICM42607_INT_CONFIG1_ASYNC_RESET         BIT(4)

#endif
