/*
 * Copyright (c) 2022 AXERA in AX650 project.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <sdhci.h>
#include <malloc.h>
#include <asm/arch/ax650.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch-axera/dma.h>
#include <dm/uclass.h>
#include <stdlib.h>
#include <nvme.h>
#include <pcie_ax650.h>
#include <init.h>
#include "ax_efuse_api.h"

typedef struct {
	u8 gen1_amplitude;
	u8 gen2_amplitude;
	u8 gen3_amplitude;
	u8 gen2_preemph;
	u8 gen3_preemph;
	u8 vboost;
} sata_port_cfg_t;

static sata_port_cfg_t port_cfg[4] = {
	{0x50, 0x60, 0x78, 0x10, 0x1D, 0x4}, //port0 defualt
	{0x50, 0x60, 0x78, 0x10, 0x1D, 0x4}, //port1 defualt
	{0x50, 0x60, 0x78, 0x10, 0x1D, 0x4}, //port2 defualt
	{0x50, 0x60, 0x78, 0x10, 0x1D, 0x4}, //port3 defualt
};

static struct mm_region ax650x_mem_map[] = {
	{
		.virt = 0x14000000UL,
		.phys = 0x14000000UL,
		.size = 0xB00000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
                        PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x14000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},{
		.virt = 0x16000000UL,
		.phys = 0x16000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},	{
		.virt = 0x100000000UL,
		.phys = 0x100000000UL,
		.size = MEM_REGION_DDR_SIZE,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
				PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x4800000UL,
		.phys = 0x4800000UL,
		.size = 0x20000,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) |
				PTE_BLOCK_INNER_SHARE
	}, {
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = ax650x_mem_map;

u32 pipe_pcs0_model_sel = 1;
u32 pipe_pcs1_model_sel = 1;

int arch_cpu_init(void)
{
	/* We do some SoC one time setting here. */
	axi_dma_hw_init();
	return 0;
}

struct boot_mode_info boot_info_data;
u32 flash_sd_or_usb_dl_mode(u32 chip_mode)
{
	if (((chip_mode & USB_DL_SD_BOOT_MASK) >> 2) == 0)
		return FLASH_SD_OR_USB_DL;
	return 0;
}

u32 get_boot_mode(u32 chip_mode)
{
	/* EMMC :0, NAND :1, NOR :2, PCIE: 3 */
	return (chip_mode & FLASH_BOOT_MASK);
}

u32 get_boot_voltage(void)
{
	u32 chip_mode;
	chip_mode = readl(COMM_SYS_CHIP_MODE);
	/* 0: 3.3V   1: 1.8V */
	return (chip_mode & BOOT_VOLTAGE_MASK) >> 3;
}

boot_mode_info_t *get_dl_and_boot_info(void)
{
	u32 chip_mode;
	boot_mode_info_t *boot_mode = (boot_mode_info_t *) BOOT_MODE_INFO_ADDR;
	memset((void *)boot_mode, 0, sizeof(boot_mode_info_t));
	boot_mode->magic = BOOT_MODE_ENV_MAGIC;
	chip_mode = readl(COMM_SYS_CHIP_MODE);
	boot_mode->storage_sel = get_boot_mode(chip_mode);
	boot_mode->dl_channel = readl(COMM_SYS_DUMMY_SW5);
#ifdef CONFIG_TARGET_AX650_SSD
	boot_mode->storage_sel = STORAGE_TYPE_SSD;
#endif
	printf("boot_mode->dl_channel = %d\n", boot_mode->dl_channel);
	printf("boot_mode->storage_sel = %d\n", boot_mode->storage_sel);
	boot_mode->mode = NORMAL_BOOT_MODE;
	if (boot_mode->dl_channel == DL_CHAN_UART1
	    || boot_mode->dl_channel == DL_CHAN_UART0) {
		boot_mode->mode = UART_UPDATE_MODE;
	}
	if (boot_mode->dl_channel == DL_CHAN_USB) {
		boot_mode->mode = USB_UPDATE_MODE;
	}
	if (boot_mode->dl_channel == DL_CHAN_SD) {
		boot_mode->mode = SD_BOOT_MODE;
	}
	return boot_mode;
}

// console ,download, coredump to disable wtd0
void wdt0_enable(bool enable)
{
	/* set wdt timeout clk to 30S */
	writel((30 * WDT0_CLK_FREQ) >> 16, (void *)(WDT0_TORR_ADDR));
	writel(0x1, (void *)(WDT0_TORR_START_ADDR));
	writel(0x0, (void *)(WDT0_TORR_START_ADDR));

	/* set wdt0 clk source to 24MHz */
	writel(BIT(12), (void *)(COMM_CK_RST_CFG_CLK_MUX0_SET));

	if (enable) {
		writel(1, WDT0_BASE);
	} else {
		writel(0, WDT0_BASE);
	}
}

#if 0
void reboot(void)
{
	wdt0_enable(true);
	printf("trigger watchdog, reboot now ...\n");
	while (1) ;
}
#else
static void chip_rst_sw(void)
{
	u32 tmp;

	tmp = readl((void *)COMM_ABORT_CFG);
	tmp |= CHIP_RST_SW;
	printf("Set REG 0x%X, value 0x%X\n", COMM_ABORT_CFG, tmp);
	writel(tmp, (void *)COMM_ABORT_CFG);
}

void reboot(void)
{
	chip_rst_sw();
	printf("trigger chip_rst_sw ...\n");
	while (1) ;
}
#endif

#ifndef CONFIG_SYSRESET
void reset_cpu(unsigned long ignored)
{
	reboot();
}
#endif

u32 dump_reason;
u32 axera_get_boot_reason(void)
{
	u32 abort_cfg;
	u32 abort_status;
	abort_status = readl(COMM_ABORT_STATUS);
	dump_reason = abort_status;
	/*clear abort alarm status */
	abort_cfg =
	    ABORT_WDT0_CLR | ABORT_THM_CLR | ABORT_SWRST_CLR;
	writel(abort_cfg, COMM_ABORT_CFG);
	/*enable watchdog, thermal abort function */
	abort_cfg = ABORT_WDT0_EN | ABORT_THM_EN;
	writel(abort_cfg, COMM_ABORT_CFG);
	return abort_status;
}

static void print_boot_reason(void)
{
	u32 tmp;
	tmp = axera_get_boot_reason();
	printf("boot_reason:%d\n",tmp);
	if (tmp & (1 << 4)) {
		printf("wdt4 reset\n!");
	}
	if (tmp & (1 << 3)) {
		printf("wdt0 reset\n!");
	}
	if (tmp & (1 << 1)) {
		printf("thm reset\n!");
	}
	if (tmp & (1 << 0)) {
		printf("swrst reset\n!");
	}
}

static void print_board_info(void)
{
	print_chip_type();
	print_board_id();
	print_plate_id();
}

//#define SATA_LIMIT_1G5
//#define SATA_LIMIT_3G
#define AX_PIPE_DEBUG
static void core_write_word(ulong reg, u32 val)
{
#ifdef AX_PIPE_DEBUG
	printf("write reg addr: 0x%lx, data: 0x%x\n", reg, val);
#endif
	writel(val, reg);
}

static u32 core_read_word(ulong reg)
{
	u32 val = readl(reg);
#ifdef AX_PIPE_DEBUG
	printf("read reg addr: 0x%lx, data: 0x%x\n", reg, val);
#endif
	return val;
}

static void axera_pcie_init(void)
{
	struct udevice *dev;

	/* master reset slave */
	gpio_request(PCIE1_PERST_GPIO, "pcie-perst");
	gpio_direction_output(PCIE1_PERST_GPIO, 1);
	gpio_set_value(PCIE1_PERST_GPIO, 0);
	udelay(200);
	gpio_set_value(PCIE1_PERST_GPIO, 1);

#ifdef CONFIG_DM_PCI
	pci_init();
#ifdef CONFIG_NVME
	nvme_scan_namespace();
#endif
#endif
	for (uclass_first_device(UCLASS_PCI_EP, &dev);
  	     dev;
  	     uclass_next_device(&dev)) {
			;
	}
}

#ifdef PCIE_SATA_CFG_VIA_BOARDID
extern int adc_read_boardid(int channel, unsigned int *data);

#define GPIO4_BASE			0x02007000
#define PORTA_DR			0x00
#define PORTA_DDR			0x04
#define PORTA_CTL			0x08
#define PCIE0_PRSNTN		6			// GPIO4_A6
static void axera_pcie0_ext_clk_en(void)
{
	u32 val;

	//software mode
	val = core_read_word((GPIO4_BASE + PORTA_CTL));
	val &= ~ BIT(PCIE0_PRSNTN);
	core_write_word((GPIO4_BASE + PORTA_CTL), val);
	//direction
	val = core_read_word((GPIO4_BASE + PORTA_DDR));
	val |= BIT(PCIE0_PRSNTN);
	core_write_word((GPIO4_BASE + PORTA_DDR), val);
	//default low
	val = core_read_word((GPIO4_BASE + PORTA_DR));
	val &= ~ BIT(PCIE0_PRSNTN);
	core_write_word((GPIO4_BASE + PORTA_DR), val);
}
#endif

#if 0
static int pipephy_los_bias_config(int port, u8 val)
{
	int ret = -1;

	if (!val || (val > 0x7)) {
		printf("%s: val 0x%x error\n", __FUNCTION__, val);
		return -1;
	}

	switch (port) {
	case 0:
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_2_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, (0x7 << PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_LOS_BIAS_LSB));
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, (val << PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_LOS_BIAS_LSB));
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_2_ADDR);
		ret = 0;
		break;

	case 1:
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_3_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, (0x7 << PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_LOS_BIAS_LSB));
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, (val << PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_LOS_BIAS_LSB));
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_3_ADDR);
		ret = 0;
		break;

	case 2:
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_2_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, (0x7 << PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_LOS_BIAS_LSB));
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, (val << PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_LOS_BIAS_LSB));
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_2_ADDR);
		break;

	case 3:
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_3_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, (0x7 << PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_LOS_BIAS_LSB));
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, (val << PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_LOS_BIAS_LSB));
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_3_ADDR);
		break;

	default:
		printf("%s: port %d error\n", __FUNCTION__, port);
		ret = -1;
		break;
	}

	return ret;
}
#endif

static void sata_port_cfg_getenv(int port)
{
	u8 gen1_amplitude = 0;
	u8 gen2_amplitude = 0;
	u8 gen3_amplitude = 0;
	u8 gen2_preemph = 0;
	u8 gen3_preemph = 0;
	u8 vboost = 0;

	switch (port)
	{
	case 0:
		gen1_amplitude = (u8)simple_strtoul(env_get("p0_g1_ampl"), NULL, 16);
		gen2_amplitude = (u8)simple_strtoul(env_get("p0_g2_ampl"), NULL, 16);
		gen3_amplitude = (u8)simple_strtoul(env_get("p0_g3_ampl"), NULL, 16);
		gen2_preemph = (u8)simple_strtoul(env_get("p0_g2_preemph"), NULL, 16);
		gen3_preemph = (u8)simple_strtoul(env_get("p0_g3_preemph"), NULL, 16);
		vboost = (u8)simple_strtoul(env_get("p0_vboost"), NULL, 16);
		break;
	case 1:
		gen1_amplitude = (u8)simple_strtoul(env_get("p1_g1_ampl"), NULL, 16);
		gen2_amplitude = (u8)simple_strtoul(env_get("p1_g2_ampl"), NULL, 16);
		gen3_amplitude = (u8)simple_strtoul(env_get("p1_g3_ampl"), NULL, 16);
		gen2_preemph = (u8)simple_strtoul(env_get("p1_g2_preemph"), NULL, 16);
		gen3_preemph = (u8)simple_strtoul(env_get("p1_g3_preemph"), NULL, 16);
		vboost = (u8)simple_strtoul(env_get("p1_vboost"), NULL, 16);
		break;
	case 2:
		gen1_amplitude = (u8)simple_strtoul(env_get("p2_g1_ampl"), NULL, 16);
		gen2_amplitude = (u8)simple_strtoul(env_get("p2_g2_ampl"), NULL, 16);
		gen3_amplitude = (u8)simple_strtoul(env_get("p2_g3_ampl"), NULL, 16);
		gen2_preemph = (u8)simple_strtoul(env_get("p2_g2_preemph"), NULL, 16);
		gen3_preemph = (u8)simple_strtoul(env_get("p2_g3_preemph"), NULL, 16);
		vboost = (u8)simple_strtoul(env_get("p2_vboost"), NULL, 16);
		break;
	case 3:
		gen1_amplitude = (u8)simple_strtoul(env_get("p3_g1_ampl"), NULL, 16);
		gen2_amplitude = (u8)simple_strtoul(env_get("p3_g2_ampl"), NULL, 16);
		gen3_amplitude = (u8)simple_strtoul(env_get("p3_g3_ampl"), NULL, 16);
		gen2_preemph = (u8)simple_strtoul(env_get("p3_g2_preemph"), NULL, 16);
		gen3_preemph = (u8)simple_strtoul(env_get("p3_g3_preemph"), NULL, 16);
		vboost = (u8)simple_strtoul(env_get("p3_vboost"), NULL, 16);
		break;
	default:
		break;
	}

	if (gen1_amplitude > 0 && gen1_amplitude <= 0x7f)
		port_cfg[port].gen1_amplitude = gen1_amplitude;
	if (gen2_amplitude > 0 && gen2_amplitude <= 0x7f)
		port_cfg[port].gen2_amplitude = gen2_amplitude;
	if (gen3_amplitude > 0 && gen3_amplitude <= 0x7f)
		port_cfg[port].gen3_amplitude = gen3_amplitude;
	if (gen2_preemph > 0 && gen2_preemph <= 0x3f)
		port_cfg[port].gen2_preemph = gen2_preemph;
	if (gen3_preemph > 0 && gen3_preemph <= 0x3f)
		port_cfg[port].gen3_preemph = gen3_preemph;
	if (vboost > 0 && vboost <= 7)
		port_cfg[port].vboost = vboost;

	printf("port[%d]: g1_ampl = 0x%x, g2_ampl = 0x%x, g3_ampl = 0x%x\n", port, port_cfg[port].gen1_amplitude, port_cfg[port].gen2_amplitude, port_cfg[port].gen3_amplitude);
	printf("port[%d]: g2_preemph = 0x%x, g3_preemph = 0x%x\n", port, port_cfg[port].gen2_preemph, port_cfg[port].gen3_preemph);
	printf("port[%d]: vboost = 0x%x\n", port, port_cfg[port].vboost);
}

static int pipephy_tx_amplitude_config(int port, int gen, u8 val)
{
	int ret = -1;
	int shift = 0;

	if (!val || (val > 0x7f)) {
		printf("%s: val 0x%x error\n", __FUNCTION__, val);
		return -1;
	}

	if (!gen || (gen > 3)) {
		printf("%s: gen %d error\n", __FUNCTION__, gen);
		return -1;
	}

	switch (port) {
	case 0:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL0_5_SATA0_P0_TX_AMPLITUDE_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL0_5_SATA0_P0_TX_AMPLITUDE_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL0_5_SATA0_P0_TX_AMPLITUDE_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_5_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_5_CLR_ADDR, (0x7f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_5_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_5_ADDR);
		ret = 0;
		break;

	case 1:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL0_7_SATA1_P0_TX_AMPLITUDE_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL0_7_SATA1_P0_TX_AMPLITUDE_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL0_7_SATA1_P0_TX_AMPLITUDE_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_7_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_7_CLR_ADDR, (0x7f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_7_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_7_ADDR);
		ret = 0;
		break;

	case 2:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL1_5_SATA0_P0_TX_AMPLITUDE_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL1_5_SATA0_P0_TX_AMPLITUDE_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL1_5_SATA0_P0_TX_AMPLITUDE_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_5_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_5_CLR_ADDR, (0x7f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_5_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_5_ADDR);
		break;

	case 3:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL1_7_SATA1_P0_TX_AMPLITUDE_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL1_7_SATA1_P0_TX_AMPLITUDE_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL1_7_SATA1_P0_TX_AMPLITUDE_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_7_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_7_CLR_ADDR, (0x7f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_7_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_7_ADDR);
		break;

	default:
		printf("%s: port %d error\n", __FUNCTION__, port);
		ret = -1;
		break;
	}

	return ret;
}

static int pipephy_tx_preemph_config(int port, int gen, u8 val)
{
	int ret = -1;
	int shift = 0;

	if (!val || (val > 0x3f)) {
		printf("%s: val 0x%x error\n", __FUNCTION__, val);
		return -1;
	}

	if (!gen || (gen > 3)) {
		printf("%s: gen %d error\n", __FUNCTION__, gen);
		return -1;
	}

	switch (port) {
	case 0:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL0_6_SATA0_P0_TX_PREEMPH_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL0_6_SATA0_P0_TX_PREEMPH_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL0_6_SATA0_P0_TX_PREEMPH_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_6_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_6_CLR_ADDR, (0x3f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_6_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_6_ADDR);
		ret = 0;
		break;

	case 1:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL0_8_SATA1_P0_TX_PREEMPH_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL0_8_SATA1_P0_TX_PREEMPH_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL0_8_SATA1_P0_TX_PREEMPH_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_8_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_8_CLR_ADDR, (0x3f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_8_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL0_8_ADDR);
		ret = 0;
		break;

	case 2:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL1_6_SATA0_P0_TX_PREEMPH_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL1_6_SATA0_P0_TX_PREEMPH_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL1_6_SATA0_P0_TX_PREEMPH_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_6_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_6_CLR_ADDR, (0x3f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_6_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_6_ADDR);
		break;

	case 3:
		shift = (3 == gen) ? PIPE_PHY_GLB_PCS_CTL1_8_SATA1_P0_TX_PREEMPH_GEN3_LSB :
			((2 == gen) ? PIPE_PHY_GLB_PCS_CTL1_8_SATA1_P0_TX_PREEMPH_GEN2_LSB : PIPE_PHY_GLB_PCS_CTL1_8_SATA1_P0_TX_PREEMPH_GEN1_LSB);
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_8_ADDR);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_8_CLR_ADDR, (0x3f << shift));
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_8_SET_ADDR, (val << shift));
		core_read_word(PIPE_PHY_GLB_PCS_CTL1_8_ADDR);
		break;

	default:
		printf("%s: port %d error\n", __FUNCTION__, port);
		ret = -1;
		break;
	}

	return ret;
}

static int pipe_phy_pre_config(void)
{
	int rc = 0;

	switch (pipe_pcs0_model_sel) {
	case 0:
		sata_port_cfg_getenv(0);
		pipephy_tx_amplitude_config(0, 1, port_cfg[0].gen1_amplitude);
		pipephy_tx_amplitude_config(0, 2, port_cfg[0].gen2_amplitude);
		pipephy_tx_amplitude_config(0, 3, port_cfg[0].gen3_amplitude);
		pipephy_tx_preemph_config(0, 2, port_cfg[0].gen2_preemph);
		pipephy_tx_preemph_config(0, 3, port_cfg[0].gen3_preemph);
	case 1:
		sata_port_cfg_getenv(1);
		pipephy_tx_amplitude_config(1, 1, port_cfg[1].gen1_amplitude);
		pipephy_tx_amplitude_config(1, 2, port_cfg[1].gen2_amplitude);
		pipephy_tx_amplitude_config(1, 3, port_cfg[1].gen3_amplitude);
		pipephy_tx_preemph_config(1, 2, port_cfg[1].gen2_preemph);
		pipephy_tx_preemph_config(1, 3, port_cfg[1].gen3_preemph);
		break;

	case 2:
		break;

	default:
		printf("%s: pipe_pcs0_model_sel=%d is wrong\n", __func__, pipe_pcs0_model_sel);
		rc = -1;
		break;
	}

	if (rc)
		return -1;

    // the following configuration is for 750 pcie
	if (AX750_CHIP == get_chip_type_id()){
		u32 reg_val = 0;
		//1. enable rtc clock and input passwd
		reg_val = core_read_word(0x10003c);
		reg_val |= 1 << 8;
		core_write_word(0x10003c, reg_val);
		core_write_word(0x285FD000, 0x61696370);

		//2. change rtc_rstn_out pad to gpio function
		core_write_word(0x285FD1EC, 0x6);
		core_read_word(0x285FD1EC);

		//3. pull low voltage of rtc_rstn_out pad
		reg_val = core_read_word(0x2006004);
		reg_val |= 0x1 << 29;
		core_write_word(0x2006004, reg_val);

		reg_val = core_read_word(0x2006000);
		reg_val &= ~(0x1 << 29);
		core_write_word(0x2006000, reg_val);

		printf("enable 750 pcie alpha/beta by config \n");
	}

	if (AX650N_CHIP != get_chip_type_id())
		return 0;

	switch (pipe_pcs1_model_sel) {
	case 0:
		sata_port_cfg_getenv(2);
		pipephy_tx_amplitude_config(2, 1, port_cfg[2].gen1_amplitude);
		pipephy_tx_amplitude_config(2, 2, port_cfg[2].gen2_amplitude);
		pipephy_tx_amplitude_config(2, 3, port_cfg[2].gen3_amplitude);
		pipephy_tx_preemph_config(2, 2, port_cfg[2].gen2_preemph);
		pipephy_tx_preemph_config(2, 3, port_cfg[2].gen3_preemph);
	case 1:
		sata_port_cfg_getenv(3);
		pipephy_tx_amplitude_config(3, 1, port_cfg[3].gen1_amplitude);
		pipephy_tx_amplitude_config(3, 2, port_cfg[3].gen1_amplitude);
		pipephy_tx_amplitude_config(3, 3, port_cfg[3].gen1_amplitude);
		pipephy_tx_preemph_config(3, 2, port_cfg[3].gen2_preemph);
		pipephy_tx_preemph_config(3, 3, port_cfg[3].gen3_preemph);
		break;

	case 2:
		break;

	default:
		printf("%s: pipe_pcs0_model_sel=%d is wrong\n", __func__, pipe_pcs1_model_sel);
		rc = -1;
		break;
	}

	if (rc)
		return -1;

	return 0;
}

static int pcie_sata_mode_init(void)
{
	u32 wdata, rdata;
	u32 sata0_spdmode = 0x2;
	u32 sata1_spdmode = 0x2;
	u32 sata2_spdmode = 0x2;
	u32 sata3_spdmode = 0x2;
	int ref_clk_en = 0;
#ifdef PCIE_SATA_CFG_VIA_BOARDID
	u32 adc_data;
	board_type_t board_id = get_board_id();

	//for RD dailybuild test
	adc_read_boardid(2,&adc_data);
	switch (board_id) {
	case AX650N_Demo:
		pipe_pcs0_model_sel = 2;		// CTRL0 for PCIE EP
		pipe_pcs1_model_sel = 2;		// CTRL1 for PCIE RC
		sata0_spdmode = 0x1;
		sata1_spdmode = 0x1;
		sata2_spdmode = 0x1;
		sata3_spdmode = 0x1;
		break;

	case AX650N_SLT:
		pipe_pcs0_model_sel = 0;		// CTRL0 for SATA
		pipe_pcs1_model_sel = 2;		// CTRL1 for PCIE RC
		break;

	case AX650N_EVB:
		if(adc_data > 800) {
			pipe_pcs0_model_sel = 0;	// CTRL0 for SATA
			pipe_pcs1_model_sel = 0;	// CTRL1 for SATA
		}
		else {
			pipe_pcs0_model_sel = 0;	// CTRL0 for SATA
			pipe_pcs1_model_sel = 2;	// CTRL1 for PCIE RC
		}
		break;

	case AX650A_Demo:
		axera_pcie0_ext_clk_en();
		pipe_pcs0_model_sel = 0;		// CTRL0 for SATA
		sata0_spdmode = 0x1;
		sata1_spdmode = 0x1;
		break;

	default:							// 650A EVB/SLT
		pipe_pcs0_model_sel = 2;		// CTRL0 for PCIE EP
		break;
	}
#else
#if defined (PIPE_SYS_LAN0_SATA_LAN1_SATA)
	pipe_pcs0_model_sel = 0;
#elif defined (PIPE_SYS_LAN0_PCIE_LAN1_SATA)
	pipe_pcs0_model_sel = 1;
#elif defined (PIPE_SYS_LAN0_PCIE_LAN1_PCIE)
	pipe_pcs0_model_sel = 2;
#else
	printf("%s: PIPE Lan0/1 CFG Error!!!\n", __func__);
	return -1;
#endif

#if defined (PIPE_SYS_LAN2_SATA_LAN3_SATA)
	pipe_pcs1_model_sel = 0;
#elif defined (PIPE_SYS_LAN2_PCIE_LAN3_SATA)
	pipe_pcs1_model_sel = 1;
#elif defined (PIPE_SYS_LAN2_PCIE_LAN3_PCIE)
	pipe_pcs1_model_sel = 2;
#else
	printf("%s: PIPE LAN2/3 CFG Error!!!\n", __func__);
	return -1;
#endif

#if defined (PIPE_CLK_USE_EXT)
	ref_clk_en = 0;
#elif defined (PIPE_CLK_USE_INTERNEL)
	ref_clk_en = 1;
#else
	printf("%s: PIPE CLK CFG Error!!!\n", __func__);
	return -1;
#endif

#if defined (SATA_LIMIT_1G5)
	sata0_spdmode = 0x0;
	sata1_spdmode = 0x0;
	sata2_spdmode = 0x0;
	sata3_spdmode = 0x0;
#elif defined (SATA_LIMIT_3G)
	sata0_spdmode = 0x1;
	sata1_spdmode = 0x1;
	sata2_spdmode = 0x1;
	sata3_spdmode = 0x1;
#endif
#endif

	core_write_word(PIPE_PHY_GLB_PCS_CTL0_0_SET_ADDR, pipe_pcs0_model_sel);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_0_SET_ADDR, pipe_pcs1_model_sel);

	if (0 != pipe_phy_pre_config())
		return -1;

	if (!ref_clk_en) {
		printf("%s: clock pipe_ref_alt_clk disable, ref_clk_en = %d, pipe_pcs0_model_sel = %d, pipe_pcs1_model_sel = %d\n",
			__func__, ref_clk_en, pipe_pcs0_model_sel, pipe_pcs1_model_sel);
		core_write_word(PIPE_PHY_CK_RST_CFG_CLK_EB_0_CLR_ADDR, 1<<PIPE_PHY_CK_RST_CFG_CLK_EB_0_PIPE_REF_ALT_CLK_EB_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_USE_PAD_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_USE_PAD_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_USE_PAD_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_USE_PAD_LSB);
	}
	else
		printf("%s: clock pipe_ref_alt_clk enable, ref_clk_en = %d, pipe_pcs0_model_sel = %d, pipe_pcs1_model_sel = %d\n",
			__func__, ref_clk_en, pipe_pcs0_model_sel, pipe_pcs1_model_sel);

	core_write_word(PIPE_SYS_CLK_EB1_SET, 1<<PIPE_SYS_CLK_EB1_ACLK_SATA_EB);
#ifdef AX_PIPE_DEBUG
	printf("sata0_spdmode=%0d, sata1_spdmode=%0d, sata2_spdmode=%0d, sata3_spdmode=%0d\n", sata0_spdmode, sata1_spdmode, sata2_spdmode, sata3_spdmode);
#endif
	rdata=0;
	rdata= core_read_word(PIPE_SYS_GLB_SATA0_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB);
	rdata |= (sata0_spdmode << PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB);
	core_write_word(PIPE_SYS_GLB_SATA0_ADDR, rdata);

	rdata=0;
	rdata = core_read_word(PIPE_SYS_GLB_SATA1_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB);
	rdata |= (sata1_spdmode << PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB);
	core_write_word(PIPE_SYS_GLB_SATA1_ADDR, rdata);

	rdata=0;
	rdata = core_read_word(PIPE_SYS_GLB_SATA2_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB);
	rdata |= (sata2_spdmode << PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB);
	core_write_word(PIPE_SYS_GLB_SATA2_ADDR, rdata);

	rdata=0;
	rdata = core_read_word(PIPE_SYS_GLB_SATA3_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB);
	rdata |= (sata3_spdmode << PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB);
	core_write_word(PIPE_SYS_GLB_SATA3_ADDR, rdata);

#if 0
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_SSP_EN_LSB);//0
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_SSP_EN_LSB);//1
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_SSP_EN_LSB);//2
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_SSP_EN_LSB);//3
#endif

	if(pipe_pcs0_model_sel == 0 && pipe_pcs1_model_sel == 0) {//2sata+2sata
		//std::randomize(wdata) with {wdata inside {32'h0, 32'ha, 32'h14, 32'h1e};};
		wdata = 0 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	else if(pipe_pcs0_model_sel == 1 && pipe_pcs1_model_sel == 1) {//1sata+1sata
		//std::randomize(wdata) with {wdata inside {32'ha, 32'h1e};};
		wdata = 0x1 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0x1 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	else if(pipe_pcs0_model_sel == 1 && pipe_pcs1_model_sel == 2) {//1sata+0sata
		//std::randomize(wdata) with {wdata inside {32'ha};};
		wdata = 0x1 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0x1 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	else if(pipe_pcs0_model_sel == 2 && pipe_pcs1_model_sel == 1) {//0sata+1sata
		//std::randomize(wdata) with {wdata inside {32'h1e};};
		wdata = 0x3 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0x3 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	else if(pipe_pcs0_model_sel == 0 && pipe_pcs1_model_sel == 1) {//2sata+1sata
		//std::randomize(wdata) with {wdata inside {32'h0, 32'ha, 32'h1e};};
		wdata = 0 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	else if(pipe_pcs0_model_sel == 1 && pipe_pcs1_model_sel == 0) {//1sata+2sata
		//std::randomize(wdata) with {wdata inside {32'ha, 32'h14, 32'h1e};};
		wdata = 0x1 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0x1 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	else if(pipe_pcs0_model_sel == 2 && pipe_pcs1_model_sel == 0) {//0sata+2sata
		//std::randomize(wdata) with {wdata inside {32'h14, 32'h1e};};
		wdata = 0x2 << PIPE_SYS_GLB_SATA_SET_PMA_CLK_SEL_SET_LSB | 0x2 << PIPE_SYS_GLB_SATA_SET_PMA_RESET_SEL_SET_LSB;
		core_write_word(PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}

	//led oe
	core_write_word(PIPE_SYS_GLB_SATA0_SET_ADDR, 1<<PIPE_SYS_GLB_SATA0_SET_LED_OE_SET_MSB);
	core_write_word(PIPE_SYS_GLB_SATA1_SET_ADDR, 1<<PIPE_SYS_GLB_SATA1_SET_LED_OE_SET_MSB);
	core_write_word(PIPE_SYS_GLB_SATA2_SET_ADDR, 1<<PIPE_SYS_GLB_SATA2_SET_LED_OE_SET_MSB);
	core_write_word(PIPE_SYS_GLB_SATA3_SET_ADDR, 1<<PIPE_SYS_GLB_SATA3_SET_LED_OE_SET_MSB);

	return 0;
}

static void release_pipe_reset(void)
{
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_SSP_EN_LSB); //ref ssp en
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TEST_POWERDOWN_LSB);
	core_write_word(PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
			1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB |
			1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB);
	core_write_word(PIPE_SYS_SW_RESET_SET, 1<<PIPE_SYS_CLK_SW_RESET_SATA_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE0_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE0_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE1_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE1_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE2_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE2_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE3_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE3_POWER_UP_SW_RST);
	core_write_word(PIPE_SYS_SW_RESET_CLR, 1<<PIPE_SYS_CLK_SW_RESET_SATA_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE0_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE0_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE1_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE1_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE2_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE2_POWER_UP_SW_RST |
			1<<PIPE_SYS_CLK_SW_RESET_PCIE3_SW_PERST | 1<<PIPE_SYS_CLK_SW_RESET_PCIE3_POWER_UP_SW_RST);
}

static void release_pipe_phy0_reset(void)
{
	//must release test powerdown first, and then release phy reset
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB);
}

static void release_pipe_phy1_reset(void)
{
	//must release test powerdown first, and then release phy reset
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB);
	core_write_word(PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB |1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB);
}

static void release_pcie0_reset(void)
{
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_4_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_4_PIPE0_RESET_N_LSB);
	core_write_word(PIPE_SYS_GLB_PCIE_CTL0_19_SET_ADDR, 1<<PIPE_SYS_GLB_PCIE_CTL0_19_SET_POWER_UP_RST_N_SET_LSB | 1<<PIPE_SYS_GLB_PCIE_CTL0_19_SET_BUTTON_RST_N_SET_LSB);
}

static void release_pcie1_reset(void)
{
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_4_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_4_PIPE0_RESET_N_LSB);
	core_write_word(PIPE_SYS_GLB_PCIE_CTL1_19_SET_ADDR, 1<<PIPE_SYS_GLB_PCIE_CTL1_19_SET_POWER_UP_RST_N_SET_LSB | 1<<PIPE_SYS_GLB_PCIE_CTL1_19_SET_BUTTON_RST_N_SET_LSB);
}

#define SATA_RX_EQ_VAL (0x2)
static int pipe_phy_config(void)
{
	int rc = 0;
	u32 val;

	switch (pipe_pcs0_model_sel) {
	case 0:
		val = core_read_word(PIPE_PCIE0_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS);
		val |= ((1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_LSB) | (1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_OVRD_LSB));
		core_write_word(PIPE_PCIE0_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS, val);
		val = core_read_word(PIPE_PCIE0_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS);
		val &= ~(0x7 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB);
		val |= ((1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_OVRD_LSB) | (SATA_RX_EQ_VAL << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB) |
				(1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_OVRD_LSB) | (1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_LSB));
		core_write_word(PIPE_PCIE0_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS, val);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 0x7 << PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TX_VBOOST_LVL_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, port_cfg[0].vboost << PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TX_VBOOST_LVL_LSB);
	case 1:
		val = core_read_word(PIPE_PCIE1_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS);
		val |= ((1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_LSB) | (1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_OVRD_LSB));
		core_write_word(PIPE_PCIE1_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS, val);
		val = core_read_word(PIPE_PCIE1_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS);
		val &= ~(0x7 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB);
		val |= ((1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_OVRD_LSB) | (SATA_RX_EQ_VAL << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB) |
				(1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_OVRD_LSB) | (1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_LSB));
		core_write_word(PIPE_PCIE1_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS, val);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 0x7 << PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TX_VBOOST_LVL_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, port_cfg[1].vboost << PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TX_VBOOST_LVL_LSB);
		break;

	case 2:
		break;

	default:
		printf("%s: pipe_pcs0_model_sel=%d is wrong\n", __func__, pipe_pcs0_model_sel);
		rc = -1;
		break;
	}

	if (rc)
		return -1;

	if (AX650N_CHIP != get_chip_type_id())
		return 0;

	switch (pipe_pcs1_model_sel) {
	case 0:
		val = core_read_word(PIPE_PCIE2_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS);
		val |= ((1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_LSB) | (1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_OVRD_LSB));
		core_write_word(PIPE_PCIE2_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS, val);
		val = core_read_word(PIPE_PCIE2_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS);
		val &= ~(0x7 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB);
		val |= ((1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_OVRD_LSB) | (SATA_RX_EQ_VAL << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB) |
				(1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_OVRD_LSB) | (1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_LSB));
		core_write_word(PIPE_PCIE2_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS, val);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 0x7 << PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TX_VBOOST_LVL_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, port_cfg[2].vboost << PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TX_VBOOST_LVL_LSB);
	case 1:
		val = core_read_word(PIPE_PCIE3_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS);
		val |= ((1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_LSB) | (1 << LANEN_DIG_TX_OVRD_IN_HI_TX_VBOOST_EN_OVRD_LSB));
		core_write_word(PIPE_PCIE3_PHY + LANEN_DIG_TX_OVRD_IN_HI_OFFS, val);
		val = core_read_word(PIPE_PCIE3_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS);
		val &= ~(0x7 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB);
		val |= ((1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_OVRD_LSB) | (SATA_RX_EQ_VAL << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_LSB) |
				(1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_OVRD_LSB) | (1 << LANEN_DIG_RX_OVRD_IN_HI_RX_EQ_EN_LSB));
		core_write_word(PIPE_PCIE3_PHY + LANEN_DIG_RX_OVRD_IN_HI_OFFS, val);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 0x7 << PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TX_VBOOST_LVL_LSB);
		core_write_word(PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, port_cfg[3].vboost << PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TX_VBOOST_LVL_LSB);
		break;

	case 2:
		break;

	default:
		printf("%s: pipe_pcs0_model_sel=%d is wrong\n", __func__, pipe_pcs1_model_sel);
		rc = -1;
		break;
	}

	//pcie phy deemph/swing config
#if 0
	core_write_word(PIPEPHY_GLB + 0x14, 0x5965bc78);
	core_write_word(PIPEPHY_GLB + 0x38, 0x5965bc78);
#else
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_1_CLR_ADDR, (0x3f << PIPE_PHY_GLB_PCS_CTL0_1_PCS_TX_DEEMPH_GEN3_6DB_LSB));
	core_write_word(PIPE_PHY_GLB_PCS_CTL0_1_SET_ADDR, (0x16 << PIPE_PHY_GLB_PCS_CTL0_1_PCS_TX_DEEMPH_GEN3_6DB_LSB));
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_1_CLR_ADDR, (0x3f << PIPE_PHY_GLB_PCS_CTL1_1_PCS_TX_DEEMPH_GEN3_6DB_LSB));
	core_write_word(PIPE_PHY_GLB_PCS_CTL1_1_SET_ADDR, (0x16 << PIPE_PHY_GLB_PCS_CTL1_1_PCS_TX_DEEMPH_GEN3_6DB_LSB));
	core_read_word(PIPE_PHY_GLB_PCS_CTL0_1_ADDR);
	core_read_word(PIPE_PHY_GLB_PCS_CTL1_1_ADDR);
#endif

	if (rc)
		return -1;

	return 0;
}

static int axera_pipe_reset(void)
{
	if (0 != pcie_sata_mode_init())
		return -1;

	release_pipe_reset();
	release_pipe_phy0_reset();
	release_pipe_phy1_reset();
	release_pcie0_reset();
	release_pcie1_reset();
	mdelay(5);

	return pipe_phy_config();
}

/* function: secure_enable
* ATTENTION: the sec_sys bit can only write once and cannot erased
* default all peripherals can be accessed either in secure/non-secure world
* if secure enabled, if some peripherals designed as secure devices
* it should configured before enter non-secure world
*/
#ifdef SECURE_ENABLE
#define BOND_OPTION0_BLK      (13)
#define BIT_SEC_SYS           (1 << 25)
static int secure_enable(void)
{
	int val;
	int ret;

	ret = ax_efuse_init();
	if(ret != EFUSE_SUCCESS) {
		printf("%s ax_efuse_init failed\n", __func__);
		return -1;
	}

	ret = ax_efuse_read(BOND_OPTION0_BLK, &val);
	if(ret != EFUSE_SUCCESS) {
		printf("%s ax_efuse_read BOND_OPTION0_BLK failed\n", __func__);
		ax_efuse_deinit();
		return -1;
	}

	if((val & BIT_SEC_SYS) != 0) {
		printf("BIT_SEC_SYS is set yet\n");
		ax_efuse_deinit();
		return 0;
	}

	val |= BIT_SEC_SYS;
	ret = ax_efuse_write(BOND_OPTION0_BLK, val);
	if(ret != EFUSE_SUCCESS) {
		printf("%s ax_efuse_write failed\n", __func__);
		ax_efuse_deinit();
		return -1;
	}

	ax_efuse_deinit();
	return 0;
}
#endif

static int resplice_bootargs(void)
{
	char *bootargs_code_str = NULL;
	char *bootargs_env_str = NULL;
	char new_bootargs[512] = "";

	char *upgrade_flag = env_get("env_upgrade_flag");
	if (!strcmp(upgrade_flag, "set")) {
		pr_debug("upgrade_flag set\n");
		#ifdef CONFIG_TARGET_AX650_SSD
			bootargs_code_str = BOOTARGS_SSD;
		#else
			bootargs_code_str = BOOTARGS_EMMC;
		#endif
		pr_debug("bootargs_code_str: %s\n",bootargs_code_str);
		bootargs_env_str = env_get("bootargs");
		pr_debug("bootargs_env_str: %s\n",bootargs_env_str);
		char *pos = strstr(bootargs_code_str , "blkdevparts");
		/* Copy the part before "blkdevparts" */
		strncpy(new_bootargs, bootargs_code_str, pos - bootargs_code_str);

		strcat(new_bootargs, strstr(bootargs_env_str , "blkdevparts"));

		pr_debug("new bootargs: %s\n",new_bootargs);
		env_set("env_upgrade_flag", "");
		env_set("bootargs", new_bootargs);
	}
	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	print_board_info();
	store_board_id();
	store_chip_type();
	wdt0_enable(1);
	print_boot_reason();
#ifndef CONFIG_SLAVE_PCIE_BOOT
	if (0 != axera_pipe_reset())
		printf("%s: axera_pipe_reset fail!!!\n", __func__);
	axera_pcie_init();
#endif
#if defined(CONFIG_TARGET_AX650_EMMC) || defined(CONFIG_TARGET_AX650_SSD) || defined(CONFIG_TARGET_AX650_XPILOT_6V) || defined(CONFIG_TARGET_AX650_MASTER)
	resplice_bootargs();
#endif
#if defined(CONFIG_CMD_AXERA_DOWNLOAD) || defined(CONFIG_CMD_AXERA_BOOT)
	setup_boot_mode();
#endif
#ifdef SECURE_ENABLE
	if (secure_enable())
		printf("WARNING: some errors happened when enable secure\n");
#endif

	return 0;
}
#endif
