/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/list.h>
#include "core.h"
#include "dwc3-axera.h"
#include "io.h"

/* clock and reset */
#define FLASH_CLK_RST_BASE_ADDR 0x110000
#define PIPE_CLK_RST_BASE_ADDR 0x30010000
#define COMMON_CLK_RST_BASE_ADDR 0x4210000
#define PIPE_PHY_CK_RST_BASE_ADDR 0x30710000
#define PIPE_PHY_GLB_BASE_ADDR 0x30700000

#define PIPE_SW_RST0 (PIPE_CLK_RST_BASE_ADDR + 0x10)
#define PIPE_SW_RST0_SET (PIPE_CLK_RST_BASE_ADDR + 0x38)
#define PIPE_SW_RST0_CLR (PIPE_CLK_RST_BASE_ADDR + 0x3C)
#define USB0_VCC_SW_RST 19
#define USB1_VCC_SW_RST 20

#define PIPE_PHY_SW_RST0 (PIPE_PHY_CK_RST_BASE_ADDR + 0x8)
#define USB0_PHY_SW_RST_APB 4
#define USB0_PHY_SW_RST 5
#define USB1_PHY_SW_RST_APB 6
#define USB1_PHY_SW_RST 7

#define FLASH_SW_RST0 (FLASH_CLK_RST_BASE_ADDR + 0x10)
#define FLASH_SW_RST0_SET (FLASH_CLK_RST_BASE_ADDR + 0x38)
#define FLASH_SW_RST0_CLR (FLASH_CLK_RST_BASE_ADDR + 0x3C)
#define USB2_PHY_SW_RST_APB 21
#define USB2_PHY_SW_RST 22
#define USB2_VCC_SW_RST 23

#define PIPE_CLK_MUX_0 (PIPE_CLK_RST_BASE_ADDR + 0x0)
#define CLK_USB_0_SUSPEND_SEL 4
#define CLK_USB_1_SUSPEND_SEL 5
#define CLK_PIPE_BUS_SEL  (0x3 << 6)	/* bit6..7 */

#define PIPE_CLK_EB_0 (PIPE_CLK_RST_BASE_ADDR + 0x4)
#define CLK_USB_REF_EB 9
#define CLK_USB0_SUSPEND_EB 3
#define CLK_USB1_SUSPEND_EB 4

#define PIPE_CLK_EB_1 (PIPE_CLK_RST_BASE_ADDR + 0x8)
#define BUS_CLK_USB0_EB 7
#define BUS_CLK_USB1_EB 8
#define CLK_USB0_REF_EB 11
#define CLK_USB1_REF_EB 12

#define PIPE_PHY_CLK_EB0 (PIPE_PHY_CK_RST_BASE_ADDR + 0x0)
#define PIPE_REF_ALT_CLK_EB	0
#define USB0_REF_ALT_CLK_EB	1
#define USB1_REF_ALT_CLK_EB	2

#define PIPE_PHY_CLK_EB1 (PIPE_PHY_CK_RST_BASE_ADDR + 0x4)
#define PCLK_USB0_PHY_EB 4
#define PCLK_USB1_PHY_EB 5

#define FLASH_CLK_MUX_0 (FLASH_CLK_RST_BASE_ADDR + 0x0)
#define CLK_FLASH_GLB_SEL  (0x3 << 5)	/* bit5..6 */

#define FLASH_CLK_EB_0 (FLASH_CLK_RST_BASE_ADDR + 0x4)
#define CLK_USB2_REF_EB 7
#define CLK_USB2_REF_ALT_CLK_EB 10

#define FLASH_CLK_EB_1 (FLASH_CLK_RST_BASE_ADDR + 0x8)
#define BUS_CLK_USB2_EB 2
#define PCLK_USB2_PHY_EB 12

/* vbus_valid */
#define PIPE_SYS_GLB_BASE_ADDR 0x30000000
#define USB2_PHY_GLB    0x28591000

#define USB0_CTRL   (PIPE_SYS_GLB_BASE_ADDR + 0x98)
#define USB1_CTRL   (PIPE_SYS_GLB_BASE_ADDR + 0x9c)
#define USB2_CTRL   (USB2_PHY_GLB + 0x0)
#define VBUSVALID  6

#define USB3_PHY0_CFG3 (PIPE_PHY_GLB_BASE_ADDR + 0x74)	//usb0
#define USB3_PHY1_CFG3 (PIPE_PHY_GLB_BASE_ADDR + 0x88)	//usb1
#define VBUSVLDEXT0  31

/* usb0: usb3.0, usb1: usb2.0, usb2: usb2.0 */
#define USB0    0
#define USB1    1
#define USB2    2

#define DEVICE_MODE 1
#define HOST_MODE   0

/* bus clock freq */
#define BUS_CLK_500M    500000000
#define BUS_CLK_400M    400000000
#define BUS_CLK_350M    350000000
#define BUS_CLK_300M    300000000
#define BUS_CLK_200M    200000000
#define BUS_CLK_24M     24000000

/* ref clk adj value */
#define GFLADJ_REFCLK_240MHZ_DECR	0xa
#define GFLADJ_REFCLK_FLADJ			0x7f0
#define GUCTL_REFCLKPER				0x29

void usb0_sw_rst(void)
{
	u32 value;
	void __iomem *addr_set;
	void __iomem *addr_clr;
	void __iomem *addr;

	/* phy reset */
	addr = ioremap(PIPE_PHY_SW_RST0, 4);
	value = readl(addr);
	value |= (0x01 << USB0_PHY_SW_RST) | (0x1 << USB0_PHY_SW_RST_APB);
	writel(value, addr);

	addr_set = ioremap(PIPE_SW_RST0_SET, 4);
	writel( (0x1 << USB0_VCC_SW_RST), addr_set);

	mdelay(20);

	value = readl(addr);
	value &= (~((0x01 << USB0_PHY_SW_RST) | (0x1 << USB0_PHY_SW_RST_APB)));
	writel(value, addr);
	iounmap(addr);

	addr_clr = ioremap(PIPE_SW_RST0_CLR, 4);
	writel( (0x1 << USB0_VCC_SW_RST), addr_clr);

	iounmap(addr_clr);
	iounmap(addr_set);

	mdelay(20);
}

void usb1_sw_rst(void)
{
	u32 value;
	void __iomem *addr_set;
	void __iomem *addr_clr;
	void __iomem *addr;

	/* phy reset */
	addr = ioremap(PIPE_PHY_SW_RST0, 4);
	value = readl(addr);
	value |= (0x01 << USB1_PHY_SW_RST) | (0x1 << USB1_PHY_SW_RST_APB);
	writel(value, addr);

	addr_set = ioremap(PIPE_SW_RST0_SET, 4);
	writel( (0x1 << USB1_VCC_SW_RST), addr_set);

	mdelay(20);

	value = readl(addr);
	value &= (~((0x01 << USB1_PHY_SW_RST) | (0x1 << USB1_PHY_SW_RST_APB)));
	writel(value, addr);
	iounmap(addr);

	addr_clr = ioremap(PIPE_SW_RST0_CLR, 4);
	writel( (0x1 << USB1_VCC_SW_RST), addr_clr);

	iounmap(addr_clr);
	iounmap(addr_set);

	mdelay(20);
}

void usb2_sw_rst(void)
{
	u32 value;
	void __iomem *addr_set;
	void __iomem *addr_clr;

	addr_set = ioremap(FLASH_SW_RST0_SET, 4);
	value = ((0x1 << USB2_VCC_SW_RST) |
			(0x1 << USB2_PHY_SW_RST_APB) |
			(0x01 << USB2_PHY_SW_RST));
	writel(value, addr_set);

	mdelay(20);

	addr_clr = ioremap(FLASH_SW_RST0_CLR, 4);
	writel(value, addr_clr);

	iounmap(addr_clr);
	iounmap(addr_set);

	mdelay(20);
}

void usb_sw_rst(struct dwc3 *dwc)
{
	int usb_id = dwc->usb_id;
	dev_info(dwc->dev, "configure usb%d sw reset\n", dwc->usb_id);

	switch (usb_id) {
	case USB0:
		usb0_sw_rst();
		break;
	case USB1:
		usb1_sw_rst();
		break;
	case USB2:
		usb2_sw_rst();
		break;
	default:
		dev_err(dwc->dev, "unsupport usb%d vbus init\n", dwc->usb_id);
		break;
	}
}

void usb0_clk_init(void)
{
	u32 value;
	void __iomem *addr;

	/* PIPE_SYS USB */
	addr = ioremap(PIPE_PHY_CLK_EB0, 4);
	value = readl(addr);
	value |= (0x1 << PIPE_REF_ALT_CLK_EB);
	value |= (0x1 << USB0_REF_ALT_CLK_EB);	//usb phy ref_clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_PHY_CLK_EB1, 4);
	value = readl(addr);
	value |= (0x1 << PCLK_USB0_PHY_EB);	//phy cr2apb pclk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_CLK_EB_0, 4);
	value = readl(addr);
	value |= (0x1 << CLK_USB_REF_EB);	//ref_clk
	value |= (0x1 << CLK_USB0_SUSPEND_EB);	//suspend clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_CLK_EB_1, 4);
	value = readl(addr);
	value |= (0x1 << BUS_CLK_USB0_EB);	//enable usb bus_clk & ram_clk, at least 60M
	value |= (0x1 << CLK_USB0_REF_EB);	//ref_clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_CLK_MUX_0, 4);
	value = readl(addr);
	value &= ~CLK_PIPE_BUS_SEL;
	value |= (0x2 << 6);	//bus_clk freq: 0:24M, 1:300M, 2:400M, 3:500M
	writel(value, addr);
	iounmap(addr);
}

void usb1_clk_init(void)
{
	u32 value;
	void __iomem *addr;

	/* PIPE_SYS USB */
	addr = ioremap(PIPE_PHY_CLK_EB0, 4);
	value = readl(addr);
	value |= (0x1 << PIPE_REF_ALT_CLK_EB);
	value |= (0x1 << USB1_REF_ALT_CLK_EB);	//usb phy ref_clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_PHY_CLK_EB1, 4);
	value = readl(addr);
	value |= (0x1 << PCLK_USB1_PHY_EB);	//phy cr2apb pclk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_CLK_EB_0, 4);
	value = readl(addr);
	value |= (0x1 << CLK_USB_REF_EB);	//ref_clk
	value |= (0x1 << CLK_USB1_SUSPEND_EB);	//suspend clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_CLK_EB_1, 4);
	value = readl(addr);
	value |= (0x1 << BUS_CLK_USB1_EB);	//enable usb bus_clk & ram_clk, at least 60M
	value |= (0x1 << CLK_USB1_REF_EB);	//ref_clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(PIPE_CLK_MUX_0, 4);
	value = readl(addr);
	value &= ~CLK_PIPE_BUS_SEL;
	value |= (0x2 << 6);		//bus_clk freq: 0:24M, 1:300M, 2:400M, 3:500M
	writel(value, addr);
	iounmap(addr);
}

void usb2_clk_init(void)
{
	u32 value;
	void __iomem *addr;

	/* FLASH_SYS USB */
	addr = ioremap(FLASH_CLK_EB_0, 4);
	value = readl(addr);
	value |= (0x1 << CLK_USB2_REF_EB);	//ref_clk
	value |= (0x1 << CLK_USB2_REF_ALT_CLK_EB);	//usb phy ref_clk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(FLASH_CLK_EB_1, 4);
	value = readl(addr);
	value |= (0x1 << BUS_CLK_USB2_EB);	//enable usb bus_clk & ram_clk, at least 60M
	value |= (0x1 << PCLK_USB2_PHY_EB);	//phy cr2apb pclk
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(FLASH_CLK_MUX_0, 4);
	value = readl(addr);
	value &= ~CLK_FLASH_GLB_SEL;
	value |= (0x1 << 5); 	//bus_clk freq: 0:24M, 1:200M, 2:350M
	writel(value, addr);
	iounmap(addr);
}

void usb_clk_init(struct dwc3 *dwc)
{
	int usb_id = dwc->usb_id;

	dev_info(dwc->dev, "init axera usb%d clock\n", dwc->usb_id);

	switch (usb_id) {
	case USB0:
		usb0_clk_init();
		break;
	case USB1:
		usb1_clk_init();
		break;
	case USB2:
		usb2_clk_init();
		break;
	default:
		dev_err(dwc->dev, "unsupport usb%d clk init\n", dwc->usb_id);
		break;
	}
}

void axera_pipe_usb_clk_init(struct dwc3 *dwc)
{
	struct clk *bus_clk;
	struct clk *ref_clk;
	struct clk *suspend_clk;
	struct clk *ref_alt_clk;
	struct clk *phy_clk;

	bus_clk = devm_clk_get(dwc->dev, "bus_clk");
	if (IS_ERR(bus_clk)) {
		dev_err(dwc->dev, "can not get usb bus clk");
		return;
	}

	clk_prepare_enable(bus_clk);
	dev_info(dwc->dev, "usb bus clock: %ldMHz\n", clk_get_rate(bus_clk) / 1000000);

	ref_clk = devm_clk_get(dwc->dev, "ref_clk");
	if (IS_ERR(ref_clk)) {
		dev_err(dwc->dev, "can not get usb ref clk");
		return;
	}
	clk_prepare_enable(ref_clk);

	suspend_clk = devm_clk_get(dwc->dev, "suspend_clk");
	if (IS_ERR(ref_clk)) {
		dev_err(dwc->dev, "can not get usb suspend clk");
		return;
	}
	clk_prepare_enable(suspend_clk);

	ref_alt_clk = devm_clk_get(dwc->dev, "ref_alt_clk");
	if (IS_ERR(ref_alt_clk)) {
		dev_err(dwc->dev, "can not get usb ref alt clk");
		return;
	}
	clk_prepare_enable(ref_alt_clk);

	phy_clk = devm_clk_get(dwc->dev, "phy_clk");
	if (IS_ERR(phy_clk)) {
		dev_err(dwc->dev, "can not get usb phy clk");
		return;
	}
	clk_prepare_enable(phy_clk);
}

void axera_flash_usb_clk_init(struct dwc3 *dwc)
{
	struct clk *bus_clk;
	struct clk *ref_clk;
	struct clk *ref_alt_clk;
	struct clk *phy_clk;

	bus_clk = devm_clk_get(dwc->dev, "bus_clk");
	if (IS_ERR(bus_clk)) {
		dev_err(dwc->dev, "can not get usb bus clk");
		return;
	}

	clk_prepare_enable(bus_clk);
	dev_info(dwc->dev, "usb bus clock: %ldMHz\n", clk_get_rate(bus_clk) / 1000000);

	ref_clk = devm_clk_get(dwc->dev, "ref_clk");
	if (IS_ERR(ref_clk)) {
		dev_err(dwc->dev, "can not get usb ref clk");
		return;
	}
	clk_prepare_enable(ref_clk);

	ref_alt_clk = devm_clk_get(dwc->dev, "ref_alt_clk");
	if (IS_ERR(ref_alt_clk)) {
		dev_err(dwc->dev, "can not get usb ref alt clk");
		return;
	}
	clk_prepare_enable(ref_alt_clk);

	phy_clk = devm_clk_get(dwc->dev, "phy_clk");
	if (IS_ERR(phy_clk)) {
		dev_err(dwc->dev, "can not get usb phy clk");
		return;
	}
	clk_prepare_enable(phy_clk);
}

void axera_usb_clk_init(struct dwc3 *dwc)
{
	int usb_id = dwc->usb_id;
	dev_info(dwc->dev, "axera usb%d clock init\n", dwc->usb_id);

	switch (usb_id) {
	case USB0:
	case USB1:
		axera_pipe_usb_clk_init(dwc);
		break;
	case USB2:
		axera_flash_usb_clk_init(dwc);
		break;
	default:
		dev_err(dwc->dev, "unsupport usb%d clk init\n", dwc->usb_id);
		break;
	}
}

void usb0_vbus_init(char usb_mode)
{
	u32 value;
	void __iomem *addr;

	addr = ioremap(USB0_CTRL, 4);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVALID);
	else
		value &= (~(0x1 << VBUSVALID));
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(USB3_PHY0_CFG3, 4);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVLDEXT0);
	else
		value &= (~(0x1 << VBUSVLDEXT0));
	writel(value, addr);
	iounmap(addr);
}

void usb1_vbus_init(char usb_mode)
{
	u32 value;
	void __iomem *addr;

	addr = ioremap(USB1_CTRL, 4);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVALID);
	else
		value &= (~(0x1 << VBUSVALID));
	writel(value, addr);
	iounmap(addr);

	addr = ioremap(USB3_PHY1_CFG3, 4);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVLDEXT0);
	else
		value &= (~(0x1 << VBUSVLDEXT0));
	writel(value, addr);
	iounmap(addr);
}

void usb2_vbus_init(char usb_mode)
{
	u32 value;
	void __iomem *addr;

	addr = ioremap(USB2_CTRL, 4);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVALID);
	else
		value &= (~(0x1 << VBUSVALID));
	writel(value, addr);
	iounmap(addr);
}

void usb_vbus_init(struct dwc3 *dwc, char usb_mode)
{
	int usb_id = dwc->usb_id;
	dev_info(dwc->dev, "configure usb%d vbusvalid\n", dwc->usb_id);

	switch (usb_id) {
	case USB0:
		usb0_vbus_init(usb_mode);
		break;
	case USB1:
		usb1_vbus_init(usb_mode);
		break;
	case USB2:
		usb2_vbus_init(usb_mode);
		break;
	default:
		dev_err(dwc->dev, "unsupport usb%d vbus init\n", dwc->usb_id);
		break;
	}
}

void axera_usb_adj_ref_clk(struct dwc3 *dwc)
{
	u32 value;

	//dev_info(dwc->dev,"axera_usb_adj_ref_clk\n");

	/* DWC3_GFLADJ: 0xc630 */
	value = dwc3_readl(dwc->regs, DWC3_GFLADJ);
	value &= ~(DWC3_GFLADJ_REFCLK_FLADJ_MASK << 8);
	value &= ~(DWC3_GFLADJ_REFCLK_240MHZ_DECR_MASK << 24);
	value |= GFLADJ_REFCLK_240MHZ_DECR << 24 |
			 DWC3_GFLADJ_REFCLK_LPM_SEL 	|
			 GFLADJ_REFCLK_FLADJ << 8;
	dwc3_writel(dwc->regs, DWC3_GFLADJ, value);

	/* DWC3_GUCTL: 0xc12c */
	value = dwc3_readl(dwc->regs, DWC3_GUCTL);
	value &= ~(DWC3_GUCTL_REFCLKPER_MASK << 22);
	value |= GUCTL_REFCLKPER << 22;
	dwc3_writel(dwc->regs, DWC3_GUCTL, value);
}

#ifdef CONFIG_TYPEC_SGM7220
struct list_head dwc3_list;

void put_dwc3_into_list(struct dwc3 *dwc)
{
	struct dwc3_dev_list *list;
	static int is_head_init;

	list = (struct dwc3_dev_list *)kmalloc(sizeof(struct dwc3_dev_list), GFP_KERNEL);
	list->dwc = dwc;

	if (!is_head_init) {
		INIT_LIST_HEAD(&dwc3_list);
		is_head_init = 1;
	}
	list_add_tail(&list->list_node, &dwc3_list);
}

struct list_head *get_dwc3_list(void)
{
	return &dwc3_list;
}
#endif

void axera_usb_global_init(struct dwc3 *dwc)
{
	u32 value;

	//dev_info(dwc->dev, "-----axera usb------\n");
	dwc->usb_id = of_alias_get_id(dwc->dev->of_node, "usb");
#if 0
	usb_clk_init(dwc);
#else
	axera_usb_clk_init(dwc);
#endif

	usb_sw_rst(dwc);

	if (dwc->usb_id == USB0 || dwc->usb_id == USB1) {
		axera_usb_adj_ref_clk(dwc);
	}

	/* if the controller is programmed to operate in 2.0 only, then
	 * setting this bit makes the internal 2.0 (utmi/ulpi) clock to be
	 * routed as the 3.0 (pipe) clock.
	 */
	if (dwc->usb_id == USB1) {
		value = dwc3_readl(dwc->regs, DWC3_GUCTL1);
		value |= (0x1 << 26);
		dwc3_writel(dwc->regs, DWC3_GUCTL1, value);
	}

	dma_set_mask(dwc->dev, DMA_BIT_MASK(48));

#ifdef CONFIG_TYPEC_SGM7220
	put_dwc3_into_list(dwc);
#endif

	//dev_info(dwc->dev, "axera usb init\n");
}

void axera_usb_host_init(struct dwc3 *dwc)
{
	dev_info(dwc->dev, "axera usb host init\n");
	usb_vbus_init(dwc, HOST_MODE);
}

void axera_usb_device_init(struct dwc3 *dwc)
{
	dev_info(dwc->dev, "axera usb gadget init\n");
	usb_vbus_init(dwc, DEVICE_MODE);
}
