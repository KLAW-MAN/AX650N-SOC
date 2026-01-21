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
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include "../include/ax_pcie_msg_ursdev.h"
#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_msg_transfer.h"
#include "../include/ax_pcie_proc.h"

struct ax_mem_list {
	struct list_head head;
	void *data;
	unsigned int data_len;
};

static struct semaphore ioctl_sem;

static int ax_pcie_msg_notifier_recv(void *handle,
				     void *buf, unsigned int length)
{
	struct ax_transfer_handle *transfer_handle =
	    (struct ax_transfer_handle *)handle;
	ax_pcie_msg_handle_t *msg_handle;

	struct ax_mem_list *mem;
	void *data;
	unsigned long flags = 0;

	msg_trace(MSG_DEBUG, "nortifier_recv addr 0x%lx len 0x%x.",
		  (unsigned long int)buf, length);

	msg_handle = (ax_pcie_msg_handle_t *) transfer_handle->data;
	data = kmalloc(length + sizeof(struct ax_mem_list), GFP_ATOMIC);
	if (!data) {
		msg_trace(MSG_ERR, "Data kmalloc failed.");
		return -1;
	}
#ifdef MSG_PROC_ENABLE
	if (pcie_proc.proc_level) {
		((struct kmalloc_msg_info *)pcie_proc.kmalloc_msg_info.data)->
			kmalloc_size += ((length + sizeof(struct ax_mem_list)) / 1024); //KB
	}
#endif

	mem = (struct ax_mem_list *)data;
	mem->data = data + sizeof(struct ax_mem_list);

	ax_pcie_msg_data_recv(transfer_handle, mem->data, length);
	mem->data_len = length;

	spin_lock_irqsave(&msg_handle->msg_lock, flags);
	list_add_tail(&mem->head, &msg_handle->mem_list);
	spin_unlock_irqrestore(&msg_handle->msg_lock, flags);

	wake_up_interruptible(&msg_handle->wait);

	return 0;
}

static void ax_pcie_msg_attr_init(struct ax_pcie_msg_attr *attr)
{
	int i;

	attr->target_id = -1;
	attr->port = MAX_MSG_PORTS + 1;

	for (i = 0; i < MAX_DEV_NUMBER; i++) {
		attr->remote_id[i] = -1;
	}
}

static void usrdev_setopt_null(ax_pcie_msg_handle_t *handle)
{
	struct ax_transfer_handle *transfer_handle =
	    (struct ax_transfer_handle *)handle->pci_handle;
	transfer_handle->msg_notifier_recvfrom = NULL;
	transfer_handle->data = 0;

}

static int ax_msg_userdev_open(struct inode *inode, struct file *file)
{
	file->private_data = 0;

	return 0;
}

static void _del_mem_list(ax_pcie_msg_handle_t *handle)
{
	struct list_head *entry, *tmp;
	struct ax_mem_list *mem;

	/* mem list empty means no data is comming */
	if (!list_empty(&handle->mem_list)) {
		list_for_each_safe(entry, tmp, &handle->mem_list) {
			mem = list_entry(entry, struct ax_mem_list, head);
			list_del(&mem->head);
			kfree(mem);
		}
	}
}

static int ax_msg_userdev_release(struct inode *inode, struct file *file)
{
	unsigned long flags = 0;
	ax_pcie_msg_handle_t *msg_handle =
	    (ax_pcie_msg_handle_t *) file->private_data;
	struct ax_transfer_handle *transfer_handle = NULL;
	if (!msg_handle) {
		msg_trace(MSG_ERR, "handle is not open.");
		return -1;
	}

	usrdev_setopt_null(msg_handle);

	spin_lock_irqsave(&msg_handle->msg_lock, flags);
	/* if mem list empty means no data comming */
	if (!list_empty(&msg_handle->mem_list)) {
		_del_mem_list(msg_handle);
	}
	spin_unlock_irqrestore(&msg_handle->msg_lock, flags);

	transfer_handle = (struct ax_transfer_handle *)msg_handle->pci_handle;
	ax_pcie_msg_close(transfer_handle);
	kfree(msg_handle);
	file->private_data = 0;

	msg_trace(MSG_INFO, "release success 0x%x", 0);

	return 0;
}

static ax_pcie_msg_handle_t *ax_pcie_msg_open(struct ax_pcie_msg_attr *attr)
{
	ax_pcie_msg_handle_t *handle = NULL;
	unsigned long data;

	if (attr == NULL) {
		msg_trace(MSG_ERR,
			  "Can not open a invalid handler,attr is NULL!");
		return NULL;
	}

	if (in_interrupt()) {
		handle = kmalloc(sizeof(ax_pcie_msg_handle_t), GFP_ATOMIC);
	} else {
		handle = kmalloc(sizeof(ax_pcie_msg_handle_t), GFP_KERNEL);
	}

	if (NULL == handle) {
		msg_trace(MSG_ERR, "Can not open target[0x%x:0x%x],"
			  " kmalloc for handler failed!",
			  attr->target_id, attr->port);
		return NULL;
	}

	data = (unsigned long)ax_pcie_open(attr->target_id, attr->port);
	if (data) {
		handle->pci_handle = data;
		return handle;
	}

	kfree(handle);

	return NULL;

}

static void usrdev_setopt_recv_pci(ax_pcie_msg_handle_t *handle)
{
	struct ax_transfer_handle *transfer_handle =
	    (struct ax_transfer_handle *)handle->pci_handle;
	transfer_handle->msg_notifier_recvfrom = &ax_pcie_msg_notifier_recv;
	transfer_handle->data = (unsigned long)handle;
}

static ssize_t ax_pcie_msg_read(struct file *file, char __user *buf,
				size_t count, loff_t *f_pos)
{
	ax_pcie_msg_handle_t *msg_handle =
	    (ax_pcie_msg_handle_t *) file->private_data;
	struct list_head *entry, *tmp;
	struct ax_mem_list *mem = NULL;
	unsigned int len = 0;
	unsigned long readed = 0;
	unsigned long flags = 0;

	if (!msg_handle) {
		msg_trace(MSG_ERR, "handle is not open.");
		return -1;
	}

	spin_lock_irqsave(&msg_handle->msg_lock, flags);

	/* if mem list empty means no data comming */
	if (!list_empty(&msg_handle->mem_list)) {
		list_for_each_safe(entry, tmp, &msg_handle->mem_list) {
			mem = list_entry(entry, struct ax_mem_list, head);
			if (mem == NULL)
				goto msg_read_err2;
			len = mem->data_len;
			if (len > count) {
				msg_trace(MSG_ERR,
					  "Message len[0x%x], read len[0x%lx]", len, count);
				list_del(&mem->head);
				goto msg_read_err1;
			}

			list_del(&mem->head);
			break;
		}

		spin_unlock_irqrestore(&msg_handle->msg_lock, flags);

		readed = copy_to_user(buf, mem->data, len);
		if (readed) {
			printk("copy to usr error!\n");
			kfree(mem);
			return -1;
		}

		kfree(mem);
#ifdef MSG_PROC_ENABLE
	if (pcie_proc.proc_level) {
		((struct kmalloc_msg_info *)pcie_proc.kmalloc_msg_info.data)->
			free_size += ((len + sizeof(struct ax_mem_list)) / 1024); //KB
	}
#endif
		msg_trace(MSG_DEBUG, "read success 0x%x", len);

		return len;

	}

msg_read_err1:
	if (mem)
		kfree(mem);
msg_read_err2:
	spin_unlock_irqrestore(&msg_handle->msg_lock, flags);

	return -1;
}

static ssize_t ax_pcie_msg_write(struct file *file, const char __user *buf,
				 size_t count, loff_t *f_pos)
{
	int ret = 0;
	unsigned long writed = 0;
	char *kbuf = NULL;
	struct ax_transfer_handle *handle;

	char msg_array[NORMAL_MESSAGE_SIZE] = { 0 };

	ax_pcie_msg_handle_t *msg_handle =
	    (ax_pcie_msg_handle_t *) file->private_data;

	if (!msg_handle || !buf) {
		msg_trace(MSG_ERR, "handle or buffer is null!");
		return -1;
	}

	if (NORMAL_MESSAGE_SIZE < count) {
		kbuf = vmalloc(count);
		if (!kbuf) {
			msg_trace(MSG_ERR, "vmalloc failed.");
			return -1;
		}

	} else if (count != 0) {

		kbuf = msg_array;

	} else {
		printk(KERN_ERR "ERR, pcie message length is 0!!!");
		return -1;
	}

	writed = copy_from_user(kbuf, buf, count);
	if (writed) {
		printk("copy from user error.");
		goto write_err;
	}

	handle = (struct ax_transfer_handle *)msg_handle->pci_handle;
	ret = ax_pcie_msg_send(handle, kbuf, count);
	if (ret < 0) {
		msg_trace(MSG_ERR, "send msg error!");
	}

write_err:
	if (NORMAL_MESSAGE_SIZE < count) {
		vfree(kbuf);
	}

	msg_trace(MSG_DEBUG, "send success %d.", ret);
	return ret;
}

static long ax_pcie_msg_ioctl(struct file *file,
			      unsigned int cmd, unsigned long arg)
{
	ax_pcie_msg_handle_t *msg_handle;
	struct ax_pcie_msg_attr attr;
	int check;
	int local_id;
	int ret = 0;

	if (down_interruptible(&ioctl_sem)) {
		msg_trace(MSG_ERR, "acquire handle sem failed!");
		return -1;
	}

	if (copy_from_user
	    ((void *)&attr, (void *)arg, sizeof(struct ax_pcie_msg_attr))) {
		printk("Get parameter from usr space failed!");
		ret = -1;
		goto ioctl_end;
	}

	if (_IOC_TYPE(cmd) == 'M') {
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(AX_MSG_IOC_ATTR_INIT):
			msg_trace(MSG_DEBUG, "AX_MSG_IOC_ATTR_INIT.");
			ax_pcie_msg_attr_init(&attr);
			if (copy_to_user
			    ((void *)arg, (void *)&attr,
			     sizeof(struct ax_pcie_msg_attr))) {
				printk("Copy param to usr space failed.\n");
				ret = -1;
				goto ioctl_end;
			}
			break;

		case _IOC_NR(AX_MSG_IOC_CONNECT):
			msg_trace(MSG_DEBUG, "AX_MSG_IOC_CONNECT.");
			msg_handle = ax_pcie_msg_open(&attr);
			if (msg_handle) {
				INIT_LIST_HEAD(&msg_handle->mem_list);
				spin_lock_init(&msg_handle->msg_lock);
				init_waitqueue_head(&msg_handle->wait);
				msg_handle->stop_flag = 0;
				file->private_data = (void *)msg_handle;
				usrdev_setopt_recv_pci(msg_handle);
				msg_trace(MSG_DEBUG, "open success 0x%lx",
					  (unsigned long)msg_handle);
			} else {
				file->private_data = NULL;
				ret = -1;
				goto ioctl_end;
			}
			break;
		case _IOC_NR(AX_MSG_IOC_GET_LOCAL_ID):
			msg_trace(MSG_DEBUG, "AX_MSG_IOC_GET_LOCAL_ID.");
			local_id = ax_pcie_msg_getlocalid();
			ret = local_id;
			goto ioctl_end;
		case _IOC_NR(AX_MSG_IOC_GET_REMOTE_ID):
			msg_trace(MSG_DEBUG, "AX_MSG_IOC_GET_REMOTE_ID.");
			ax_pcie_msg_getremoteids(attr.remote_id);
			if (copy_to_user
			    ((void *)arg, (void *)&attr,
			     sizeof(struct ax_pcie_msg_attr))) {
				printk("Copy para to usr space failed.\n");
				ret = -1;
				goto ioctl_end;
			}
			break;
		case _IOC_NR(AX_MSG_IOC_RESET_DEVICE):
			msg_trace(MSG_DEBUG, "reset device.");
			host_reset_device(attr.target_id);
			break;
		case _IOC_NR(AX_MSG_IOC_CHECK):
			msg_trace(MSG_DEBUG, "AX_MSG_IOC_CHECK.");
			check = ax_pcie_msg_check_remote(attr.target_id);
			ret = check;
			goto ioctl_end;
		case _IOC_NR(AX_MSG_IOC_PCIE_STOP):
			msg_trace(MSG_DEBUG, "AX_MSG_IOC_PCIE_STOP.");
			msg_handle = (ax_pcie_msg_handle_t *)file->private_data;
			msg_handle->stop_flag = 1;
			wake_up_interruptible(&msg_handle->wait); //Wake up the fd still in the poll
			goto ioctl_end;
		default:
			msg_trace(MSG_INFO, "warning not defined cmd.");
			break;
		}
	}

ioctl_end:
	up(&ioctl_sem);

	return ret;
}

static unsigned int ax_pcie_msg_poll(struct file *file,
				     struct poll_table_struct *table)
{
	unsigned long flags = 0;
	ax_pcie_msg_handle_t *handle =
	    (ax_pcie_msg_handle_t *) file->private_data;
	if (!handle) {
		msg_trace(MSG_ERR, "handle is not open");
		return -1;
	}

	poll_wait(file, &handle->wait, table);
	if (handle->stop_flag == 1) {
		return POLLHUP | POLLREMOVE;
	}

	spin_lock_irqsave(&handle->msg_lock, flags);
	/* if mem list empty means no data comming */
	if (!list_empty(&handle->mem_list)) {
		spin_unlock_irqrestore(&handle->msg_lock, flags);
		msg_trace(MSG_DEBUG,
			  "poll not empty handle 0x%lx", (unsigned long)handle);
		return POLLIN | POLLRDNORM;
	}
	spin_unlock_irqrestore(&handle->msg_lock, flags);

	return 0;
}

static struct file_operations ax_pcie_msg_fops = {
	.owner = THIS_MODULE,
	.open = ax_msg_userdev_open,
	.release = ax_msg_userdev_release,
	.unlocked_ioctl = ax_pcie_msg_ioctl,
	.write = ax_pcie_msg_write,
	.read = ax_pcie_msg_read,
	.poll = ax_pcie_msg_poll,
};

static struct miscdevice ax_pcie_msg_usrdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &ax_pcie_msg_fops,
	.name = "msg_userdev"
};

static int __init ax_pcie_msg_init(void)
{
	printk("ax pcie msg init\n");

	sema_init(&ioctl_sem, 1);

	misc_register(&ax_pcie_msg_usrdev);

#ifdef MSG_PROC_ENABLE
	pcie_proc.get_rp_wp = update_proc_rp_wp;
#endif

	return 0;
}

static void __exit ax_pcie_msg_exit(void)
{
	ax_pcie_msg_kfifo_free();
#ifdef MSG_PROC_ENABLE
	pcie_proc.get_rp_wp = NULL;
#endif
	misc_deregister(&ax_pcie_msg_usrdev);
}

module_init(ax_pcie_msg_init);
module_exit(ax_pcie_msg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
