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
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/export.h>
#include "../include/ax_pcie_dev.h"

#define DRV_MODULE_NAME			"ax_pcie_boot"

#define	PCIE_MALLOC_BUF_ERR			-21

#define AX_PCIE_ENDPOINT_MAGIC			0x0
#define MAX_TRANSFER_SIZE			(0x300000)

#define AX_PCIE_ENDPOINT_COMMAND		0x4
#define AX_PCIE_COMMAND_READ			BIT(3)
#define AX_PCIE_COMMAND_WRITE			BIT(4)
#define AX_PCIE_COMMAND_BOOT_REASION		BIT(5)

#define AX_PCIE_ENDPOINT_STATUS			0x8
#define STATUS_READ_SUCCESS			BIT(0)
#define STATUS_READ_FAIL			BIT(1)

#define STATUS_WRITE_SUCCESS			BIT(13)
#define STATUS_WRITE_FAIL			BIT(14)

#define STATUS_COPY_SUCCESS			BIT(4)
#define STATUS_COPY_FAIL			BIT(5)
#define STATUS_IRQ_RAISED			BIT(6)
#define STATUS_SRC_ADDR_INVALID			BIT(7)
#define STATUS_DST_ADDR_INVALID			BIT(8)

#define	BOOT_START_DEVICES			BIT(0)

#define AX_PCIE_ENDPOINT_LOWER_SRC_ADDR		0x0c
#define AX_PCIE_ENDPOINT_UPPER_SRC_ADDR		0x10

#define AX_PCIE_ENDPOINT_LOWER_DST_ADDR		0x14
#define AX_PCIE_ENDPOINT_UPPER_DST_ADDR		0x18

#define AX_PCIE_ENDPOINT_SIZE			0x1c
#define AX_PCIE_ENDPOINT_CHECKSUM		0x20

#define AX_PCIE_ENDPOINT_BOOT_REASON_TYPE	0x24
#define AX_PCIE_ENDPOINT_FINISH_STATUS		0x28

#define AX_GET_ALL_DEVICES		_IOW('P', 0x1, unsigned long)
#define AX_PCIE_BOOT			_IOW('P', 0x2, unsigned long)
#define AX_START_DEVICES		_IOW('P', 0x3, unsigned long)
#define AX_PCIE_DUMP			_IOW('P', 0x4, unsigned long)
#define AX_PCIE_RESET_DEVICE		_IOW('P', 0x5, unsigned long)
#define AX_PCIE_BOOT_REASON		_IOW('P', 0x6, unsigned long)

#define MAX_DEVICE_NUM  32
#define PCIE_BOOT_MAGIC 0xfc
#define PCIE_DUMP_MAGIC 0xfd

struct ax_device_info {
	unsigned int id;
	unsigned int dev_type;
};

struct boot_attr {
	unsigned int id;
	unsigned int type;
	unsigned long int src;
	unsigned long int dest;
	unsigned int len;
	unsigned int image_len;

	struct ax_device_info remote_devices[MAX_DEVICE_NUM];
};

static inline u32 pcie_dev_readl(struct axera_dev *ax_dev, u32 offset)
{
	return readl(ax_dev->bar_virt[0] + offset);
}

static inline void pcie_dev_writel(struct axera_dev *ax_dev,
				   u32 offset, u32 value)
{
	writel(value, ax_dev->bar_virt[0] + offset);
}

static int pcie_boot_get_devices_info(struct boot_attr *attr)
{
	struct axera_dev *ax_dev;
	int i = 0;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		axera_trace(AXERA_DEBUG, "axera: slot index = %d",
			    ax_dev->slot_index);
		if (ax_dev->slot_index != 0) {
			attr->remote_devices[i].id = ax_dev->slot_index;
			attr->remote_devices[i].dev_type = ax_dev->device_id;
			axera_trace(AXERA_DEBUG,
				    "device slot[%d] vendor-device id[%x]",
				    attr->remote_devices[i].id,
				    attr->remote_devices[i].dev_type);
		}
	}

	return 0;
}

static int transfer_boot_data(struct axera_dev *ax_dev, struct boot_attr *attr)
{
	bool ret = false;
	u32 reg = 0;
	void *addr;
	dma_addr_t phys_addr;
	struct pci_dev *pdev = ax_dev->pdev;
	struct device *dev = &pdev->dev;
	void *orig_addr;
	dma_addr_t orig_phys_addr;
	size_t offset;
	size_t alignment = 0xffff;
	int i;
	u32 size = 0;
	u32 last_length = 0;
	u64 source_addr = 0;
	u64 target_addr = 0;
	u32 max_loop_times = 0;

	axera_trace(AXERA_DEBUG, "transfer boot image data");

	size = attr->len;
	source_addr = attr->src;
	target_addr = attr->dest;
	axera_trace(AXERA_DEBUG, "dest = %llx", target_addr);

	max_loop_times = size / MAX_TRANSFER_SIZE;
	last_length = size % MAX_TRANSFER_SIZE;
	if (last_length > 0)
		max_loop_times += 1;

	axera_trace(AXERA_DEBUG, "transfer loop count = %d last_length = %x",
		    max_loop_times, last_length);

	orig_addr = dma_alloc_coherent(dev, MAX_TRANSFER_SIZE + alignment,
				       &orig_phys_addr, GFP_KERNEL);
	if (!orig_addr) {
		dev_err(dev, "Failed to allocate address\n");
		return PCIE_MALLOC_BUF_ERR;
	}

	if (alignment && !IS_ALIGNED(orig_phys_addr, alignment)) {
		phys_addr = PTR_ALIGN(orig_phys_addr, alignment);
		offset = phys_addr - orig_phys_addr;
		addr = orig_addr + offset;
	} else {
		phys_addr = orig_phys_addr;
		addr = orig_addr;
	}

	for (i = 0; i < max_loop_times; i++) {

		axera_trace(AXERA_DEBUG, "loop [%d] transfer ...", i);
		if ((i == (max_loop_times - 1)) && (last_length != 0)) {
			size = last_length;
		} else {
			size = MAX_TRANSFER_SIZE;
		}

		axera_trace(AXERA_DEBUG, "boot loop[%d] size = %x", i, size);

		ret = copy_from_user(addr, (void *)source_addr, size);
		if (ret != 0) {
			axera_trace(AXERA_ERR, "copy_from_user fail");
			ret = -EFAULT;
			goto boot_err;
		}

		source_addr += size;

		axera_trace(AXERA_DEBUG, "phys_addr = %llx", phys_addr);

		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_SRC_ADDR,
				lower_32_bits(phys_addr));
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_SRC_ADDR,
				upper_32_bits(phys_addr));
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_DST_ADDR,
				lower_32_bits(target_addr));
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_DST_ADDR,
				upper_32_bits(target_addr));

		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_SIZE, size);
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_COMMAND,
				AX_PCIE_COMMAND_READ);

		while (1) {
			reg = pcie_dev_readl(ax_dev, AX_PCIE_ENDPOINT_STATUS);
			if (reg == 0) {
				continue;
			} else {
				if (reg & STATUS_READ_SUCCESS) {
					axera_trace(AXERA_INFO,
						    "loop[%d]transfer image success.",
						    i);
					pcie_dev_writel(ax_dev,
							AX_PCIE_ENDPOINT_STATUS,
							0x0);
					ret = 0;
					break;
				} else {
					axera_trace(AXERA_INFO,
						    "loop[%d]transfer image fail.",
						    i);
					pcie_dev_writel(ax_dev,
							AX_PCIE_ENDPOINT_STATUS,
							0x0);
					ret = -EFAULT;
					goto boot_err;
				}
			}
		}

		target_addr += size;
	}

boot_err:
	dma_free_coherent(dev, MAX_TRANSFER_SIZE + alignment, orig_addr, orig_phys_addr);
	return ret;
}

static int transfer_dump_data(struct axera_dev *ax_dev, struct boot_attr *attr)
{
	bool ret = false;
	u32 reg = 0;
	void *cma_vir_addr;
	dma_addr_t phys_addr;
	struct pci_dev *pdev = ax_dev->pdev;
	struct device *dev = &pdev->dev;
	void *orig_addr;
	dma_addr_t orig_phys_addr;
	size_t offset;
	size_t alignment = 0xffff;
	int i;
	u32 size = 0;
	u32 last_length = 0;
	u64 source_addr = 0;
	u64 target_addr = 0;
	u32 max_loop_times = 0;

	axera_trace(AXERA_DEBUG, "transfer dump image data");

	size = attr->len;
	source_addr = attr->src;
	target_addr = attr->dest;

	axera_trace(AXERA_DEBUG, "target_addr = %llx", target_addr);

	max_loop_times = size / MAX_TRANSFER_SIZE;
	last_length = size % MAX_TRANSFER_SIZE;
	if (last_length > 0)
		max_loop_times += 1;

	axera_trace(AXERA_DEBUG, "transfer loop count = %d last_length = %x",
		    max_loop_times, last_length);

	orig_addr = dma_alloc_coherent(dev, MAX_TRANSFER_SIZE + alignment,
				       &orig_phys_addr, GFP_KERNEL);
	if (!orig_addr) {
		dev_err(dev, "Failed to allocate address\n");
		return PCIE_MALLOC_BUF_ERR;
	}

	if (alignment && !IS_ALIGNED(orig_phys_addr, alignment)) {
		phys_addr = PTR_ALIGN(orig_phys_addr, alignment);
		offset = phys_addr - orig_phys_addr;
		cma_vir_addr = orig_addr + offset;
	} else {
		phys_addr = orig_phys_addr;
		cma_vir_addr = orig_addr;
	}

	for (i = 0; i < max_loop_times; i++) {

		axera_trace(AXERA_DEBUG, "loop [%d] transfer ...", i);
		if ((i == (max_loop_times - 1)) && (last_length != 0)) {
			size = last_length;
		} else {
			size = MAX_TRANSFER_SIZE;
		}

		axera_trace(AXERA_DEBUG, "boot loop[%d] size = %x", i, size);


		axera_trace(AXERA_DEBUG, "phys_addr = %llx", phys_addr);

		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_SRC_ADDR,
				lower_32_bits(source_addr));
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_SRC_ADDR,
				upper_32_bits(source_addr));
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_DST_ADDR,
				lower_32_bits(phys_addr));
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_DST_ADDR,
				upper_32_bits(phys_addr));

		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_SIZE, size);
		pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_COMMAND,
				AX_PCIE_COMMAND_WRITE);

		while (1) {
			reg = pcie_dev_readl(ax_dev, AX_PCIE_ENDPOINT_STATUS);
			if (reg == 0) {
				continue;
			} else {
				if (reg & STATUS_WRITE_SUCCESS) {
					axera_trace(AXERA_INFO, "loop[%d]transfer image success.", i);
					ret = copy_to_user((void *)target_addr, cma_vir_addr, size);
					if (ret != 0) {
						axera_trace(AXERA_ERR, "copy_to_user fail");
						pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_STATUS, 0x0);
						ret = -EFAULT;
						goto dump_err;
					}

					pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_STATUS, 0x0);
					ret = 0;
					break;
				} else {
					axera_trace(AXERA_INFO, "loop[%d]transfer image fail.", i);
					pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_STATUS, 0x0);
					ret = -EFAULT;
					goto dump_err;
				}
			}
		}

		source_addr += size;
		target_addr += size;
	}

dump_err:
	dma_free_coherent(dev, MAX_TRANSFER_SIZE + alignment, orig_addr, orig_phys_addr);
	return ret;
}

static void start_devices(struct axera_dev *ax_dev)
{
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_FINISH_STATUS,
			BOOT_START_DEVICES);
}

static void ax_reset_devices(struct axera_dev *ax_dev)
{
	g_axera_pcie_opt->reset_device(ax_dev);
}

static void pcie_boot_start_devices(struct boot_attr *attr)
{
	struct axera_dev *ax_dev;
	int i = 0;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		if (attr->id == ax_dev->slot_index) {
			start_devices(ax_dev);
		}
	}
}

static int pcie_boot_transfer_data(struct boot_attr *attr)
{
	struct axera_dev *ax_dev;
	int i = 0;
	int ret = 0;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		if (attr->id == ax_dev->slot_index) {
			ret = transfer_boot_data(ax_dev, attr);
		}
	}

	return ret;
}

static int get_boot_reason(struct axera_dev *ax_dev)
{
	int ret = 0;

	ret = pcie_dev_readl(ax_dev, AX_PCIE_ENDPOINT_BOOT_REASON_TYPE);
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_BOOT_REASON_TYPE, 0x0);
	return ret;
}

static int pcie_get_boot_reason(struct boot_attr *attr)
{
	struct axera_dev *ax_dev;
	int i = 0;
	int ret = 0;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		if (attr->id == ax_dev->slot_index) {
			ret = get_boot_reason(ax_dev);
		}
	}

	return ret;
}

static int pcie_dump_transfer_data(struct boot_attr *attr)
{
	struct axera_dev *ax_dev;
	int i = 0;
	int ret = 0;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		if (attr->id == ax_dev->slot_index) {
			ret = transfer_dump_data(ax_dev, attr);
		}
	}

	return ret;
}

static int pcie_reset_device(struct boot_attr *attr)
{
	struct axera_dev *ax_dev;
	int i = 0;
	int ret = 0;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		if (attr->id == ax_dev->slot_index) {
			ax_reset_devices(ax_dev);
		}
	}

	return ret;

}

static long ax_pcie_boot_ioctl(struct file *file, unsigned int cmd,
			       unsigned long arg)
{
	int ret = 0;
	struct boot_attr attr;

	axera_trace(AXERA_DEBUG, "Pcie boot ioctl");

	if (copy_from_user
	    ((void *)&attr, (void *)arg, sizeof(struct boot_attr))) {
		printk("copy params form usr failed.\n");
		return -1;
	}

	switch (cmd) {
	case AX_GET_ALL_DEVICES:
		axera_trace(AXERA_DEBUG, "cmd = AX_GET_ALL_DEVICES");
		ret = pcie_boot_get_devices_info(&attr);

		if (copy_to_user
		    ((void *)arg, (void *)&attr, sizeof(struct boot_attr))) {
			printk("Copy data to usr space failed.\n");
			return -EFAULT;
		}
		break;
	case AX_PCIE_BOOT:
		axera_trace(AXERA_DEBUG, "cmd = AX_PCIE_BOOT");
		ret = pcie_boot_transfer_data(&attr);
		break;
	case AX_START_DEVICES:
		axera_trace(AXERA_DEBUG, "cmd = AX_START_DEVICES");
		pcie_boot_start_devices(&attr);
		break;
	case AX_PCIE_DUMP:
		axera_trace(AXERA_DEBUG, "cmd = AX_PCIE_DUMP");
		ret = pcie_dump_transfer_data(&attr);
		break;
	case AX_PCIE_BOOT_REASON:
		axera_trace(AXERA_DEBUG, "cmd = AX_GET_BOOT_REASON");
		ret = pcie_get_boot_reason(&attr);
		break;
	case AX_PCIE_RESET_DEVICE:
		axera_trace(AXERA_DEBUG, "cmd = AX_PCIE_RESET_DEVICE");
		pcie_reset_device(&attr);
		break;
	}

	return ret;
}

static struct file_operations boot_usrdev_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ax_pcie_boot_ioctl,
};

static struct miscdevice ax_boot_usrdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &boot_usrdev_fops,
	.name = DRV_MODULE_NAME,
};

int __init ax_pcie_boot_init(void)
{
	int err;

	err = misc_register(&ax_boot_usrdev);
	if (err) {
		axera_trace(AXERA_ERR, "Failed to register device");
		goto err_kfree_name;
	}

	return err;
err_kfree_name:
	kfree(ax_boot_usrdev.name);
	return err;
}

void __exit ax_pcie_boot_exit(void)
{
	misc_deregister(&ax_boot_usrdev);
}

module_init(ax_pcie_boot_init);
module_exit(ax_pcie_boot_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
