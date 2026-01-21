/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_gpio.h>
#include <linux/phy.h>

#include "stmmac.h"
#include "stmmac_platform.h"

#define EMAC0   0
#define EMAC1   1

#define MM_SYS_GLB_BASE_ADDR	0x10000000
#define PIPE_SYS_GLB_BASE_ADDR	0x30000000
#define MM_CLK_RST_BASE_ADDR	0x10010000
#define PIPE_CLK_RST_BASE_ADDR	0x30010000

/* phy interface select & rgmii clk delay */
//EMAC0
#define EMAC0_MM_REG   (MM_SYS_GLB_BASE_ADDR + 0x9C)
#define EMAC0_PHY_IF_SEL	4
#define EMAC0_RGMII_TX_DLY	2
#define EMAC0_RGMII_RX_DLY	0
//EMAC1
#define EMAC1_PP_REG   (PIPE_SYS_GLB_BASE_ADDR + 0x94)
#define EMAC1_PHY_IF_SEL	4
#define EMAC1_RGMII_TX_DLY	2
#define EMAC1_RGMII_RX_DLY 	0

//EMAC SW RST
#define EMAC0_MM_SW_RST0 (MM_CLK_RST_BASE_ADDR + 0x10)
#define EMAC0_MM_SW_RST0_SET (MM_CLK_RST_BASE_ADDR + 0x38)
#define EMAC0_MM_SW_RST0_CLR (MM_CLK_RST_BASE_ADDR + 0x3C)
#define EMAC0_SW_RST 7

#define EMAC1_PP_SW_RST0 (PIPE_CLK_RST_BASE_ADDR + 0x10)
#define EMAC1_PP_SW_RST0_SET (PIPE_CLK_RST_BASE_ADDR + 0x38)
#define EMAC1_PP_SW_RST0_CLR (PIPE_CLK_RST_BASE_ADDR + 0x3C)
#define EMAC1_SW_RST 2

//EMAC RGMMI TX SEL
#define EMAC0_MM_MUX_REG   (MM_CLK_RST_BASE_ADDR + 0x0)
#define EMAC0_RGMII_TX_SEL	12
#define EMAC1_PP_MUX_REG   (PIPE_CLK_RST_BASE_ADDR + 0x0)
#define EMAC1_RGMII_TX_SEL	9

//EMAC RMII SEL
#define EMAC0_MM_CLK_DIV   (MM_CLK_RST_BASE_ADDR + 0xc)
#define EMAC0_RMII_RX_DIVN		10
#define EMAC0_RMII_RX_DIVN_UP	14

#define EMAC1_PP_CLK_DIV   (PIPE_CLK_RST_BASE_ADDR + 0xc)
#define EMAC1_RMII_RX_DIVN		11
#define EMAC1_RMII_RX_DIVN_UP	15

///EMAC CLK EN
#define EMAC0_MM_CLK_EB0 (MM_CLK_RST_BASE_ADDR + 0x4)
#define EMAC0_MM_CLK_EB1 (MM_CLK_RST_BASE_ADDR + 0x8)
#define EMAC0_ACLK			0
#define EMAC0_RGMII_TX_EB 	6
#define EMAC0_RMII_PHY_EB 	7
#define EMAC0_RMII_RX_EB 	8

#define EMAC1_PP_CLK_EB0 (PIPE_CLK_RST_BASE_ADDR + 0x4)
#define EMAC1_PP_CLK_EB1 (PIPE_CLK_RST_BASE_ADDR + 0x8)
#define EMAC1_ACLK			2
#define EMAC1_RGMII_TX_EB 	6
#define EMAC1_RMII_PHY_EB 	7
#define EMAC1_RMII_RX_EB 	8

//EMAC LP
#define MM_RMII_LP_MODE		0x4250530
#define PP_RMII_LP_MODE		0x4250cd0
#define RMII_LP_EN			(0x1 << 0)

/* bus clock freq */
#define BUS_CLK_624M    624000000
#define BUS_CLK_500M    500000000
#define BUS_CLK_400M    400000000
#define BUS_CLK_300M    300000000
#define BUS_CLK_24M     24000000
#define EPHY_CLK_25M    25000000

struct axera_eqos {
	struct device *dev;
	struct plat_stmmacenet_data *plat;
	int phy_interface;
	int bus_clock;
	struct gpio_desc *gpiod_reset;
	int bus_id;
	unsigned char rmii_loopback_mode;
};

static void emac0_rgmii_set_tx_speed(int speed)
{
	u32 value;
	void __iomem *addr;
	u8 pos = EMAC0_RGMII_TX_SEL;

	addr = ioremap(EMAC0_MM_MUX_REG, 4);
	value = readl(addr);
	value &= ~(0x3 << pos);

	switch (speed) {
	case SPEED_10:
		value |= (0x0 << pos);
		break;
	case SPEED_100:
		value |= (0x1 << pos);
		break;
	case SPEED_1000:
		value |= (0x2 << pos);
		break;
	default:
		printk(KERN_ERR"EMAC:un-supported emac0 rgmii phy speed:%d\n", speed);
		return;
	}

	writel(value, addr);

	iounmap(addr);
}

static void emac1_rgmii_set_tx_speed(int speed)
{
	u32 value;
	void __iomem *addr;
	u8 pos = EMAC1_RGMII_TX_SEL;

	addr = ioremap(EMAC1_PP_MUX_REG, 4);
	value = readl(addr);
	value &= ~(0x3 << pos);

	switch (speed) {
	case SPEED_10:
		value |= (0x0 << pos);
		break;
	case SPEED_100:
		value |= (0x1 << pos);
		break;
	case SPEED_1000:
		value |= (0x2 << pos);
		break;
	default:
		printk(KERN_ERR"EMAC:un-supported emac1 rgmii phy speed:%d\n", speed);
		return;
	}

	writel(value, addr);

	iounmap(addr);
}

static void emac_rgmii_set_tx_speed(int bus_id, int speed)
{
	switch (bus_id) {
	case EMAC0:
		emac0_rgmii_set_tx_speed(speed);
		break;

	case EMAC1:
		emac1_rgmii_set_tx_speed(speed);
		break;

	default:
		printk(KERN_ERR"EMAC:only support EMAC0 and EMAC1\n");
		break;
	}
}

static void emac0_rmii_set_speed(int speed)
{
	u32 value;
	void __iomem *addr;
	u8 divn_pos = EMAC0_RMII_RX_DIVN;
	u8 divn_update = EMAC0_RMII_RX_DIVN_UP;

	//clk set
	addr = ioremap(EMAC0_MM_CLK_DIV, 4);
	value = readl(addr);
	value &= ~(0xF << divn_pos);

	switch (speed) {
	case SPEED_10:
		value |= (0xA << divn_pos);	//2.5M
		break;
	case SPEED_100:
		value |= (0x1 << divn_pos);	//25M
		break;
	default:
		printk(KERN_ERR"EMAC:un-supported emac0 rmii phy speed:%d\n", speed);
		return;
	}

	//clk update
	value |= (0x1 << divn_update);

	writel(value, addr);

	iounmap(addr);

}

static void emac1_rmii_set_speed(int speed)
{
	u32 value;
	void __iomem *addr;
	u8 divn_pos = EMAC1_RMII_RX_DIVN;
	u8 divn_update = EMAC1_RMII_RX_DIVN_UP;

	//clk set
	addr = ioremap(EMAC1_PP_CLK_DIV, 4);
	value = readl(addr);
	value &= ~(0xF << divn_pos);

	switch (speed) {
	case SPEED_10:
		value |= (0xA << divn_pos);	//2.5M
		break;
	case SPEED_100:
		value |= (0x1 << divn_pos);	//25M
		break;
	default:
		printk(KERN_ERR"EMAC:un-supported emac1 rmii phy speed:%d\n", speed);
		return;
	}

	//clk update
	value |= (0x1 << divn_update);

	writel(value, addr);

	iounmap(addr);
}

static void emac_rmii_set_speed(int bus_id, int speed)
{
	switch (bus_id) {
	case EMAC0:
		emac0_rmii_set_speed(speed);
		break;

	case EMAC1:
		emac1_rmii_set_speed(speed);
		break;

	default:
		printk(KERN_ERR"EMAC:only support EMAC0 and EMAC1 rmii\n");
		break;
	}
}

static void axera_eqos_fix_speed(void *priv, unsigned int speed)
{
	struct axera_eqos *eqos = priv;
	int bus_id = eqos->plat->bus_id;

	dev_dbg(eqos->dev, "fix axera emac speed: %dMhz\n", speed);

	if (eqos->phy_interface == PHY_INTERFACE_MODE_RGMII) {
		dev_dbg(eqos->dev, "adjust RGMII tx clock\n");
		emac_rgmii_set_tx_speed(bus_id, speed);
	} else if (eqos->phy_interface == PHY_INTERFACE_MODE_RMII) {
		dev_dbg(eqos->dev, "adjust RMII clock\n");
		emac_rmii_set_speed(bus_id, speed);
	} else {
		dev_err(eqos->dev, "unknown phy interface: %d\n", eqos->phy_interface);
	}
}

static void emac0_phy_select(struct plat_stmmacenet_data *plat_dat)
{
	u32 value;
	void __iomem *addr;

	addr = ioremap(EMAC0_MM_REG, 4);
	value = readl(addr);

	if (plat_dat->interface == PHY_INTERFACE_MODE_RGMII) {
		value |= (0x1 << EMAC0_PHY_IF_SEL);
	} else if (plat_dat->interface == PHY_INTERFACE_MODE_RMII) {
		value &= (~(0x1 << EMAC0_PHY_IF_SEL));
	} else if (plat_dat->interface == PHY_INTERFACE_MODE_GMII) {
		return;
	} else {
		printk(KERN_ERR"EMAC0: don't support this phy interface\n");
		return;
	}

	writel(value, addr);

	iounmap(addr);
}

static void emac1_phy_select(struct plat_stmmacenet_data *plat_dat)
{
	u32 value;
	void __iomem *addr;

	addr = ioremap(EMAC1_PP_REG, 4);
	value = readl(addr);

	if (plat_dat->interface == PHY_INTERFACE_MODE_RGMII) {
		value |= (0x1 << EMAC1_PHY_IF_SEL);
	} else if (plat_dat->interface == PHY_INTERFACE_MODE_RMII) {
		value &= (~(0x1 << EMAC1_PHY_IF_SEL));
	} else if (plat_dat->interface == PHY_INTERFACE_MODE_GMII) {
		return;
	} else {
		printk(KERN_ERR"EMAC1: don't support this phy interface\n");
		return;
	}

	writel(value, addr);

	iounmap(addr);
}

static void select_phy_interface(struct plat_stmmacenet_data *plat_dat)
{
	int bus_id = plat_dat->bus_id;

	switch (bus_id) {
	case EMAC0:
		emac0_phy_select(plat_dat);
		break;

	case EMAC1:
		emac1_phy_select(plat_dat);
		break;

	default:
		printk(KERN_ERR"EMAC:only support EMAC0 and EMAC1 Phy\n");
		break;
	}
}

static void emac0_sw_rst(void)
{
	void __iomem *addr_set;
	void __iomem *addr_clr;

	addr_set = ioremap(EMAC0_MM_SW_RST0_SET, 4);
	if (!addr_set) {
		printk(KERN_ERR "EMAC0: cannot map mmio space at 0x%llx\n",
			(unsigned long long)EMAC0_MM_SW_RST0_SET);
		return;
	}

	addr_clr = ioremap(EMAC0_MM_SW_RST0_CLR, 4);
	if (!addr_clr) {
		printk(KERN_ERR "EMAC0: cannot map mmio space at 0x%llx\n",
			(unsigned long long)EMAC0_MM_SW_RST0_CLR);
		goto ioremap_err;
	}

	//set reset
	writel((0x1 << EMAC0_SW_RST), addr_set);
	//delay
	mdelay(5);
	//clr reset
	writel((0x1 << EMAC0_SW_RST), addr_clr);

	iounmap(addr_clr);
ioremap_err:
	iounmap(addr_set);
}

static void emac1_sw_rst(void)
{
	void __iomem *addr_set;
	void __iomem *addr_clr;

	addr_set = ioremap(EMAC1_PP_SW_RST0_SET, 4);
	if (!addr_set) {
		printk(KERN_ERR "EMAC1: cannot map mmio space at 0x%llx\n",
			(unsigned long long)EMAC1_PP_SW_RST0_SET);
		return;
	}

	addr_clr = ioremap(EMAC1_PP_SW_RST0_CLR, 4);
	if (!addr_clr) {
		printk(KERN_ERR "EMAC1: cannot map mmio space at 0x%llx\n",
			(unsigned long long)EMAC1_PP_SW_RST0_CLR);
		goto ioremap_err;
	}

	//set reset
	writel((0x1 << EMAC1_SW_RST), addr_set);
	//delay
	mdelay(5);
	//clr reset
	writel((0x1 << EMAC1_SW_RST), addr_clr);

	iounmap(addr_clr);
ioremap_err:
	iounmap(addr_set);
}

static void emac_sw_rst(int bus_id)
{
	if (bus_id == EMAC0) {
		emac0_sw_rst();
	} else if (bus_id == EMAC1) {
		emac1_sw_rst();
	} else {
		printk(KERN_ERR"EMAC:only support EMAC0 and EMAC1 reset\n");
		return;
	}
}

static int emac_phy_rst(struct axera_eqos *eqos)
{
	if (!eqos->gpiod_reset) {
		dev_err(eqos->dev, "EMAC:no phy reset gpio found\n");
		return -1;
	}

	gpiod_direction_output(eqos->gpiod_reset, 0);

	/* For a complete PHY reset, must be asserted low for at least 10ms. */
	gpiod_set_value_cansleep(eqos->gpiod_reset, 0);
	msleep(15);
	gpiod_set_value_cansleep(eqos->gpiod_reset, 1);
	msleep(75);

	return 0;
}

int axera_dwmac_config_dt(
			struct platform_device *pdev,
			struct plat_stmmacenet_data *plat,
			struct axera_eqos *eqos
			)
{
	int rc;
	struct clk *ephy_clk;
	struct clk *rgmii_tx_clk;
	struct clk *rmmi_phy_clk;
	struct clk *rmii_rx_clk;

	/* emac bus clock, 500MHz */
	plat->stmmac_clk = devm_clk_get(&pdev->dev, "emac_aclk");
	if (IS_ERR(plat->stmmac_clk)) {
		dev_warn(&pdev->dev, "Cannot get CSR clock\n");
		plat->stmmac_clk = NULL;
	} else {
		eqos->bus_clock = clk_get_rate(plat->stmmac_clk);
		clk_prepare_enable(plat->stmmac_clk);
		dev_info(&pdev->dev, "emac bus clock: %ldMHz\n",
						 clk_get_rate(plat->stmmac_clk) / 1000000);
	}

	//ephy clock
	ephy_clk = devm_clk_get(&pdev->dev, "ephy_clk");
	if (IS_ERR(ephy_clk)) {
		dev_warn(&pdev->dev, "can't get ephy_clk\n");
		ephy_clk = NULL;
	} else {
		if (clk_set_rate(ephy_clk, EPHY_CLK_25M)) {
			dev_warn(&pdev->dev, "emac phy clk set fail: %s\n",
				 "ephy_clk");
		}

		rc = clk_prepare_enable(ephy_clk);
		if (rc)
			dev_warn(&pdev->dev, "enable ephy_clk failed\n");

		dev_info(&pdev->dev, "emac phy clock: %ldMHz\n",
		 clk_get_rate(ephy_clk) / 1000000);
	}

	/* rgmii tx clock */
	if (eqos->phy_interface == PHY_INTERFACE_MODE_RGMII) {
		rgmii_tx_clk = devm_clk_get(&pdev->dev, "rgmii_tx_clk");
		if (IS_ERR(rgmii_tx_clk)) {
			dev_warn(&pdev->dev, "Cannot set rgmii_tx_clk\n");
		} else {
			rc = clk_prepare_enable(rgmii_tx_clk);
			if (rc == 0) {
				dev_dbg(&pdev->dev, "set rgmii_tx_clk ok\n");
			}
		}
	}

	/*rmmi phy clock */
	if (eqos->phy_interface == PHY_INTERFACE_MODE_RMII) {
		/* phy clock, 50MHz */
		rmmi_phy_clk = devm_clk_get(&pdev->dev, "rmii_phy_clk");
		if (IS_ERR(rmmi_phy_clk)) {
			dev_warn(&pdev->dev, "Cannot set phy-clk\n");
		} else {
			rc = clk_prepare_enable(rmmi_phy_clk);
			if (rc == 0) {
				dev_dbg(&pdev->dev, "set rmii_phy_clk ok\n");
			}
		}

		rmii_rx_clk = devm_clk_get(&pdev->dev, "rmii_rx_clk");
		if (IS_ERR(rmii_rx_clk)) {
			dev_warn(&pdev->dev, "Cannot set rmii_rx_clk\n");
		} else {
			rc = clk_prepare_enable(rmii_rx_clk);
			if (rc == 0) {
				dev_dbg(&pdev->dev, "set rmii-rx-clk ok\n");
			}
		}

		if (eqos->rmii_loopback_mode) {
			int bus_id = eqos->plat->bus_id;
			u32 value;
			void __iomem *addr;
			if (bus_id == EMAC0) {
				dev_info(eqos->dev, "EMAC0 RMII LOOPBACK\n");
				addr = ioremap(MM_RMII_LP_MODE, 4);
				value = readl(addr);
				value |= RMII_LP_EN;	/* RMII CLK: MAC -> PHY */
				writel(value, addr);
				iounmap(addr);
			} else if (bus_id == EMAC1) {
				dev_info(eqos->dev, "EMAC1 RMII LOOPBACK\n");
				addr = ioremap(PP_RMII_LP_MODE, 4);
				value = readl(addr);
				value |= RMII_LP_EN;	/* RMII CLK: MAC -> PHY */
				writel(value, addr);
				iounmap(addr);
			} else {
				dev_err(eqos->dev, "WARNING: only support EMAC0 and EMAC1\n");
			}
		}
	}

	/* ptp clock */
	plat->clk_ptp_ref = devm_clk_get(&pdev->dev, "ptp_clk");
	if (IS_ERR(plat->clk_ptp_ref)) {
		plat->clk_ptp_rate = clk_get_rate(plat->stmmac_clk);
		plat->clk_ptp_ref = NULL;
		dev_info(&pdev->dev, "PTP uses main clock %dMHz\n", plat->clk_ptp_rate/1000000);
	} else {
		plat->clk_ptp_rate = clk_get_rate(plat->clk_ptp_ref);
		dev_info(&pdev->dev, "PTP rate: %dMHz\n", plat->clk_ptp_rate/1000000);
	}
	plat->ptp_max_adj = plat->clk_ptp_rate;

	if (!eqos->gpiod_reset) {
		eqos->gpiod_reset =
				devm_gpiod_get_optional(&pdev->dev, "phy-rst", GPIOD_OUT_LOW);
		if (!eqos->gpiod_reset) {
			dev_err(&pdev->dev, "failed to get emac phy reset gpio\n");
			return -ENODEV;
		}

		if (IS_ERR(eqos->gpiod_reset)) {
			return PTR_ERR(eqos->gpiod_reset);
		}

		gpiod_direction_output(eqos->gpiod_reset, 0);
	}

	return 0;
}

static void emac0_clk_init(struct axera_eqos *eqos)
{
	u32 value;
	void __iomem *addr;

	/* enable aclk & pclk */
	addr = ioremap(EMAC0_MM_CLK_EB1, 4);
	value = readl(addr);
	value |= (0x1 << EMAC0_ACLK);
	writel(value, addr);
	iounmap(addr);

	/* enable rgmii tx clk */
	addr = ioremap(EMAC0_MM_CLK_EB0, 4);
	value = readl(addr);
	value |= (0x1 << EMAC0_RGMII_TX_EB);
	writel(value, addr);
	iounmap(addr);

	/* configure rmii clk */
	addr = ioremap(EMAC1_PP_CLK_EB0, 4);
	value = readl(addr);
	value |= (0x1 << EMAC0_RMII_RX_EB);	/* enable rmii rx clk */
	if (eqos->rmii_loopback_mode) {
		value |= (0x1 << EMAC0_RMII_PHY_EB);	/* RMII CLK: MAC -> PHY */
	} else {
		value &= ~(0x1 << EMAC0_RMII_PHY_EB);	/* RMII CLK: PHY -> MAC */
	}
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(MM_RMII_LP_MODE, 4);
	value = readl(addr);
	if (eqos->rmii_loopback_mode) {
		value |= RMII_LP_EN;	/* RMII CLK: MAC -> PHY */
	} else {
		value &= ~RMII_LP_EN;	/* RMII CLK: PHY -> MAC */
	}
	writel(value, addr);
	iounmap(addr);
}

static void emac1_clk_init(struct axera_eqos *eqos)
{
	u32 value;
	void __iomem *addr;

	/* enable aclk & pclk */
	addr = ioremap(EMAC1_PP_CLK_EB1, 4);
	value = readl(addr);
	value |= (0x1 << EMAC1_ACLK);
	writel(value, addr);
	iounmap(addr);

	/* enable rgmii tx clk */
	addr = ioremap(EMAC1_PP_CLK_EB0, 4);
	value = readl(addr);
	value |= (0x1 << EMAC1_RGMII_TX_EB);
	writel(value, addr);
	iounmap(addr);

	/* configure rmii clk */
	addr = ioremap(EMAC1_PP_CLK_EB0, 4);
	value = readl(addr);
	value |= (0x1 << EMAC1_RMII_RX_EB);	/* enable rmii rx clk */
	if (eqos->rmii_loopback_mode) {
		value |= (0x1 << EMAC1_RMII_PHY_EB);	/* RMII CLK: MAC -> PHY */
	} else {
		value &= ~(0x1 << EMAC1_RMII_PHY_EB);	/* RMII CLK: PHY -> MAC */
	}
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PP_RMII_LP_MODE, 4);
	value = readl(addr);
	if (eqos->rmii_loopback_mode) {
		value |= RMII_LP_EN;	/* RMII CLK: MAC -> PHY */
	} else {
		value &= ~RMII_LP_EN;	/* RMII CLK: PHY -> MAC */
	}
	writel(value, addr);
	iounmap(addr);
}

void emac_clk_init(struct axera_eqos *eqos)
{
	int bus_id = eqos->plat->bus_id;

	if (bus_id == EMAC0) {
		emac0_clk_init(eqos);
	} else if (bus_id == EMAC1) {
		emac1_clk_init(eqos);
	} else {
		dev_err(eqos->dev, "WARNING: only support EMAC0 and EMAC1\n");
		return;
	}
}

/* for emac and phy reset */
void ax_reset_phy(struct platform_device *pdev, void *private)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct stmmac_priv *priv = netdev_priv(ndev);
	struct plat_stmmacenet_data *plat_dat = priv->plat;
	struct axera_eqos *eqos = plat_dat->bsp_priv;

	dev_info(&pdev->dev, "axera_reset_phy\n");
	emac_phy_rst(eqos);

	return;
}

void ax_reset_emac(struct platform_device *pdev, void *private)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct stmmac_priv *priv = netdev_priv(ndev);
	struct plat_stmmacenet_data *plat_dat = priv->plat;

	dev_info(&pdev->dev, "ax_reset_emac\n");

	emac_sw_rst(plat_dat->bus_id);
	select_phy_interface(plat_dat);

	return;
}

/* for axera resume */
int plat_init(struct platform_device *pdev, void *private)
{
	dev_dbg(&pdev->dev, "axera emac resume!\n");
	return 0;
}

/* for axera suspend */
void plat_exit(struct platform_device *pdev, void *private)
{
	dev_dbg(&pdev->dev, "axera emac suspend!\n");
	return;
}

static int dwmac_axera_probe(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	int ret;
	struct axera_eqos *eqos;

	dev_info(&pdev->dev, "probe axera emac\n");
	eqos = devm_kzalloc(&pdev->dev, sizeof(*eqos), GFP_KERNEL);
	if (!eqos) {
		return -ENOMEM;
	}
	memset(eqos, 0, sizeof(*eqos));

	ret = stmmac_get_platform_resources(pdev, &stmmac_res);
	if (ret) {
		return ret;
	}

	if (pdev->dev.of_node) {
		plat_dat = stmmac_probe_config_dt(pdev, stmmac_res.mac);
		if (IS_ERR(plat_dat)) {
			dev_err(&pdev->dev, "dts configuration failed\n");
			return PTR_ERR(plat_dat);
		}
	} else {
		dev_err(&pdev->dev, "no emac dts provided\n");
		return -EINVAL;
	}

	eqos->rmii_loopback_mode =
			of_property_read_bool(pdev->dev.of_node, "axera,rmii_loopback");

	/* TODO: need to improve, mdio_clk = csr_clk / 324 */
	if (plat_dat->bus_id == EMAC1) {
		plat_dat->clk_csr = 0x6;
	} else {
		plat_dat->clk_csr = 0x7;
	}

#ifdef CONFIG_DWMAC_AXERA_HAPS
	/* in haps, mdio_clk = csr_clk / 4 = 10M / 4 = 2.5M */
	plat_dat->clk_csr = 0x1000;
#endif
	plat_dat->bsp_priv = eqos;

	eqos->dev = &pdev->dev;
	eqos->phy_interface = plat_dat->interface;
	eqos->plat = plat_dat;
	eqos->bus_id = plat_dat->bus_id;

	plat_dat->fix_mac_speed = axera_eqos_fix_speed;
	plat_dat->bsp_priv = eqos;
	plat_dat->init = plat_init;
	plat_dat->exit = plat_exit;

#ifdef CONFIG_DWMAC_AXERA_HAPS
	emac_clk_init(eqos);
#else
	ret = axera_dwmac_config_dt(pdev, plat_dat, eqos);
	if (ret) {
		goto err_remove_config_dt;
	}
#endif

	dev_info(&pdev->dev, "reset axera emac and phy...\n");
	emac_sw_rst(plat_dat->bus_id);
	select_phy_interface(plat_dat);
	plat_dat->sph_disable = 1;

	ret = stmmac_dvr_probe(&pdev->dev, plat_dat, &stmmac_res);
	if (ret) {
		goto err_remove_config_dt;
	}

	return 0;

err_remove_config_dt:
	if (pdev->dev.of_node) {
		stmmac_remove_config_dt(pdev, plat_dat);
	}

	return ret;
}

static const struct of_device_id dwmac_axera_match[] = {
	{.compatible = "axera,dwmac-4.10a"},
	{}
};

MODULE_DEVICE_TABLE(of, dwmac_axera_match);

static struct platform_driver dwmac_axera_driver = {
	.probe = dwmac_axera_probe,
	.remove = stmmac_pltfr_remove,
	.driver = {
		.name = STMMAC_RESOURCE_NAME,
		.pm = &stmmac_pltfr_pm_ops,
		.of_match_table = of_match_ptr(dwmac_axera_match),
	},
};

module_platform_driver(dwmac_axera_driver);

MODULE_DESCRIPTION("axera dwmac driver");
MODULE_LICENSE("GPL v2");
