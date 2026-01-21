/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <common.h>
#include <dm/device.h>
#include <dm/device_compat.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/of_extra.h>
#include <linux/delay.h>
#include <miiphy.h>
#include <phy.h>
#include <net/dsa.h>
#ifdef CONFIG_TARGET_AX650_XPILOT_6V
#include <asm/arch/ax650.h>
#endif

#include "rtl9068.h"
#include "rtl906x_reg_list.h"
#include "rtl906x_regField_list.h"

static int rtl_phy_read(struct udevice *dev, int port, int devad, int regnum)
{
	struct rtl9068_priv *priv = dev_get_priv(dev->parent);

	return priv->info->phy_read(priv, port, regnum);
}

static int rtl_phy_write(struct udevice *dev, int port, int devad, int regnum, u16 val)
{
	struct rtl9068_priv *priv = dev_get_priv(dev->parent);

	return priv->info->phy_write(priv, port, regnum, val);
}

static int rtl9068_mdio_bind(struct udevice *dev)
{
	char name[32];
	static int num_devices;

	sprintf(name, "rtl9068-mdio-%d", num_devices++);
	device_set_name(dev, name);

	return 0;
}

static const struct mdio_ops rtl9068_mdio_ops = {
	.read = rtl_phy_read,
	.write = rtl_phy_write,
};

U_BOOT_DRIVER(rtl9068_mdio) = {
	.name		= "rtl9068_mdio",
	.id		= UCLASS_MDIO,
	.ops		= &rtl9068_mdio_ops,
	.bind		= rtl9068_mdio_bind,
	.platdata_auto_alloc_size = sizeof(struct mdio_perdev_priv),
};

static int rtl9068_setup_mdio(struct rtl9068_priv *priv)
{
	struct udevice *dev = priv->dev;
	struct udevice *mdev;
	const char *name;
	ofnode node;
	int ret;

	/* bind phy ports of mdio child node to mv88e6xxx_mdio device */
	node = dev_read_subnode(dev, "mdio");
	if (!ofnode_valid(node))
		return 0;

	name = ofnode_get_name(node);
	ret = device_bind_driver_to_node(dev,
					 "rtl9068_mdio",
					 name, node, NULL);
	if (ret) {
		dev_err(dev, "failed to bind %s: %d\n", name, ret);
	} else {
		/* need to probe it as there is no compatible to do so */
		ret = uclass_get_device_by_ofnode(UCLASS_MDIO, node, &mdev);
		if (ret)
			dev_err(dev, "failed to probe %s: %d\n", name, ret);
		priv->phy_bus = miiphy_get_dev_by_name(name);
		priv->parent = priv->dev->parent;
		priv->bus = priv->dev->parent->name;
	}

	return ret;
}

static int rtl906x_regif_mdcmdio_write(struct rtl9068_priv *priv, u32 switch_addr, u32 reg_val)
{
	int ret;
	unsigned short read_data;
	int i = 100;

	// set write data
	ret = miiphy_write(priv->bus, priv->id, 0, reg_val & 0x0000FFFF);
	if (ret) {
		dev_err(priv->dev, "failed to write 0x0: %d\n", ret);
		return ret;
	}
	ret = miiphy_write(priv->bus, priv->id, 2, ((reg_val & 0xFFFF0000) >> 16));
	if (ret) {
		dev_err(priv->dev, "failed to write 0x2: %d\n", ret);
		return ret;
	}

	// set address
	ret = miiphy_write(priv->bus, priv->id, 4, switch_addr & 0x0000FFFF);
	if (ret) {
		dev_err(priv->dev, "failed to write 0x4: %d\n", ret);
		return ret;
	}
	ret = miiphy_write(priv->bus, priv->id, 6, ((switch_addr & 0x00FF0000) >> 16) | (0x81u << 8));
	if (ret) {
		dev_err(priv->dev, "failed to write 0x6: %d\n", ret);
		return ret;
	}

	do {
		ret = miiphy_read(priv->bus, priv->id, 6, &read_data);
		if (ret) {
			dev_err(priv->dev, "failed to read 0x6: %d\n", ret);
			return ret;
		}
		if (read_data & 0x8000) {
			udelay(1);
		} else {
			break;
		}
	} while (i--);
	if (i == 0) {
		dev_err(priv->dev, "failed to write timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int rtl906x_regif_mdcmdio_read(struct rtl9068_priv *priv, u32 switch_addr, u32 *reg_val)
{
	int ret;
	unsigned short read_data;
	int i = 100;

	// set address
	ret = miiphy_write(priv->bus, priv->id, 4, switch_addr & 0x0000FFFF);
	if (ret) {
		dev_err(priv->dev, "failed to write 0x4: %d\n", ret);
		return ret;
	}
	ret = miiphy_write(priv->bus, priv->id, 6, ((switch_addr & 0x00FF0000) >> 16) | (0x80u << 8));
	if (ret) {
		dev_err(priv->dev, "failed to write 0x6: %d\n", ret);
		return ret;
	}

	do {
		ret = miiphy_read(priv->bus, priv->id, 6, &read_data);
		if (ret) {
			dev_err(priv->dev, "failed to read 0x6: %d\n", ret);
			return ret;
		}
		if (read_data & 0x8000) {
			udelay(1);
		} else {
			break;
		}
	} while (i--);
	if (i == 0) {
		dev_err(priv->dev, "failed to read timeout\n");
		return -ETIMEDOUT;
	}

	// read back data
	ret = miiphy_read(priv->bus, priv->id, 0, &read_data);
	if (ret) {
		dev_err(priv->dev, "failed to read 0x0: %d\n", ret);
		return ret;
	}
	*reg_val = read_data & 0xffff;
	ret = miiphy_read(priv->bus, priv->id, 2, &read_data);
	if (ret) {
		dev_err(priv->dev, "failed to read 0x0: %d\n", ret);
		return ret;
	}
	*reg_val |= ((read_data & 0xffff) << 16);
	return 0;
}

static int reg_field_set(u32 offset, u32 len, u32 value, u32 *data)
{
	u32 mask = 0;
	if (len < 32) {
		mask = (1 << len) - 1;
	} else {
		mask = 0xFFFFFFFF;
	}
	*data &= ~(mask << offset);
	*data |= ((value & mask) << offset);
	return 0;
}

static int reg_field_get(u32 offset, u32 len, u32 *value, u32 data)
{
	data = data >> offset;
	if (len < 32) {
		*value = data & ((1u << len) - 1);
	} else {
		*value = data;
	}
	return 0;
}

static int reg_field_write(struct rtl9068_priv *priv, u32 regAddr, u32 lsp, u32 len, u32 value)
{
	int ret = 0;
	u32 data = 0;
	ret = rtl906x_regif_mdcmdio_read(priv, regAddr, &data);
	reg_field_set(lsp, len, value, &data);
	ret |= rtl906x_regif_mdcmdio_write(priv, regAddr, data);
	return ret;
}

static int reg_field_read(struct rtl9068_priv *priv, u32 regAddr, u32 lsp, u32 len, u32 *value)
{
	int ret = 0;
	u32 data = 0;

	ret = rtl906x_regif_mdcmdio_read(priv, regAddr, &data);
	reg_field_get(lsp, len, value, data);
	return ret;
}

static int rtk_std_mdx_init(struct rtl9068_priv *priv, u8 baudr, rtk_mdx_preamble_t preamble)
{
	int ret = 0;
	u32 regVal = 0;

	if((baudr > 0x1F) || (0 == baudr)) {
		printk("unsupported baud rate: %d\n", baudr);
		return -ENODATA;
	}
	if (preamble >= RTK_MDX_PREAMBLE_END) {
		printk("unsupported baud preamble: %d\n", preamble);
		return -ENODATA;
	}
	ret = rtl906x_regif_mdcmdio_read(priv, MDC_MDIO_MASTER_CONTROL0, &regVal);
	if (ret < 0) {
		dev_err(priv->dev, "failed to read %#x: %d\n", MDC_MDIO_MASTER_CONTROL0, ret);
		return ret;
	}
	reg_field_set(MDC_MDIO_MASTER_CONTROL0_SEL_SMI_FREQ_LSP, MDC_MDIO_MASTER_CONTROL0_SEL_SMI_FREQ_LEN, baudr, &regVal);
	reg_field_set(MDC_MDIO_MASTER_CONTROL0_SEL_PREM_LSP, MDC_MDIO_MASTER_CONTROL0_SEL_PREM_LEN, preamble, &regVal);
	ret = rtl906x_regif_mdcmdio_write(priv, MDC_MDIO_MASTER_CONTROL0, regVal);
	if (ret) {
		dev_err(priv->dev, "failed to write %#x: %d\n", MDC_MDIO_MASTER_CONTROL0, ret);
		return ret;
	}
	/* Select GPIOD to used as MDC/MDIO Master */
	return reg_field_write(priv, GPIO_FUNCSELECT_REG, GPIO_FUNCSELECT_REG_SEL_MDIOMASTER_GPIO_LSP, GPIO_FUNCSELECT_REG_SEL_MDIOMASTER_GPIO_LEN, 0);
}

static int mdx_polling_cmd_bit(struct rtl9068_priv *priv)
{
	int ret = 0;
	int i = 200;
	u32 regval = 0;

	do {
		ret = reg_field_read(priv,
		       MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0,
		       MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0_CDM_LSP,
		       MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0_CDM_LEN,
		       &regval);
		if (ret < 0) {
			dev_err(priv->dev, "failed to read %#x: %d\n", MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0, ret);
			return ret;
		}
		if (regval != 0) {
			udelay(1);
		} else {
			break;
		}
	} while (i--);
	if (i == 0) {
		dev_err(priv->dev, "mdx_polling_cmd_bit timeout\n");
		return -ETIMEDOUT;
	}
	return 0;
}

static int rtl9068_phy_read(struct rtl9068_priv *priv, int port, int regnum)
{
	int ret = 0;
	u32 regVal = 0;
	u32 data_tmp = 0;
	/* Set PHY ID */
	reg_field_write(priv,
		 MDC_MDIO_MASTER_CONTROL1,
		 MDC_MDIO_MASTER_CONTROL1_PHYADDR_4_0_LSP,
		 MDC_MDIO_MASTER_CONTROL1_PHYADDR_4_0_LEN,
		 port);
	/* Set reg */
	regVal = (regnum << 8) | 0xFB;
	rtl906x_regif_mdcmdio_write(priv, MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0, regVal);
	ret = mdx_polling_cmd_bit(priv);
	if (ret) {
		return ret;
	}
	/* Read data */
	ret = reg_field_read(priv,
		      MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0,
		      MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0_DATA_15_0_LSP,
		      MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0_DATA_15_0_LEN,
		      &data_tmp);
	if (ret) {
		dev_err(priv->dev, "failed to read %#x\n", MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0);
		return ret;
	}

	return data_tmp;
}

static int rtl9068_phy_write(struct rtl9068_priv *priv, int port, int regnum, u16 val)
{
	u32 regVal = 0;
	/* Set PHY ID */
	reg_field_write(priv,
		 MDC_MDIO_MASTER_CONTROL1,
		 MDC_MDIO_MASTER_CONTROL1_PHYADDR_4_0_LSP,
		 MDC_MDIO_MASTER_CONTROL1_PHYADDR_4_0_LEN,
		 port);
	/* Set control register and trigger*/
	regVal = (val << 16) | (regnum << 8u) | 0xF9;
	return rtl906x_regif_mdcmdio_write(priv, MDC_MDIO_MASTER_REG_ACCESS_CONTROL_0, regVal);

}

static const struct rtl9068_info rtl9068_info_ops = {
	.phy_read = rtl9068_phy_read,
	.phy_write = rtl9068_phy_write,
};

static int rtl9068_setup(struct rtl9068_priv *priv)
{
	int ret;

	ret = rtl9068_setup_mdio(priv);
	/* Step 1. Initialize configuration. Set the MDC is 2.5MHz and preamble is 32 bits */
	/* The mdx clock = 12.5MHz / baudr */
	ret |= rtk_std_mdx_init(priv, 5, RTK_MDX_PREAMBLE_32);
	if (ret) {
		dev_err(priv->dev, "failed to rtl9068_setup\n");
	}

	dev_info(priv->dev, "rtl9068_setup ok\n");

	return ret;
}

static int rtl9068_port_probe(struct udevice *dev, int port, struct phy_device *phy)
{
	return phy_config(phy);
}

static int rtl9068_port_enable(struct udevice *dev, int port, struct phy_device *phy)
{
	struct dsa_pdata *pdata = dev_get_uclass_plat(dev);

	if (port == pdata->cpu_port)
		return 0;

	return phy_startup(phy);
}

static void rtl9068_port_disable(struct udevice *dev, int port, struct phy_device *phy)
{
}

static const struct dsa_ops rtl9068_dsa_ops = {
	.port_probe = rtl9068_port_probe,
	.port_enable = rtl9068_port_enable,
	.port_disable = rtl9068_port_disable,
};


static int rtl9068_probe(struct udevice *dev)
{
	struct rtl9068_priv *priv = dev_get_priv(dev);
	char *reg_str = "reg";
	int ret;

	if (ofnode_valid(dev_ofnode(dev)) &&
	    !ofnode_is_enabled(dev_ofnode(dev))) {
		dev_dbg(dev, "switch disabled\n");
		return -ENODEV;
	}

	priv->dev = dev;

#ifdef CONFIG_TARGET_AX650_XPILOT_6V
	if (get_board_id()) {
		reg_str = "beta-reg";
	}
#endif
	ret = ofnode_read_u32(dev_ofnode(dev), reg_str, &priv->id);
	if (ret)
		return ret;

	priv->info = &rtl9068_info_ops;

	/* probe internal mdio bus */
	ret = rtl9068_setup(priv);
	if (ret)
		return ret;

	return 0;
}

static const struct udevice_id rtl9068_ids[] = {
	{ .compatible = "axera,rtl9068" },
	{ /* sentinel */ },
	{ }
};

U_BOOT_DRIVER(rtl9068) = {
	.name		= "axera,rtl9068",
	.id		= UCLASS_DSA,
	.of_match	= rtl9068_ids,
	.probe		= rtl9068_probe,
	.ops		= &rtl9068_dsa_ops,
	.priv_auto_alloc_size	= sizeof(struct rtl9068_priv),
};
