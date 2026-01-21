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
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/time64.h>
#include <uapi/linux/time.h>
#include <linux/timekeeping.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/namei.h>
#include <linux/fs.h>
#include <asm/unistd.h>
#include <linux/types.h>
#include <linux/init.h>
#include "axcl_pcie_slave.h"

static ax_pcie_msg_handle_t *port_handle[AXERA_MAX_MAP_DEV][MAX_MSG_PORTS];
static spinlock_t g_axcl_lock;

static int axcl_pcie_notifier_recv(void *handle, void *buf, unsigned int length)
{
	struct ax_transfer_handle *transfer_handle =
	    (struct ax_transfer_handle *)handle;
	ax_pcie_msg_handle_t *msg_handle;

	struct ax_mem_list *mem;
	void *data;
	unsigned long flags = 0;

	axcl_trace(AXCL_DEBUG, "nortifier_recv addr 0x%lx len 0x%x.",
		   (unsigned long int)buf, length);

	msg_handle = (ax_pcie_msg_handle_t *) transfer_handle->data;
	axcl_trace(AXCL_DEBUG, "notify msg handle 0x%lx",
		   (unsigned long)msg_handle);
	data = kmalloc(length + sizeof(struct ax_mem_list), GFP_ATOMIC);
	if (!data) {
		axcl_trace(AXCL_ERR, "Data kmalloc failed.");
		return -1;
	}

	mem = (struct ax_mem_list *)data;
	mem->data = data + sizeof(struct ax_mem_list);

	ax_pcie_msg_data_recv(transfer_handle, mem->data, length);
	mem->data_len = length;

	spin_lock_irqsave(&g_axcl_lock, flags);
	list_add_tail(&mem->head, &msg_handle->mem_list);
	spin_unlock_irqrestore(&g_axcl_lock, flags);

	if (transfer_handle->target_id == 0
	    && transfer_handle->port == AXCL_NOTIFY_PORT) {
		atomic_inc(&msg_handle->event);
		wake_up_interruptible(&msg_handle->wait);
	}

	return 0;
}

int axcl_pcie_recv(ax_pcie_msg_handle_t *handle, void *buf, size_t count)
{
	struct list_head *entry, *tmp;
	struct ax_mem_list *mem = NULL;
	unsigned long flags;
	unsigned int len = 0;

	if (!handle) {
		axcl_trace(AXCL_ERR, "handle is not open.");
		return -1;
	}

	spin_lock_irqsave(&g_axcl_lock, flags);

	if (!list_empty(&handle->mem_list)) {
		list_for_each_safe(entry, tmp, &handle->mem_list) {
			mem = list_entry(entry, struct ax_mem_list, head);
			if (mem == NULL)
				goto recv_err2;
			len = mem->data_len;
			if (len > count) {
				axcl_trace(AXCL_ERR,
					   "Message len[0x%x], read len[0x%lx]",
					   len, count);
				list_del(&mem->head);
				goto recv_err1;
			}

			list_del(&mem->head);
			break;
		}

		spin_unlock_irqrestore(&g_axcl_lock, flags);
		memcpy(buf, mem->data, len);
		kfree(mem);
		axcl_trace(AXCL_DEBUG, "read success 0x%x", len);

		return len;
	}

recv_err1:
	if (mem)
		kfree(mem);
recv_err2:
	spin_unlock_irqrestore(&g_axcl_lock, flags);
	return len;
}

int axcl_pcie_msg_send(struct ax_transfer_handle *handle, void *kbuf,
		       unsigned int count)
{
	return ax_pcie_msg_send(handle, kbuf, count);
}

static void axcl_setopt_recv_pci(ax_pcie_msg_handle_t *handle)
{
	struct ax_transfer_handle *transfer_handle =
	    (struct ax_transfer_handle *)handle->pci_handle;
	transfer_handle->msg_notifier_recvfrom = &axcl_pcie_notifier_recv;
	transfer_handle->data = (unsigned long)handle;
}

ax_pcie_msg_handle_t *axcl_pcie_port_open(unsigned int target,
					  unsigned int port)
{
	ax_pcie_msg_handle_t *handle = NULL;
	unsigned long data;

	if (in_interrupt()) {
		handle = kmalloc(sizeof(ax_pcie_msg_handle_t), GFP_ATOMIC);
	} else {
		handle = kmalloc(sizeof(ax_pcie_msg_handle_t), GFP_KERNEL);
	}

	if (NULL == handle) {
		axcl_trace(AXCL_ERR, "Can not open target[0x%x:0x%x],"
			   " kmalloc for handler failed!", target, port);
		return NULL;
	}

	data = (unsigned long)ax_pcie_open(target, port);
	if (data) {
		handle->pci_handle = data;
		return handle;
	}

	kfree(handle);

	return NULL;
}

int axcl_pcie_slave_port_open(unsigned int target, unsigned int port)
{
	ax_pcie_msg_handle_t *msg_handle;

	msg_handle = axcl_pcie_port_open(target, port);
	if (msg_handle) {
		INIT_LIST_HEAD(&msg_handle->mem_list);
		init_waitqueue_head(&msg_handle->wait);
		atomic_set(&msg_handle->event, 0);
		axcl_setopt_recv_pci(msg_handle);
		port_handle[target][port] = msg_handle;
		axcl_trace(AXCL_DEBUG,
			   "open success target/port/handle[0x%x][0x%x][0x%lx]",
			   target, port, (unsigned long)msg_handle);
	} else {
		axcl_trace(AXCL_ERR, "open fail target/port[0x%x][0x%x]",
			   target, port);
		return -1;
	}
	return 0;
}

static int axcl_pcie_open(struct inode *inode, struct file *file)
{
	unsigned int target = 0;
	unsigned int port = AXCL_NOTIFY_PORT;

	file->private_data = port_handle[target][port];
	return 0;
}

static int axcl_pcie_release(struct inode *inode, struct file *file)
{
	return 0;
}

int axcl_pcie_port_manage(struct axcl_device_info *devinfo)
{
	ax_pcie_msg_handle_t *msg_handle;
	struct ax_transfer_handle *handle;
	unsigned int target = devinfo->device;
	unsigned int port = AXCL_NOTIFY_PORT;
	int ret;

	msg_handle = port_handle[target][port];
	if (!msg_handle) {
		axcl_trace(AXCL_ERR, "Manage port handle is NULL.");
		return -1;
	}

	handle = (struct ax_transfer_handle *)msg_handle->pci_handle;
	if (devinfo->cmd == AXCL_PORT_CREATE_COMPLETION) {
		axcl_trace(AXCL_INFO, "slave port create completion ack.");
		ret =
		    axcl_pcie_msg_send(handle, (void *)devinfo,
				       sizeof(struct axcl_device_info));
		if (ret < 0) {
			axcl_trace(AXCL_ERR,
				   "Send create port completion msg info failed: %d.",
				   ret);
			return -1;
		}
		return 0;
	}

	ret =
	    axcl_pcie_recv(msg_handle, (void *)devinfo,
			   sizeof(struct axcl_device_info));
	if (ret <= 0) {
		axcl_trace(AXCL_ERR, "axcl request port failed");
		return -1;
	}

	return ret;
}

void axcl_pcie_heart_beat(struct device_heart_packet *heartbeat)
{
	unsigned long flags = 0;
	heartbeat->device = g_axera_pcie_opt->local_slotid;
	ax_pcie_dbi_spin_lock(&flags);
	/* device_heart_packet.last_timestamp cannot be changed */
	memcpy(g_axera_dev_map[0]->shm_base_virt, heartbeat,
	       offsetof(struct device_heart_packet, last_timestamp));
	ax_pcie_dbi_spin_unlock(&flags);
}

static long axcl_pcie_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	struct axcl_device_info devinfo;
	struct device_heart_packet heartbeat;
	int ret = 0;

	switch (cmd) {
	case IOC_AXCL_PORT_MANAGE:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_PORT_MANAGE.");
		if (copy_from_user
		    ((void *)&devinfo, (void *)arg,
		     sizeof(struct axcl_device_info))) {
			printk("Get parameter from usr space failed!");
			return -1;
		}
		ret = axcl_pcie_port_manage(&devinfo);
		if (ret < 0) {
			axcl_trace(AXCL_ERR, "axcl pcie req port failed.");
		} else {
			if (copy_to_user
			    ((void *)arg, (void *)&devinfo,
			     sizeof(struct axcl_device_info))) {
				printk("copy to usr space failed!");
				ret = -1;
				goto out;
			}
		}
		break;
	case IOC_AXCL_HEART_BEATS:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_HEART_BEATS.");
		if (copy_from_user
		    ((void *)&heartbeat, (void *)arg,
		     sizeof(struct device_heart_packet))) {
			printk("Get parameter from usr space failed!");
			return -1;
		}
		axcl_pcie_heart_beat(&heartbeat);
		break;
	default:
		printk("warning not defined cmd.\n");
		break;
	}

out:
	return ret;
}

static ssize_t axcl_pcie_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *f_pos)
{
	return 0;
}

static ssize_t axcl_pcie_read(struct file *file, char __user *buf,
			      size_t count, loff_t *f_pos)
{
	return 0;
}

static unsigned int axcl_pcie_poll(struct file *file,
				   struct poll_table_struct *table)
{
	ax_pcie_msg_handle_t *handle =
	    (ax_pcie_msg_handle_t *) file->private_data;

	poll_wait(file, &handle->wait, table);

	if (atomic_read(&handle->event) > 0) {
		atomic_dec(&handle->event);
		return POLLIN | POLLRDNORM;
	}
	return 0;
}

static struct file_operations axcl_pcie_fops = {
	.owner = THIS_MODULE,
	.open = axcl_pcie_open,
	.release = axcl_pcie_release,
	.unlocked_ioctl = axcl_pcie_ioctl,
	.write = axcl_pcie_write,
	.read = axcl_pcie_read,
	.poll = axcl_pcie_poll,
};

static struct miscdevice axcl_usrdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &axcl_pcie_fops,
	.name = "axcl_device"
};

int axcl_timestamp_sync(struct axera_dev *ax_dev, int timeout)
{
	struct timespec64 tv_start;
	struct timespec64 tv_end;
	unsigned int runtime;
	struct timespec64 new_ts;

	ktime_get_ts64(&tv_start);
	while (1) {
		if (*(volatile unsigned int *)ax_dev->shm_base_virt ==
		    0x22222222) {
			memcpy(&new_ts, ax_dev->shm_base_virt + 0x10,
			       sizeof(struct timespec64));
			break;
		}
		if (timeout == -1) {
			continue;
		} else {
			ktime_get_ts64(&tv_end);
			runtime =
			    ((tv_end.tv_sec - tv_start.tv_sec) * 1000000) +
			    ((tv_end.tv_nsec - tv_start.tv_nsec) / 1000);
			if (runtime > (timeout * 1000)) {
				return -1;
				break;
			}
		}
	}

	/* settimeofday */
	return do_settimeofday64(&new_ts);
}

static int __init axcl_pcie_slave_init(void)
{
	int ret;
	axcl_trace(AXCL_DEBUG, "axcl pcie slave init.");

	spin_lock_init(&g_axcl_lock);

	/* 2. create notify port11 */
	ret = axcl_pcie_slave_port_open(0, AXCL_NOTIFY_PORT);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "axcl pcie port open failed.");
		return ret;
	}
	/* create heartbeat port10 */
	ret = axcl_pcie_slave_port_open(0, AXCL_HEARTBEAT_PORT);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "axcl pcie notify port open failed.");
		return -1;
	}

	/* 3. rc and ep handshake */
	ret = ax_pcie_msg_check_remote(0);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "axcl pcie check remote device fail: %x.",
			   ret);
		return -1;
	}

	/* 4. timestamp sync */
	ret = axcl_timestamp_sync(g_axera_dev_map[0], 5000);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "axcl pcie timestamp sync failed.");
	}

	/* Clear reserved share memory */
	memset32(g_axera_dev_map[0]->shm_base_virt, 0, 0x10);

	misc_register(&axcl_usrdev);

	return 0;
}

static void __exit axcl_pcie_slave_exit(void)
{
	misc_deregister(&axcl_usrdev);
}

module_init(axcl_pcie_slave_init);
module_exit(axcl_pcie_slave_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
