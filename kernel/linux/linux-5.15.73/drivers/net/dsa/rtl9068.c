/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/iopoll.h>
#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_irq.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phylink.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio/driver.h>
#include <net/dsa.h>

#include "rtl9068.h"
#include "rtl906x_reg_list.h"
#include "rtl906x_regField_list.h"

static int rtl_phy_read(struct mii_bus *bus, int port, int regnum)
{
	struct rtl9068_priv *priv = bus->priv;

	return priv->info->phy_read(priv, port, regnum);
}

static int rtl_phy_write(struct mii_bus *bus, int port, int regnum, u16 val)
{
	struct rtl9068_priv *priv = bus->priv;

	return priv->info->phy_write(priv, port, regnum, val);
}

static int rtl9068_setup_mdio(struct rtl9068_priv *priv)
{
	struct device_node *mdio_np = NULL;
	struct device *dev = priv->dev;
	struct mii_bus *bus;
	static int idx;
	int ret;

	mdio_np = of_get_compatible_child(dev->of_node, "realtek,smi-mdio");
	if (!mdio_np) {
		dev_err(dev, "no MDIO bus node\n");
		return -ENODEV;
	}

	bus = devm_mdiobus_alloc(dev);
	if (!bus) {
		ret = -ENOMEM;
		goto err_put_node;
	}

	bus->priv = priv;
	bus->name = KBUILD_MODNAME "-mii";
	bus->read = rtl_phy_read;
	bus->write = rtl_phy_write;
	snprintf(bus->id, MII_BUS_ID_SIZE, KBUILD_MODNAME "-%d", idx++);
	bus->dev.of_node = mdio_np;
	bus->parent = dev;
	priv->ds->slave_mii_bus = bus;

	ret = devm_of_mdiobus_register(dev, bus, mdio_np);
	if (ret) {
		dev_err(dev, "failed to register MDIO bus: %d\n", ret);
		goto err_put_node;
	}

	return 0;

err_put_node:
	of_node_put(mdio_np);

	return ret;
}

static int rtl906x_regif_mdcmdio_write(struct rtl9068_priv *priv, u32 switch_addr, u32 reg_val)
{
	int ret;
	int i = 100;

	// set write data
	ret = mdiobus_write_nested(priv->bus, priv->id, 0, reg_val & 0x0000FFFF);
	if (ret) {
		dev_err(priv->dev, "failed to write 0x0: %d\n", ret);
		return ret;
	}
	ret = mdiobus_write_nested(priv->bus, priv->id, 2, ((reg_val & 0xFFFF0000) >> 16));
	if (ret) {
		dev_err(priv->dev, "failed to write 0x2: %d\n", ret);
		return ret;
	}

	// set address
	ret = mdiobus_write_nested(priv->bus, priv->id, 4, switch_addr & 0x0000FFFF);
	if (ret) {
		dev_err(priv->dev, "failed to write 0x4: %d\n", ret);
		return ret;
	}
	ret = mdiobus_write_nested(priv->bus, priv->id, 6, ((switch_addr & 0x00FF0000) >> 16) | (0x81u << 8));
	if (ret) {
		dev_err(priv->dev, "failed to write 0x6: %d\n", ret);
		return ret;
	}

	do {
		ret = mdiobus_read_nested(priv->bus, priv->id, 6);
		if (ret < 0) {
			dev_err(priv->dev, "failed to read 0x6: %d\n", ret);
			return ret;
		}
		if (ret & 0x8000) {
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
	int i = 100;

	// set address
	ret = mdiobus_write_nested(priv->bus, priv->id, 4, switch_addr & 0x0000FFFF);
	if (ret) {
		dev_err(priv->dev, "failed to write 0x4: %d\n", ret);
		return ret;
	}
	ret = mdiobus_write_nested(priv->bus, priv->id, 6, ((switch_addr & 0x00FF0000) >> 16) | (0x80u << 8));
	if (ret) {
		dev_err(priv->dev, "failed to write 0x6: %d\n", ret);
		return ret;
	}

	do {
		ret = mdiobus_read_nested(priv->bus, priv->id, 6);
		if (ret < 0) {
			dev_err(priv->dev, "failed to read 0x6: %d\n", ret);
			return ret;
		}
		if (ret & 0x8000) {
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
	ret = mdiobus_read_nested(priv->bus, priv->id, 0);
	if (ret < 0) {
		dev_err(priv->dev, "failed to read 0x0: %d\n", ret);
		return ret;
	}
	*reg_val = ret & 0xffff;
	ret = mdiobus_read_nested(priv->bus, priv->id, 2);
	if (ret < 0) {
		dev_err(priv->dev, "failed to read 0x0: %d\n", ret);
		return ret;
	}
	*reg_val |= ((ret & 0xffff) << 16);
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

static int rtl9068_setup(struct dsa_switch *ds)
{
	struct rtl9068_priv *priv = ds->priv;
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


static const struct rtl9068_info rtl9068_info_ops = {
	.phy_read = rtl9068_phy_read,
	.phy_write = rtl9068_phy_write,
};

static enum dsa_tag_protocol rtl9068_get_tag_protocol(struct dsa_switch *ds, int port,
						      enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_NONE;
}

static void rtl9068_phylink_validate (struct dsa_switch *ds, int port,
				      unsigned long *supported,
				      struct phylink_link_state *state)
{
	phylink_set_port_modes(supported);
	phylink_set(supported, Autoneg);

	phylink_set(supported, 1000baseT_Full);
	phylink_set(supported, 10baseT_Half);
	phylink_set(supported, 10baseT_Full);
	phylink_set(supported, 100baseT_Half);
	phylink_set(supported, 100baseT_Full);
}

static const struct dsa_switch_ops rtl9068_switch_ops = {
	.get_tag_protocol	= rtl9068_get_tag_protocol,
	.setup			= rtl9068_setup,
	.phylink_validate = rtl9068_phylink_validate,
};

static const struct of_device_id rtl9068_of_match[] = {
	{ .compatible = "axera,rtl9068" },
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, rtl9068_of_match);

static int rtl9068_probe(struct mdio_device *mdiodev)
{
	struct rtl9068_priv *priv;
	struct device_node *dn;

	dn = mdiodev->dev.of_node;

	priv = devm_kzalloc(&mdiodev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	priv->ds = devm_kzalloc(&mdiodev->dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds) {
		return -ENOMEM;
	}

	if (of_property_read_u32(dn, "switch-id", &priv->id)) {
		dev_err(&mdiodev->dev, "don't get switch-id\n");
		return -ENODATA;
	}

	priv->info = &rtl9068_info_ops;

	priv->ds->dev = &mdiodev->dev;
	priv->ds->num_ports = rtl9068_NUM_PORTS;

	priv->bus = mdiodev->bus;
	priv->dev = &mdiodev->dev;
	priv->ds->priv = priv;
	priv->ds->ops = &rtl9068_switch_ops;
	mutex_init(&priv->reg_mutex);
	dev_set_drvdata(&mdiodev->dev, priv);

	return dsa_register_switch(priv->ds);
}

static void rtl9068_remove(struct mdio_device *mdiodev)
{
	struct rtl9068_priv *priv = dev_get_drvdata(&mdiodev->dev);

	if (!priv) {
		return;
	}

	dsa_unregister_switch(priv->ds);
	mutex_destroy(&priv->reg_mutex);

	dev_set_drvdata(&mdiodev->dev, NULL);
}

static void rtl9068_shutdown(struct mdio_device *mdiodev)
{
	struct rtl9068_priv *priv = dev_get_drvdata(&mdiodev->dev);

	if (!priv) {
		return;
	}

	dsa_switch_shutdown(priv->ds);

	dev_set_drvdata(&mdiodev->dev, NULL);
}

static struct mdio_driver rtl9068_mdio_driver = {
	.probe  = rtl9068_probe,
	.remove = rtl9068_remove,
	.shutdown = rtl9068_shutdown,
	.mdiodrv.driver = {
		.name = "rtl9068",
		.of_match_table = rtl9068_of_match,
	},
};

mdio_module_driver(rtl9068_mdio_driver);

MODULE_DESCRIPTION("axera rtl9068 driver");
MODULE_LICENSE("GPL v2");
