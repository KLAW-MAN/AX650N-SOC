// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Axera AHCI SATA driver
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/libata.h>
#include <linux/ahci_platform.h>
#include "ahci.h"

#define DRV_NAME "ahci-axera"

//#define AX_PIPE_RESET
#ifdef AX_PIPE_RESET
#define PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR      (0x1018)
#define PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR      (0x101C)
#define PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR      (0x103C)
#define PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR      (0x1040)
#define PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR      (0x2018)
#define PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR      (0x201C)
#define PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR      (0x2018)
#define PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR      (0x203C)
#define PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR      (0x2040)

#define PIPE_SYS_GLB_SATA0_ADDR               (0xA4)
#define PIPE_SYS_GLB_SATA1_ADDR               (0xA8)
#define PIPE_SYS_GLB_SATA2_ADDR               (0xAC)
#define PIPE_SYS_GLB_SATA3_ADDR               (0xB0)
#define PIPE_SYS_GLB_SATA0_SET_ADDR           (0x10A4)
#define PIPE_SYS_GLB_SATA1_SET_ADDR           (0x10A8)
#define PIPE_SYS_GLB_SATA2_SET_ADDR           (0x10AC)
#define PIPE_SYS_GLB_SATA3_SET_ADDR           (0x10B0)

#define PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB 2
#define PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB 2
#define PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB 2
#define PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB 2

#define PIPE_SYS_GLB_SATA0_SET_LED_OE_SET_MSB 9
#define PIPE_SYS_GLB_SATA1_SET_LED_OE_SET_MSB 9
#define PIPE_SYS_GLB_SATA2_SET_LED_OE_SET_MSB 9
#define PIPE_SYS_GLB_SATA3_SET_LED_OE_SET_MSB 9

#define PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_SSP_EN_LSB         24
#define PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_SSP_EN_LSB         24
#define PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_SSP_EN_LSB         24
#define PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_SSP_EN_LSB         24

#define PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TEST_POWERDOWN_LSB     4
#define PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TEST_POWERDOWN_LSB     4
#define PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TEST_POWERDOWN_LSB     4
#define PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TEST_POWERDOWN_LSB     4
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_SET_ADDR                   (0x20)
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR                   (0x24)
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB      0
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB      1
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB      2
#define PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB      3
#define PIPE_CK_RST_CFG_CLK_EB_1_SET_ADDR                       (0x28)
#define PIPE_CK_RST_CFG_SW_RST_0_SET_ADDR                       (0x38)
#define PIPE_CK_RST_CFG_SW_RST_0_CLR_ADDR                       (0x3C)
#define PIPE_PHY_GLB_PCS_CTL0_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB  20
#define PIPE_PHY_GLB_PCS_CTL0_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB  20
#define PIPE_PHY_GLB_PCS_CTL1_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB  20
#define PIPE_PHY_GLB_PCS_CTL1_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB  20

#define PIPE_PHY_GLB_PCS_CTL0_0_SET_ADDR                        (0x1010)
#define PIPE_PHY_GLB_PCS_CTL1_0_SET_ADDR                        (0x1034)
#define PIPE_PHY_GLB_PCS_CTL0_2_SET_PHY0_CSR_REF_SSP_EN_SET_MSB 24
#define PIPE_PHY_GLB_PCS_CTL1_2_SET_PHY0_CSR_REF_SSP_EN_SET_MSB 24
#define PIPE_PHY_GLB_PCS_CTL1_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB 24
#define PIPE_PHY_GLB_PCS_CTL0_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB 24
#define PIPE_SYS_GLB_SATA_SET_ADDR                              (0x10A0)

enum {
	PIPE_GLB_REG = 0,
	PIPEPHY_GLB_REG = 1,
	PIPE_CLK_RST_REG = 2,
	PIPEPHY_CK_RST_REG = 3,
};

struct axera_pipe {
	void __iomem *pipe_glb_base;		/* DT pipe sys */
	void __iomem *pipe_clk_rst_base;	/* DT pipe clk */
	void __iomem *pipephy_glb_base;	/* pipe phy base */
	void __iomem *pipephy_ck_rst_base; /* pipe phy clk base */
	phys_addr_t pipe_glb_paddr;		/* DT pipe sys */
	phys_addr_t pipe_clk_rst_paddr;	/* DT pipe clk */
	phys_addr_t pipephy_glb_paddr;	/* pipe phy base */
	phys_addr_t pipephy_ck_rst_paddr; /* pipe phy clk base */
};
#endif

static const struct ata_port_info ahci_axera_port_info = {
	.flags		= AHCI_FLAG_COMMON,
	.pio_mask	= ATA_PIO4,
	.udma_mask	= ATA_UDMA6,
	.port_ops	= &ahci_platform_ops,
};

static struct scsi_host_template ahci_platform_sht = {
	AHCI_SHT(DRV_NAME),
};

#ifdef AX_PIPE_RESET
static void ax_pipe_writel(struct axera_pipe *axera_sata_phy, int reg_type, u32 offset, u32 val)
{
	void __iomem *base;
	phys_addr_t paddr;

	switch (reg_type) {
	case PIPE_GLB_REG:
		base = axera_sata_phy->pipe_glb_base;
		paddr = axera_sata_phy->pipe_glb_paddr;
		break;

	case PIPEPHY_GLB_REG:
		base = axera_sata_phy->pipephy_glb_base;
		paddr = axera_sata_phy->pipephy_glb_paddr;
		break;

	case PIPE_CLK_RST_REG:
		base = axera_sata_phy->pipe_clk_rst_base;
		paddr = axera_sata_phy->pipe_clk_rst_paddr;
		break;

	case PIPEPHY_CK_RST_REG:
		base = axera_sata_phy->pipephy_ck_rst_base;
		paddr = axera_sata_phy->pipephy_ck_rst_paddr;
		break;
	}
	//printk("write reg addr: 0x%llx, data: 0x%x\n", paddr + offset, val);
	__raw_writel(val, base + offset);
}

static u32 ax_pipe_readl(struct axera_pipe *axera_sata_phy, int reg_type, u32 offset)
{
	void __iomem *base;
	phys_addr_t paddr;
	u32 val;

	switch (reg_type) {
	case PIPE_GLB_REG:
		base = axera_sata_phy->pipe_glb_base;
		paddr = axera_sata_phy->pipe_glb_paddr;
		break;

	case PIPEPHY_GLB_REG:
		base = axera_sata_phy->pipephy_glb_base;
		paddr = axera_sata_phy->pipephy_glb_paddr;
		break;

	case PIPE_CLK_RST_REG:
		base = axera_sata_phy->pipe_clk_rst_base;
		paddr = axera_sata_phy->pipe_clk_rst_paddr;
		break;

	case PIPEPHY_CK_RST_REG:
		base = axera_sata_phy->pipephy_ck_rst_base;
		paddr = axera_sata_phy->pipephy_ck_rst_paddr;
		break;
	}
	val = __raw_readl(base + offset);
	//printk("read reg addr: 0x%llx, data: 0x%x\n", paddr + offset, val);
	return val;
}

//#define SATA_SPEED_LIMIT
static void sata_mode_init(struct axera_pipe *axpipe)
{
	u32 wdata = 0, rdata;
	u32 sata0_spdmode, sata1_spdmode, sata2_spdmode, sata3_spdmode;
	u32 pipe_pcs0_model_sel;
	u32 pipe_pcs1_model_sel;

#ifdef SATA_SPEED_LIMIT
	sata0_spdmode = 0x0; // 1.5 Gb/s
	sata1_spdmode = 0x0; // 1.5 Gb/s
	sata2_spdmode = 0x0; // 1.5 Gb/s
	sata3_spdmode = 0x0; // 1.5 Gb/s
#else
	sata0_spdmode = 0x2; // 6.0 Gb/s
	sata1_spdmode = 0x2; // 6.0 Gb/s
	sata2_spdmode = 0x2; // 6.0 Gb/s
	sata3_spdmode = 0x2; // 6.0 Gb/s
#endif
	pipe_pcs0_model_sel = 0;
	pipe_pcs1_model_sel = 0;
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_0_SET_ADDR, pipe_pcs0_model_sel);
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_0_SET_ADDR, pipe_pcs1_model_sel);
	rdata = 0;
	rdata = ax_pipe_readl(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA0_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB);
	rdata |= (sata0_spdmode << PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB);
	//rdata[PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_MSB:PIPE_SYS_GLB_SATA0_P0_PHY_SPDMODE_LSB]=sata0_spdmode;
	ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA0_ADDR, rdata);
	rdata = 0;
	rdata = ax_pipe_readl(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA1_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB);
	rdata |= (sata1_spdmode << PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB);
	//rdata[PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_MSB:PIPE_SYS_GLB_SATA1_P0_PHY_SPDMODE_LSB]=sata1_spdmode;
	ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA1_ADDR, rdata);
	rdata = 0;
	rdata = ax_pipe_readl(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA2_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB);
	rdata |= (sata2_spdmode << PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB);
	//rdata[PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_MSB:PIPE_SYS_GLB_SATA2_P0_PHY_SPDMODE_LSB]=sata2_spdmode;
	ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA2_ADDR, rdata);
	rdata = 0;
	rdata = ax_pipe_readl(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA3_ADDR);
	rdata &= ~(0x3 << PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB);
	rdata |= (sata3_spdmode << PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB);
	//rdata[PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_MSB:PIPE_SYS_GLB_SATA3_P0_PHY_SPDMODE_LSB]=sata3_spdmode;
	ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA3_ADDR, rdata);
	//printf("sata0_spdmode=%0d, sata1_spdmode=%0d, sata2_spdmode=%0d, sata3_spdmode=%0d\n", sata0_spdmode, sata1_spdmode, sata2_spdmode, sata3_spdmode);

	if (pipe_pcs0_model_sel == 0 && pipe_pcs1_model_sel == 0) {//2sata+2sata
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_SET_PHY0_CSR_REF_SSP_EN_SET_MSB);//0
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//1
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_SET_PHY0_CSR_REF_SSP_EN_SET_MSB);//2
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//3
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	} else if (pipe_pcs0_model_sel == 1 && pipe_pcs1_model_sel == 1) {//1sata+1sata
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//1
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//3
		//std::randomize(wdata) with {wdata inside {32'ha, 32'h1e};};
		wdata = 0x1e;
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	} else if (pipe_pcs0_model_sel == 1 && pipe_pcs1_model_sel == 2) {//1sata+0sata
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//1
		//std::randomize(wdata) with {wdata inside {32'ha};};
		wdata = 0xa;
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	} else if (pipe_pcs0_model_sel == 2 && pipe_pcs1_model_sel == 1) {//0sata+1sata
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//3
		//std::randomize(wdata) with {wdata inside {32'h1e};};
		wdata = 0x1e;
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	} else if (pipe_pcs0_model_sel == 0 && pipe_pcs1_model_sel == 1) {//2sata+1sata
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_SET_PHY0_CSR_REF_SSP_EN_SET_MSB);//0
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//1
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//3
		//std::randomize(wdata) with {wdata inside {32'h0, 32'ha, 32'h1e};};
		wdata = 0x1e;
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	} else if (pipe_pcs0_model_sel == 1 && pipe_pcs1_model_sel == 0) {//1sata+2sata
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//1
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_SET_PHY0_CSR_REF_SSP_EN_SET_MSB);//2
		ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_SET_PHY1_CSR_REF_SSP_EN_SET_MSB);//3
		//std::randomize(wdata) with {wdata inside {32'ha, 32'h14, 32'h1e};};
		wdata = 0x1e;
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA_SET_ADDR, wdata);
	}
	if (0) {
		//led oe
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA0_SET_ADDR, 1<<PIPE_SYS_GLB_SATA0_SET_LED_OE_SET_MSB);
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA1_SET_ADDR, 1<<PIPE_SYS_GLB_SATA1_SET_LED_OE_SET_MSB);
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA2_SET_ADDR, 1<<PIPE_SYS_GLB_SATA2_SET_LED_OE_SET_MSB);
		ax_pipe_writel(axpipe, PIPE_GLB_REG, PIPE_SYS_GLB_SATA3_SET_ADDR, 1<<PIPE_SYS_GLB_SATA3_SET_LED_OE_SET_MSB);
	}
}

static void release_pipe_reset(struct axera_pipe *axpipe)
{
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_REF_SSP_EN_LSB); //ref ssp en
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_2_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_REF_SSP_EN_LSB); //ref ssp en
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_REF_SSP_EN_LSB); //ref ssp en
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_SET_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_REF_SSP_EN_LSB); //ref ssp en
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_PHY0_CSR_TEST_POWERDOWN_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_PHY1_CSR_TEST_POWERDOWN_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_PHY0_CSR_TEST_POWERDOWN_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_PHY1_CSR_TEST_POWERDOWN_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_CK_RST_REG, PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB |
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB);
	ax_pipe_writel(axpipe, PIPE_CLK_RST_REG, PIPE_CK_RST_CFG_SW_RST_0_SET_ADDR, 0x00000ff8);
	ax_pipe_writel(axpipe, PIPE_CLK_RST_REG, PIPE_CK_RST_CFG_SW_RST_0_CLR_ADDR, 0x00000ff8);
}

static void release_pipe_phy0_reset(struct axera_pipe *axpipe)
{
	//must release test powerdown first, and then release phy reset
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL0_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL0_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_CK_RST_REG, PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE0_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE1_PHY_SW_PRST_LSB);
}

static void release_pipe_phy1_reset(struct axera_pipe *axpipe)
{
	//must release test powerdown first, and then release phy reset
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_2_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_2_CLR_PHY0_CSR_PHY_RESET_CLR_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_GLB_REG, PIPE_PHY_GLB_PCS_CTL1_3_CLR_ADDR, 1<<PIPE_PHY_GLB_PCS_CTL1_3_CLR_PHY1_CSR_PHY_RESET_CLR_LSB);
	ax_pipe_writel(axpipe, PIPEPHY_CK_RST_REG, PIPE_PHY_CK_RST_CFG_SW_RST_0_CLR_ADDR,
		1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE2_PHY_SW_PRST_LSB | 1<<PIPE_PHY_CK_RST_CFG_SW_RST_0_PCIE3_PHY_SW_PRST_LSB);
}

static int ahci_axera_phy_init(struct platform_device *pdev, struct axera_pipe *axera_sata_phy)
{
	struct resource *res;
	struct device *dev = &pdev->dev;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pipe_glb");
	axera_sata_phy->pipe_glb_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(axera_sata_phy->pipe_glb_base))
		return PTR_ERR(axera_sata_phy->pipe_glb_base);
	axera_sata_phy->pipe_glb_paddr = res->start;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pipe_clk_rst");
	axera_sata_phy->pipe_clk_rst_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(axera_sata_phy->pipe_clk_rst_base))
		return PTR_ERR(axera_sata_phy->pipe_clk_rst_base);
	axera_sata_phy->pipe_clk_rst_paddr = res->start;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pipephy_glb");
	axera_sata_phy->pipephy_glb_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(axera_sata_phy->pipephy_glb_base))
		return PTR_ERR(axera_sata_phy->pipephy_glb_base);
	axera_sata_phy->pipephy_glb_paddr = res->start;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pipephy_ck_rst");
	axera_sata_phy->pipephy_ck_rst_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(axera_sata_phy->pipephy_ck_rst_base))
		return PTR_ERR(axera_sata_phy->pipephy_ck_rst_base);
	axera_sata_phy->pipephy_ck_rst_paddr = res->start;
	/*printk("%s:\npipe_glb: 0x%llx\npipe_clk_rst: 0x%llx\npipephy_glb: 0x%llx\npipephy_ck_rst: 0x%llx\n",
		__FUNCTION__,
		axera_sata_phy->pipe_glb_paddr, axera_sata_phy->pipe_clk_rst_paddr,
		axera_sata_phy->pipephy_glb_paddr, axera_sata_phy->pipephy_ck_rst_paddr);*/

	sata_mode_init(axera_sata_phy);
	release_pipe_reset(axera_sata_phy);
	release_pipe_phy0_reset(axera_sata_phy);
	release_pipe_phy1_reset(axera_sata_phy);

	return 0;
};
#endif

static int ahci_axera_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ahci_host_priv *hpriv;
	int rc;
#ifdef AX_PIPE_RESET
	struct axera_pipe *axera_sata_phy;

	axera_sata_phy = devm_kzalloc(dev, sizeof(struct axera_pipe), GFP_KERNEL);
	if (!axera_sata_phy)
		return -ENOMEM;
#endif

	hpriv = ahci_platform_get_resources(pdev,
					    AHCI_PLATFORM_GET_RESETS);
	if (IS_ERR(hpriv))
		return PTR_ERR(hpriv);

	rc = ahci_platform_enable_resources(hpriv);
	if (rc)
		return rc;

#ifdef AX_PIPE_RESET
	rc = ahci_axera_phy_init(pdev, axera_sata_phy);
	if (rc)
		goto disable_resources;
#endif

	of_property_read_u32(dev->of_node,
			     "ports-implemented", &hpriv->force_port_map);

	rc = ahci_platform_init_host(pdev, hpriv, &ahci_axera_port_info, &ahci_platform_sht);
	if (rc)
		goto disable_resources;

	return 0;
disable_resources:
	ahci_platform_disable_resources(hpriv);
	return rc;
}

static SIMPLE_DEV_PM_OPS(ahci_axera_pm_ops, ahci_platform_suspend,
			 ahci_platform_resume);

static const struct of_device_id ahci_axera_of_match[] = {
	{ .compatible = "axera,ax650-ahci", },
	{},
};
MODULE_DEVICE_TABLE(of, ahci_axera_of_match);

static struct platform_driver ahci_axera_driver = {
	.probe = ahci_axera_probe,
	.remove = ata_platform_remove_one,
	.shutdown = ahci_platform_shutdown,
	.driver = {
		.name = DRV_NAME,
		.of_match_table = ahci_axera_of_match,
		.pm = &ahci_axera_pm_ops,
	},
};
module_platform_driver(ahci_axera_driver);

MODULE_DESCRIPTION("Axera AHCI SATA driver");
MODULE_AUTHOR("Chi Chen <chenchi@axera-tech.com>");
MODULE_LICENSE("GPL");
