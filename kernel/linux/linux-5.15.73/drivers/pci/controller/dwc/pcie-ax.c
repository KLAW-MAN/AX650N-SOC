// SPDX-License-Identifier: GPL-2.0
/*
 * pcie-axera - PCIe controller driver for AXERA SoCs
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/resource.h>
#include <linux/types.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/gpio/consumer.h>
#include <linux/reset.h>
#include <linux/sizes.h>
#include "pcie-axera.h"

#include "../../pci.h"
#include "pcie-designware.h"

#define	AXERA_PCIE0_CPU_TO_BUS_ADDR			0x3FFFFFFF
#define	AXERA_PCIE1_CPU_TO_BUS_ADDR			0x1FFFFFFF

#define	PCIE0_AX_CTL_INTX_REG			0x100
#define	PCIE1_AX_CTL_INTX_REG			0x188
#define	PCIE_INTA_EN		(1 << 21)

#define	DEVICE_TYPE_EP				0x0
#define	DEVICE_TYPE_LEG_EP			0x1
#define	DEVICE_TYPE_RC				0x4

#define	LTSSM_EN				0x10

static spinlock_t s_shmem_data_lock;

struct axera_pcie_of_data {
	enum dw_pcie_device_mode mode;
};

void ax_pcie_spin_lock(unsigned long *flags)
{
	spin_lock_irqsave(&s_shmem_data_lock, *flags);
}

EXPORT_SYMBOL_GPL(ax_pcie_spin_lock);

void ax_pcie_spin_unlock(unsigned long *flags)
{
	spin_unlock_irqrestore(&s_shmem_data_lock, *flags);
}

EXPORT_SYMBOL_GPL(ax_pcie_spin_unlock);

static u64 axera_pcie_cpu_addr_fixup(struct dw_pcie *pci, u64 cpu_addr)
{
	struct axera_pcie *axera = to_axera_pcie(pci);

	if (axera->pcie_id == PCIE0_ID)
		return cpu_addr & AXERA_PCIE0_CPU_TO_BUS_ADDR;
	else
		return cpu_addr & AXERA_PCIE1_CPU_TO_BUS_ADDR;
}

static int pcie_model_sel_read(struct device *dev, int pcie_id)
{
	void __iomem *pipe_phy_glb;
	u32 val;

	pipe_phy_glb = devm_ioremap(dev, PIPE_PHY_GLB_BASE, PIPE_PHY_GLB_SIZE);
	if (pcie_id == PCIE0_ID) {
		val = readl(pipe_phy_glb + PCIE0_PIPE_PHY_MODEL_SEL_OFFSET);
	} else {
		val = readl(pipe_phy_glb + PCIE1_PIPE_PHY_MODEL_SEL_OFFSET);
	}
	val &= PCIE_PIPE_PHY_MODEL_SEL_MASK;
	return val;
}

static void axera_pcie_set_ltssm_status(struct axera_pcie *pcie, int enable)
{
	u32 val = 0;

	if (pcie->pcie_id == PCIE0_ID) {
		if (enable) {
			/* set ltssm disable */
			val = axera_pcie_readl(pcie, PIPE_PCIE0_LTSSM_REG);
			val |= LTSSM_EN;
			axera_pcie_writel(pcie, PIPE_PCIE0_LTSSM_REG, val);
		} else {
			val = axera_pcie_readl(pcie, PIPE_PCIE0_LTSSM_REG);
			val &= ~LTSSM_EN;
			axera_pcie_writel(pcie, PIPE_PCIE0_LTSSM_REG, val);
		}
	} else {
		if (enable) {
			val = axera_pcie_readl(pcie, PIPE_PCIE1_LTSSM_REG);
			val |= LTSSM_EN;
			axera_pcie_writel(pcie, PIPE_PCIE1_LTSSM_REG, val);
		} else {
			val = axera_pcie_readl(pcie, PIPE_PCIE1_LTSSM_REG);
			val &= ~LTSSM_EN;
			axera_pcie_writel(pcie, PIPE_PCIE1_LTSSM_REG, val);
		}
	}
}

static void axera_pcie_armisc(struct axera_pcie *pcie, int enable)
{
	u32 val = 0;

	if (pcie->pcie_id == PCIE0_ID) {
		if (enable) {
			/* enable armisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE0_ARMISC);
			val |= AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE0_ARMISC, val);
		} else {
			/* disable armisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE0_ARMISC);
			val &= ~AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE0_ARMISC, val);
		}
	} else {
		if (enable) {
			/* enable armisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE1_ARMISC);
			val |= AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE1_ARMISC, val);
		} else {
			/* disable armisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE1_ARMISC);
			val &= ~AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE1_ARMISC, val);
		}
	}
}

static void axera_pcie_awmisc(struct axera_pcie *pcie, int enable)
{
	u32 val = 0;

	if (pcie->pcie_id == PCIE0_ID) {
		if (enable) {
			/* enable awmisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE0_AWMISC);
			val |= AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE0_AWMISC, val);
		} else {
			/* disable awmisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE0_AWMISC);
			val &= ~AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE0_AWMISC, val);
		}
	} else {
		if (enable) {
			/* enable awmisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE1_AWMISC);
			val |= AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE1_AWMISC, val);
		} else {
			/* disable awmisc */
			val = axera_pcie_readl(pcie, PIPE_PCIE1_AWMISC);
			val &= ~AWRMISC_EN;
			axera_pcie_writel(pcie, PIPE_PCIE1_AWMISC, val);
		}
	}
}

static int axera_pcie_clock_init(struct axera_pcie *pcie)
{
	int ret = 0;
	struct dw_pcie *pci = pcie->pci;
	struct device *dev = pci->dev;

	ret = clk_prepare_enable(pcie->pclk);
	if (ret) {
		dev_err(dev, "Clk enable failed\n");
		return -ENOMEM;
	}

	ret = clk_set_rate(pcie->aclk_sel, 250000000);
	if (ret) {
		dev_err(dev, "Aclk sel enable failed\n");
		return -ENOMEM;
	}

	ret = clk_set_rate(pcie->aclk, 500000000);
	if (ret) {
		dev_err(dev, "Pcie aclk enable failed\n");
		return -ENOMEM;
	}

	return ret;
}

static u32 axera_pcie_read_dbi(struct dw_pcie *pci, void __iomem *base,
			       u32 reg, size_t size)
{
	struct axera_pcie *axera = to_axera_pcie(pci);
	unsigned long flags = 0;
	u32 val;

	ax_pcie_spin_lock(&flags);
	axera_pcie_armisc(axera, true);
	dw_pcie_read(base + reg, size, &val);
	axera_pcie_armisc(axera, false);
	ax_pcie_spin_unlock(&flags);

	return val;
}

static void axera_pcie_write_dbi2(struct dw_pcie *pci, void __iomem *base,
				  u32 reg, size_t size, u32 val)
{
	struct axera_pcie *axera = to_axera_pcie(pci);
	unsigned long flags = 0;

	ax_pcie_spin_lock(&flags);
	axera_pcie_awmisc(axera, true);
	dw_pcie_write(base + reg, size, val);
	axera_pcie_awmisc(axera, false);
	ax_pcie_spin_unlock(&flags);
}

static void axera_pcie_write_dbi(struct dw_pcie *pci, void __iomem *base,
				 u32 reg, size_t size, u32 val)
{
	struct axera_pcie *axera = to_axera_pcie(pci);
	unsigned long flags = 0;

	ax_pcie_spin_lock(&flags);
	axera_pcie_awmisc(axera, true);
	dw_pcie_write(base + reg, size, val);
	axera_pcie_awmisc(axera, false);
	ax_pcie_spin_unlock(&flags);
}

static int axera_pcie_rd_own_conf(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 *val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(bus->sysdata);

	if (PCI_SLOT(devfn)) {
		*val = ~0;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	*val = dw_pcie_read_dbi(pci, where, size);
	return PCIBIOS_SUCCESSFUL;
}

static int axera_pcie_wr_own_conf(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(bus->sysdata);

	if (PCI_SLOT(devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	dw_pcie_write_dbi(pci, where, size, val);
	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops axera_pci_ops = {
	.read = axera_pcie_rd_own_conf,
	.write = axera_pcie_wr_own_conf,
};

static int axera_pcie_link_up(struct dw_pcie *pci)
{
	u32 val;

	val = dw_pcie_readl_dbi(pci, PCIE_PHY_DEBUG_R0);
	val &= PCIE_PHY_DEBUG_R0_LINK_STATE;
	return (val == PCIE_PHY_DEBUG_R0_LINK_UP);
}

static void axera_pcie_stop_link(struct dw_pcie *pci)
{
	struct axera_pcie *axera = to_axera_pcie(pci);

	axera_pcie_set_ltssm_status(axera, false);
}

static int axera_pcie_establish_link(struct dw_pcie *pci)
{
	struct axera_pcie *axera = to_axera_pcie(pci);
	struct device *dev = pci->dev;

	if (dw_pcie_link_up(pci)) {
		dev_err(dev, "link is already up\n");
		return 0;
	}

	axera_pcie_set_ltssm_status(axera, true);
	return 0;
}

static int axera_pcie_host_init(struct pcie_port *pp)
{
	pp->bridge->ops = &axera_pci_ops;
	return 0;
}

static const struct dw_pcie_host_ops axera_pcie_host_ops = {
	.host_init = axera_pcie_host_init,
};

static void axera_pcie_ep_init(struct dw_pcie_ep *ep)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	enum pci_barno bar;

	for (bar = BAR_0; bar <= BAR_5; bar++)
		dw_pcie_ep_reset_bar(pci, bar);

}

static void axera_pcie_raise_legacy_irq(struct axera_pcie *axera)
{
	int val;
	if (axera->pcie_id == PCIE0_ID) {
		/* Set inta irq */
		val = axera_pcie_readl(axera, PCIE0_AX_CTL_INTX_REG);
		val |= PCIE_INTA_EN;
		axera_pcie_writel(axera, PCIE0_AX_CTL_INTX_REG, val);
		mdelay(1);
		/* clr inta irq */
		val = axera_pcie_readl(axera, PCIE0_AX_CTL_INTX_REG);
		val &= ~PCIE_INTA_EN;
		axera_pcie_writel(axera, PCIE0_AX_CTL_INTX_REG, val);
	} else {
		/* Set inta irq */
		val = axera_pcie_readl(axera, PCIE1_AX_CTL_INTX_REG);
		val &= PCIE_INTA_EN;
		axera_pcie_writel(axera, PCIE1_AX_CTL_INTX_REG, PCIE_INTA_EN);
		mdelay(1);
		/* clr inta irq */
		val = axera_pcie_readl(axera, PCIE1_AX_CTL_INTX_REG);
		val &= ~PCIE_INTA_EN;
		axera_pcie_writel(axera, PCIE1_AX_CTL_INTX_REG, val);
	}

}

static void axera_pcie_raise_msi_irq(struct axera_pcie *axera, u8 interrupt_num)
{

	u32 val;
	int retry = 200;
	int i;
	unsigned long flags;

	spin_lock_irqsave(&axera->irq_lock, flags);
	if (axera->pcie_id == PCIE0_ID) {
		/* set msi vector */
		val = axera_pcie_readl(axera, PIPE_PCIE0_VEN_MSI_REG);
		val |= (interrupt_num - 1);
		axera_pcie_writel(axera, PIPE_PCIE0_VEN_MSI_REG, val);

		/* update msi vector */
		val = axera_pcie_readl(axera, PIPE_PCIE0_REG_UPDATE);
		val |= 0x1;
		axera_pcie_writel(axera, PIPE_PCIE0_REG_UPDATE, val);

		udelay(10);
		/* clr reg update */
		val = axera_pcie_readl(axera, PIPE_PCIE0_REG_UPDATE);
		val &= ~(0x3);
		axera_pcie_writel(axera, PIPE_PCIE0_REG_UPDATE, val);

		/* send msi irq */
		val = axera_pcie_readl(axera, PIPE_PCIE0_VEN_MSI_REG);
		val |= (0x1 << 11);
		axera_pcie_writel(axera, PIPE_PCIE0_VEN_MSI_REG, val);

		for (i = 0; i < retry; i++) {
			/* read msi req down */
			val = axera_pcie_readl(axera, PIPE_PCIE0_VEN_MSI_REG);
			if ((val & BIT(12)))
				break;
			udelay(1);
		}

		/* clr send req and vector */
		val = axera_pcie_readl(axera, PIPE_PCIE0_VEN_MSI_REG);
		val &= ~(0x81f);
		axera_pcie_writel(axera, PIPE_PCIE0_VEN_MSI_REG, val);
	} else {
		/* set msi vector */
		val = axera_pcie_readl(axera, PIPE_PCIE1_VEN_MSI_REG);
		val |= (interrupt_num - 1);
		axera_pcie_writel(axera, PIPE_PCIE1_VEN_MSI_REG, val);

		/* update msi vector */
		val = axera_pcie_readl(axera, PIPE_PCIE1_REG_UPDATE);
		val |= 0x1;
		axera_pcie_writel(axera, PIPE_PCIE1_REG_UPDATE, val);

		udelay(10);
		/* clr reg update */
		val = axera_pcie_readl(axera, PIPE_PCIE1_REG_UPDATE);
		val &= ~(0x3);
		axera_pcie_writel(axera, PIPE_PCIE1_REG_UPDATE, val);

		/* send msi irq */
		val = axera_pcie_readl(axera, PIPE_PCIE1_VEN_MSI_REG);
		val |= (0x1 << 11);
		axera_pcie_writel(axera, PIPE_PCIE1_VEN_MSI_REG, val);

		for (i = 0; i < retry; i++) {
			/* read msi req down */
			val = axera_pcie_readl(axera, PIPE_PCIE1_VEN_MSI_REG);
			if ((val & BIT(12)))
				break;
			udelay(1);
		}

		/* clr send req and vector */
		val = axera_pcie_readl(axera, PIPE_PCIE1_VEN_MSI_REG);
		val &= ~(0x81f);
		axera_pcie_writel(axera, PIPE_PCIE1_VEN_MSI_REG, val);
	}
	spin_unlock_irqrestore(&axera->irq_lock, flags);

}

static int axera_pcie_raise_irq(struct dw_pcie_ep *ep, u8 func_no,
				enum pci_epc_irq_type type, u16 interrupt_num)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	struct axera_pcie *axera = to_axera_pcie(pci);

	switch (type) {
	case PCI_EPC_IRQ_LEGACY:
		axera_pcie_raise_legacy_irq(axera);
		break;
	case PCI_EPC_IRQ_MSI:
		axera_pcie_raise_msi_irq(axera, interrupt_num);
		break;
	default:
		dev_err(pci->dev, "UNKNOWN IRQ type\n");
	}

	return 0;
}

// static struct dw_pcie_ep_ops pcie_ep_ops = {
// 	.ep_init = axera_pcie_ep_init,
// 	.raise_irq = axera_pcie_raise_irq,
// };

static void axera_set_device_type(struct axera_pcie *axera)
{
	int val;

	if (axera->pcie_id == PCIE0_ID) {
		if (axera->mode == DW_PCIE_RC_TYPE) {
			val = axera_pcie_readl(axera, PIPE_PCIE0_DEVICE_TYPE);
			val |= DEVICE_TYPE_RC;
			axera_pcie_writel(axera, PIPE_PCIE0_DEVICE_TYPE, val);
		} else {
			val = axera_pcie_readl(axera, PIPE_PCIE0_DEVICE_TYPE);
			val |= DEVICE_TYPE_EP;
			axera_pcie_writel(axera, PIPE_PCIE0_DEVICE_TYPE, val);
		}
	} else {
		if (axera->mode == DW_PCIE_RC_TYPE) {
			val = axera_pcie_readl(axera, PIPE_PCIE1_DEVICE_TYPE);
			val |= DEVICE_TYPE_RC;
			axera_pcie_writel(axera, PIPE_PCIE1_DEVICE_TYPE, val);
		} else {
			val = axera_pcie_readl(axera, PIPE_PCIE1_DEVICE_TYPE);
			val |= DEVICE_TYPE_EP;
			axera_pcie_writel(axera, PIPE_PCIE1_DEVICE_TYPE, val);
		}
	}
}

static void axera_pcie_aspm_status_set(struct dw_pcie *pci)
{
	int val;

	/*disable pcie aspm support*/
	dw_pcie_dbi_ro_wr_en(pci);
	val = dw_pcie_readl_dbi(pci, PCIE_LINK_ASPM_CAP);
	val &= ~(0x3 << 10);
	dw_pcie_writel_dbi(pci, PCIE_LINK_ASPM_CAP, val);
}

static int axera_add_pcie_port(struct axera_pcie *axera,
			       struct platform_device *pdev)
{
	int ret;
	struct dw_pcie *pci = axera->pci;
	struct pcie_port *pp = &pci->pp;
	struct device *dev = pci->dev;

	pp->msi_irq = platform_get_irq(pdev, 1);
	if (pp->msi_irq < 0) {
		dev_err(dev, "missing IRQ resource\n");
		return pp->msi_irq;
	}

	pp->ops = &axera_pcie_host_ops;

	ret = dw_pcie_host_init(pp);
	if (ret) {
		dev_err(dev, "failed to initialize host\n");
		return ret;
	}

	return 0;
}

static const struct dw_pcie_ops dw_pcie_ops = {
	.write_dbi = axera_pcie_write_dbi,
	.read_dbi = axera_pcie_read_dbi,
	.write_dbi2 = axera_pcie_write_dbi2,
	.link_up = axera_pcie_link_up,
	.start_link = axera_pcie_establish_link,
	.stop_link = axera_pcie_stop_link,
	.cpu_addr_fixup = axera_pcie_cpu_addr_fixup,
};

static const struct axera_pcie_of_data axera_pcie_rc_of_data = {
	.mode = DW_PCIE_RC_TYPE,
};

static const struct axera_pcie_of_data axera_pcie_ep_of_data = {
	.mode = DW_PCIE_EP_TYPE,
};

static const struct of_device_id of_axera_pcie_match[] = {
	{
	 .compatible = "axera,axera-pcie",
	 .data = &axera_pcie_rc_of_data,
	 },
	{},
};

static int axera_pcie_probe(struct platform_device *pdev)
{
	int ret;
	int pcie_id;
	int model_sel;
	void __iomem *base;
	struct resource *res;
	struct dw_pcie *pci;
	struct axera_pcie *axera;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	const struct of_device_id *match;
	const struct axera_pcie_of_data *data;
	enum dw_pcie_device_mode mode;

	pcie_id = of_alias_get_id(np, "pcie");
	if (pcie_id != PCIE0_ID && pcie_id != PCIE1_ID) {
		dev_err(dev, "Invalid pcie id %d\n", pcie_id);
		return -ENOMEM;
	}
	printk("axera pcie%d rc probe", pcie_id);
	model_sel = pcie_model_sel_read(dev, pcie_id);
	if (!model_sel) {
		dev_err(dev, "model_sel = %d, pcie %d not supported\n", model_sel, pcie_id);
		return -EPERM;
	}

	dma_set_mask(dev, DMA_BIT_MASK(64));
	dma_set_coherent_mask(dev, DMA_BIT_MASK(64));

	match = of_match_device(of_match_ptr(of_axera_pcie_match), dev);
	if (!match)
		return -EINVAL;

	data = (struct axera_pcie_of_data *)match->data;
	mode = (enum dw_pcie_device_mode)data->mode;

	axera = devm_kzalloc(dev, sizeof(*axera), GFP_KERNEL);
	if (!axera)
		return -ENOMEM;

	pci = devm_kzalloc(dev, sizeof(*pci), GFP_KERNEL);
	if (!pci)
		return -ENOMEM;

	pci->dev = dev;
	pci->ops = &dw_pcie_ops;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pipe_sys");
	base = devm_ioremap(dev, res->start, resource_size(res));
	if (IS_ERR(base))
		return PTR_ERR(base);

	axera->pclk = devm_clk_get(dev, "pclk");
	if (IS_ERR(axera->pclk)) {
		dev_err(dev, "failed to get pclk\n");
		return -ENOMEM;
	}

	axera->aclk = devm_clk_get(dev, "aclk");
	if (IS_ERR(axera->aclk)) {
		dev_err(dev, "failed to get aclk\n");
		return -ENOMEM;
	}

	axera->aclk_sel = devm_clk_get(dev, "aclk_sel");
	if (IS_ERR(axera->aclk_sel)) {
		dev_err(dev, "failed to get aclk_sel\n");
		return -ENOMEM;
	}

	axera->phy_refclk = devm_clk_get(dev, "phy_refclk");
	if (IS_ERR(axera->phy_refclk)) {
		dev_err(dev, "failed to get phy_ref_alt_clk\n");
		return -ENOMEM;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	pci->dbi_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(pci->dbi_base))
		return PTR_ERR(pci->dbi_base);

	axera->base = base;
	axera->pcie_id = pcie_id;
	axera->pci = pci;
	axera->mode = mode;

	spin_lock_init(&s_shmem_data_lock);

	platform_set_drvdata(pdev, axera);

	ret = axera_pcie_clock_init(axera);
	if (ret < 0)
		return ret;

	axera_pcie_aspm_status_set(pci);

	if (!IS_ENABLED(CONFIG_PCIE_AXERA_HOST)) {
		return -ENODEV;
	}

	axera_set_device_type(axera);

	ret = axera_add_pcie_port(axera, pdev);
	if (ret < 0)
		return ret;

	spin_lock_init(&axera->irq_lock);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int axera_pcie_suspend(struct device *dev)
{
	struct axera_pcie *axera = dev_get_drvdata(dev);
	struct dw_pcie *pci = axera->pci;
	u32 val;

	if (axera->mode != DW_PCIE_RC_TYPE)
		return 0;

	/* clear MSE */
	val = dw_pcie_readl_dbi(pci, PCI_COMMAND);
	val &= ~PCI_COMMAND_MEMORY;
	dw_pcie_writel_dbi(pci, PCI_COMMAND, val);

	axera_pcie_stop_link(pci);

	return 0;
}

static int axera_pcie_resume(struct device *dev)
{
	struct axera_pcie *axera = dev_get_drvdata(dev);
	struct dw_pcie *pci = axera->pci;
	u32 val;

	if (axera->mode != DW_PCIE_RC_TYPE)
		return 0;

	/* set MSE */
	val = dw_pcie_readl_dbi(pci, PCI_COMMAND);
	val |= PCI_COMMAND_MEMORY;
	dw_pcie_writel_dbi(pci, PCI_COMMAND, val);

	axera_pcie_establish_link(pci);

	return dw_pcie_wait_for_link(pci);
}
#endif

static const struct dev_pm_ops axera_pcie_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(axera_pcie_suspend, axera_pcie_resume)
};

static struct platform_driver axera_pcie_driver = {
	.probe = axera_pcie_probe,
	.driver = {
		   .name = "axera-pcie",
		   .of_match_table = of_axera_pcie_match,
		   .suppress_bind_attrs = true,
		   .pm	= &axera_pcie_pm_ops,
		   },
};

builtin_platform_driver(axera_pcie_driver);
