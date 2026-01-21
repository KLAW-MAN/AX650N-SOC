/*
 * (C) Copyright 2020 AXERA
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __CONFIG_AX650_COMMON_H
#define __CONFIG_AX650_COMMON_H
#include "ax_common.h"
/*noncached memory size: 1MB*/

//#define CONFIG_SYS_NONCACHED_MEMORY (1 << 20)

/* #define CONFIG_NR_DRAM_BANKS            1 *//* defined in u-boot/Kconfig */
#define CONFIG_SYS_MAXARGS		16

#define CONFIG_SYS_MALLOC_LEN		(64 << 20)	//64M
#define CONFIG_SYS_CBSIZE		1024 /* Console I/O Buffer Size */
#define CONFIG_SKIP_LOWLEVEL_INIT

// #define GD_FLG_SKIP_RELOC       0x00800 /* Don't relocate

#define CONFIG_SYS_INIT_SP_ADDR		0x14300000
#define CONFIG_SYS_LOAD_ADDR		0x14380000

#define CONFIG_SYS_BOOTM_LEN	(64 << 20)	/* 64M */

#define DTB_IMAGE_ADDR 0x110080000
#define KERNEL_IMAGE_ADDR 0x100200000	/* 2M align */
#define KERNEL_HEADER_SIZE 0x40
#define HEADER_IMAGE_ADDR 0x11ff10000
#define ATF_IMAGE_ADDR  0x4801000

#define VO_NR 2
#define AX_VO_DPU0_BASE_ADDR 0x10111000
#define AX_VO_DPU1_BASE_ADDR 0x10112000

#define HDMI_DEV_NR 2
#define AX_DW_HDMI0_BASE_ADDR 0x10030000
#define AX_DW_HDMI1_BASE_ADDR 0x10040000

#define AX_DW_MIPI_DSI_BASE_ADDR 0x10060000

#define MM_SYS_GLB_BASE_ADDR 0x10000000
#define MM_CK_BASE_ADDR 0x10010000
#define DPHY_SYS_BASE_ADDR 0x10013000
#define MM_PHY_CK_RST_BASE_ADDR 0x10014000
#define AON_SYS_PLL_BASE_ADDR 0x4E10000

/* FAT sd card locations. */
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION	1
#define CONFIG_SYS_SDRAM_BASE		0x280000000
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_NS16550_SERIAL

/* spi NAND Flash */
#define CONFIG_SYS_MAX_NAND_DEVICE 		1
#define CONFIG_SYS_NAND_SELF_INIT

/* spi NAND Flash file system: YAFFS2 */
/* #define CONFIG_CMD_NAND_YAFFS */

#ifndef CONFIG_SPL_BUILD //TBC
#define ENV_MEM_LAYOUT_SETTINGS \
	"scriptaddr=0x00000000\0" \
	"pxefile_addr_r=0x400100000\0" \
	"fdt_addr_r=0x401f00000\0" \
	"kernel_addr_r=0x402000000\0" \
	"ramdisk_addr_r=0x404000000\0"
#endif

#define CONFIG_SYS_NS16550_REG_SIZE (-4)
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_NS16550_CLK (200000000)
#define CONFIG_SYS_NS16550_COM1 (0x2016000)
#define CONFIG_SYS_NS16550_COM2 (0x2017000)
#define CONFIG_SYS_NS16550_COM3 (0x2018000)

/* axera xhci host driver */
/* #define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS	2 */
#define CONFIG_SYS_MMC_MAX_BLK_COUNT    1024
/* keep the FDL BUF 4KB bundary*/
#define FDL_BUF_ADDR        0x100000000
#define FDL_BUF_LEN          (2 << 20)	//2M
#define SPARSE_IMAGE_BUF_ADDR	0x100000000
#define SPARSE_IMAGE_BUF_LEN	0x10000000
#define SD_UPDATE_BUF 0x120000000
#define USB_UPDATE_BUF			0x120000000
#define SDHCI_ALIGN_BUF_ADDR	0x120000000
#define DL_WRITE_TEMP_BUF_ADDR	0x110000000
#define SDHCI_ALIGN_BUF_BLKS	0x1000
#define OTA_BUF_ADDR           0x140000000
#define OTA_BUF_LEN            0x100000000
#define CONFIG_IMAGE_SPARSE_FILLBUF_SIZE 0x80000
// #define SPL_BIN_EMMC_OFF       0

#define CONFIG_BOARD_LATE_INIT
#define CONFIG_CMD_MMC
#define CONFIG_SYS_MMC_ENV_DEV	0
#define CONFIG_SYS_MMC_ENV_PART	0

#define CONFIG_GICV2
#define GIC_BASE        0x4900000
#define GICD_BASE       (GIC_BASE + 0x1000)
#define GICC_BASE       (GIC_BASE + 0x2000)

// #define FLASH_SYS_DUMMY_SW0    0x910024
// #define AP_SYS_DUMMPY_SW1      0x208068

#define PWM_TIMER_BASE          0x205000 //TBC
#define TIMER2LOADCOUNT_OFFSET  0x14
#define TIMER2CONTROLREG_OFFSET 0x1C
#define TIMER2LOADCOUNT2_OFFSET 0xB4

#if defined(CONFIG_TARGET_AX650_XPILOT_6V)

//#define PCIE_SATA_CFG_VIA_BOARDID
#ifndef PCIE_SATA_CFG_VIA_BOARDID
//#define PIPE_CLK_USE_INTERNEL
#define PIPE_CLK_USE_EXT

//#define PIPE_SYS_LAN0_SATA_LAN1_SATA
//#define PIPE_SYS_LAN0_PCIE_LAN1_SATA
#define PIPE_SYS_LAN0_PCIE_LAN1_PCIE

//#define PIPE_SYS_LAN2_SATA_LAN3_SATA
//#define PIPE_SYS_LAN2_PCIE_LAN3_SATA
#define PIPE_SYS_LAN2_PCIE_LAN3_PCIE
#endif
#elif defined(CONFIG_TARGET_AX650_PIPRO_BOX)
//#define PIPE_CLK_USE_INTERNEL
#define PIPE_CLK_USE_EXT

#define PIPE_SYS_LAN0_SATA_LAN1_SATA
//#define PIPE_SYS_LAN0_PCIE_LAN1_SATA
//#define PIPE_SYS_LAN0_PCIE_LAN1_PCIE

//#define PIPE_SYS_LAN2_SATA_LAN3_SATA
#define PIPE_SYS_LAN2_PCIE_LAN3_SATA
//#define PIPE_SYS_LAN2_PCIE_LAN3_PCIE
#else
#define PCIE_SATA_CFG_VIA_BOARDID
#ifndef PCIE_SATA_CFG_VIA_BOARDID
#define PIPE_CLK_USE_INTERNEL
//#define PIPE_CLK_USE_EXT

//#define PIPE_SYS_LAN0_SATA_LAN1_SATA
#define PIPE_SYS_LAN0_PCIE_LAN1_SATA
//#define PIPE_SYS_LAN0_PCIE_LAN1_PCIE

//#define PIPE_SYS_LAN2_SATA_LAN3_SATA
//#define PIPE_SYS_LAN2_PCIE_LAN3_SATA
#define PIPE_SYS_LAN2_PCIE_LAN3_PCIE
#endif
#endif

#ifdef CONFIG_DWC_AHSATA_AXERA
#define CONFIG_LBA48
//#define SYS_MEM_FB_START            (0x102000000)
//#define SYS_MEM_CLB_START           (0x102100000)
#define SYS_MEM_CTB_START           (0x102800000)
#define SYS_MEM_DATA_START          (0x102C00000)
#define AX_SATA_BIST_SUPPORT
#endif

#define AX_DEBUG	1
#ifndef pr_fmt
#define pr_fmt(fmt)	fmt
#endif

#define ax_debug_cond(cond, fmt, args...)		\
	do {						\
		if (cond)				\
			printf(pr_fmt(fmt), ##args);	\
	} while (0)

#define ax_debug(fmt, args...)			\
	ax_debug_cond(AX_DEBUG, fmt, ##args)

#ifdef  AXERA_DEBUG_KCONFIG
#define DEBUG_BOOT_ARGS "kasan_multi_shot "
#else
#define DEBUG_BOOT_ARGS ""
#endif

#ifndef OS_MEM_ARGS
#ifdef CONFIG_TARGET_AX650_PIPRO_BOX
#define OS_MEM_ARGS "mem=6144M"
#else
#define OS_MEM_ARGS "mem=1024M"
#endif
#endif
/* bootargs for eMMC */
#define BOOTARGS_EMMC_COMMON_ARGS "" \
    DEBUG_BOOT_ARGS OS_MEM_ARGS" ""console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 board_id=0x0,boot_reason=0x0,\
    initcall_debug=0 loglevel=8 usbcore.autosuspend=-1"

/* bootargs for eMMC */
#ifdef CONFIG_BOOT_OPTIMIZATION
#define BOOTARGS_EMMC_NO_PART	BOOTARGS_EMMC_COMMON_ARGS \
				" ax_boot_delay=10 " \
				" rootfstype=ext4 rw rootwait root="
#else
#define BOOTARGS_EMMC_NO_PART	BOOTARGS_EMMC_COMMON_ARGS \
				" rootfstype=ext4 rw rootwait root="
#endif

#define EMMCPARTS_DEFAULT_PRE	" blkdevparts=mmcblk0:" \
    "1536K(uboot)," \
    "1536K(uboot_bk)," \
    "1M(env)," \
    "20M(param)," \
    "6M(logo)," \
    "1M(dtb)," \
    "64M(kernel)," \
    "1M(atf),"

#define EMMCPARTS_AB_PRE     " blkdevparts=mmcblk0:" \
    "1536K(uboot)," \
    "1536K(uboot_bk)," \
    "1M(env)," \
    "20M(param)," \
    "20M(param_B)," \
    "6M(logo)," \
    "6M(logo_B)," \
    "1M(dtb)," \
    "1M(dtb_B)," \
    "64M(kernel)," \
    "64M(kernel_B)," \
    "1M(atf)," \
    "1M(atf_B),"

#ifdef OPTEE_BOOT
#define EMMCPARTS_DEFAULT_POST	EMMCPARTS_DEFAULT_PRE \
    "1M(optee),"
#define EMMCPARTS_AB_POST EMMCPARTS_AB_PRE \
    "1M(optee)," \
    "1M(optee_B),"
#else
#define EMMCPARTS_DEFAULT_POST	EMMCPARTS_DEFAULT_PRE
#define EMMCPARTS_AB_POST EMMCPARTS_AB_PRE
#endif

#define EMMCPARTS_DEFAULT	EMMCPARTS_DEFAULT_POST \
    "4736M(rootfs)," \
    "384M(soc)," \
    "1900M(opt)," \

#define EMMCPARTS_MASTER	EMMCPARTS_DEFAULT_POST \
	"2048M(rootfs)," \
	"1024M(soc)," \
	"4096M(opt)," \

#define EMMCPARTS_AB     EMMCPARTS_AB_POST \
    "128M(rootfs)," \
    "128M(rootfs_B)," \
    "512M(soc)," \
    "512M(soc_B)," \
    "4864M(opt)," \

#ifdef OPTEE_BOOT
#define ROOTFS_PART   "/dev/mmcblk0p10"
#define ROOTFS_PART_A "/dev/mmcblk0p16"
#define ROOTFS_PART_B "/dev/mmcblk0p17"
#else
#define ROOTFS_PART   "/dev/mmcblk0p9"
#define ROOTFS_PART_A "/dev/mmcblk0p14"
#define ROOTFS_PART_B "/dev/mmcblk0p15"
#endif

#define BOOTARGS_EMMC_DEFAULT 	BOOTARGS_EMMC_NO_PART \
				ROOTFS_PART \
				EMMCPARTS_DEFAULT

#define BOOTARGS_EMMC_A BOOTARGS_EMMC_NO_PART \
			ROOTFS_PART_A \
			EMMCPARTS_AB

#define BOOTARGS_EMMC_B BOOTARGS_EMMC_NO_PART \
			ROOTFS_PART_B \
			EMMCPARTS_AB

#ifdef CONFIG_AXERA_SUPPORT_AB_OTA
#define BOOTARGS_EMMC BOOTARGS_EMMC_A
#else
#define BOOTARGS_EMMC BOOTARGS_EMMC_DEFAULT
#endif

/* bootargs for master board */
#define BOOTARGS_MASTER  	BOOTARGS_EMMC_NO_PART \
				ROOTFS_PART \
				EMMCPARTS_MASTER

/* bootargs for SD */
#define BOOTARGS_SD "mem=1024M console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 init=/sbin/init noinitrd root=179:26 rw rootdelay=3 rootfstype=ext4"

#define BOOTARGS_SPINAND "mem=1024M console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 board_id=0x0,boot_reason=0x0,noinitrd\
root=/dev/mtdblock7 rw rootfstype=ubifs ubi.mtd=7,2048 root=ubi0:rootfs init=/linuxrc \
mtdparts=spi5.0:4M(uboot),768K(env),1M(atf),1M(dtb),32M(kernel),512K(param),192M(rootfs)"

#define BOOTARGS_SPINAND_A BOOTARGS_SPINAND
#define BOOTARGS_SPINAND_B BOOTARGS_SPINAND

/* bootargs for pcie */
#define BOOTARGS_PCIE DEBUG_BOOT_ARGS OS_MEM_ARGS" ""root=/dev/axramdisk rootfstype=ext4 console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 loglevel=4 board_id=0x0,boot_reason=0x0"

/* bootargs for ssd */
#ifdef OPTEE_BOOT
#define BOOTARGS_SSD	DEBUG_BOOT_ARGS OS_MEM_ARGS" ""console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 board_id=0x0,boot_reason=0x0,\
initcall_debug=0 loglevel=8 ax_boot_delay=10 root=/dev/nvme0n1p7 rootfstype=ext4 rw rootwait \
mtdparts=spi5.0:1536K(uboot),1536K(uboot_bk),1M(env) \
blkdevparts=nvme0n1:20M(param),6M(logo),1M(dtb),64M(kernel),1M(atf),1M(optee),2048M(rootfs),1024M(soc),4096M(opt)"
#else
#define BOOTARGS_SSD	DEBUG_BOOT_ARGS OS_MEM_ARGS" ""console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 board_id=0x0,boot_reason=0x0,\
initcall_debug=0 loglevel=8 ax_boot_delay=10 root=/dev/nvme0n1p6 rootfstype=ext4 rw rootwait \
mtdparts=spi5.0:1536K(uboot),1536K(uboot_bk),1M(env) \
blkdevparts=nvme0n1:20M(param),6M(logo),1M(dtb),64M(kernel),1M(atf),2048M(rootfs),1024M(soc),4096M(opt)"
#endif

/* bootargs for pipro box */
#ifdef OPTEE_BOOT
#define BOOTARGS_PIPRO	DEBUG_BOOT_ARGS OS_MEM_ARGS" ""console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 board_id=0x0,boot_reason=0x0,\
initcall_debug=0 loglevel=8 usbcore.autosuspend=-1 ax_boot_delay=10 root=/dev/mmcblk0p10 rootfstype=ext4 rw rootwait \
blkdevparts=mmcblk0:1536K(uboot),1536K(uboot_bk),1M(env),20M(param),6M(logo),1M(dtb),64M(kernel),1M(atf),1M(optee),24576M(rootfs),512M(soc),4096M(opt)"
#else
#define BOOTARGS_PIPRO	DEBUG_BOOT_ARGS OS_MEM_ARGS" ""console=ttyS0,115200n8 earlycon=uart8250,mmio32,0x2016000 board_id=0x0,boot_reason=0x0,\
initcall_debug=0 loglevel=8 usbcore.autosuspend=-1 ax_boot_delay=10 root=/dev/mmcblk0p9 rootfstype=ext4 rw rootwait \
blkdevparts=mmcblk0:1536K(uboot),1536K(uboot_bk),1M(env),20M(param),6M(logo),1M(dtb),64M(kernel),1M(atf),24576M(rootfs),512M(soc),4096M(opt)"
#endif

/* bootargs for SPI NOR Flash */
#define MTDIDS_SPINOR		"nor0=spi5.0"
#define MTDPARTS_SPINOR		"mtdparts=spi5.0:3M(uboot)," \
				"16M(kernel)," \
				"512K(param)," \
				"10M(rootfs)"
#define BOOTARGS_SPINOR		"mem=1024M console=ttyS0,115200n8 noinitrd earlycon=uart8250,mmio32 \
root=/dev/mtdblock5 rootfstype=jffs2 rw \
mtdparts=spi4.0:3M(uboot),16M(kernel),512K(param),10M(rootfs)"

/* bootargs for SPI NAND Flash */
#define MTDIDS_DEFAULT		"nand0=spi5.0"
#define MTDPARTS_DEFAULT	"mtdparts=spi5.0:4M(uboot)," \
				"768K(env)," \
				"1M(atf)," \
				"1M(dtb)," \
				"32M(kernel)," \
				"512K(param)," \
				"192M(rootfs)"

//#define CONFIG_USE_BOOTARGS
#ifndef CONFIG_BOOTARGS
#define CONFIG_BOOTARGS "mem=1024M console=ttyS0,115200n8 noinitrd earlycon=uart8250,mmio32 \
root=/dev/mtdblock7 rw rootfstype=ubifs ubi.mtd=7,2048 root=ubi0:rootfs init=/linuxrc \
mtdparts=spi5.0:4M(uboot),768K(env),1M(atf),1M(dtb),32M(kernel),512K(param),192M(rootfs)"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS		\
	"bootlimit=3\0" \
	"altbootcmd=" \
	"  echo Rollback to previous Slot; " \
	"  if test ${ustate} = 1; " \
	"    then setenv ustate 3; " \
	"  fi; " \
	"  if test ${bootpart} = A; " \
	"    then setenv bootpart B; " \
	"  else " \
	"     setenv bootpart A; " \
	"  fi; setenv bootcount 0; saveenv; " \
	"  run bootcmd\0" \

#endif
