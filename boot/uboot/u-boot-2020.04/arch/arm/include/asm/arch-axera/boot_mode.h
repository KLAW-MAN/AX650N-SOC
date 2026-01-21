/*
 * AXERA AX630X
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __BOOT_MODE_H__
#define __BOOT_MODE_H__

typedef enum boot_mode {
	SYSDUMP_MODE = 0x0,
	SD_UPDATE_MODE = 0x1,
	USB_UPDATE_MODE = 0x2,
	UART_UPDATE_MODE = 0x3,
	TFTP_UPDATE_MODE = 0x4,
	USB_STOR_MODE = 0x5,
	SD_BOOT_MODE = 0x6,
	NORMAL_BOOT_MODE = 0x7,
	CMD_UNDEFINED_MODE,
	BOOTMODE_FUN_NUM,
} boot_mode_t;

typedef boot_mode_t (*s_boot_func_array)(void);

typedef enum dl_channel {
	DL_CHAN_UNKNOWN = 0x0,
	DL_CHAN_SDIO = 0x1,
	DL_CHAN_SPI = 0x2,
	DL_CHAN_USB = 0x3,
	DL_CHAN_UART0 = 0x4,	//0x4 is uart0, 0x5 is uart1
	DL_CHAN_UART1 = 0x5,
	DL_CHAN_SD = 0x6,
} dl_channel_t;

typedef enum storage_type_sel {
	STORAGE_TYPE_EMMC = 0x0,
	STORAGE_TYPE_NAND = 0x1,
	STORAGE_TYPE_NOR = 0x2,
	STORAGE_TYPE_PCIE = 0x3,
	STORAGE_TYPE_SSD = 0x4,
	STORAGE_TYPE_UNKNOWN = 0x5,
} storage_sel_t;

typedef enum chip_type {
	NONE_CHIP_TYPE = 0x0,
	AX650A_CHIP = 0x1,
	AX650N_CHIP = 0x2,
	AX650C_CHIP = 0x3,
	AX750_CHIP = 0x4,
	AX650_CHIP_MAX = 0x5,
} chip_type_t;

typedef enum board_type {
	AX650A_Demo = 0,
	AX650N_Demo = 1,
	AX650A_EVB = 3,
	AX650N_EVB = 2,
	AX650A_SLT = 4,
	AX650N_SLT = 5,
	AX650N_PCIE = 7,
	AX650_BOARD_MAX = 8,
} board_type_t;

typedef enum adp_plate_type {
	ADP_RX_DPHY_2X4LANE = 0,
	ADP_RX_DPHY_8X2LANE,
	ADP_RX_DPHY_4X4LANE,
	ADP_RX_DPHY_2X8_LVDS_1X16LANE,
	ADP_RX_DPHY_2X4LANE_N,
	ADP_RX_CPHY_8X2TRIO,
	ADP_RX_CPHY_4X3TRIO,
	ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120,
	ADP_RX_BT1120_2X10BIT,
	ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE,
	ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE,
	ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE,
	ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE,
	ADP_RX_BT_TO_CVBS,
	ADP_RX_MAX,
} adp_plate_type_t;

#define BOOT_MODE_ENV_MAGIC	0x12345678
#define COMM_SYS_GLB		0x4200000	//aon_glb
#define COMM_SYS_CHIP_MODE	(COMM_SYS_GLB + 0x10)
#define COMM_SYS_DUMMY_SW5	(COMM_SYS_GLB + 0x1B4)
#define COMM_SYS_DUMMY_SW1	(COMM_SYS_GLB + 0x184)
#define COMM_SYS_DUMMY_SW2	(COMM_SYS_GLB + 0x190)
typedef struct boot_mode_info {
	u32 magic;		//0x12345678
	boot_mode_t mode;
	dl_channel_t dl_channel;	//usb,uart0,uart1,uart2...
	storage_sel_t storage_sel;
} boot_mode_info_t;

/* chip_mode[3] */
#define BOOT_VOLTAGE_MASK	(1 << 3)
/* chip_mode[2] */
#define USB_DL_SD_BOOT_MASK	(1 << 2)
/* chip_mode[1:0] */
#define FLASH_BOOT_MASK	(3 << 0)

#define FLASH_EMMC			0
#define FLASH_NAND			1
#define FLASH_NOR			2
#define FLASH_PCIE			3
#define FLASH_SD			4
#define FLASH_SD_OR_USB_DL	5

//int get_part_info(struct blk_desc *dev_desc, const char *name, disk_partition_t * info);
#endif
