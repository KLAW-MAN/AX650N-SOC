#ifndef _LINUX_AX650_PCIE_H
#define _LINUX_AX650_PCIE_H

#include <linux/dma-mapping.h>
#include <linux/irq.h>
#include <linux/io.h>
#include <linux/msi.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include "pcie-designware.h"


/* Pipe sys register offset */
#define	PIPE_PCIE0_LTSSM_REG			0x100	/* bit[4] disable: enable:1 */
#define	PIPE_PCIE1_LTSSM_REG			0x188	/* bit[4] disable: enable:1 */
#define	PIPE_PCIE0_PWRUP_BUTTON_RST		0x10c	/* bit[15:14]=1 is power_up_rst and button_rst_n */
#define	PIPE_PCIE1_PWRUP_BUTTON_RST		0x194	/* bit[15:14]=1 is power_up_rst and button_rst_n */
#define	PIPE_PCIE0_DEVICE_TYPE			0x10c	/* bit[3:0]=0xf is EP mode */
#define	PIPE_PCIE1_DEVICE_TYPE			0x194	/* bit[3:0]=0xf is EP mode */


#define	PIPE_PCIE0_APP_HOLD_PHY_RST		0x110	/* bit[14]=0 set enable linktraing */
#define	PIPE_PCIE1_APP_HOLD_PHY_RST		0x198	/* bit[14]=0 set enable linktraing */
#define	PIPE_PCIE0_ARMISC			0xd0	/* bit[21]=1 set enable dbi inferface */
#define	PIPE_PCIE0_AWMISC			0xc0	/* bit[21]=1 set enable dbi inferface */
#define	PIPE_PCIE1_ARMISC			0x158	/* bit[21]=1 set enable dbi inferface */
#define	PIPE_PCIE1_AWMISC			0x148	/* bit[21]=1 set enable dbi inferface */

/* Pipe clk register offset */
#define	PIPE_AUX_CLK_PCIE		0x4	/* bit[0]=1 to set aux_clk_pcie */
#define	PIPE_SW_RST			0x10	/* bit[6:3]=0 to set pcie0_sw_rst */
#define	PIPE_CLK_PIPE_BUS_SEL		0x0	/* bit[7:6]=0x3 set bus clk to 500M */

/* Pipe phy register offset */
#define PIPE_PHY_GLB_BASE		0x30700000
#define	PCIE0_PIPE_PHY_MODEL_SEL_OFFSET	0x10
#define	PCIE1_PIPE_PHY_MODEL_SEL_OFFSET	0x34
#define	PCIE0_PHY0_CSR_REF_SSP_EN	0x18	/* bit[24]=1 set ref ssp enable */
#define	PCIE0_PHY1_CSR_REF_SSP_EN	0x1c	/* bit[24]=1 set ref ssp enable */
#define	PCIE1_PHY0_CSR_REF_SSP_EN	0x3C	/* bit[24]=1 set ref ssp enable */
#define	PCIE1_PHY1_CSR_REF_SSP_EN	0x40	/* bit[24]=1 set ref ssp enable */
#define	PCIE0_PHY0_CSR_REF_USE_PAD	0x18	/* bit[18]=1 ref use pad */
#define	PCIE0_PHY1_CSR_REF_USE_PAD	0x1c	/* bit[18]=1 ref use pad */
#define	PCIE1_PHY0_CSR_REF_USE_PAD	0x3c	/* bit[18]=1 ref use pad */
#define	PCIE1_PHY1_CSR_REF_USE_PAD	0x40	/* bit[18]=1 ref use pad */
#define	PIPE_PCIE0_PHY_MODEL_SEL	0x10	/* bit[1:0]=2 is pcie0 x2 */
#define	PIPE_PCIE1_PHY_MODEL_SEL	0x34	/* bit[1:0]=2 is pcie1 x2 */
#define	PCIE0_PHY0_CSR_TEST_PWRDOWN	0x18	/* bit[4]=0 clr phy0 powerdown */
											/* bit[20]=0 clr phy0 phy_reset  */
#define	PCIE0_PHY1_CSR_TEST_PWRDOWN	0x1c	/* bit[4]=0 clr phy1 powerdown */
											/* bit[20]=0 clr phy1 phy_reset  */
#define	PCIE1_PHY0_CSR_TEST_PWRDOWN	0x3c	/* bit[4]=0 clr phy0 powerdown */
											/* bit[20]=0 clr phy0 phy_reset  */
#define	PCIE1_PHY1_CSR_TEST_PWRDOWN	0x40	/* bit[4]=0 clr phy1 powerdown */
											/* bit[20]=0 clr phy1 phy_reset  */
#define	PIPE_PIPE0_RESET_N		0x20	/* bit[0]=1 set pipe0_reset_n */

/* Pipe phy clk register offset */
#define	PIPE_PHY_SW_PRST		0x8	/* bit[1:0]=0 clr phy0/1 sw prst */

/* Pipe msi register offset */
#define	PIPE_PCIE0_VEN_MSI_REG		0xD8
#define	PIPE_PCIE1_VEN_MSI_REG		0x160
#define	PIPE_PCIE0_REG_UPDATE		0xE0
#define	PIPE_PCIE1_REG_UPDATE		0x168

#define	PCIE_LINK_ASPM_CAP	0x7c

#define	PCIE_PL_DEBUG0_OFF		0x728
#define	PCIE0_ID	0
#define	PCIE1_ID	1
#define	PCIE_PERST_DELAY_US		200

#define	PCIE_LTSSM_STATUS_L0	0x11

#define PLR_OFFSET			0x700
#define PCIE_PHY_DEBUG_R0		(PLR_OFFSET + 0x28)
#define PCIE_PHY_DEBUG_R0_LINK_STATE	0x3f
#define PCIE_PHY_DEBUG_R0_LINK_UP	0x11

#define	AWRMISC_EN		(0x1 << 21)
#define	PHY0_REF_SSP_EN		(0x1 << 24)
#define	PHY1_REF_SSP_EN		(0x1 << 24)
#define	PHY0_REF_USE_PAD	(0x1 << 18)
#define	PHY1_REF_USE_PAD	(0x1 << 18)
#define	PHY0_CSR_TEST_POWERDOWN	(1 << 4)
#define	PHY1_CSR_TEST_POWERDOWN	(1 << 4)
#define	PIPE0_RESET_N		0x1
#define	PHY0_CSR_PHY_RST	(0x1 << 20)
#define	PHY1_CSR_PHY_RST	(0x1 << 20)
#define	POWER_UP_BUTTON_N_RST	(0x3 << 14)
#define	APP_HOLD_PHY_RST	(0x1 << 14)
#define	PIPE_PHY_GLB_SIZE	0x100
#define	PCIE_PIPE_PHY_MODEL_SEL_MASK	0x3

//struct axera_pcie;
struct axera_pcie {
	struct dw_pcie		*pci;
	void __iomem		*base;		/* DT pipe sys */
	int			pcie_id;
	struct clk	*pclk;
	struct clk	*aclk;
	struct clk	*aclk_sel;
	struct clk	*phy_refclk;
	struct gpio_desc *reset;
	int			link_gen;
	struct irq_domain	*irq_domain;
	enum dw_pcie_device_mode mode;
	spinlock_t irq_lock;
};

#define to_axera_pcie(x)	dev_get_drvdata((x)->dev)

static inline u32 axera_pcie_readl(struct axera_pcie *pcie, u32 offset)
{
	return readl(pcie->base + offset);
}

static inline void axera_pcie_writel(struct axera_pcie *pcie, u32 offset,
				      u32 value)
{
	writel(value, pcie->base + offset);
}
#endif