
#include <common.h>
#include <asm-generic/io.h>
#include <malloc.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/sizes.h>

/* clock and reset */
#define FLASH_CLK_RST_BASE_ADDR 0x110000
#define PIPE_CLK_RST_BASE_ADDR 0x30010000
#define COMMON_CLK_RST_BASE_ADDR 0x4210000
#define PIPE_PHY_CK_RST_BASE_ADDR 0x30710000
#define PIPE_PHY_GLB_BASE_ADDR 0x30700000

#define PIPE_SW_RST0 (PIPE_CLK_RST_BASE_ADDR + 0x10)
#define USB0_VCC_SW_RST 19
#define USB1_VCC_SW_RST 20

#define PIPE_PHY_SW_RST0 (PIPE_PHY_CK_RST_BASE_ADDR + 0x8)
#define USB0_PHY_SW_RST_APB 4
#define USB0_PHY_SW_RST 5
#define USB1_PHY_SW_RST_APB 6
#define USB1_PHY_SW_RST 7

#define FLASH_SW_RST0 (FLASH_CLK_RST_BASE_ADDR + 0x10)
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

struct dwc3_axera {
	struct udevice *dev;
};

void usb1_clk_init(void)
{
	u32 value;
	void *addr;

	/* PIPE_SYS USB */
	addr = (void *)(PIPE_PHY_CLK_EB0);
	value = readl(addr);
	value |= (0x1 << USB1_REF_ALT_CLK_EB);	//usb phy ref_clk
	writel(value, addr);

	addr = (void *)(PIPE_PHY_CLK_EB1);
	value = readl(addr);
	value |= (0x1 << PCLK_USB1_PHY_EB);	//phy cr2apb pclk
	writel(value, addr);

	addr = (void *)(PIPE_CLK_EB_0);
	value = readl(addr);
	value |= (0x1 << CLK_USB_REF_EB);	//ref_clk
	value |= (0x1 << CLK_USB1_SUSPEND_EB);	//suspend clk
	writel(value, addr);

	addr = (void *)(PIPE_CLK_EB_1);
	value = readl(addr);
	value |= (0x1 << BUS_CLK_USB1_EB);	//enable usb bus_clk & ram_clk, at least 60M
	value |= (0x1 << CLK_USB1_REF_EB);	//ref_clk
	writel(value, addr);

	addr = (void *)(PIPE_CLK_MUX_0);
	value = readl(addr);
	value &= ~CLK_PIPE_BUS_SEL;
	value |= (0x2 << 6);		//bus_clk freq: 0:24M, 1:300M, 2:400M, 3:500M
	writel(value, addr);
}

void usb1_sw_rst(void)
{
	u32 value1, value2;
	void  *addr1;
	void  *addr2;

	/* phy reset */
	addr2 = (void *)(PIPE_PHY_SW_RST0);
	value2 = readl(addr2);
	value2 |= (0x01 << USB1_PHY_SW_RST) | (0x1 << USB1_PHY_SW_RST_APB);
	writel(value2, addr2);

	addr1 = (void *)(PIPE_SW_RST0);
	value1 = readl(addr1);
	value1 |= (0x1 << USB1_VCC_SW_RST);
	writel(value1, addr1);

	mdelay(20);

	value2 = readl(addr2);
	value2 &= (~((0x01 << USB1_PHY_SW_RST) | (0x1 << USB1_PHY_SW_RST_APB)));
	writel(value2, addr2);

	value1 = readl(addr1);
	value1 &= ~(0x1 << USB1_VCC_SW_RST);
	writel(value1, addr1);

	mdelay(20);
}

void usb1_vbus_init(char usb_mode)
{
	u32 value;
	void *addr;

	addr = (void *)(USB1_CTRL);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVALID);
	else
		value &= (~(0x1 << VBUSVALID));
	writel(value, addr);

	addr = (void *)(USB3_PHY1_CFG3);
	value = readl(addr);
	if (usb_mode == DEVICE_MODE)
		value |= (0x1 << VBUSVLDEXT0);
	else
		value &= (~(0x1 << VBUSVLDEXT0));
	writel(value, addr);
}

static int dwc3_axera_probe(struct udevice *dev)
{
	struct dwc3_axera *data = dev_get_platdata(dev);
	data->dev = dev;

	printf("\ndwc3 axera probe...\n");
	usb1_clk_init();
	usb1_sw_rst();
	usb1_vbus_init(HOST_MODE);

	return 0;
}

static int dwc3_axera_remove(struct udevice *dev)
{
	return 0;
}

static const struct udevice_id dwc3_axera_of_match[] = {
	{ .compatible = "axera,ax650-dwc3", },
	{},
};

U_BOOT_DRIVER(dwc3_axera) = {
	.name = "dwc3-axera",
	.id = UCLASS_SIMPLE_BUS,
	.of_match = dwc3_axera_of_match,
	.probe = dwc3_axera_probe,
	.remove = dwc3_axera_remove,
	.platdata_auto_alloc_size = sizeof(struct dwc3_axera),
	.flags = DM_FLAG_OS_PREPARE,
};
