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
#include "../dma/ax_pcie_dma_trans.h"

static struct semaphore ioctl_sem;
static p2p_ep_info_t g_p2p_ep_info;
struct device *this_device;
struct device *pci_dma_device_get(void);
static void p2p_fifo_dma_rw_done(struct pcie_dma_task *task);

int ax_p2p_user_open(struct inode *inode, struct file *file)
{
	file->private_data = &g_p2p_ep_info;

	return 0;
}

int ax_pcie_p2p_release(struct inode *inode, struct file *file)
{
	p2p_trace(P2P_DEBUG, "enter");
	return 0;
}

ssize_t ax_pcie_p2p_read(struct file *file, char __user *buf, size_t count, loff_t *f_pos)
{
	p2p_trace(P2P_DEBUG, "enter");
	return -1;
}

ssize_t ax_pcie_p2p_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *f_pos)
{
	p2p_trace(P2P_DEBUG, "enter");
	return 0;
}

static void p2p_dev_cfg_print(const p2p_device_config_t *dev_cfg)
{
	int i;

	printk("device_num:%d\n", dev_cfg->device_num);
	for (i = 0; i < dev_cfg->device_num; i++) {
		printk("[%d]\n", i);
		printk("target_id:0x%x\n", dev_cfg->devices[i].target_id);
		printk("dma_pci_addr:%llx\n", dev_cfg->devices[i].dma_pci_addr);
		printk("mbox_pci_addr:%llx\n", dev_cfg->devices[i].mbox_pci_addr);
		printk("cmm_phy_addr:%llx\n", dev_cfg->devices[i].cmm_phy_addr);
		printk("cmm_phy_addr:%llx\n", dev_cfg->devices[i].cmm_size);
	}
}

static int p2p_mailbox_irq_trigger(void *mailboxaddr, void *data)
{
	int i, size = 32, timeout = 100;
	u32 infor_data, data_reg, regno;
	static volatile int mailbox_fromid = CPU0_MASTERID;
	unsigned long flags;

	P2P_POINTER_CHECK(mailboxaddr);
	ax_pcie_dbi_spin_lock(&flags);

	regno = REG32(mailboxaddr + P2P_SLOT_REQ);	/* reg req */
	//p2p_info("regno = %x", regno);
	while (regno == P2P_REGNO_INVALID && timeout > 0) {
		regno = REG32(mailboxaddr + P2P_SLOT_REQ);
		timeout--;
		udelay(10);
	}

	if (regno == P2P_REGNO_INVALID) {
		ax_pcie_dbi_spin_unlock(&flags);
		p2p_err("ep request msg irq failed!");
		return -3;
	}

	data_reg = MAILBOX_REG_BASE + PCIE_DATA + (regno << 2);	/* data_reg addr */
	//p2p_info("data_reg = %x", data_reg);
	for (i = 0; i < (size / 4); i++) {
		REG32(mailboxaddr + data_reg) = ((unsigned int *)data)[i];
	}
	infor_data = (mailbox_fromid << 28) | P2P_MASTERID << 24 | size;	/* use 16-19 saving masterid */
	//p2p_info("infor_data = %x", infor_data);
	REG32(mailboxaddr + MAILBOX_REG_BASE + PCIE_INFO + (regno << 2)) = infor_data;

	ax_pcie_dbi_spin_unlock(&flags);
	smp_mb();
	return 0;
}

static int p2p_dma_data_submit(p2p_item_send_pack_t *item_pack, unsigned long src, unsigned long dst, unsigned int dma_len)
{
	int ret = 0;
	struct pcie_dma_task task = {0};
	struct pcie_dma_task *ptask;

	if (!dma_len)
		return 0;

	task.state = DMA_TASK_TODO;
	task.dir = PCI_DMA_WRITE;
	task.src = src;
	task.dest = dst;
	task.len = dma_len;
	task.finish = p2p_fifo_dma_rw_done;
	task.llpen = 0;
	if (item_pack->timeout) {
		item_pack->start_tm = ktime_get();
		task.private_data = (void *)item_pack;
	}

	ptask = __pcie_create_task(&task, __VDATA);
	P2P_POINTER_CHECK(ptask);

retry:
	if (item_pack->timeout > 0) {
		ret = wait_event_interruptible_timeout(item_pack->wait, item_pack->done, ((item_pack->timeout * HZ) / 1000));
		if (ret < 0)
			goto retry;
		else if (ret == 0)
			ret = -ETIMEDOUT;
		else
			ret = 0;

	} else if (item_pack->timeout < 0) {
		ret = wait_event_interruptible(item_pack->wait, item_pack->done);
		if (ret < 0)
			goto retry;
	}

	return ret;
}

static void p2p_set_mailbox_outbound_map(unsigned long long addr)
{
	if (g_p2p_ep_info.mbox_pci_addr != addr) {
		g_axera_pcie_opt->start_ob_map(PCIE_P2P_IRQ_BASE, addr, OUTBOUND_INDEX3, P2P_BAR1_SIZE);
		g_p2p_ep_info.mbox_pci_addr = addr;
		p2p_info("outbound mbox_pci_addr: 0x%llx", addr);
	}
}

static int p2p_fifo_piston_start(p2p_item_send_pack_t *item_pack)
{
	int ret;
	p2p_device_info_t *device = NULL;
	u64 dma_pci_addr_offset;

	p2p_info("[%d]", item_pack->device_index);
	device = &g_p2p_ep_info.dev_cfg.devices[item_pack->device_index];
	P2P_POINTER_CHECK(device);
	p2p_set_mailbox_outbound_map(device->mbox_pci_addr);
	p2p_info("[%d] src:0x%llx dst:0x%llx size:0x%x", item_pack->device_index, item_pack->send.src_phy_addr,
		 device->dma_pci_addr, item_pack->send.transfer_size);
	if (item_pack->send.dst_phy_addr < device->cmm_phy_addr) {
		p2p_err("dst_phy_addr:0x%llx < cmm_phy_addr:0x%llx!", item_pack->send.dst_phy_addr, device->cmm_phy_addr);
		return -EINVAL;
	}
	if (item_pack->send.dst_phy_addr + item_pack->send.transfer_size > device->cmm_phy_addr + device->cmm_size) {
		p2p_err("dst_phy_addr:0x%llx + 0x%x > cmm_phy_addr:0x%llx + 0x%llx!",
			item_pack->send.dst_phy_addr, item_pack->send.transfer_size,
			device->cmm_phy_addr, device->cmm_size);
		return -EINVAL;
	}
	dma_pci_addr_offset = device->dma_pci_addr + (item_pack->send.dst_phy_addr - device->cmm_phy_addr);
	p2p_info("dma_pci_addr_offset:0x%llx", dma_pci_addr_offset);
	ret = p2p_dma_data_submit(item_pack, item_pack->send.src_phy_addr, dma_pci_addr_offset, item_pack->send.transfer_size);
	if (ret) {
		p2p_err("p2p_dma_data_submit [%d]", ret);
		return ret;
	}

	return ret;
}

static void p2p_fifo_dma_rw_done(struct pcie_dma_task *task)
{
	int ret;
	p2p_item_send_pack_t *item_pack = task->private_data;
	mailbox_msg_t msg = {0};

	p2p_info("src_target_id:0x%x", g_axera_pcie_opt->local_slotid);

	ret = kfifo_get(&g_p2p_ep_info.p2p_in, &msg); /* get one item from fifo and discard */
	if (ret <= 0) {
		p2p_err("input fifo get!");
		goto exit;
	}

	p2p_info("targetid:%d count:%d", msg.src_target_id, msg.customer_data[0]);
	ret = p2p_mailbox_irq_trigger((void *)g_p2p_ep_info.bar1_virt, &msg); /* send massage to receiving ep */
	if (ret < 0) {
		p2p_err("p2p_mailbox_irq_trigger!");
		goto exit;
	}

exit:
	if (item_pack) {
		item_pack->end_tm = ktime_get();
		item_pack->done = 1;
		wake_up(&item_pack->wait);
	}

	return;
}

static int p2p_fifo_item_package(const p2p_item_send_t *item, p2p_item_send_pack_t *pack)
{
	int j;

	for (j = 0; j < P2P_DEVICE_NUMBER_MAX; j++) {
		if (item->dst_target_id == g_p2p_ep_info.dev_cfg.devices[j].target_id)
			break;
	}
	if (j >= P2P_DEVICE_NUMBER_MAX) {
		p2p_err("can not find target_id:0x%x in device table!", item->dst_target_id);
		return -EFAULT;
	}
	pack->device_index = j;
	pack->send = *item;
	p2p_info("g_p2p_ep_info.target_handle[0].device_index:%d", j);
	return 0;
}

static int p2p_fifo_input(p2p_item_send_t *item)
{
	int ret;
	mailbox_msg_t msg;
	p2p_item_send_pack_t item_pack;

	if (kfifo_is_full(&g_p2p_ep_info.p2p_in)) {
		p2p_err("input fifo overflow!");
		return -ENFILE;
	}
	P2P_RET_CHECK(p2p_fifo_item_package(item, &item_pack));

	item_pack.start_tm = 0;
	item_pack.end_tm = 0;
	item_pack.done = 0;
	item_pack.timeout = item->timeout;
	init_waitqueue_head(&item_pack.wait);

	msg.src_target_id = g_axera_pcie_opt->local_slotid;
	memcpy(msg.customer_data, item->customer_data, sizeof(msg.customer_data[0]) * 7);
	ret = kfifo_put(&g_p2p_ep_info.p2p_in, msg);
	if (ret <= 0) {
		p2p_err("input fifo full!");
		return -ENFILE;
	}

	ret = p2p_fifo_piston_start(&item_pack);
	if (!ret && item_pack.timeout)
		item->time_consume = (item_pack.end_tm - item_pack.start_tm + 500) / 1000;

	return ret;
}

static int p2p_fifo_output(p2p_item_get_t *item)
{
	int ret = kfifo_get(&g_p2p_ep_info.p2p_out, item);
	if (ret <= 0) {
		p2p_err("output fifo empty!");
		return -EFAULT;
	}
	return 0;
}

static int p2p_user_phy_inbound(void)
{
	int j = 0;

	P2P_POINTER_CHECK(g_axera_dev_map[0]);

	for (j = 0; j < P2P_DEVICE_NUMBER_MAX; j++)
		if (g_axera_pcie_opt->local_slotid == g_p2p_ep_info.dev_cfg.devices[j].target_id)
			break;

	if (j >= P2P_DEVICE_NUMBER_MAX) {
		p2p_err("can not find target_id:0x%x in device table!", g_axera_pcie_opt->local_slotid);
		return -EINVAL;
	}
	p2p_info("user cmm addr:%llx", g_p2p_ep_info.dev_cfg.devices[j].cmm_phy_addr);
	if (!g_p2p_ep_info.dev_cfg.devices[j].cmm_phy_addr) {
		p2p_err("device[%d] target_id:0x%x cmm addr is 0!", j, g_axera_pcie_opt->local_slotid);
		return -EINVAL;
	}
	if (!IS_ALIGNED(g_p2p_ep_info.dev_cfg.devices[j].cmm_phy_addr, g_axera_dev_map[0]->shm_size)) {
		p2p_err("device[%d] target_id:0x%x cmm addr:0x%llx should be 0x%llx aligned!",
			j, g_axera_pcie_opt->local_slotid, g_p2p_ep_info.dev_cfg.devices[j].cmm_phy_addr, g_axera_dev_map[0]->shm_size);
		return -EINVAL;
	}
	g_axera_pcie_opt->start_ib_map(g_p2p_ep_info.dev_cfg.devices[j].cmm_phy_addr, INBOUND_INDEX0, BAR_0);
	return 0;
}

#if 0
static int p2p_fifo_input_test(void)
{
	int i, j;
	u32 *mmb_buf = NULL;
	p2p_item_send_t send_item = {0};

	for (j = 0; j < P2P_DEVICE_NUMBER_MAX; j++) {
		g_p2p_ep_info.dev_cfg.devices[j].cmm_phy_addr = 0;
	}

	P2P_POINTER_CHECK(g_axera_dev_map[0]);
	p2p_info("bus:0x%x bar0 cmm addr = 0x%llx", g_axera_pcie_opt->local_slotid, g_axera_dev_map[0]->ib_base);
	if (g_axera_pcie_opt->local_slotid != 0x83)
		return 0;

	send_item.transfer_size = 0x100000;
	mmb_buf = (u32 *)dma_alloc_coherent(this_device, send_item.transfer_size,
		(dma_addr_t *)(&send_item.src_phy_addr), GFP_KERNEL);
	P2P_POINTER_CHECK(mmb_buf);
	memset(mmb_buf, 0x32, send_item.transfer_size);

	p2p_info("send_item.src_phy_addr:%llx", send_item.src_phy_addr);
	send_item.dst_phy_addr = 0x10000;
	send_item.dst_target_id = 0x84;

	for (i = 0; i < 20; i++) {
		send_item.customer_data[0] = i + 10;
		p2p_info("start count:%d", send_item.customer_data[0]);
		p2p_fifo_input(&send_item);
	}

	return 0;
}
#endif

static int p2p_bw_test(p2p_bwt_infos_t *bwt_info)
{
	int i, j, ret = 0;
	u32 mmb_buf_size = 0, count = 0;
	u64 time_start, time_total = 0, time_interval;
	dma_addr_t dma_addr;
	void *mmb_buf = NULL;
	p2p_item_send_t send_item = {0,};
	p2p_device_info_t *device;

	if (!bwt_info->target_num || (bwt_info->target_num > P2P_DEVICE_NUMBER_MAX - 1)) {
		ret = -EINVAL;
		goto exit;
	}

	for (i = 0; i < bwt_info->target_num; i++) {
		for (j = 0; j < P2P_DEVICE_NUMBER_MAX; j++) {
			if (bwt_info->infos[i].target_id == g_p2p_ep_info.dev_cfg.devices[j].target_id)
				break;
		}

		if (j >= P2P_DEVICE_NUMBER_MAX) {
			p2p_err("infos[%d] target_id:%d invalid", i, bwt_info->infos[i].target_id);
			ret = -EINVAL;
			goto exit;
		}

		if (!bwt_info->infos[i].loop_count) {
			p2p_err("infos[%d] loop_count invalid", i);
			ret = -EINVAL;
			goto exit;
		}

		if (!bwt_info->infos[i].chunk_size) {
			p2p_err("infos[%d] chunk_size invalid", i);
			ret = -EINVAL;
			goto exit;
		}

		if (bwt_info->infos[i].chunk_size > mmb_buf_size)
			mmb_buf_size = bwt_info->infos[i].chunk_size;
	}

	mmb_buf = dma_alloc_coherent(this_device, mmb_buf_size, &dma_addr, GFP_KERNEL);
	if (!mmb_buf) {
		p2p_err("failed to alloc(0x%x) bytes for dma buf", mmb_buf_size);
		ret = -EINVAL;
		goto exit;
	}

	p2p_info("src dma-buf info:%llx-%x", (u64)dma_addr, mmb_buf_size);

	memset(mmb_buf, 0x78, mmb_buf_size);

	send_item.src_phy_addr = (u64)dma_addr;
	send_item.timeout = -1;
	send_item.customer_data[6] = P2P_BWT_MAGIC_DATA;

	bwt_info->result.max = 0;
	bwt_info->result.min = 0;
	bwt_info->result.avg = 0;

	for (i = 0; i < bwt_info->target_num; i++) {
		device = &g_p2p_ep_info.dev_cfg.devices[i];

		send_item.dst_target_id = bwt_info->infos[i].target_id;
		send_item.dst_phy_addr = device->cmm_phy_addr;
		send_item.transfer_size = bwt_info->infos[i].chunk_size;

		for (j = 0; j < bwt_info->infos[i].loop_count; j += 1) {
			time_start = ktime_get();
			ret = p2p_fifo_input(&send_item);
			if (ret < 0) {
				p2p_err("failed to send target %d\n", send_item.dst_target_id);
				goto exit;
			}

			time_interval = ktime_get() - time_start;
			if (bwt_info->result.max < time_interval)
				bwt_info->result.max = time_interval;
			if (!bwt_info->result.min || (bwt_info->result.min > time_interval))
				bwt_info->result.min = time_interval;
			count += 1;
			time_total += time_interval;
		}
	}

	bwt_info->result.max = (bwt_info->result.max + 500) / 1000;
	bwt_info->result.min = (bwt_info->result.min + 500) / 1000;
	bwt_info->result.avg = ((time_total / count) + 500) / 1000;
exit:
	if (mmb_buf)
		dma_free_coherent(this_device, mmb_buf_size, mmb_buf, dma_addr);

	return ret;
}

long ax_pcie_p2p_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	p2p_item_send_t send_item;
	p2p_item_get_t get_item;
	unsigned int local_id;
	unsigned long long mbox_pci_addr;

	if (down_interruptible(&ioctl_sem)) {
		p2p_err("acquire handle sem failed!");
		return -EINVAL;
	}

	if (_IOC_TYPE(cmd) == AX_P2P_IOC_BASE) {
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(AX_P2P_IOC_CONFIG_SET):
			p2p_info("AX_P2P_IOC_CONFIG_SET");
			if (copy_from_user((void *)&g_p2p_ep_info.dev_cfg, (void *)arg, sizeof(p2p_device_config_t))) {
				p2p_err("Set parameter to usr space failed!");
				ret = -EFAULT;
				goto ioctl_end;
			}
			if (g_p2p_ep_info.dev_cfg.device_num <= 0 || g_p2p_ep_info.dev_cfg.device_num > P2P_DEVICE_NUMBER_MAX) {
				p2p_err("device_num:%d not in (%d, %d)!", g_p2p_ep_info.dev_cfg.device_num, 0, P2P_DEVICE_NUMBER_MAX);
				ret = -EINVAL;
				goto ioctl_end;
			}
			p2p_dev_cfg_print(&g_p2p_ep_info.dev_cfg);

			ret = p2p_user_phy_inbound();
			if (ret < 0) {
				p2p_err("p2p_user_phy_inbound, [%d]!", ret);
				goto ioctl_end;
			}

			g_p2p_ep_info.status = P2P_OPT_INIT;
			g_p2p_ep_info.mbox_pci_addr = 0;
			break;
		case _IOC_NR(AX_P2P_IOC_CONFIG_GET):
			if (g_p2p_ep_info.status != P2P_OPT_INIT) {
				p2p_err("Not config P2P para! status:%d", g_p2p_ep_info.status);
				ret = -EPERM;
				goto ioctl_end;
			}
			p2p_dev_cfg_print(&g_p2p_ep_info.dev_cfg);
			if (copy_to_user((void *)arg, &g_p2p_ep_info.dev_cfg, sizeof(p2p_device_config_t))) {
				p2p_err("Get parameter from usr space failed");
				ret = -EFAULT;
				goto ioctl_end;
			}
			break;
		case _IOC_NR(AX_P2P_IOC_ITEM_SEND):
			p2p_info("AX_P2P_IOC_ITEM_SEND");
			if (g_p2p_ep_info.status != P2P_OPT_INIT) {
				p2p_err("Not config P2P para! status:%d", g_p2p_ep_info.status);
				ret = -EPERM;
				goto ioctl_end;
			}
			if (copy_from_user((void *)&send_item, (void *)arg, sizeof(p2p_item_send_t))) {
				p2p_err("Send P2P item!");
				ret = -EFAULT;
				goto ioctl_end;
			}
			ret = p2p_fifo_input(&send_item);
			if (ret < 0) {
				p2p_err("p2p_fifo_input! [%d]", ret);
				goto ioctl_end;
			}
			if (send_item.timeout &&
			    copy_to_user((void *)&((p2p_item_send_t *)arg)->time_consume, (void *)&send_item.time_consume, sizeof(send_item.time_consume))) {
				ret = -EFAULT;
				goto ioctl_end;
			}
			break;
		case _IOC_NR(AX_P2P_IOC_ITEM_GET):
			p2p_info("AX_P2P_IOC_ITEM_GET");
			if (g_p2p_ep_info.status != P2P_OPT_INIT) {
				p2p_err("Not config P2P para! status:%d", g_p2p_ep_info.status);
				ret = -EPERM;
				goto ioctl_end;
			}
			ret = p2p_fifo_output(&get_item);
			if (ret < 0) {
				p2p_err("p2p_fifo_output! [%d]", ret);
				goto ioctl_end;
			}
			if (copy_to_user((void *)arg, (void *)&get_item, sizeof(p2p_item_get_t))) {
				p2p_err("Get P2P item!");
				ret = -EFAULT;
				goto ioctl_end;
			}
			break;
		case _IOC_NR(AX_P2P_IOC_START_BWT): {
			p2p_bwt_infos_t bwt_info;

			p2p_info("AX_P2P_IOC_START_BWT");

			if (g_p2p_ep_info.status != P2P_OPT_INIT) {
				p2p_err("Not config P2P para! status:%d", g_p2p_ep_info.status);
				ret = -EPERM;
				goto ioctl_end;
			}

			if (copy_from_user((void *)&bwt_info, (void *)arg, sizeof(p2p_bwt_infos_t))) {
				p2p_err("bandwidth test start failed");
				ret = -EFAULT;
				goto ioctl_end;
			}

			ret = p2p_bw_test(&bwt_info);
			if (ret)
				goto ioctl_end;

			if (copy_to_user((void *)&((p2p_bwt_infos_t *)arg)->result, (void *)&bwt_info.result, sizeof(bwt_info.result))) {
				ret = -EFAULT;
				goto ioctl_end;
			}
			break;
		}

		case _IOC_NR(AX_P2P_IOC_DISABLE):
			p2p_info("AX_P2P_IOC_DISABLE");
			if (!g_axera_dev_map[0]) {
				p2p_err("g_axera_dev_map[0] is null!");
				ret = -EINVAL;
				goto ioctl_end;
			}
			kfifo_reset(&g_p2p_ep_info.p2p_out);
			g_axera_pcie_opt->start_ib_map(g_axera_dev_map[0]->shm_base, INBOUND_INDEX0, BAR_0);
			g_p2p_ep_info.status = P2P_OPT_DEINIT;
			wake_up_interruptible(&g_p2p_ep_info.usr_wait);
			break;

		case _IOC_NR(AX_P2P_IOC_GET_LOCAL_ID):
			p2p_info("AX_P2P_IOC_GET_LOCAL_ID");
			local_id = g_axera_pcie_opt->local_slot_number();
			if (copy_to_user((void *)arg, &local_id, sizeof(local_id))) {
				ret = -EFAULT;
				goto ioctl_end;
			}
			break;
		case _IOC_NR(AX_P2P_IOC_MAILBOX_OUTBOUND):
			if (copy_from_user((void *)&mbox_pci_addr, (void *)arg, sizeof(mbox_pci_addr))) {
				p2p_err("Get mailbox pci addr failed");
				ret = -EFAULT;
				goto ioctl_end;
			}
			p2p_set_mailbox_outbound_map(mbox_pci_addr);
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

unsigned int ax_pcie_p2p_poll(struct file *file, struct poll_table_struct *table)
{
	p2p_ep_info_t *handle = (p2p_ep_info_t *)file->private_data;

	P2P_POINTER_CHECK(handle);

	poll_wait(file, &g_p2p_ep_info.usr_wait, table);

	if (g_p2p_ep_info.status == P2P_OPT_DEINIT) {
		return POLLREMOVE;
	}

	p2p_info("poll enter");
	/* if mem list empty means no data comming */
	if (!kfifo_is_empty(&g_p2p_ep_info.p2p_out)) {
		p2p_info("p2p poll trigger");
		return POLLIN | POLLRDNORM;
	}

	return 0;
}

static struct file_operations ax_pcie_p2p_fops = {
	.owner = THIS_MODULE,
	.open = ax_p2p_user_open,
	.release = ax_pcie_p2p_release,
	.unlocked_ioctl = ax_pcie_p2p_ioctl,
	.write = ax_pcie_p2p_write,
	.read = ax_pcie_p2p_read,
	.poll = ax_pcie_p2p_poll,
};

static struct miscdevice ax_pcie_p2p = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &ax_pcie_p2p_fops,
	.name = "p2p"
};

/* Printk cannot be added to the interrupt function, otherwise there will be scheduling delays of approximately 10ms. */
static void p2p_malibox_irq_handler(int fromid, int regno, int count)
{
	unsigned int status = 0;
	p2p_item_get_t item_get = {0};

	status = g_axera_pcie_opt->clear_msg_irq(regno, count, &item_get.msg);
	// p2p_info("fromid:%d regno:%d count:%d status:%d", fromid, regno, count, status);
	if (status) {
		p2p_info("targetid:0x%x data[6]:0x%x", item_get.msg.src_target_id, item_get.msg.customer_data[6]);
		if (item_get.msg.customer_data[6] == P2P_BWT_MAGIC_DATA)
			return;

		if (kfifo_is_full(&g_p2p_ep_info.p2p_out)) {
			p2p_err("out fifo is full!");
			return;
		}
		kfifo_put(&g_p2p_ep_info.p2p_out, item_get);

		wake_up((wait_queue_head_t *)&g_p2p_ep_info.usr_wait);
		p2p_info("mailbox count:%d", item_get.msg.customer_data[0]);
	}
}

static int __init ax_pcie_p2p_init(void)
{
	int ret;

	p2p_info("ax pcie p2p init slot:0x%x is host:%d", g_axera_pcie_opt->local_slotid, g_axera_pcie_opt->is_host());

	sema_init(&ioctl_sem, 1);
	misc_register(&ax_pcie_p2p);

	if (g_axera_pcie_opt->slave_request_msg_irq(P2P_MASTERID, p2p_malibox_irq_handler)) {
		p2p_err("ep request msg irq failed!");
		ret = -EFAULT;
		goto err0;
	}

	this_device = pci_dma_device_get();
	g_p2p_ep_info.bar1_virt = (unsigned long)devm_ioremap(this_device, PCIE_P2P_IRQ_BASE, SZ_64K);
	if (!g_p2p_ep_info.bar1_virt) {
		p2p_err("bar1_virt remap fail!");
		ret = -ENOMEM;
		goto err0;
	}

	init_waitqueue_head(&g_p2p_ep_info.usr_wait);
	spin_lock_init(&g_p2p_ep_info.hw_lock);
	INIT_KFIFO(g_p2p_ep_info.p2p_in);
	INIT_KFIFO(g_p2p_ep_info.p2p_out);
	p2p_info("ax_pcie_p2p_init end");
	return 0;

err0:
	misc_deregister(&ax_pcie_p2p);
	return ret;
}

static void __exit ax_pcie_p2p_exit(void)
{
	kfifo_free(&g_p2p_ep_info.p2p_in);
	kfifo_free(&g_p2p_ep_info.p2p_out);
	devm_iounmap(this_device, (void *)g_p2p_ep_info.bar1_virt);
	misc_deregister(&ax_pcie_p2p);
}

module_init(ax_pcie_p2p_init);
module_exit(ax_pcie_p2p_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
