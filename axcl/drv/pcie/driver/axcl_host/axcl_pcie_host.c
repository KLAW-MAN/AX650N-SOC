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
#include <linux/time.h>
#include <linux/time64.h>
#include <uapi/linux/time.h>
#include <linux/sched.h>
#include <linux/firmware.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include "axcl_pcie_host.h"

#define AXCL_DUMP_ADDR_SRC	(0x100000000)

static DEFINE_MUTEX(ioctl_mutex);
static struct task_struct *heartbeat[32];
ax_pcie_msg_handle_t *port_handle[AXERA_MAX_MAP_DEV][MAX_MSG_PORTS];

static dma_addr_t phys_addr;
static void *dma_virt_addr;

struct device_connect_status heartbeat_status;
unsigned int port_info[AXERA_MAX_MAP_DEV][MAX_MSG_PORTS] = { 0 };

static wait_queue_head_t heart_waitqueue[AXERA_MAX_MAP_DEV];
static bool htcondition[AXERA_MAX_MAP_DEV] = { false };

static struct axcl_handle_t *axcl_handle;

static inline u32 pcie_dev_readl(struct axera_dev *ax_dev, u32 offset)
{
	return readl(ax_dev->bar_virt[0] + offset);
}

static inline void pcie_dev_writel(struct axera_dev *ax_dev,
				   u32 offset, u32 value)
{
	writel(value, ax_dev->bar_virt[0] + offset);
}

void axcl_devices_heartbeat_status_set(unsigned int target,
				       enum heartbeat_type status)
{
	heartbeat_status.status[target] = status;
}

enum heartbeat_type axcl_devices_heartbeat_status_get(unsigned int target)
{
	return heartbeat_status.status[target];
}

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
	data = kmalloc(length + sizeof(struct ax_mem_list), GFP_ATOMIC);
	if (!data) {
		axcl_trace(AXCL_ERR, "Data kmalloc failed.");
		return -1;
	}

	mem = (struct ax_mem_list *)data;
	mem->data = data + sizeof(struct ax_mem_list);

	ax_pcie_msg_data_recv(transfer_handle, mem->data, length);
	mem->data_len = length;

	spin_lock_irqsave(&msg_handle->msg_lock, flags);
	list_add_tail(&mem->head, &msg_handle->mem_list);
	spin_unlock_irqrestore(&msg_handle->msg_lock, flags);

	return 0;
}

int axcl_pcie_msg_send(struct ax_transfer_handle *handle, void *kbuf,
		       unsigned int count)
{
	return ax_pcie_msg_send(handle, kbuf, count);
}

static int axcl_pcie_recv(ax_pcie_msg_handle_t *handle, void *buf, size_t count)
{
	struct list_head *entry, *tmp;
	struct ax_mem_list *mem = NULL;
	unsigned long flags;
	unsigned int len = 0;

	if (!handle) {
		axcl_trace(AXCL_ERR, "handle is not open.");
		return -1;
	}

	spin_lock_irqsave(&handle->msg_lock, flags);

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

		spin_unlock_irqrestore(&handle->msg_lock, flags);

		memcpy(buf, mem->data, len);

		kfree(mem);

		axcl_trace(AXCL_DEBUG, "read success 0x%x", len);

		return len;
	}

recv_err1:
	if (mem)
		kfree(mem);
recv_err2:
	spin_unlock_irqrestore(&handle->msg_lock, flags);
	return len;
}

int axcl_pcie_recv_timeout(ax_pcie_msg_handle_t *handle, void *buf,
			   size_t count, int timeout)
{
	struct timespec64 tv_start;
	struct timespec64 tv_end;
	unsigned int runtime;
	int ret;

	ktime_get_ts64(&tv_start);
	while (1) {
		ret = axcl_pcie_recv(handle, buf, count);
		if (ret)
			break;
		if (timeout == -1) {
			continue;
		} else if (timeout == 0) {
			break;
		} else {
			ktime_get_ts64(&tv_end);
			runtime =
			    ((tv_end.tv_sec - tv_start.tv_sec) * 1000000) +
			    ((tv_end.tv_nsec - tv_start.tv_nsec) / 1000);
			if (runtime > (timeout * 1000)) {
				ret = -2;
				break;
			}
		}
		msleep(100);
	}
	return ret;
}

static int axcl_heartbeat_recv_timeout(struct device_heart_packet *hbeat, int timeout)
{
	struct timespec64 tv_start;
	struct timespec64 tv_end;
	unsigned int runtime;
	int ret = 0;

	ktime_get_ts64(&tv_start);
	while (1) {
		if (hbeat->timestamp != hbeat->last_timestamp) {
			hbeat->last_timestamp = hbeat->timestamp;
			ret = 0;
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
				axcl_trace(AXCL_ERR, "master: %d, slave: %d, runtime: %d, timeout: %d", hbeat->last_timestamp, hbeat->timestamp, runtime, timeout);
				ret = -2;
				break;
			}
		}
		msleep(1000);
	}
	return ret;
}

static int send_buf_alloc(struct axera_dev *ax_dev)
{
	struct device *dev = &ax_dev->pdev->dev;

	dma_virt_addr = dma_alloc_coherent(dev, MAX_TRANSFER_SIZE,
					   &phys_addr, GFP_KERNEL);
	if (!dma_virt_addr) {
		dev_err(dev, "Failed to allocate address\n");
		return -1;
	}
	return 0;
}

static void free_buf_alloc(struct axera_dev *ax_dev)
{
	struct device *dev = &ax_dev->pdev->dev;

	dma_free_coherent(dev, MAX_TRANSFER_SIZE, dma_virt_addr, phys_addr);
}

static int wait_device_recv_completion(struct axera_dev *ax_dev, unsigned int flags,
				unsigned int timeout)
{
	int ret = -1;
	unsigned int reg = 0;
	unsigned int runtime;
	struct timespec64 tv_start = { 0 };
	struct timespec64 tv_end = { 0 };

	ktime_get_ts64(&tv_start);
	while (1) {
		reg = pcie_dev_readl(ax_dev, AX_PCIE_ENDPOINT_STATUS);
		if (reg != 0) {
			if (reg & flags) {
				pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_STATUS,
						0x0);
				smp_mb();
				ret = 0;
				break;
			} else {
				pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_STATUS,
						0x0);
				smp_mb();
				ret = -EFAULT;
				break;
			}
		}
		if (timeout == -1) {
			continue;
		} else {
			ktime_get_ts64(&tv_end);
			runtime =
			    ((tv_end.tv_sec - tv_start.tv_sec) * 1000000) +
			    ((tv_end.tv_nsec - tv_start.tv_nsec) / 1000);
			if (runtime > (timeout * 1000)) {
				return -2;
			}
		}
	}
	return ret;
}

static int axcl_get_dev_boot_reason(struct axera_dev *ax_dev)
{
	int boot_reason = 0;

	boot_reason = pcie_dev_readl(ax_dev, AX_PCIE_ENDPOINT_BOOT_REASON_TYPE);
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_BOOT_REASON_TYPE, 0x0);
	return boot_reason;
}

static int axcl_dump_data(struct axera_dev *ax_dev, unsigned long src, int size)
{
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_SRC_ADDR,
			lower_32_bits(src));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_SRC_ADDR,
			upper_32_bits(src));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_DST_ADDR,
			lower_32_bits(phys_addr));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_DST_ADDR,
			upper_32_bits(phys_addr));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_SIZE, size);
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_COMMAND,
			AX_PCIE_COMMAND_WRITE);

	return wait_device_recv_completion(ax_dev, STATUS_WRITE_SUCCESS, 6000);
}

static void sysdumpheader(int boot_reason)
{
	printk("\n===================sysdump Header===================\n");
	printk("nBrs: %d\n", boot_reason);
	printk("nSrc: 0x%lx\n", AXCL_DUMP_ADDR_SRC);
	printk("nSize: %d\n", sysdump_ctrl.size);
	printk("Path: %s\n", sysdump_ctrl.path);
}

static int axcl_device_sysdump(struct axera_dev *ax_dev)
{
	struct file *filp;
	loff_t pos = 0;
	struct timespec64 ts;
	struct tm tm;
	time64_t local_time;
	unsigned long minutes_west;
	char filename[AXCL_NAME_LEN];
	int boot_reason;
	int dumpsize = sysdump_ctrl.size;
	int size;
	int ret;
	unsigned long src = AXCL_DUMP_ADDR_SRC;
	ssize_t bytes_write;

	boot_reason = axcl_get_dev_boot_reason(ax_dev);
	sysdumpheader(boot_reason);
	if (!boot_reason) {
		printk("[INFO]: DEV NORMAL, SKIP DUMP\n");
		return 0;
	}

	printk("[INFO]: DEV %x EXCEPTION, DUMP...\n", ax_dev->slot_index);

	ktime_get_real_ts64(&ts);
	minutes_west = sys_tz.tz_minuteswest;
	local_time = ts.tv_sec + (minutes_west * 60 * -1);
	time64_to_tm(local_time, 0, &tm);
	snprintf(filename, AXCL_NAME_LEN, "%s/%s_%x.%04ld%02d%02d%02d%02d%02d",
		 sysdump_ctrl.path, AXCL_SYSDUMP_NAME, ax_dev->slot_index,
		 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
		 tm.tm_min, tm.tm_sec);

	filp = filp_open(filename, O_RDWR | O_CREAT, 0777);
	if (IS_ERR(filp)) {
		axcl_trace(AXCL_ERR, "Failed to open file %s", filename);
		return PTR_ERR(filp);
	}

	while (dumpsize) {
		size = min_t(uint, dumpsize, MAX_TRANSFER_SIZE);
		ret = axcl_dump_data(ax_dev, src, size);
		if (ret < 0) {
			printk("[STATUS]: DUMP FAILED[%d]\n", ret);
			bytes_write = ret;
			goto out;
		}

		bytes_write = kernel_write(filp, dma_virt_addr, size, &pos);
		if (bytes_write < 0) {
			printk("[STATUS]: WRITE FAILED[%ld]\n", bytes_write);
			break;
		}

		src += size;
		dumpsize -= size;
	}

out:
	filp_close(filp, NULL);
	return bytes_write;
}

static void start_devices(struct axera_dev *ax_dev)
{
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_FINISH_STATUS,
			BOOT_START_DEVICES);
}

static int axcl_boot_device(struct axera_dev *ax_dev, unsigned long dest,
			    int size)
{
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_SRC_ADDR,
			lower_32_bits(phys_addr));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_SRC_ADDR,
			upper_32_bits(phys_addr));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_LOWER_DST_ADDR,
			lower_32_bits(dest));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_UPPER_DST_ADDR,
			upper_32_bits(dest));
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_SIZE, size);
	pcie_dev_writel(ax_dev, AX_PCIE_ENDPOINT_COMMAND, AX_PCIE_COMMAND_READ);

	return wait_device_recv_completion(ax_dev, STATUS_READ_SUCCESS, 6000);
}

static void dumppacheader(PAC_HEAD_T *pacHeader)
{
	printk("\n=====================PAC Header=====================\n");
	printk("nMagic: 0x%X\n", pacHeader->nMagic);
	printk("nPacVer: %d\n", pacHeader->nPacVer);
	printk("u64PacSize: %lld\n", pacHeader->u64PacSize);
	printk("szProdName: %s\n", pacHeader->szProdName);
	printk("szProdVer: %s\n", pacHeader->szProdVer);
	printk("nFileOffset: %d\n", pacHeader->nFileOffset);
	printk("nFileCount: %d\n", pacHeader->nFileCount);
	printk("nAuth: %d\n", pacHeader->nAuth);
	printk("crc16: 0x%X\n", pacHeader->crc16);
	printk("md5: 0x%X\n", pacHeader->md5);
}

static void dumpImgFileHeader(PAC_FILE_T *imgHeader)
{
	printk("\n=====================IMG Header=====================\n");
	printk("szID: %s\n", imgHeader->szID);
	printk("szType: %s\n", imgHeader->szType);
	printk("szFile: %s\n", imgHeader->szFile);
	printk("u64CodeOffset: 0x%llX\n", imgHeader->u64CodeOffset);
	printk("u64CodeSize: %lld\n", imgHeader->u64CodeSize);
	printk("tBlock.u64Base: 0x%llX\n", imgHeader->tBlock.u64Base);
	printk("tBlock.u64Size: %lld\n", imgHeader->tBlock.u64Size);
	printk("tBlock.szPartID: %s\n", imgHeader->tBlock.szPartID);
	printk("nFlag: %d\n", imgHeader->nFlag);
	printk("nSelect: %d\n", imgHeader->nSelect);
}

static int axcl_load_fwfile(struct axera_dev *ax_dev, const struct firmware *firmware)
{
	int size, sent = 0;
	int filecount = 0;
	int count = 0;
	int ret = 0;
	int imgsize = 0;
	void *imgaddr;
	unsigned long dest;
	PAC_HEAD_T *pacHeader;
	PAC_FILE_T *imgHeader;

	ret = send_buf_alloc(ax_dev);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "send buf alloc failed,");
		return -1;
	}

	pacHeader = (PAC_HEAD_T *) firmware->data;
	if (!pacHeader) {
		axcl_trace(AXCL_ERR, "firmware data is NULL.");
		ret = -1;
		goto out;
	}

	printk("\n############### Dev %x Firmware loading ##############\n",
	       ax_dev->slot_index);
	dumppacheader(pacHeader);
	filecount = pacHeader->nFileCount;
	while (count < filecount) {
		if (sysdump_ctrl.debug) {
			if (count == 1) {
				ret = axcl_device_sysdump(ax_dev);
				if (ret > 0) {
					printk("[STATUS]: SUCCESS\n");
				}
			}
		}

		imgHeader =
		    (PAC_FILE_T *) ((firmware->data + pacHeader->nFileOffset) +
				    (sizeof(PAC_FILE_T) * count));

		dumpImgFileHeader(imgHeader);
		imgsize = imgHeader->u64CodeSize;
		dest = imgHeader->tBlock.u64Base;
		imgaddr = (void *)firmware->data + imgHeader->u64CodeOffset;
		sent = 0;

		while (imgsize) {
			size = min_t(uint, imgsize, MAX_TRANSFER_SIZE);
			memcpy(dma_virt_addr, imgaddr + sent, size);
			ret = axcl_boot_device(ax_dev, dest, size);
			if (ret < 0) {
				printk("[STATUS]: FAILED[%d]\n", ret);
				goto out;
			}

			dest += size;
			sent += size;
			imgsize -= size;
		}
		printk("[STATUS]: SUCCESS\n");
		count++;
	}

	if (count == filecount) {
		start_devices(ax_dev);
	}

out:
	free_buf_alloc(ax_dev);
	return ret;
}

static int axcl_firmware_load(struct axera_dev *ax_dev)
{
	const struct firmware *firmware;
	char filename[AXCL_NAME_LEN];
	int ret;

	snprintf(filename, AXCL_NAME_LEN, AXCL_PATH_NAME);
	ret = request_firmware(&firmware, filename, &ax_dev->pdev->dev);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "Patch file not found %s.", filename);
		return ret;
	}

	ret = axcl_load_fwfile(ax_dev, firmware);
	release_firmware(firmware);
	return ret;
}

static void axcl_clean_port_info(int dev, struct device_handle_t *dev_handle)
{
	int i;
	struct list_head *entry, *tmp;
	struct usr_port_info *usr_port = NULL;

	for (i = 0; i < dev_handle->port_num; i++) {
		port_info[dev][dev_handle->ports[i]] = 0;
	}

	if (!list_empty(&dev_handle->usr_port_list)) {
		list_for_each_safe(entry, tmp, &dev_handle->usr_port_list) {
			usr_port = list_entry(entry, struct usr_port_info, head);
			for (i = 0; i < usr_port->port_num; i++) {
				port_info[dev][usr_port->ports[i]] = 0;
			}
			list_del(&usr_port->head);
			kfree(usr_port);
		}
	}
}

static int heartbeat_recv_thread(void *pArg)
{
	unsigned int target = *(unsigned int *)pArg;
	unsigned int timeout = 50000;
	struct device_heart_packet *hbeat;
	struct axera_dev *axdev;
	struct list_head *entry, *tmp;
	struct list_head *deventry, *devtmp;
	struct device_handle_t *dev_handle = NULL;
	struct process_info_t *process_handle = NULL;
	int ret;

	axcl_trace(AXCL_DEBUG, "target 0x%x thread running", target);

	axdev = g_axera_pcie_opt->slot_to_axdev(target);
	if (!axdev) {
		axcl_trace(AXCL_ERR, "Get axdev is failed");
		return -1;
	}

	hbeat = (struct device_heart_packet *)axdev->shm_base_virt;

	do {
		if (kthread_should_stop()) {
			break;
		}

		if (axcl_devices_heartbeat_status_get(target) == AXCL_HEARTBEAT_DEAD) {
			wait_event_timeout(heart_waitqueue[target], htcondition[target], 5000);
			if (axcl_devices_heartbeat_status_get(target) == AXCL_HEARTBEAT_DEAD) {
				axcl_trace(AXCL_ERR, "device %x: dead!", target);
				continue;
			}
		}

		ret = axcl_heartbeat_recv_timeout(hbeat, timeout);
		if (ret < 0) {
			axcl_trace(AXCL_ERR, "device %x: dead!", target);
			mutex_lock(&ioctl_mutex);
			axcl_devices_heartbeat_status_set(target, AXCL_HEARTBEAT_DEAD);
			htcondition[target] = false;

			if (axcl_handle) {
				if (!list_empty(&axcl_handle->process_list)) {
					list_for_each_safe(entry, tmp, &axcl_handle->process_list) {
						process_handle = list_entry(entry, struct process_info_t, head);
						if (process_handle == NULL)
							continue;
						if (!list_empty(&process_handle->dev_list)) {
							list_for_each_safe(deventry, devtmp, &process_handle->dev_list) {
								dev_handle = list_entry(deventry, struct device_handle_t, head);
								if (dev_handle == NULL)
									continue;
								if (dev_handle->device == target) {
									axcl_clean_port_info(target, dev_handle);
									list_del(&dev_handle->head);
									kfree(dev_handle);
									atomic_inc(&process_handle->event);
									wake_up_interruptible(&process_handle->wait);
								}
							}
						} else {
							atomic_inc(&process_handle->event);
							wake_up_interruptible(&process_handle->wait);
						}
					}
				}
			}
			mutex_unlock(&ioctl_mutex);
			continue;
		}

		if ((0 < hbeat->interval)
		    && (hbeat->interval < AXCL_RECV_TIMEOUT)) {
			timeout = hbeat->interval;
			axcl_trace(AXCL_DEBUG, "timeout: %d", timeout);
		}
		axcl_devices_heartbeat_status_set(target, AXCL_HEARTBEAT_ALIVE);
	} while (1);

	return 0;
}

static struct device_handle_t *find_dev_handle(struct process_info_t *process_handle, unsigned int target)
{
	struct list_head *entry, *tmp;
	struct device_handle_t *dev_handle = NULL;


	if (!list_empty(&process_handle->dev_list)) {
		list_for_each_safe(entry, tmp, &process_handle->dev_list) {
			dev_handle = list_entry(entry, struct device_handle_t, head);
			if (dev_handle->device == target) {
				return dev_handle;
			}
		}
	}
	return NULL;
}

static int axcl_pcie_port_request_usr_port(struct process_info_t *process_handle, struct axcl_device_info *devinfo)
{
	int i;
	int count = 0;
	unsigned int target = devinfo->device;
	struct device_handle_t *dev_handle = NULL;
	unsigned int pid = devinfo->pid;
	struct usr_port_info *usr_port = NULL;

	if (devinfo->port_num == 0 || devinfo->port_num > AXCL_MAX_PORT) {
		axcl_trace(AXCL_ERR, "request port num %d is invalid", devinfo->port_num);
		return -EINVAL;
	}

	dev_handle = find_dev_handle(process_handle, target);
	if (!dev_handle) {
		axcl_trace(AXCL_ERR, "device %d not found", target);
		return -1;
	}

	for (i = AXCL_BASE_PORT; i < MAX_MSG_PORTS; i++) {
		if (port_info[target][i] == 0)
			break;
	}
	if (MAX_MSG_PORTS - i < devinfo->port_num) {
		axcl_trace(AXCL_ERR, "not enough port, already used %d ports", i);
		return -1;
	}

	usr_port = kmalloc(sizeof(struct usr_port_info), GFP_KERNEL);
	if (!usr_port) {
		axcl_trace(AXCL_ERR, "kmalloc for usr_port_info failed");
		return -ENOMEM;
	}
	memset(usr_port, 0, sizeof(struct usr_port_info));
	INIT_LIST_HEAD(&usr_port->head);

	for (i = AXCL_BASE_PORT; i < MAX_MSG_PORTS; i++) {
		if (port_info[target][i] == i)
			continue;

		if (count == devinfo->port_num)
			break;

		port_info[target][i] = i;
		devinfo->ports[count] = i;
		usr_port->ports[count] = i;
		count++;
		axcl_trace(AXCL_INFO, "ports requested: %d, target: %d, pid: %d", i, target, pid);
	}

	usr_port->port_num = count;
	list_add_tail(&usr_port->head, &dev_handle->usr_port_list);
	return 0;
}

static int axcl_pcie_port_recycle_usr_port(struct process_info_t *process_handle, struct axcl_device_info *devinfo)
{
	int i;
	unsigned int target = devinfo->device;
	struct device_handle_t *dev_handle = NULL;
	struct usr_port_info *usr_port = NULL;
	struct list_head *entry, *tmp;
	int match = 0;

	if (devinfo->port_num == 0 || devinfo->port_num > AXCL_MAX_PORT) {
		axcl_trace(AXCL_ERR, "recycle port num %d is invalid", devinfo->port_num);
		return -EINVAL;
	}

	dev_handle = find_dev_handle(process_handle, target);
	if (!dev_handle) {
		axcl_trace(AXCL_ERR, "device %d not found", target);
		return -1;
	}

	if (list_empty(&dev_handle->usr_port_list)) {
		axcl_trace(AXCL_ERR, "usr port list is empty");
		return -1;
	}

	list_for_each_safe(entry, tmp, &dev_handle->usr_port_list) {
		usr_port = list_entry(entry, struct usr_port_info, head);

		if (usr_port->port_num == devinfo->port_num) {
			match = true;
			for (i = 0; i < devinfo->port_num; i++) {
				if (usr_port->ports[i] != devinfo->ports[i]) {
					match = false;
					break;
				}
			}

			if (match) {
				for (i = 0; i < devinfo->port_num; i++) {
					port_info[target][devinfo->ports[i]] = 0;
				}

				list_del(&usr_port->head);
				kfree(usr_port);
				return 0;
			}
		}
	}

	axcl_trace(AXCL_ERR, "no matching usr_port found for recycling");
	return -1;
}

static int axcl_pcie_port_destroy(struct process_info_t *process_handle, struct axcl_device_info *devinfo)
{
	int ret;
	unsigned int target = devinfo->device;
	unsigned int port = AXCL_NOTIFY_PORT;
	struct ax_transfer_handle *handle;
	struct device_handle_t *dev_handle = NULL;

	dev_handle = find_dev_handle(process_handle, target);
	if (!dev_handle) {
		axcl_trace(AXCL_ERR, "Not such the device need to destory.");
		return -1;
	}

	handle =
	    (struct ax_transfer_handle *)port_handle[target][port]->pci_handle;
	ret = axcl_pcie_msg_send(handle, (void *)devinfo,
			       		sizeof(struct axcl_device_info));
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "Send destory port msg info failed: %d.",
			   	ret);
		return -1;
	}
	axcl_clean_port_info(target, dev_handle);
	list_del(&dev_handle->head);
	kfree(dev_handle);
	return 0;
}

static int axcl_pcie_port_create(struct process_info_t *process_handle, struct axcl_device_info *devinfo)
{
	struct ax_transfer_handle *handle;
	struct axcl_device_info rdevinfo;
	struct device_handle_t *dev_handle = NULL;
	unsigned int target = devinfo->device;
	unsigned int pid = devinfo->pid;
	unsigned int port = AXCL_NOTIFY_PORT;
	int count = 0;
	int ret;
	int i;

	dev_handle = find_dev_handle(process_handle, target);
	if (dev_handle) {
		devinfo->cmd = dev_handle->cmd;
		devinfo->port_num = dev_handle->port_num;
		memcpy(devinfo->ports, dev_handle->ports,
					devinfo->port_num * sizeof(int));
		axcl_trace(AXCL_INFO, "copied %d ports to target %d", devinfo->port_num, target);
		return 0;
	}

	dev_handle = kmalloc(sizeof(struct device_handle_t), GFP_KERNEL);
	if (NULL == dev_handle) {
		axcl_trace(AXCL_ERR,
			   "kmalloc for device handler failed, target = %x",
			   target);
		return -1;
	}
	INIT_LIST_HEAD(&dev_handle->head);
	INIT_LIST_HEAD(&dev_handle->usr_port_list);

	for (i = AXCL_BASE_PORT; i < MAX_MSG_PORTS; i++) {
		if (port_info[target][i] == i)
			continue;

		if (count == AXCL_MAX_PORT)
			break;

		port_info[target][i] = i;
		devinfo->ports[count] = i;
		dev_handle->ports[count] = i;

		axcl_trace(AXCL_INFO,
			   "alloc port:  i: 0x%x, target: %d, pid: %d", i,
			   target, pid);
		count++;
	}

	devinfo->port_num = count;
	dev_handle->device = target;
	dev_handle->port_num = count;

	handle =
	    (struct ax_transfer_handle *)port_handle[target][port]->pci_handle;
	ret =
	    axcl_pcie_msg_send(handle, (void *)devinfo,
			       sizeof(struct axcl_device_info));
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "Send create port msg info failed: %d.",
			   ret);
		kfree(dev_handle);
		return -1;
	}

	ret =
	    axcl_pcie_recv_timeout(port_handle[target][port], (void *)&rdevinfo,
				   sizeof(struct axcl_device_info),
				   AXCL_RECV_TIMEOUT);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "Recv port ack timeout.");
		kfree(dev_handle);
		return -1;
	}

	if (rdevinfo.cmd == AXCL_PORT_CREATE_FAIL) {
		axcl_trace(AXCL_ERR, "slave port create failed.");
		for (i = 0; i < devinfo->port_num; i++) {
			port_info[target][devinfo->ports[i]] = 0;
		}
		devinfo->cmd = rdevinfo.cmd;
		kfree(dev_handle);
		return -1;
	}

	devinfo->cmd = rdevinfo.cmd;
	dev_handle->cmd = rdevinfo.cmd;

	list_add_tail(&dev_handle->head, &process_handle->dev_list);
	return 0;
}

static int axcl_pcie_port_manage(struct file *file, struct axcl_device_info *devinfo)
{
	struct process_info_t *process_handle = file->private_data;
	unsigned int pid = devinfo->pid;
	int ret;

	axcl_trace(AXCL_DEBUG, "pid = %d, cpid = %d, ctgid = %d, cname = %s",
		   pid, current->pid, current->tgid, current->comm);

	if (!pid) {
		axcl_trace(AXCL_ERR, "current process is NULL.");
		return -1;
	}

	if (!process_handle) {
		axcl_trace(AXCL_ERR, "process handler is NULL");
		return -1;
	}

	switch (devinfo->cmd) {
	case AXCL_PORT_CREATE:
		ret = axcl_pcie_port_create(process_handle, devinfo);
		if (ret) {
			axcl_trace(AXCL_ERR, "axcl pcie port create failed.");
		}
		break;
	case AXCL_PORT_DESTROY:
		ret = axcl_pcie_port_destroy(process_handle, devinfo);
		if (ret) {
			axcl_trace(AXCL_ERR, "axcl pcie port destroy failed.");
		}
		break;
	case AXCL_PORT_REQUEST_USR_PORT:
		ret = axcl_pcie_port_request_usr_port(process_handle, devinfo);
		if (ret) {
			axcl_trace(AXCL_ERR, "axcl pcie port request usr port failed.");
		}
		break;
	case AXCL_PORT_RECYCLE_USR_PORT:
		ret = axcl_pcie_port_recycle_usr_port(process_handle, devinfo);
		if (ret) {
			axcl_trace(AXCL_ERR, "axcl pcie port recycle usr port failed.");
		}
		break;
	default:
		axcl_trace(AXCL_ERR, "invalid port manage cmd: %d", devinfo->cmd);
		ret = -1;
		break;
	}

	return ret;
}

static ax_pcie_msg_handle_t *axcl_pcie_port_open(unsigned int target,
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

static void axcl_setopt_recv_pci(ax_pcie_msg_handle_t *handle)
{
	struct ax_transfer_handle *transfer_handle =
	    (struct ax_transfer_handle *)handle->pci_handle;
	transfer_handle->msg_notifier_recvfrom = &axcl_pcie_notifier_recv;
	transfer_handle->data = (unsigned long)handle;
}

static int axcl_pcie_host_port_open(unsigned int target, unsigned int port)
{
	ax_pcie_msg_handle_t *msg_handle;

	msg_handle = axcl_pcie_port_open(target, port);
	if (msg_handle) {
		INIT_LIST_HEAD(&msg_handle->mem_list);
		spin_lock_init(&msg_handle->msg_lock);
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

static int axcl_timestamp_sync(unsigned int target)
{
	struct timespec64 new_ts;
	struct timezone new_tz;
	struct axera_dev *ax_dev;

	ax_dev = g_axera_pcie_opt->slot_to_axdev(target);
	if (!ax_dev) {
		trans_trace(TRANS_ERR, "Device[0x%x] does not exist!",
			    target);
		return -1;
	}

	/* gettimeofday */
	ktime_get_real_ts64(&new_ts);
	memcpy(&new_tz, &sys_tz, sizeof(sys_tz));
	memcpy(ax_dev->shm_base_virt + 0x10, &new_ts,
	       sizeof(struct timespec64));
	*(volatile unsigned int *)ax_dev->shm_base_virt = 0x22222222;

	return 0;
}

static void axcl_get_devices(struct device_list_t *devlist)
{
	int i = 0;

	devlist->type = 0;
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		devlist->devices[i] = g_axera_dev_map[i]->slot_index;
	}
	devlist->num = i;
}

static void axcl_get_bus_info(struct axcl_bus_info_t *businfo)
{
	unsigned int i;
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; ++i) {
		if (g_axera_dev_map[i]->slot_index == businfo->device) {
			businfo->domain = g_axera_dev_map[i]->domain;
			businfo->slot = g_axera_dev_map[i]->dev_slot;
			businfo->func = g_axera_dev_map[i]->dev_func;
			break;
		}
	}
}

static int axcl_get_pid_info(struct axcl_pid_info_t *pidinfo)
{
	struct process_info_t *process_handle = NULL;
	struct device_handle_t *dev_handle = NULL;
	struct list_head *entry, *tmp;
	struct list_head *deventry, *devtmp;
	unsigned int target = pidinfo->device;
	unsigned int count = 0;
	unsigned long long pidaddr = pidinfo->pid;
	unsigned int pidnum = pidinfo->num;
	int *pidbuf;

	if (!pidnum) {
		axcl_trace(AXCL_ERR, "No device pid");
		return 0;
	}

	if (!pidinfo->pid) {
		axcl_trace(AXCL_ERR, "User pid addr is NULL");
		return -1;
	}

	pidbuf = vmalloc(sizeof(int) * pidnum);
	if (!pidbuf) {
		axcl_trace(AXCL_ERR, "Pid buf malloc failed");
		return -1;
	}
	memset(pidbuf, 0, sizeof(int) * pidnum);

	if (axcl_handle) {
		if (!list_empty(&axcl_handle->process_list)) {
			list_for_each_safe(entry, tmp, &axcl_handle->process_list) {
				process_handle = list_entry(entry, struct process_info_t, head);
				if (process_handle == NULL)
					continue;
				if (!list_empty(&process_handle->dev_list)) {
					list_for_each_safe(deventry, devtmp, &process_handle->dev_list) {
						dev_handle = list_entry(deventry, struct device_handle_t, head);
						if (dev_handle == NULL)
							continue;
						if (dev_handle->device == target) {
							if (count >= pidnum)
								break;
							pidbuf[count] = process_handle->pid;
							count++;
							break;
						}
					}
				}
			}
		}
	}

	if (copy_to_user((void *)pidaddr, pidbuf, sizeof(int) * pidnum)) {
		axcl_trace(AXCL_ERR, "IOC_AXCL_PID_INFO copy to usr space failed!");
		vfree(pidbuf);
		return -1;
	}
	vfree(pidbuf);
	return 0;
}

static void axcl_get_pid_num(struct axcl_pid_num_t *pidnum)
{
	struct process_info_t *process_handle = NULL;
	struct device_handle_t *dev_handle = NULL;
	struct list_head *entry, *tmp;
	struct list_head *deventry, *devtmp;
	unsigned int target = pidnum->device;
	unsigned int count = 0;

	if (axcl_handle) {
		if (!list_empty(&axcl_handle->process_list)) {
			list_for_each_safe(entry, tmp, &axcl_handle->process_list) {
				process_handle = list_entry(entry, struct process_info_t, head);
				if (process_handle == NULL)
					continue;
				if (!list_empty(&process_handle->dev_list)) {
					list_for_each_safe(deventry, devtmp, &process_handle->dev_list) {
						dev_handle = list_entry(deventry, struct device_handle_t, head);
						if (dev_handle == NULL)
							continue;
						if (dev_handle->device == target) {
							count++;
							break;
						}
					}
				}
			}
		}
	}
	pidnum->num = count;
}

static int axcl_pcie_open(struct inode *inode, struct file *file)
{
	struct process_info_t *process_handle = NULL;
	struct list_head *entry, *tmp;

	axcl_trace(AXCL_DEBUG, "axcl pcie open pid = %d.", current->pid);

	if (!axcl_handle) {
		axcl_trace(AXCL_ERR, "axcl handler is NULL");
		return -1;
	}

	mutex_lock(&ioctl_mutex);
	if (!list_empty(&axcl_handle->process_list)) {
		list_for_each_safe(entry, tmp, &axcl_handle->process_list) {
			process_handle = list_entry(entry, struct process_info_t, head);
			if (process_handle) {
				if (process_handle->pid == current->pid) {
					file->private_data = process_handle;
					mutex_unlock(&ioctl_mutex);
					return 0;
				}
			}
		}
	}

	process_handle = kmalloc(sizeof(struct process_info_t), GFP_KERNEL);
	if (NULL == process_handle) {
		axcl_trace(AXCL_ERR,
			   "kmalloc for process handler failed, pid = %d",
			   current->pid);
		mutex_unlock(&ioctl_mutex);
		return -1;
	}

	INIT_LIST_HEAD(&process_handle->head);
	INIT_LIST_HEAD(&process_handle->dev_list);
	atomic_set(&process_handle->event, 0);
	init_waitqueue_head(&process_handle->wait);
	process_handle->pid = current->pid;

	list_add_tail(&process_handle->head, &axcl_handle->process_list);
	file->private_data = process_handle;

	mutex_unlock(&ioctl_mutex);
	return 0;
}

static int axcl_wake_up_poll(struct file *file)
{
	struct process_info_t *process_handle = file->private_data;

	if (!process_handle) {
		axcl_trace(AXCL_ERR, "wake up process handler is NULL");
		return -1;
	}

	atomic_inc(&process_handle->event);
	wake_up_interruptible(&process_handle->wait);

	return 0;
}

static int axcl_pcie_release(struct inode *inode, struct file *file)
{
	struct list_head *entry, *tmp;
	struct axcl_device_info devinfo;
	struct ax_transfer_handle *handle;
	struct process_info_t *process_handle = file->private_data;
	struct device_handle_t *dev_handle = NULL;
	unsigned int target;
	unsigned int port = AXCL_NOTIFY_PORT;
	int ret;

	axcl_trace(AXCL_DEBUG,
		   "current pid = %d, current tgid = %d, current name = %s",
		   current->pid, current->tgid, current->comm);

	if (!process_handle) {
		axcl_trace(AXCL_ERR, "release: process handle is NULL");
		return -1;
	}

	mutex_lock(&ioctl_mutex);
	if (!list_empty(&process_handle->dev_list)) {
		list_for_each_safe(entry, tmp, &process_handle->dev_list) {
			dev_handle = list_entry(entry, struct device_handle_t, head);
			if (dev_handle == NULL)
				continue;
			target = dev_handle->device;
			axcl_trace(AXCL_DEBUG, "axcl pcie release target: 0x%x",
				   target);
			devinfo.device = dev_handle->device;
			devinfo.cmd = AXCL_PORT_DESTROY;
			devinfo.port_num = dev_handle->port_num;
			memcpy(devinfo.ports, dev_handle->ports,
			       dev_handle->port_num * sizeof(int));
			handle =
			    (struct ax_transfer_handle *)
			    port_handle[target][port]->pci_handle;
			ret =
			    axcl_pcie_msg_send(handle, (void *)&devinfo,
					       sizeof(struct axcl_device_info));
			if (ret < 0) {
				axcl_trace(AXCL_ERR,
					   "Send dev %x destroy port msg info failed: %d",
					   target, ret);
			}
			axcl_clean_port_info(target, dev_handle);
			list_del(&dev_handle->head);
			kfree(dev_handle);
		}
	}

	list_del(&process_handle->head);
	kfree(process_handle);

	mutex_unlock(&ioctl_mutex);
	return 0;
}

static long axcl_pcie_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	int ret = 0;
	struct axcl_device_info devinfo;
	struct device_list_t devlist;
	struct axcl_bus_info_t businfo;
	struct axcl_pid_info_t pidinfo;
	struct axcl_pid_num_t pidnum;
	struct process_info_t *process_handle = file->private_data;
	struct device_handle_t *dev_handle = NULL;

	mutex_lock(&ioctl_mutex);
	switch (cmd) {
	case IOC_AXCL_DEVICE_LIST:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_DEVICE_LIST.");
		axcl_get_devices(&devlist);
		if (copy_to_user
		    ((void *)arg, (void *)&devlist,
		     sizeof(struct device_list_t))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_DEVICE_LIST copy to usr space failed!");
			ret = -1;
			goto out;
		}
		break;
	case IOC_AXCL_PORT_MANAGE:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_PORT_MANAGE.");
		if (copy_from_user
		    ((void *)&devinfo, (void *)arg,
		     sizeof(struct axcl_device_info))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_PORT_MANAGE Get parameter from usr space failed!");
			ret = -1;
			goto out;
		}
		ret = axcl_pcie_port_manage(file, &devinfo);
		if (ret < 0) {
			axcl_trace(AXCL_ERR, "axcl pcie req port failed.");
			goto out;
		} else {
			if (copy_to_user
			    ((void *)arg, (void *)&devinfo,
			     sizeof(struct axcl_device_info))) {
				axcl_trace(AXCL_ERR,
					   "IOC_AXCL_PORT_MANAGE copy to usr space failed!");
				ret = -1;
				goto out;
			}
		}
		break;
	case IOC_AXCL_CONN_STATUS:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_CONN_STATUS.");
		if (copy_to_user
		    ((void *)arg, (void *)&heartbeat_status,
		     sizeof(struct device_connect_status))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_CONN_STATUS copy to usr space failed!");
			ret = -1;
			goto out;
		}
		break;
	case IOC_AXCL_WAKEUP_POLL:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_WAKEUP_STATUS.");
		ret = axcl_wake_up_poll(file);
		if (ret < 0) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_WAKEUP_STATUS AXCL wake up poll failed.");
			goto out;
		}
		break;
	case IOC_AXCL_DEVICE_RESET:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_DEVICE_RESET.");
		if (copy_from_user
		    ((void *)&devinfo, (void *)arg,
		     sizeof(struct axcl_device_info))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_DEVICE_RESET Get parameter from usr space failed!");
			ret = -1;
			goto out;
		}
		host_reset_device(devinfo.device);

		dev_handle = find_dev_handle(process_handle, devinfo.device);
		if (dev_handle) {
			axcl_clean_port_info(devinfo.device, dev_handle);
			list_del(&dev_handle->head);
			kfree(dev_handle);
		}
		axcl_devices_heartbeat_status_set(devinfo.device, AXCL_HEARTBEAT_DEAD);
		break;
	case IOC_AXCL_DEVICE_BOOT:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_DEVICE_BOOT.");
		if (copy_from_user
		    ((void *)&devinfo, (void *)arg,
		     sizeof(struct axcl_device_info))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_DEVICE_RESET Get parameter from usr space failed!");
			ret = -1;
			goto out;
		}
		host_reset_device(devinfo.device);

		dev_handle = find_dev_handle(process_handle, devinfo.device);
		if (dev_handle) {
			axcl_clean_port_info(devinfo.device, dev_handle);
			list_del(&dev_handle->head);
			kfree(dev_handle);
		}

		ret =
		    axcl_firmware_load(g_axera_pcie_opt->slot_to_axdev
				       (devinfo.device));
		if (ret < 0) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_DEVICE_BOOT Device %x firmware load failed.",
				   devinfo.device);
			axcl_devices_heartbeat_status_set(devinfo.device,
							  AXCL_HEARTBEAT_DEAD);
			goto out;
		}
		ret = ax_pcie_msg_check_remote(devinfo.device);
		if (ret < 0) {
			axcl_trace(AXCL_ERR,
				   "axcl pcie check remote device %x failed",
				   devinfo.device);
			axcl_devices_heartbeat_status_set(devinfo.device,
							  AXCL_HEARTBEAT_DEAD);
			goto out;
		}
		axcl_devices_heartbeat_status_set(devinfo.device, AXCL_HEARTBEAT_ALIVE);
		axcl_timestamp_sync(devinfo.device);
		printk("device %x: alive!\n", devinfo.device);
		htcondition[devinfo.device] = true;
		wake_up(&heart_waitqueue[devinfo.device]);
		break;
	case IOC_AXCL_BUS_INFO:
		axcl_trace(AXCL_DEBUG, "IOC_AXCL_BUS_INFO.");
		if (copy_from_user
		    ((void *)&businfo, (void *)arg,
		     sizeof(struct axcl_bus_info_t))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_BUS_INFO Get parameter from usr space failed!");
			ret = -1;
			goto out;
		}

		axcl_get_bus_info(&businfo);

		if (copy_to_user
		    ((void *)arg, (void *)&businfo,
		     sizeof(struct axcl_bus_info_t))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_BUS_INFO copy to usr space failed!");
			ret = -1;
			goto out;
		}
		break;
	case IOC_AXCL_PID_NUM:
		if (copy_from_user
		    ((void *)&pidnum, (void *)arg,
		     sizeof(struct axcl_pid_num_t))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_PID_NUM Get parameter from usr space failed!");
			ret = -1;
			goto out;
		}
		axcl_get_pid_num(&pidnum);
		if (copy_to_user
		    ((void *)arg, (void *)&pidnum,
		     sizeof(struct axcl_pid_num_t))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_PID_NUM copy to usr space failed!");
			ret = -1;
			goto out;
		}
		break;
	case IOC_AXCL_PID_INFO:
		if (copy_from_user
		    ((void *)&pidinfo, (void *)arg,
		     sizeof(struct axcl_pid_info_t))) {
			axcl_trace(AXCL_ERR,
				   "IOC_AXCL_PID_INFO Get parameter from usr space failed!");
			ret = -1;
			goto out;
		}

		axcl_trace(AXCL_INFO, "IOC_AXCL_PID_INFO target = 0x%x",
			   pidinfo.device);

		ret = axcl_get_pid_info(&pidinfo);
		if (ret < 0) {
			axcl_trace(AXCL_ERR, "axcl get pid info failed");
		}
		break;
	default:
		axcl_trace(AXCL_ERR, "warning not defined cmd.");
		break;
	}

out:
	mutex_unlock(&ioctl_mutex);
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
	struct process_info_t *process_handle = file->private_data;

	if (!process_handle) {
		axcl_trace(AXCL_ERR, "process handle is NULL");
		return -1;
	}

	mutex_lock(&ioctl_mutex);
	poll_wait(file, &process_handle->wait, table);

	if (atomic_read(&process_handle->event) > 0) {
		atomic_dec(&process_handle->event);
		mutex_unlock(&ioctl_mutex);
		return POLLIN | POLLRDNORM;
	}
	mutex_unlock(&ioctl_mutex);
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
	.name = "axcl_host"
};

static int axcl_common_prot_create(unsigned int target)
{
	int ret;

	/* create notify port */
	ret = axcl_pcie_host_port_open(target, AXCL_NOTIFY_PORT);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "axcl pcie notify port open failed.");
		return -1;
	}
	/* create heartbeat port */
	ret = axcl_pcie_host_port_open(target, AXCL_HEARTBEAT_PORT);
	if (ret < 0) {
		axcl_trace(AXCL_ERR, "axcl pcie heartbeat port open failed.");
		return -1;
	}
	return 0;
}

static int __init axcl_pcie_host_init(void)
{
	int i, ret;
	unsigned int target;
	axcl_trace(AXCL_DEBUG, "axcl pcie host init.");

	mutex_init(&ioctl_mutex);

	for (i = 0; i < AXERA_MAX_MAP_DEV; i++) {
		heartbeat_status.status[i] = -1;
	}

	/* 1. firmware loading */
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		target = g_axera_dev_map[i]->slot_index;
		ret = axcl_firmware_load(g_axera_dev_map[i]);
		if (ret < 0) {
			axcl_trace(AXCL_ERR, "Device %x firmware load failed.",
				   target);
			axcl_devices_heartbeat_status_set(target,
							  AXCL_HEARTBEAT_DEAD);
		}
	}

	/* 2. create port */
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		target = g_axera_dev_map[i]->slot_index;
		if (axcl_devices_heartbeat_status_get(target) ==
		    AXCL_HEARTBEAT_DEAD)
			continue;

		ret = axcl_common_prot_create(target);
		if (ret < 0) {
			axcl_devices_heartbeat_status_set(target,
							  AXCL_HEARTBEAT_DEAD);
		}
	}

	/* 3. rc and ep handshake */
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		target = g_axera_dev_map[i]->slot_index;
		if (axcl_devices_heartbeat_status_get(target) ==
		    AXCL_HEARTBEAT_DEAD)
			continue;
		axcl_trace(AXCL_ERR, "axcl wait dev %x handshake...", target);
		ret = ax_pcie_msg_check_remote(target);
		if (ret < 0) {
			axcl_trace(AXCL_ERR,
				   "axcl pcie check remote device %x failed",
				   target);
			axcl_devices_heartbeat_status_set(target,
							  AXCL_HEARTBEAT_DEAD);
		}
		axcl_devices_heartbeat_status_set(target, AXCL_HEARTBEAT_ALIVE);
	}

	/* 4. timestamp sync */
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		target = g_axera_dev_map[i]->slot_index;
		if (axcl_devices_heartbeat_status_get(target) ==
		    AXCL_HEARTBEAT_DEAD)
			continue;
		axcl_timestamp_sync(target);
	}

	/* 5. create heartbeat recv thread */
	axcl_handle = kmalloc(sizeof(struct axcl_handle_t), GFP_KERNEL);
	if (NULL == axcl_handle) {
		axcl_trace(AXCL_ERR, "kmalloc for axcl handler failed");
		return -1;
	}
	INIT_LIST_HEAD(&axcl_handle->process_list);

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		target = g_axera_dev_map[i]->slot_index;
		if (axcl_devices_heartbeat_status_get(target) ==
		    AXCL_HEARTBEAT_DEAD)
			continue;

		init_waitqueue_head(&heart_waitqueue[target]);
		htcondition[target] = true;
		heartbeat[i] =
		    kthread_create(heartbeat_recv_thread,
				   &g_axera_dev_map[i]->slot_index,
				   "heartbeat_kthd");
		if (IS_ERR(heartbeat[i]))
			return PTR_ERR(heartbeat[i]);
		wake_up_process(heartbeat[i]);
	}

	misc_register(&axcl_usrdev);

	return 0;
}

static void axcl_del_mem_list(void)
{
	struct list_head *entry, *tmp;
	struct list_head *deventry, *devtmp;
	struct process_info_t *process_handle = NULL;
	struct device_handle_t *dev_handle = NULL;

	mutex_lock(&ioctl_mutex);
	if (!axcl_handle) {
		axcl_trace(AXCL_ERR, "axcl handle is NULL");
		mutex_unlock(&ioctl_mutex);
		return;
	}

	/* mem list empty means no data is comming */
	if (!list_empty(&axcl_handle->process_list)) {
		list_for_each_safe(entry, tmp, &axcl_handle->process_list) {
			process_handle = list_entry(entry, struct process_info_t, head);
			if (process_handle == NULL)
				continue;
			if (!list_empty(&process_handle->dev_list)) {
				list_for_each_safe(deventry, devtmp, &process_handle->dev_list) {
					dev_handle = list_entry(deventry, struct device_handle_t, head);
					if (dev_handle == NULL)
						continue;
					list_del(&dev_handle->head);
					kfree(dev_handle);
				}
			}
			list_del(&process_handle->head);
			kfree(process_handle);
		}
	}
	kfree(axcl_handle);
	mutex_unlock(&ioctl_mutex);
}

static void __exit axcl_pcie_host_exit(void)
{
	int i;
	unsigned int target;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		target = g_axera_dev_map[i]->slot_index;
		if (heartbeat[i]) {
			htcondition[target] = true;
			wake_up(&heart_waitqueue[target]);
			kthread_stop(heartbeat[i]);
		}
		host_reset_device(target);
	}

	axcl_del_mem_list();

	misc_deregister(&axcl_usrdev);
}

module_init(axcl_pcie_host_init);
module_exit(axcl_pcie_host_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
MODULE_IMPORT_NS(VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver);
#else
MODULE_IMPORT_NS("VFS_internal_I_am_really_a_filesystem_and_am_NOT_a_driver");
#endif