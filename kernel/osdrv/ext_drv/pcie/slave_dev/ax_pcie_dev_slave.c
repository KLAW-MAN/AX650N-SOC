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
#include <linux/export.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/kthread.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/pci_ids.h>
#include <linux/random.h>
#include <linux/pci_regs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/sizes.h>
#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_proc.h"
#include "../include/ax_pcie_msg_transfer.h"

struct axera_dev *g_axera_dev_map[MAX_PCIE_DEVICES] = { 0 };

EXPORT_SYMBOL(g_axera_dev_map);

struct ax_pcie_operation *g_axera_pcie_opt;
EXPORT_SYMBOL(g_axera_pcie_opt);

unsigned int shm_size;
module_param(shm_size, uint, S_IRUGO);

unsigned long int shm_phys_addr;
module_param(shm_phys_addr, ulong, S_IRUGO);

// static size_t bar_size[] = { 2097152, 65536 }; //defualt
static size_t bar_size[] = { SZ_8M, 65536 }; //defualt

static int is_host(void)
{
	return 0;
}

static struct axera_dev *alloc_axeradev(void)
{
	struct axera_dev *axera_dev;
	int size;

	size = sizeof(*axera_dev);
	axera_dev = kmalloc(size, GFP_KERNEL);
	if (!axera_dev) {
		axera_trace(AXERA_ERR, "Alloc axera_dev failed!");
		return NULL;
	}

	memset(axera_dev, 0, size);

	return axera_dev;
}

struct axera_dev *slot_to_axdev(unsigned int slot)
{
	/* For slave, the host remains the one and only remote device */
	return g_axera_dev_map[0];
}

struct device *get_pcie_dev(unsigned int slot)
{
	struct axera_dev *dev = slot_to_axdev(slot);

	return (dev && dev->pdev) ? &dev->pdev->dev : NULL;
}
EXPORT_SYMBOL(get_pcie_dev);

static int ax_pcie_get_controller_resource(struct device *dev)
{
	unsigned int ctl_id;
	void __iomem *dbi_base;
	void __iomem *glb_base;
	unsigned int irq;
	struct resource *res;
	struct device_node *np = dev->of_node;
	struct platform_device *pdev = to_platform_device(dev);

	of_property_read_u32(np, "ctl-id", &ctl_id);
	if ((ctl_id != 0) && (ctl_id != 1)) {
		axera_trace(AXERA_ERR, "Get pcie controller id fail");
		return -1;
	}

	g_axera_pcie_opt->local_controller[ctl_id] =
	    devm_kzalloc(dev, sizeof(struct ax_pcie_controller), GFP_KERNEL);
	if (NULL == g_axera_pcie_opt->local_controller[ctl_id]) {
		axera_trace(AXERA_ERR, "kmalloc for pcie_controller%d failed!",
			    ctl_id);
		return -ENOMEM;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dbi");
	dbi_base = devm_ioremap(dev, res->start, resource_size(res));
	if (IS_ERR(dbi_base))
		return PTR_ERR(dbi_base);

	g_axera_pcie_opt->local_controller[ctl_id]->config_base = res->start;
	g_axera_pcie_opt->local_controller[ctl_id]->config_virt = dbi_base;
	g_axera_pcie_opt->local_controller[ctl_id]->config_size = resource_size(res);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pipe_sys");
	glb_base = devm_ioremap(dev, res->start, resource_size(res));
	if (IS_ERR(glb_base))
		return PTR_ERR(glb_base);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		axera_trace(AXERA_ERR, "missing IRQ resource: %d", irq);
		return irq;
	}
	g_axera_pcie_opt->local_controller[ctl_id]->irq = irq;
	g_axera_pcie_opt->local_ctl_id = ctl_id;
	g_axera_pcie_opt->local_controller[ctl_id]->glb_base = glb_base;

	return 0;
}

static int axdev_slv_init(struct ax_pcie_operation *opt)
{
	struct ax_pcie_operation *pci_opt = opt;

	if (NULL == pci_opt) {
		axera_trace(AXERA_ERR,
			    "axdev_slv_init failed, ax_pcie_operation is NULL!");
		return -1;
	}

	pci_opt->is_host = is_host;
	pci_opt->slot_to_axdev = slot_to_axdev;
	pci_opt->remote_device_number = 1;

	return 0;
}

int _ax_pcie_alloc_space(struct device *dev, struct axera_dev *ax_dev, size_t size, enum pci_barno bar)
{
	void *space;
	size_t alignment = 0xffff;
	dma_addr_t org_phys_addr;
	dma_addr_t phys_addr;
	u64 offset;

	if (size > alignment) {
		alignment = size - 1;
	}

	if (shm_phys_addr) {
		if (shm_phys_addr & 0xffff) {
			axera_trace(AXERA_ERR,
				    "Invalid address, address needs 64K alignment");
		} else {
			phys_addr = shm_phys_addr;
			space = devm_ioremap(dev, shm_phys_addr, size);
			if (!space) {
				dev_err(dev, "shm_phys_addr ioremap fail\n");
				return -1;
			}
		}
	}

	if (!shm_phys_addr) {
		space =
		    dma_alloc_coherent(dev, size + alignment, &org_phys_addr,
				       GFP_KERNEL | __GFP_ZERO);
		if (!space) {
			dev_err(dev, "failed to allocate mem space\n");
			return -1;
		}

		ax_dev->org_bar_info[bar].addr = space;
		ax_dev->org_bar_info[bar].size = size + alignment;
		ax_dev->org_bar_info[bar].phys_addr = org_phys_addr;

		phys_addr = (org_phys_addr + alignment) & ~alignment;
		offset = phys_addr - org_phys_addr;
		space = space + offset;
	}

	ax_dev->bar_info[bar].addr = space;
	ax_dev->bar_info[bar].size = size;
	ax_dev->bar_info[bar].phys_addr = phys_addr;

	axera_trace(AXERA_DEBUG, "phys_addr = %llx", phys_addr);
	axera_trace(AXERA_DEBUG, "space = %p", space);
	return 0;
}

static int ax_pcie_alloc_space(struct device *dev)
{
	struct axera_dev *ax_dev = g_axera_dev_map[0];
	unsigned int size = bar_size[BAR_0];
	int ret = 0;

	if (shm_size)
		size = shm_size;

	ret = _ax_pcie_alloc_space(dev, ax_dev, size, BAR_0);
	if (ret < 0)
		dev_err(dev, "Failed to allocate space for BAR%d\n", BAR_0);

	return ret;
}

static void ax_pcie_bar_info_map(void)
{
	struct axera_dev *ax_dev = g_axera_dev_map[0];

	/* Use bar0 to map ddr share memory */
	ax_dev->shm_base = ax_dev->bar_info[BAR_0].phys_addr;
#if SUPPORT_PCI_NET
	ax_dev->shm_size = ax_dev->bar_info[BAR_0].size / 2;
#else
	ax_dev->shm_size = ax_dev->bar_info[BAR_0].size;
#endif
	ax_dev->shm_end = ax_dev->shm_base + ax_dev->shm_size;
	ax_dev->ib_base = ax_dev->shm_base;
	g_axera_pcie_opt->start_ib_map(ax_dev->shm_base, INBOUND_INDEX0, BAR_0);

	axera_trace(AXERA_DEBUG, "hm_base:%llx\n", ax_dev->shm_base);
	/* Use bar1 to map mailbox */
	ax_dev->bar_info[BAR_1].phys_addr = MAILBOX_REG_MAP_ADDR;
	ax_dev->bar_info[BAR_1].size = bar_size[BAR_1];
	g_axera_pcie_opt->start_ib_map((u64)(ax_dev->bar_info[BAR_1].phys_addr), INBOUND_INDEX1, BAR_1);

	/* Use bar4 to system reset register */
	g_axera_pcie_opt->slave_reset_map();
	return;
}

static int pcie_axdev_slave_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct axera_dev *ax_dev = g_axera_dev_map[0];
	int ret;
	void (*slave_handler) (int fromid, int regno, int count);

	axera_trace(AXERA_DEBUG, "ax pcie slave dev probe\n");

	dma_set_mask(dev, DMA_BIT_MASK(64));
	dma_set_coherent_mask(dev, DMA_BIT_MASK(64));

	ret = ax_pcie_get_controller_resource(dev);
	if (ret < 0) {
		axera_trace(AXERA_ERR, "get controller resource fail\n");
		return ret;
	}

	ret = ax_pcie_alloc_space(dev);
	if (ret < 0) {
		dev_err(dev, "slave ddr share memory alloc failed\n");
		return ret;
	}

#ifdef SHMEM_FROM_MASTER
	axera_pcie_awmisc_enable(false);
	axera_pcie_armisc_enable(false);
#else
	axera_pcie_awmisc_enable(true);
	axera_pcie_armisc_enable(true);
#endif

#ifdef SHMEM_FROM_MASTER
	ax_dev->ob_base_virt = devm_ioremap(dev, PCIE_SPACE_SHMEM_BASE, SZ_4M);
	ax_dev->ob_base = PCIE_SPACE_SHMEM_BASE;
	ax_dev->ob_size = SZ_4M;
	axera_trace(AXERA_DEBUG, "ob base virt = %p\n", ax_dev->ob_base_virt);
#endif

	ax_pcie_bar_info_map();
	g_axera_pcie_opt->local_slot_number();
	axera_trace(AXERA_DEBUG, "g_axera_pcie_opt->local_slotid:%d remote_device_number:%d\n", g_axera_pcie_opt->local_slotid, g_axera_pcie_opt->remote_device_number);

	slave_handler = slave_message_irq_handler;
	if (g_axera_pcie_opt->slave_request_msg_irq(PCIE_MASTERID, slave_handler)) {
		msg_trace(MSG_ERR, "Slave request msg irq failed!");
		ret = -1;
		goto alloc_space_error;
	}

	ax_dev->pdev = pdev;
	ax_dev->slot_index = 0;

	return 0;

alloc_space_error:
	if (shm_phys_addr) {
		devm_iounmap(dev, ax_dev->org_bar_info[0].addr);
	} else {
		dma_free_coherent(&pdev->dev, ax_dev->org_bar_info[BAR_0].size,
			ax_dev->org_bar_info[BAR_0].addr, ax_dev->org_bar_info[BAR_0].phys_addr);
	}
	return ret;
}

static int pcie_axdev_slave_remove(struct platform_device *pdev)
{
	struct axera_dev *ax_dev = g_axera_dev_map[0];

	g_axera_pcie_opt->release_msg_irq(ax_dev);
	if (shm_phys_addr) {
		devm_iounmap(&pdev->dev, ax_dev->org_bar_info[0].addr);
	} else {
		dma_free_coherent(&pdev->dev, ax_dev->org_bar_info[BAR_0].size,
				ax_dev->org_bar_info[BAR_0].addr, ax_dev->org_bar_info[BAR_0].phys_addr);
	}
	return 0;
}

static const struct of_device_id pcie_ep_match[] = {
	{.compatible = "axera,axera-pcie-ep",},
	{ },
};

MODULE_DEVICE_TABLE(of, pcie_ep_match);

static struct platform_driver axdev_driver = {
	.driver = {
		.name =	"ax_pcie_ep",
		.of_match_table	= pcie_ep_match,
	},

	.probe = pcie_axdev_slave_probe,
	.remove = pcie_axdev_slave_remove,
};

static struct platform_driver *const ax_pcie_driver[] = {
	&axdev_driver,
};

static int __init ax_pcie_slave_init(void)
{
	int ret = 0;

	g_axera_pcie_opt = kmalloc(sizeof(struct ax_pcie_operation), GFP_KERNEL);
	if (!g_axera_pcie_opt) {
		axera_trace(AXERA_ERR, "alloc ax_pcie_operation failed!");
		return -ENOMEM;
	}

	g_axera_dev_map[0] = alloc_axeradev();
	if (!g_axera_dev_map[0]) {
		axera_trace(AXERA_ERR, "alloc device err!");
		ret = -ENOMEM;
		goto allo_axdev_err;
	}

	ret = axdev_slv_init(g_axera_pcie_opt);
	if (ret) {
		axera_trace(AXERA_ERR, "axdev_slv_init failed!");
		goto axdev_slv_init_err;
	}

	ret = ax_pcie_arch_init(g_axera_pcie_opt);
	if (ret) {
		axera_trace(AXERA_ERR, "ax_pci_arch_init failed!");
		goto ax_pci_arch_init_err;
	}

	ret = platform_register_drivers(ax_pcie_driver, ARRAY_SIZE(ax_pcie_driver));
	if (ret) {
		pr_err("Failed to register platform driver --> %d\n", ret);
		goto platform_register_driver_err;
	}

#ifdef MSG_PROC_ENABLE
	if (ax_pcie_proc_init("slave_transfer_info")) {
		axera_trace(AXERA_ERR, "init proc failed!\n");
		ret = -1;
		goto proc_init_err;
	}
#endif

	return ret;

#ifdef MSG_PROC_ENABLE
proc_init_err:
platform_unregister_drivers(ax_pcie_driver, ARRAY_SIZE(ax_pcie_driver));
#endif
platform_register_driver_err:
ax_pcie_arch_exit();
ax_pci_arch_init_err:
axdev_slv_init_err:
	kfree(g_axera_dev_map[0]);
allo_axdev_err:
	kfree(g_axera_pcie_opt);

	return ret;
}

static void __exit ax_pcie_slave_exit(void)
{
#ifdef MSG_PROC_ENABLE
	ax_pcie_proc_exit("slave_transfer_info");
#endif
	platform_unregister_drivers(ax_pcie_driver, ARRAY_SIZE(ax_pcie_driver));
	ax_pcie_arch_exit();
	kfree(g_axera_dev_map[0]);
	kfree(g_axera_pcie_opt);
}

module_init(ax_pcie_slave_init);
module_exit(ax_pcie_slave_exit);

MODULE_AUTHOR("Axera");
MODULE_DESCRIPTION("Axera slave dev");
MODULE_LICENSE("GPL v2");
