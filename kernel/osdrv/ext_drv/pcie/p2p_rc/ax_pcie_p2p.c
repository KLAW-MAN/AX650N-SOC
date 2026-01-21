/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_msg_transfer.h"
#include "../include/ax_pcie_proc.h"
#include "../include/ax_pcie_p2p.h"

static struct semaphore ioctl_sem;

#ifndef IS_AX_SLAVE
/* If ACS is supported, disable ACS, otherwise bypass */
static int pci_bridge_has_acs_redir(struct pci_dev *pdev)
{
	int pos;
	u16 ctrl;

	pos = pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_ACS);
	if (!pos) {
		return 0;
	}
	pci_read_config_word(pdev, pos + PCI_ACS_CTRL, &ctrl);

	ctrl = ctrl & ~(PCI_ACS_RR | PCI_ACS_CR | PCI_ACS_EC);
	pci_write_config_word(pdev, pos + PCI_ACS_CTRL, ctrl);

	return 0;
}

static int ax_pcie_p2p_support_check(struct pci_dev *pdev)
{
	struct pci_dev *parent;

	parent = to_pci_dev(pdev->dev.parent);

	P2P_RET_CHECK_V2(pci_is_root_bus(parent->bus));
	P2P_RET_CHECK(pci_bridge_has_acs_redir(parent));

	p2p_info("bus = 0x%x\n", parent->bus->number);

	return parent->bus->number;
}

static int ax_pcie_p2p_check(void)
{
	int i;
	int bus;
	struct axera_dev *ax_dev;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		bus = ax_pcie_p2p_support_check(ax_dev->pdev);
		if (bus <= 0) {
			p2p_err("bus number[%d] illegal!", bus);
			return -1;
		}
	}
	return 0;
}
#endif

static int ax_pcie_p2p_device_info_get(p2p_device_config_t *cfg_p)
{
	int i;
	u32 bar0_addr, bar1_addr;
	struct axera_dev *ax_dev = NULL;
	p2p_device_info_t *info_p;

	p2p_info("remote_device_number:%d", g_axera_pcie_opt->remote_device_number);
	cfg_p->device_num = g_axera_pcie_opt->remote_device_number;

	for (i = 0; i < cfg_p->device_num; i++) {
		ax_dev = g_axera_dev_map[i];
		info_p = &cfg_p->devices[i];
		g_axera_pcie_opt->pci_config_read(ax_dev, CONFIG_BAR0_ADDRESS_OFFSET, &bar0_addr);
		g_axera_pcie_opt->pci_config_read(ax_dev, CONFIG_BAR1_ADDRESS_OFFSET, &bar1_addr);

		info_p->target_id = ax_dev->slot_index;
		info_p->dma_pci_addr = bar0_addr & P2P_BAR_MASK;
		info_p->cmm_phy_addr = ax_dev->ib_base;
		info_p->cmm_size = ax_dev->bar_info[BAR_0].size;
		info_p->mbox_pci_addr = bar1_addr;

		p2p_info("targetid = 0x%x", info_p->target_id);
		p2p_info("bar0 pci addr = 0x%x", bar0_addr);
		p2p_info("bar0 cmm addr = 0x%llx", info_p->cmm_phy_addr);
		p2p_info("bar0 pci size = 0x%llx", info_p->cmm_size);
		p2p_info("bar1 pci addr = 0x%x", bar1_addr);
	}
	return 0;
}

static int ax_p2p_user_open(struct inode *inode, struct file *file)
{
	file->private_data = 0;
	p2p_trace(MSG_INFO, "open success");
	return 0;
}


static int ax_pcie_p2p_release(struct inode *inode, struct file *file)
{
	p2p_trace(MSG_INFO, "release success");
	return 0;
}

static long ax_pcie_p2p_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	p2p_device_config_t cfg_val;

	if (down_interruptible(&ioctl_sem)) {
		p2p_err("acquire handle sem failed!");
		return -EINVAL;
	}

	if (_IOC_TYPE(cmd) == AX_P2P_IOC_BASE) {
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(AX_P2P_IOC_CONFIG_GET):
			ret = ax_pcie_p2p_check();
			if (ret < 0) {
				p2p_err("p2p is not support");
				goto ioctl_end;
			}
			ax_pcie_p2p_device_info_get(&cfg_val);
			if (copy_to_user((void *)arg, &cfg_val, sizeof(p2p_device_config_t))) {
				p2p_err("Get parameter from usr space failed");
				ret = -EFAULT;
				goto ioctl_end;
			}
			break;
		default:
			ret = -EINVAL;
			p2p_err("CMD:%d not defined!", cmd);
			goto ioctl_end;;
		}
	}

ioctl_end:
	up(&ioctl_sem);

	return ret;
}

static struct file_operations ax_pcie_p2p_fops = {
    .owner = THIS_MODULE,
    .open = ax_p2p_user_open,
    .release = ax_pcie_p2p_release,
    .unlocked_ioctl = ax_pcie_p2p_ioctl,
};

static struct miscdevice ax_pcie_p2p = {
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &ax_pcie_p2p_fops,
    .name = "p2p"};

static int __init ax_pcie_p2p_init(void)
{
	p2p_info("ax pcie p2p init slot:0x%x is host:%d", g_axera_pcie_opt->local_slotid, g_axera_pcie_opt->is_host());

	sema_init(&ioctl_sem, 1);
	misc_register(&ax_pcie_p2p);

	return 0;
}

static void __exit ax_pcie_p2p_exit(void)
{
	misc_deregister(&ax_pcie_p2p);
}

module_init(ax_pcie_p2p_init);
module_exit(ax_pcie_p2p_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
