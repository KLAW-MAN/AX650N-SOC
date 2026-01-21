/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef ICM42652_H_
#define ICM42652_H_

#include <linux/bits.h>
#include <linux/bitfield.h>
#include <linux/regmap.h>
#include <linux/mutex.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/regulator/consumer.h>
#include <linux/pm.h>

#define ICM42652_DEBUG		1
#if (defined (ICM42652_DEBUG)  &&  (ICM42652_DEBUG != 0))
#define AX_DEBUG(fmt, args...) \
	do {	\
		pr_err("%s(%d): " fmt, __func__, __LINE__, ##args); \
	} while (0)
#else
#define AX_DEBUG(fmt, ...) \
	do {	\
	} while (0)
#endif

#define MAX_FIFO_COUNT              2064
#define MAX_FIFO_PKG_COUNT          129
typedef unsigned long long int      AX_U64;

/* serial bus slew rates */
enum icm42652_slew_rate {
	ICM42652_SLEW_RATE_20_60NS,
	ICM42652_SLEW_RATE_12_36NS,
	ICM42652_SLEW_RATE_6_18NS,
	ICM42652_SLEW_RATE_4_12NS,
	ICM42652_SLEW_RATE_2_6NS,
	ICM42652_SLEW_RATE_INF_2NS,
};

enum icm42652_sensor_mode {
	ICM42652_SENSOR_MODE_OFF,
	ICM42652_SENSOR_MODE_STANDBY,
	ICM42652_SENSOR_MODE_LOW_POWER,
	ICM42652_SENSOR_MODE_LOW_NOISE,
	ICM42652_SENSOR_MODE_NB,
};

/* gyroscope fullscale values */
enum icm42652_gyro_fs {
	ICM42652_GYRO_FS_2000DPS,
	ICM42652_GYRO_FS_1000DPS,
	ICM42652_GYRO_FS_500DPS,
	ICM42652_GYRO_FS_250DPS,
	ICM42652_GYRO_FS_125DPS,
	ICM42652_GYRO_FS_62_5DPS,
	ICM42652_GYRO_FS_31_25DPS,
	ICM42652_GYRO_FS_12_625DPS,
};

/* accelerometer fullscale values */
enum icm42652_accel_fs {
	ICM42652_ACCEL_FS_16G,
	ICM42652_ACCEL_FS_8G,
	ICM42652_ACCEL_FS_4G,
	ICM42652_ACCEL_FS_2G,
	ICM42652_ACCEL_FS_NB,
};

/* ODR suffixed by LN or LP are Low-Noise or Low-Power mode only */
enum icm42652_odr {
	ICM42652_ODR_32000HZ = 1,
	ICM42652_ODR_16000HZ,
	ICM42652_ODR_8000HZ,
	ICM42652_ODR_4000HZ,
	ICM42652_ODR_2000HZ,
	ICM42652_ODR_1000HZ,
	ICM42652_ODR_200HZ,
	ICM42652_ODR_100HZ,
	ICM42652_ODR_50HZ,
	ICM42652_ODR_25HZ,
	ICM42652_ODR_12_5HZ,
	ICM42652_ODR_NB,
};

struct icm42652_sensor_conf {
	int mode;
	int fs;
	int odr;
	int filter;
};

struct icm42652_conf {
	struct icm42652_sensor_conf gyro;
	struct icm42652_sensor_conf accel;
	bool temp_en;
};

struct icm42652_fifo_sensor_data {
	uint8_t xh;
	uint8_t xl;
	uint8_t yh;
	uint8_t yl;
	uint8_t zh;
	uint8_t zl;
} __packed;

/* FIFO data packet */
struct icm42652_fifo_2sensors_packet {
	uint8_t header;
	struct icm42652_fifo_sensor_data accel;
	struct icm42652_fifo_sensor_data gyro;
	uint8_t temp;
	uint8_t timestamph;
	uint8_t timestampl;
} __packed;

struct icm42652_fifo {
	AX_U64 pkg_cnt;
	AX_U64 pts_us;
	AX_U64 read_time;
	struct icm42652_fifo_2sensors_packet data[MAX_FIFO_PKG_COUNT];
};

struct icm42652_state {
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
	struct icm42652_fifo fifo;
};


#define ICM42652_FIFO_PACKET_SIZE		16

/* cs pin bit[16:18] set 6 to switch gpio mode,default set 0x60094 */
#define CS_PIN_SEL 0x049BC248

/* Bank 0 */
#define MPUREG_DEVICE_CONFIG      0x11
#define MPUREG_CHIP_CONFIG        MPUREG_DEVICE_CONFIG // Retro-compatibility
#define MPUREG_DRIVE_CONFIG       0x13
#define MPUREG_INT_CONFIG         0x14
#define MPUREG_FIFO_CONFIG        0x16
#define MPUREG_TEMP_DATA1_UI      0x1D
#define MPUREG_ACCEL_DATA_X1_UI   0x1F
#define MPUREG_GYRO_DATA_X1_UI    0x25
#define MPUREG_TMST_FSYNCH        0x2B
#define MPUREG_TMST_FSYNCL        0x2C
#define MPUREG_INT_STATUS         0x2D
#define MPUREG_FIFO_COUNTH        0x2E
#define MPUREG_FIFO_BYTE_COUNT1   MPUREG_FIFO_COUNTH // Retro-compatibility
#define MPUREG_FIFO_COUNTL        0x2F
#define MPUREG_FIFO_BYTE_COUNT2   MPUREG_FIFO_COUNTL // Retro-compatibility
#define MPUREG_FIFO_DATA          0x30
#define MPUREG_APEX_DATA0         0x31
#define MPUREG_APEX_DATA1         0x32
#define MPUREG_APEX_DATA2         0x33
#define MPUREG_APEX_DATA3         0x34
#define MPUREG_APEX_DATA4         0x35
#define MPUREG_APEX_DATA5         0x36
#define MPUREG_INT_STATUS2        0x37
#define MPUREG_INT_STATUS3        0x38
#define MPUREG_SIGNAL_PATH_RESET  0x4B
#define MPUREG_INTF_CONFIG0       0x4C
#define MPUREG_INTF_CONFIG1       0x4D
#define MPUREG_PWR_MGMT_0         0x4E
#define MPUREG_GYRO_CONFIG0       0x4F
#define MPUREG_ACCEL_CONFIG0      0x50
#define MPUREG_GYRO_CONFIG1       0x51
#define MPUREG_GYRO_ACCEL_CONFIG0 0x52
#define MPUREG_ACCEL_CONFIG1      0x53
#define MPUREG_TMST_CONFIG        0x54
#define MPUREG_APEX_CONFIG0       0x56
#define MPUREG_SMD_CONFIG         0x57
#define MPUREG_FIFO_CONFIG1       0x5F
#define MPUREG_FIFO_CONFIG2       0x60
#define MPUREG_FIFO_CONFIG3       0x61
#define MPUREG_FSYNC_CONFIG       0x62
#define MPUREG_INT_CONFIG0        0x63
#define MPUREG_INT_CONFIG1        0x64
#define MPUREG_INT_SOURCE0        0x65
#define MPUREG_INT_SOURCE1        0x66
#define MPUREG_INT_SOURCE3        0x68
#define MPUREG_INT_SOURCE4        0x69
#define MPUREG_FIFO_LOST_PKT0     0x6C
#define MPUREG_FIFO_LOST_PKT1     0x6D
#define MPUREG_SELF_TEST_CONFIG   0x70
#define MPUREG_WHO_AM_I           0x75
#define MPUREG_REG_BANK_SEL       0x76

/* Bank 1 */
#define MPUREG_SENSOR_CONFIG           0x03
#define MPUREG_GYRO_CONFIG_STATIC2_B1  0x0B
#define MPUREG_GYRO_CONFIG_STATIC3_B1  0x0C
#define MPUREG_GYRO_CONFIG_STATIC4_B1  0x0D
#define MPUREG_GYRO_CONFIG_STATIC5_B1  0x0E
#define MPUREG_GYRO_CONFIG_STATIC6_B1  0x0F
#define MPUREG_GYRO_CONFIG_STATIC7_B1  0x10
#define MPUREG_GYRO_CONFIG_STATIC8_B1  0x11
#define MPUREG_GYRO_CONFIG_STATIC9_B1  0x12
#define MPUREG_GYRO_CONFIG_STATIC10_B1 0x13
#define MPUREG_XG_ST_DATA_B1           0x5F
#define MPUREG_YG_ST_DATA_B1           0x60
#define MPUREG_ZG_ST_DATA_B1           0x61
#define MPUREG_TMST_VAL0_B1            0x62
#define MPUREG_TMST_VAL1_B1            0x63
#define MPUREG_TMST_VAL2_B1            0x64
#define MPUREG_INTF_CONFIG4_B1         0x7A
#define MPUREG_INTF_CONFIG5_B1         0x7B
#define MPUREG_INTF_CONFIG6_B1         0x7C

/* Bank 2 */
#define MPUREG_ACCEL_CONFIG_STATIC2_B2 0x03
#define MPUREG_ACCEL_CONFIG_STATIC3_B2 0x04
#define MPUREG_ACCEL_CONFIG_STATIC4_B2 0x05
#define MPUREG_XA_ST_DATA_B2           0x3B
#define MPUREG_YA_ST_DATA_B2           0x3C
#define MPUREG_ZA_ST_DATA_B2           0x3D

/* Bank 3 */
#define MPUREG_PU_PD_CONFIG1_B3   0x06
#define MPUREG_PU_PD_CONFIG2_B3   0x0E

/* Bank 4 */
#define MPUREG_FDR_CONFIG_B4      0x09
#define MPUREG_APEX_CONFIG1_B4    0x40
#define MPUREG_APEX_CONFIG2_B4    0x41
#define MPUREG_APEX_CONFIG3_B4    0x42
#define MPUREG_APEX_CONFIG4_B4    0x43
#define MPUREG_APEX_CONFIG5_B4    0x44
#define MPUREG_APEX_CONFIG6_B4    0x45
#define MPUREG_APEX_CONFIG7_B4    0x46
#define MPUREG_APEX_CONFIG8_B4    0x47
#define MPUREG_APEX_CONFIG9_B4    0x48
#define MPUREG_APEX_CONFIG10_B4   0x49
#define MPUREG_ACCEL_WOM_X_THR_B4 0x4A
#define MPUREG_ACCEL_WOM_Y_THR_B4 0x4B
#define MPUREG_ACCEL_WOM_Z_THR_B4 0x4C
#define MPUREG_INT_SOURCE6_B4     0x4D
#define MPUREG_INT_SOURCE7_B4     0x4E
#define MPUREG_INT_SOURCE8_B4     0x4F
#define MPUREG_INT_SOURCE9_B4     0x50
#define MPUREG_INT_SOURCE10_B4    0x51
#define MPUREG_OFFSET_USER_0_B4   0x77
#define MPUREG_OFFSET_USER_1_B4   0x78
#define MPUREG_OFFSET_USER_2_B4   0x79
#define MPUREG_OFFSET_USER_3_B4   0x7A
#define MPUREG_OFFSET_USER_4_B4   0x7B
#define MPUREG_OFFSET_USER_5_B4   0x7C
#define MPUREG_OFFSET_USER_6_B4   0x7D
#define MPUREG_OFFSET_USER_7_B4   0x7E
#define MPUREG_OFFSET_USER_8_B4   0x7F

/* Virtual register addresses: @bank on MSB (4 upper bits), @address on LSB */

/* Bank selection register, available in all banks */

#define ICM42652_REG_BANK_SEL                    0x76
#define ICM42652_BANK_SEL_MASK                   GENMASK(2, 0)
#define ICM42652_REG_BANK_MASK                   GENMASK(15, 8)
#define ICM42652_REG_ADDRESS_MASK                GENMASK(7, 0)

/* GPIO_PAD_SEL */
#define BIT_INTF_CONFIG5_GPIO_PAD_SEL_POS        1
#define BIT_INTF_CONFIG5_GPIO_PAD_SEL_MASK       (0x3 << BIT_INTF_CONFIG5_GPIO_PAD_SEL_POS)
#define MPUREG_INTF_CONFIG5_B1                   0x7B

/* FIFO_MODE */
#define BIT_FIFO_CONFIG_MODE_POS              6
#define BIT_FIFO_CONFIG_MODE_MASK             (0x03 << BIT_FIFO_CONFIG_MODE_POS)
typedef enum {
	IXM42XXX_FIFO_CONFIG_MODE_STOP_ON_FULL  = (0x02 << BIT_FIFO_CONFIG_MODE_POS),
	IXM42XXX_FIFO_CONFIG_MODE_STREAM        = (0x01 << BIT_FIFO_CONFIG_MODE_POS),
	IXM42XXX_FIFO_CONFIG_MODE_BYPASS        = (0x00 << BIT_FIFO_CONFIG_MODE_POS),
} IXM42XXX_FIFO_CONFIG_MODE_t;

/* FSYNC_UI_SEL */
#define BIT_FSYNC_CONFIG_UI_SEL_POS       4
#define BIT_FSYNC_CONFIG_UI_SEL_MASK   (0x7 << BIT_FSYNC_CONFIG_UI_SEL_POS)
#define MPUREG_TMST_CONFIG        0x54
#define MPUREG_FSYNC_CONFIG       0x62
#define MPUREG_INT_CONFIG0        0x63
#define MPUREG_INT_CONFIG1        0x64
#define MPUREG_INT_SOURCE0        0x65
#define MPUREG_INT_SOURCE1        0x66
#define MPUREG_INT_SOURCE3        0x68
#define MPUREG_INT_SOURCE4        0x69

/* TMST_FSYNC */
#define BIT_TMST_CONFIG_TMST_FSYNC_POS        1
#define BIT_TMST_CONFIG_TMST_FSYNC_MASK    (0x1 << BIT_TMST_CONFIG_TMST_FSYNC_POS)

/* User bank 0 (MSB 0x00) */
#define ICM42652_REG_WHOAMI                      0x75
#define ICM42652_WHOAMI_ICM42652                 0x6F

#define ICM42652_REG_DEVICE_CONFIG               0x0011

#define ICM42652_REG_DRIVE_CONFIG                0x0013
#define ICM42652_DRIVE_CONFIG_I2C_MASK           GENMASK(5, 3)
#define ICM42652_DRIVE_CONFIG_I2C(_rate)         \
		FIELD_PREP(ICM42652_DRIVE_CONFIG_I2C_MASK, (_rate))
#define ICM42652_DRIVE_CONFIG_SPI_MASK           GENMASK(2, 0)
#define ICM42652_DRIVE_CONFIG_SPI(_rate)         \
		FIELD_PREP(ICM42652_DRIVE_CONFIG_SPI_MASK, (_rate))

#define ICM42652_REG_INT_CONFIG                  0x0014
#define ICM42652_INT_CONFIG_INT2_LATCHED         BIT(5)
#define ICM42652_INT_CONFIG_INT2_PUSH_PULL       BIT(4)
#define ICM42652_INT_CONFIG_INT2_ACTIVE_HIGH     BIT(3)
#define ICM42652_INT_CONFIG_INT2_ACTIVE_LOW      0x00
#define ICM42652_INT_CONFIG_INT1_LATCHED         BIT(2)
#define ICM42652_INT_CONFIG_INT1_PUSH_PULL       BIT(1)
#define ICM42652_INT_CONFIG_INT1_ACTIVE_HIGH     BIT(0)
#define ICM42652_INT_CONFIG_INT1_ACTIVE_LOW      0x00

#define ICM42652_REG_FIFO_CONFIG                0x0016
#define ICM42652_FIFO_CONFIG_MASK                GENMASK(7, 6)
#define ICM42652_FIFO_CONFIG_BYPASS              \
		FIELD_PREP(ICM42652_FIFO_CONFIG_MASK, 0)
#define ICM42652_FIFO_CONFIG_STREAM              \
		FIELD_PREP(ICM42652_FIFO_CONFIG_MASK, 1)
#define ICM42652_FIFO_CONFIG_STOP_ON_FULL        \
		FIELD_PREP(ICM42652_FIFO_CONFIG_MASK, 2)

/* all sensor data are 16 bits (2 registers wide) in big-endian */
#define ICM42652_REG_TEMP_DATA                   0x001D
#define ICM42652_REG_ACCEL_DATA_X                0x001F
#define ICM42652_REG_ACCEL_DATA_Y                0x0021
#define ICM42652_REG_ACCEL_DATA_Z                0x0023
#define ICM42652_REG_GYRO_DATA_X                 0x0025
#define ICM42652_REG_GYRO_DATA_Y                 0x0027
#define ICM42652_REG_GYRO_DATA_Z                 0x0029
#define ICM42652_DATA_INVALID                    -32768

#define ICM42652_REG_INT_STATUS                  0x002D
#define ICM42652_INT_STATUS_UI_FSYNC             BIT(6)
#define ICM42652_INT_STATUS_PLL_RDY              BIT(5)
#define ICM42652_INT_STATUS_RESET_DONE           BIT(4)
//#define ICM42652_INT_STATUS_DATA_RDY           BIT(3)
#define ICM42652_INT_STATUS_FIFO_THS             BIT(2)
#define ICM42652_INT_STATUS_FIFO_FULL            BIT(1)
#define ICM42652_INT_STATUS_AGC_RDY              BIT(0)


#define MPUREG_FIFO_CONFIG2       0x60

/*
 * FIFO access registers
 * FIFO count is 16 bits (2 registers) big-endian
 * FIFO data is a continuous read register to read FIFO content
 */
#define ICM42652_REG_FIFO_COUNT                  0x002E
#define ICM42652_REG_FIFO_DATA                   0x0030

#define ICM42652_REG_SIGNAL_PATH_RESET           0x004B
#define ICM42652_DEVICE_CONFIG_ABORT_ADN_RESET   BIT(3)
#define ICM42652_SIGNAL_PATH_RESET_FIFO_FLUSH    BIT(1)

/* default configuration: all data big-endian and fifo count in bytes */
#define ICM42652_REG_INTF_CONFIG0                0x004C
#define ICM42652_INTF_CONFIG0_FIFO_COUNT_REC     BIT(6)
#define ICM42652_INTF_CONFIG0_FIFO_COUNT_ENDIAN  BIT(5)
#define ICM42652_INTF_CONFIG0_SENSOR_DATA_ENDIAN BIT(4)

#define ICM42652_REG_INTF_CONFIG1                0x004D

#define ICM42652_REG_PWR_MGMT0                   0x004E
#define ICM42652_PWR_MGMT0_IDLE                  BIT(4)
#define ICM42652_PWR_MGMT0_GYRO(_mode)           \
		FIELD_PREP(GENMASK(3, 2), (_mode))
#define ICM42652_PWR_MGMT0_ACCEL(_mode)          \
		FIELD_PREP(GENMASK(1, 0), (_mode))

#define ICM42652_REG_GYRO_CONFIG0                0x004F
#define ICM42652_GYRO_CONFIG0_FS(_fs)            \
		FIELD_PREP(GENMASK(7, 5), (_fs))
#define ICM42652_GYRO_CONFIG0_ODR(_odr)          \
		FIELD_PREP(GENMASK(3, 0), (_odr))

#define ICM42652_REG_ACCEL_CONFIG0               0x0050
#define ICM42652_ACCEL_CONFIG0_FS(_fs)           \
		FIELD_PREP(GENMASK(7, 5), (_fs))
#define ICM42652_ACCEL_CONFIG0_ODR(_odr)         \
		FIELD_PREP(GENMASK(3, 0), (_odr))

#define ICM42652_REG_GYRO_CONFIG1                0x0051
#define ICM42652_GYRO_CONFIG1_GYRO_FILT(_f)      \
		FIELD_PREP(GENMASK(7, 5), (_f))

#define ICM42652_REG_ACCEL_CONFIG1               0x0053

/* FIFO watermark is 16 bits (2 registers wide) in little-endian */
#define ICM42652_REG_FIFO_WATERMARK              0x0060
#define ICM42652_FIFO_WATERMARK_VAL(_wm)         \
		cpu_to_le16((_wm) & GENMASK(11, 0))
/* FIFO is 2048 bytes, let 12 samples for reading latency */
#define ICM42652_FIFO_WATERMARK_MAX	             (2048 - 12 * 16)

#define IXM42XXX_DEFAULT_WOM_THS_MG 52>>2 /* = 52mg/4 */

/* Interrupt enum state for INT1, INT2, and IBI */
typedef enum {
	INV_IXM42XXX_DISABLE = 0,
	INV_IXM42XXX_ENABLE
} inv_ixm42xxx_interrupt_value;

typedef struct {
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_UI_FSYNC;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_UI_DRDY;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_FIFO_THS;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_FIFO_FULL;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_SMD;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_WOM_X;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_WOM_Y;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_WOM_Z;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_STEP_DET;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_STEP_CNT_OVFL;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_TILT_DET;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_FF_DET;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_LOWG_DET;
	inv_ixm42xxx_interrupt_value INV_IXM42XXX_TAP_DET;
} inv_ixm42xxx_interrupt_parameter_t;

/*
 * MPUREG_INT_SOURCE0
 * Register Name: INT_SOURCE0
 */
#define BIT_INT_UI_FSYNC_INT_EN_POS         6
#define BIT_INT_PLL_RDY_INT_EN_POS          5
#define BIT_INT_RESET_DONE_INT_EN_POS       4
#define BIT_INT_UI_DRDY_INT_EN_POS          3
#define BIT_INT_FIFO_THS_INT_EN_POS         2
#define BIT_INT_FIFO_FULL_INT_EN_POS        1
#define BIT_INT_UI_AGC_RDY_INT_EN_POS       0

#define BIT_INT_SOURCE0_UI_FSYNC_INT1_EN    0x40
#define BIT_INT_SOURCE0_PLL_RDY_INT1_EN     0x20
#define BIT_INT_SOURCE0_RESET_DONE_INT1_EN  0x10
#define BIT_INT_SOURCE0_UI_DRDY_INT1_EN     0x08
#define BIT_INT_SOURCE0_FIFO_THS_INT1_EN    0x04
#define BIT_INT_SOURCE0_FIFO_FULL_INT1_EN   0x02
#define BIT_INT_SOURCE0_UI_AGC_RDY_INT1_EN  0x01

/*
 * MPUREG_INT_SOURCE1
 * Register Name: INT_SOURCE1
 */
#define BIT_INT_SMD_INT_EN_POS              3
#define BIT_INT_WOM_Z_INT_EN_POS            2
#define BIT_INT_WOM_Y_INT_EN_POS            1
#define BIT_INT_WOM_X_INT_EN_POS            0

#define BIT_INT_SOURCE1_SMD_INT1_EN         0x08
#define BIT_INT_SOURCE1_WOM_Z_INT1_EN       0x04
#define BIT_INT_SOURCE1_WOM_Y_INT1_EN       0x02
#define BIT_INT_SOURCE1_WOM_X_INT1_EN       0x01


/*
 * MPUREG_INT_SOURCE6_B4
 * Register Name: INT_SOURCE6
 */
#define BIT_INT_STEP_DET_INT_EN_POS      5
#define BIT_INT_STEP_CNT_OVFL_INT_EN_POS 4
#define BIT_INT_TILT_DET_INT_EN_POS      3
#define BIT_INT_LOWG_DET_INT_EN_POS      2
#define BIT_INT_FF_DET_INT_EN_POS        1
#define BIT_INT_TAP_DET_INT_EN_POS       0

#define BIT_INT_SOURCE6_STEP_DET_INT1_EN      0x20
#define BIT_INT_SOURCE6_STEP_CNT_OVFL_INT1_EN 0x10
#define BIT_INT_SOURCE6_TILT_DET_INT1_EN      0x8
#define BIT_INT_SOURCE6_LOWG_DET_INT1_EN      0x4
#define BIT_INT_SOURCE6_FF_DET_INT1_EN        0x2
#define BIT_INT_SOURCE6_TAP_DET_INT1_EN       0x1

/*
 * MPUREG_INT_SOURCE7_B4
 * Register Name: INT_SOURCE7
 */
#define BIT_INT_SOURCE7_STEP_DET_INT2_EN      0x20
#define BIT_INT_SOURCE7_STEP_CNT_OVFL_INT2_EN 0x10
#define BIT_INT_SOURCE7_TILT_DET_INT2_EN      0x8
#define BIT_INT_SOURCE7_LOWG_DET_INT2_EN      0x4
#define BIT_INT_SOURCE7_FF_DET_INT2_EN        0x2
#define BIT_INT_SOURCE7_TAP_DET_INT2_EN       0x1

/*
 * MPUREG_INT_SOURCE8_B4
 * Register Name: INT_SOURCE8
 */
#define BIT_INT_OIS1_DRDY_IBI_EN_POS  6
#define BIT_INT_UI_FSYNC_IBI_EN_POS   5
#define BIT_INT_PLL_RDY_IBI_EN_POS    4
#define BIT_INT_UI_DRDY_IBI_EN_POS    3
#define BIT_INT_FIFO_THS_IBI_EN_POS   2
#define BIT_INT_FIFO_FULL_IBI_EN_POS  1
#define BIT_INT_UI_AGC_RDY_IBI_EN_POS 0

#define BIT_INT_SOURCE8_OIS1_DRDY_IBI_EN  0x40
#define BIT_INT_SOURCE8_UI_FSYNC_IBI_EN   0x20
#define BIT_INT_SOURCE8_PLL_RDY_IBI_EN    0x10
#define BIT_INT_SOURCE8_UI_DRDY_IBI_EN    0x08
#define BIT_INT_SOURCE8_FIFO_THS_IBI_EN   0x04
#define BIT_INT_SOURCE8_FIFO_FULL_IBI_EN  0x02
#define BIT_INT_SOURCE8_UI_AGC_RDY_IBI_EN 0x01

/*
 * MPUREG_INT_SOURCE9_B4
 * Register Name: INT_SOURCE9
 */
#define BIT_INT_SMD_IBI_EN_POS                 4
#define BIT_INT_WOM_Z_IBI_EN_POS               3
#define BIT_INT_WOM_Y_IBI_EN_POS               2
#define BIT_INT_WOM_X_IBI_EN_POS               1

#define BIT_INT_SOURCE9_SMD_IBI_EN             0x10
#define BIT_INT_SOURCE9_WOM_Z_IBI_EN           0x08
#define BIT_INT_SOURCE9_WOM_Y_IBI_EN           0x04
#define BIT_INT_SOURCE9_WOM_X_IBI_EN           0x02

/*
 * MPUREG_INT_SOURCE10_B4
 * Register Name: INT_SOURCE10
 */
#define BIT_INT_STEP_DET_IBI_EN_POS      5
#define BIT_INT_STEP_CNT_OVFL_IBI_EN_POS 4
#define BIT_INT_TILT_DET_IBI_EN_POS      3
#define BIT_INT_LOWG_DET_IBI_EN_POS      2
#define BIT_INT_FF_DET_IBI_EN_POS        1
#define BIT_INT_TAP_DET_IBI_EN_POS       0

#define BIT_INT_SOURCE10_STEP_DET_IBI_EN      0x20
#define BIT_INT_SOURCE10_STEP_CNT_OVFL_IBI_EN 0x10
#define BIT_INT_SOURCE10_TILT_DET_IBI_EN      0x08
#define BIT_INT_SOURCE10_LOWG_DET_IBI_EN      0x04
#define BIT_INT_SOURCE10_FF_DET_IBI_EN        0x02
#define BIT_INT_SOURCE10_TAP_DET_IBI_EN       0x01

#define ICM42652_REG_INT_SOURCE0                 0x0065
#define ICM42652_INT_SOURCE0_FSYNC_INT1_EN       BIT(6)
#define ICM42652_INT_SOURCE0_PLL_RDY_INT1_EN     BIT(5)
#define ICM42652_INT_SOURCE0_RESET_DONE_INT1_EN  BIT(4)
#define ICM42652_INT_SOURCE0_DRDY_INT1_EN        BIT(3)
#define ICM42652_INT_SOURCE0_FIFO_THS_INT1_EN    BIT(2)
#define ICM42652_INT_SOURCE0_FIFO_FULL_INT1_EN   BIT(1)
#define ICM42652_INT_SOURCE0_AGC_RDY_INT1_EN     BIT(0)

#define ICM42652_REG_TMST_CONFIG1                0x54
#define ICM42652_TMST_CONFIG1_MASK               GENMASK(4, 0)
#define ICM42652_TMST_CONFIG1_TMST_TO_REGS_EN    BIT(4)
#define ICM42652_TMST_CONFIG1_TMST_RES_16US      BIT(3)
#define ICM42652_TMST_CONFIG1_TMST_DELTA_EN      BIT(2)
#define ICM42652_TMST_CONFIG1_TMST_FSYNC_EN      BIT(1)
#define ICM42652_TMST_CONFIG1_TMST_EN            BIT(0)

#define ICM42652_REG_FIFO_CONFIG1                0x05F
#define ICM42652_FIFO_CONFIG1_RESUME_PARTIAL_RD  BIT(6)
#define ICM42652_FIFO_CONFIG1_WM_GT_TH           BIT(5)
#define ICM42652_FIFO_CONFIG1_FIFO_HIRES_EN      BIT(4)
#define ICM42652_FIFO_CONFIG1_TMST_FSYNC_EN      BIT(3)
#define ICM42652_FIFO_CONFIG1_TEMP_EN            BIT(2)
#define ICM42652_FIFO_CONFIG1_GYRO_EN            BIT(1)
#define ICM42652_FIFO_CONFIG1_ACCEL_EN           BIT(0)

#define ICM42652_REG_INT_CONFIG1                 0x64
#define ICM42652_INT_CONFIG1_TPULSE_DURATION     BIT(6)
#define ICM42652_INT_CONFIG1_ASYNC_RESET         BIT(4)

#endif
