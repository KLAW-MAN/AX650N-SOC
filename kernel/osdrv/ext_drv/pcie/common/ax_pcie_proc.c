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
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/sysctl.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_proc.h"

#ifdef IS_THIRD_PARTY_PLATFORM
#define AX_PROC_NAME "ax_proc"
static struct proc_dir_entry *ax_proc;
#endif
#define AX_PCIE_POLICY	"policy"
#define PCIE_DUMP_NAME	"sysdump"
#define PCIE_DUMP_PATH	"path"
#define PCIE_DUMP_SIZE	"size"
#define PCIE_DUMP_DEBUG	"debug"

#define SYSDUMP_DEFAULT_PATH	"/opt"
#define SYSDUMP_DEFAULT_SIZE	(0x40000000)
#define SYSDUMP_DEFAULT_DEBUG	(0)
static struct proc_dir_entry *ax_pcie_proc;
static struct proc_dir_entry *sysdump_proc;
static struct pcie_proc_entry pcie_proc_items;
static struct pcie_proc_entry policy_proc_items;
static struct pcie_proc_entry path_proc_items;
static struct pcie_proc_entry size_proc_items;
static struct pcie_proc_entry debug_proc_items;

struct proc_message pcie_proc;
EXPORT_SYMBOL(pcie_proc);

struct sysdump_control sysdump_ctrl;
EXPORT_SYMBOL(sysdump_ctrl);

static struct msg_send_cost send_cost;
static struct dev_msg_cost msg_cost;
static struct msg_bandwidth msg_bandwidth;
static struct kmalloc_msg_info kmalloc_msg;
static struct msg_len msg_len;
static struct msg_number msg_num;
static struct rp_wp rp_wp;
static struct dma_count dma_count;
static struct dma_list_count dma_list_count;
static struct dma_info dma_info;

static void show_pcie_proc_info(struct seq_file *s)
{
	unsigned int i = 0;

	seq_printf(s,
		   "------------ Time Consuming for A Message (usec)-----------\n"
		   "msg last cost	" "msg max cost \n");
	seq_printf(s,
		   "%08d\t"
		   "%08d\t",
		   ((struct msg_send_cost *)pcie_proc.msg_send_cost.data)->cur_msg_cost,
		   ((struct msg_send_cost *)pcie_proc.msg_send_cost.data)->max_msg_cost);
	seq_printf(s, "\n\n");

	seq_printf(s,
		   "------------ Send msg bandwidth (MB/s)-----------\n"
		    "total size(byte)	" "total time(us)	"	"bandwidth\n");
	seq_printf(s,
		   "%08ld\t\t"
		   "%08ld\t\t"
		   "%08dMB/s\t",
		   ((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->total_data,
		   ((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->total_time,
		   ((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->send_bandwidth);
	seq_printf(s, "\n\n");

	seq_printf(s, "------------ Message Size Record -----------\n"
		   "last irq msg    " "max irq msg	\n");
	seq_printf(s,
		   "0x%08x\t"
		   "0x%08x\t",
		   ((struct msg_len *)pcie_proc.msg_send_len.data)->cur_irq_send,
		   ((struct msg_len *)pcie_proc.msg_send_len.data)->max_irq_send);
	seq_printf(s, "\n\n");

	seq_printf(s, "------------ kmalloc Size Record (KB) -----------\n"
		   "kmalloc msg size    " "free msg size   \n");
	seq_printf(s,
		   "%08ld\t\t"
		   "%08ld\t",
		   ((struct kmalloc_msg_info *)pcie_proc.kmalloc_msg_info.data)->kmalloc_size,
		   ((struct kmalloc_msg_info *)pcie_proc.kmalloc_msg_info.data)->free_size);
	seq_printf(s, "\n\n");

	seq_printf(s, "------------ Message Number Record -----------\n"
		   "      dev       " "irq-msg sent    " "irq-msg recv  \n");
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		((struct msg_number *)pcie_proc.msg_number.data)->target[i].
		    dev_slot = g_axera_dev_map[i]->slot_index;
		seq_printf(s, "%8d\t" "0x%08x\t" "0x%08x\t",
			   ((struct msg_number *)pcie_proc.msg_number.data)->target[i].dev_slot,
			   ((struct msg_number *)pcie_proc.msg_number.data)->target[i].irq_send,
			   ((struct msg_number *)pcie_proc.msg_number.data)->target[i].irq_recv);
	}
	seq_printf(s, "\n\n");

	seq_printf(s,
		   "------------ Shared memory Read/Write pointer -----------\n"
		   "      dev       " "irq send rp     " "irq send wp     "
		   "irq recv rp     " "irq recv wp   \n");
	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].dev_slot
		    = g_axera_dev_map[i]->slot_index;
		seq_printf(s,
			   "%8d\t"
			   "0x%08llx\t"
			   "0x%08llx\t"
			   "0x%08llx\t"
			   "0x%08llx\t",
			   ((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].dev_slot,
			   ((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_send_rp,
			   ((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_send_wp,
			   ((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_recv_rp,
			   ((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_recv_wp);
	}
	seq_printf(s, "\n\n");

	seq_printf(s, "------------ DMA Transfer Timers Record -----------\n"
		   "data DMA read   " "data DMA write\n");
	seq_printf(s,
		   "0x%08x\t"
		   "0x%08x\t",
		   ((struct dma_count *)pcie_proc.dma_count.data)->data_read,
		   ((struct dma_count *)pcie_proc.dma_count.data)->data_write);
	seq_printf(s, "\n\n");

	seq_printf(s, "------------ DMA Transfer Lists Record -----------\n"
		   "data-r all      "
		   "data-r busy     " "data-w all      " "data-w busy   \n");
	seq_printf(s,
		   "0x%08x\t"
		   "0x%08x\t"
		   "0x%08x\t"
		   "0x%08x\t",
		   ((struct dma_list_count *)pcie_proc.dma_list.data)->data_read_all,
		   ((struct dma_list_count *)pcie_proc.dma_list.data)->data_read_busy,
		   ((struct dma_list_count *)pcie_proc.dma_list.data)->data_write_all,
		   ((struct dma_list_count *)pcie_proc.dma_list.data)->data_write_busy);
	seq_printf(s, "\n\n");

	seq_printf(s, "------------ Last DMA Transfer Information -----------\n"
		   "direction       "
		   "src address     " "dest address    " "size          \n");
	seq_printf(s,
		   "   read         "
		   "0x%lx\t"
		   "0x%lx\t"
		   "0x%08x\t",
		   ((struct dma_info *)pcie_proc.dma_info.data)->read.src_address,
		   ((struct dma_info *)pcie_proc.dma_info.data)->read.dest_address,
		   ((struct dma_info *)pcie_proc.dma_info.data)->read.len);
	seq_printf(s, "\n");
	seq_printf(s,
		   "   write        "
		   "0x%lx\t"
		   "0x%lx\t"
		   "0x%08x\t",
		   ((struct dma_info *)pcie_proc.dma_info.data)->write.src_address,
		   ((struct dma_info *)pcie_proc.dma_info.data)->write.dest_address,
		   ((struct dma_info *)pcie_proc.dma_info.data)->write.len);
	seq_printf(s, "\n");
	seq_printf(s, "\n");
}

static int pcie_proc_read(struct seq_file *s, void *v)
{
	if (pcie_proc.get_rp_wp)
		pcie_proc.get_rp_wp();
	if (pcie_proc.get_dma_trans_list_info)
		pcie_proc.get_dma_trans_list_info();

	show_pcie_proc_info(s);

	return 0;
}

static int policy_proc_read(struct seq_file *s, void *v)
{
	seq_printf(s, "%d\n", pcie_proc.proc_level);
	return 0;
}

static int policy_proc_write(struct file *file,
				  const char __user *buffer, size_t count,
				  loff_t *ppos)
{
	char buf[3] = {0};
	long val;
	int ret;

	if (count > 2)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count)) {
		proc_trace(PCIE_PROC_ERR, "copy from user failed");
		return -1;
	}

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return ret;

	pcie_proc.proc_level = val;
	return count;
}

static int path_proc_read(struct seq_file *s, void *v)
{
	seq_printf(s, "%s\n", sysdump_ctrl.path);
	return 0;
}

static int path_proc_write(struct file *file,
				  const char __user *buffer, size_t count,
				  loff_t *ppos)
{
	char buf[256] = {0};

	if (count > 255)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count)) {
		proc_trace(PCIE_PROC_ERR, "copy from user failed");
		return -1;
	}

	memset(sysdump_ctrl.path, 0, sizeof(sysdump_ctrl.path));
	strncpy(sysdump_ctrl.path, buf, count);
	return count;
}

static int size_proc_read(struct seq_file *s, void *v)
{
	seq_printf(s, "%d\n", sysdump_ctrl.size);
	return 0;
}

static int size_proc_write(struct file *file,
				  const char __user *buffer, size_t count,
				  loff_t *ppos)
{
	char buf[20] = {0};
	long val;
	int ret;

	if (count > 19)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count)) {
		proc_trace(PCIE_PROC_ERR, "copy from user failed");
		return -1;
	}

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return ret;

	sysdump_ctrl.size = val;
	return count;
}

static int debug_proc_read(struct seq_file *s, void *v)
{
	seq_printf(s, "%d\n", sysdump_ctrl.debug);
	return 0;
}

static int debug_proc_write(struct file *file,
				  const char __user *buffer, size_t count,
				  loff_t *ppos)
{
	char buf[3] = {0};
	long val;
	int ret;

	if (count > 2)
		return -EINVAL;

	if (copy_from_user(buf, buffer, count)) {
		proc_trace(PCIE_PROC_ERR, "copy from user failed");
		return -1;
	}

	ret = kstrtoul(buf, 10, &val);
	if (ret)
		return ret;

	sysdump_ctrl.debug = val;
	return count;
}

static int ax_pcie_proc_open(struct inode *inode, struct file *file)
{
	struct pcie_proc_entry *item = ax_pde_data(inode);
	return single_open(file, item->read, item);
}

static ssize_t ax_pcie_proc_write(struct file *file,
				  const char __user *buf, size_t count,
				  loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct pcie_proc_entry *item = s->private;

	if (item->write) {
		return item->write(file, buf, count, ppos);
	}

	return -ENOSYS;
}

static int ax_pcie_proc_transfer_init(void)
{
	int i = 0;

	pcie_proc.proc_level = 1;
	pcie_proc.get_rp_wp = NULL;
	pcie_proc.get_dma_trans_list_info = NULL;
	send_cost.cur_msg_cost = 0;
	send_cost.cur_msg_cost = 0;
	pcie_proc.msg_send_cost.data = &send_cost;
	msg_len.cur_irq_send = 0;
	msg_len.max_irq_send = 0;
	pcie_proc.msg_send_len.data = &msg_len;

	msg_cost.cur_link_cost = 0;
	msg_cost.max_link_cost = 0;
	msg_cost.dev_slot = 0;
	msg_cost.cur_cost_port = 0;
	msg_cost.max_cost_port = 0;
	pcie_proc.dev_msg_cost.data = &msg_cost;

	msg_bandwidth.total_data = 0;
	msg_bandwidth.total_time = 0;
	msg_bandwidth.send_bandwidth = 0;
	pcie_proc.msg_bandwidth.data = &msg_bandwidth;

	kmalloc_msg.free_size = 0;
	kmalloc_msg.kmalloc_size = 0;
	pcie_proc.kmalloc_msg_info.data = &kmalloc_msg;

	for (i = 0; i < MAX_PCIE_DEVICES; i++) {
		msg_num.target[i].dev_slot = 0;
		msg_num.target[i].irq_send = 0;
		msg_num.target[i].irq_recv = 0;
	}
	pcie_proc.msg_number.data = &msg_num;

	for (i = 0; i < MAX_PCIE_DEVICES; i++) {
		rp_wp.target[i].dev_slot = 0;
		rp_wp.target[i].irq_send_rp = 0;
		rp_wp.target[i].irq_send_wp = 0;
		rp_wp.target[i].irq_recv_rp = 0;
		rp_wp.target[i].irq_recv_wp = 0;
	}

	pcie_proc.rp_wp.data = &rp_wp;

	dma_count.data_write = 0;
	dma_count.data_read = 0;
	pcie_proc.dma_count.data = &dma_count;

	dma_list_count.data_read_all = 0;
	dma_list_count.data_read_busy = 0;
	dma_list_count.data_write_all = 0;
	dma_list_count.data_write_busy = 0;
	pcie_proc.dma_list.data = &dma_list_count;

	dma_info.read.src_address = 0x0;
	dma_info.read.dest_address = 0x0;
	dma_info.read.len = 0x0;
	dma_info.write.src_address = 0x0;
	dma_info.write.dest_address = 0x0;
	dma_info.write.len = 0x0;
	pcie_proc.dma_info.data = &dma_info;
	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 6, 0))
static struct file_operations msg_proc_ops = {
	.owner = THIS_MODULE,
	.open = ax_pcie_proc_open,
	.read = seq_read,
	.write = ax_pcie_proc_write,
	.llseek = seq_lseek,
	.release = single_release
};
#else
static struct proc_ops msg_proc_ops = {

	.proc_open = ax_pcie_proc_open,
	.proc_read = seq_read,
	.proc_write = ax_pcie_proc_write,
	.proc_lseek = seq_lseek,
	.proc_release = single_release
};
#endif

static void sysdump_ctrl_init(void)
{
	strncpy(sysdump_ctrl.path, SYSDUMP_DEFAULT_PATH, strlen(SYSDUMP_DEFAULT_PATH));
	sysdump_ctrl.size = SYSDUMP_DEFAULT_SIZE;
	sysdump_ctrl.debug = SYSDUMP_DEFAULT_DEBUG;
}

static int pcie_sysdump_proc_create(void)
{
	struct proc_dir_entry *path;
	struct proc_dir_entry *size;
	struct proc_dir_entry *debug;

	sysdump_ctrl_init();

	memset(&path_proc_items, 0x00, sizeof(path_proc_items));
	memset(&size_proc_items, 0x00, sizeof(size_proc_items));
	memset(&debug_proc_items, 0x00, sizeof(debug_proc_items));

	sysdump_proc = proc_mkdir(PCIE_DUMP_NAME, ax_pcie_proc);
	if (!sysdump_proc) {
		printk("Create sysdump proc node failed\n");
		return -1;
	}

	path = proc_create_data(PCIE_DUMP_PATH, 0644, sysdump_proc, &msg_proc_ops, &path_proc_items);
	if (!path) {
		printk("Create path proc node failed\n");
	}
	strncpy(path_proc_items.entry_name, PCIE_DUMP_PATH, strlen(PCIE_DUMP_PATH));
	path_proc_items.entry = path;
	path_proc_items.read = path_proc_read;
	path_proc_items.write = path_proc_write;

	size = proc_create_data(PCIE_DUMP_SIZE, 0644, sysdump_proc, &msg_proc_ops, &size_proc_items);
	if (!size) {
		printk("Create size proc node failed\n");
	}
	strncpy(size_proc_items.entry_name, PCIE_DUMP_SIZE, strlen(PCIE_DUMP_SIZE));
	size_proc_items.entry = size;
	size_proc_items.read = size_proc_read;
	size_proc_items.write = size_proc_write;

	debug = proc_create_data(PCIE_DUMP_DEBUG, 0644, sysdump_proc, &msg_proc_ops, &debug_proc_items);
	if (!debug) {
		printk("Create debug proc node failed\n");
	}
	strncpy(debug_proc_items.entry_name, PCIE_DUMP_DEBUG, strlen(PCIE_DUMP_DEBUG));
	debug_proc_items.entry = debug;
	debug_proc_items.read = debug_proc_read;
	debug_proc_items.write = debug_proc_write;

	return 0;
}

static int ax_pcie_proc_create(char *entry_name)
{
	struct proc_dir_entry *entry;
	struct proc_dir_entry *policy;

	if (ax_pcie_proc_transfer_init()) {
		printk(KERN_ERR "Proc message init failed\n");
		return -1;
	}

	entry = proc_create_data(entry_name, 0, ax_pcie_proc, &msg_proc_ops, &pcie_proc_items);
	if (!entry) {
		printk("Create %s proc node failed\n", entry_name);
	}
	strncpy(pcie_proc_items.entry_name, entry_name, strlen(entry_name));
	pcie_proc_items.entry = entry;
	pcie_proc_items.read = pcie_proc_read;
	pcie_proc_items.write = NULL;

	policy = proc_create_data(AX_PCIE_POLICY, 0644, ax_pcie_proc, &msg_proc_ops, &policy_proc_items);
	if (!policy) {
		printk("Create %s proc node failed\n", AX_PCIE_POLICY);
	}
	strncpy(policy_proc_items.entry_name, AX_PCIE_POLICY, strlen(AX_PCIE_POLICY));
	policy_proc_items.entry = policy;
	policy_proc_items.read = policy_proc_read;
	policy_proc_items.write = policy_proc_write;

	pcie_sysdump_proc_create();

	return 0;
}

static void ax_pcie_proc_remove(char *entry_name)
{
	if (path_proc_items.entry)
		remove_proc_entry(path_proc_items.entry_name, sysdump_proc);
	if (size_proc_items.entry)
		remove_proc_entry(size_proc_items.entry_name, sysdump_proc);
	if (debug_proc_items.entry)
		remove_proc_entry(debug_proc_items.entry_name, sysdump_proc);
	if (sysdump_proc)
		remove_proc_entry(PCIE_DUMP_NAME, ax_pcie_proc);
	if (pcie_proc_items.entry)
		remove_proc_entry(pcie_proc_items.entry_name, ax_pcie_proc);
	if (policy_proc_items.entry)
		remove_proc_entry(policy_proc_items.entry_name, ax_pcie_proc);

	pcie_proc_items.entry = NULL;
	policy_proc_items.entry = NULL;
	path_proc_items.entry = NULL;
	size_proc_items.entry = NULL;
	debug_proc_items.entry = NULL;
}

int ax_pcie_proc_init(char *entry_name)
{
	int ret;

#ifdef IS_THIRD_PARTY_PLATFORM
	ax_proc = proc_mkdir(AX_PROC_NAME, NULL);
	if (NULL == ax_proc) {
		proc_trace(PCIE_PROC_ERR, "make ax_proc dir failed!");
		return -1;
	}
#endif
	ax_pcie_proc = proc_mkdir(PROC_DIR_NAME, NULL);
	if (NULL == ax_pcie_proc) {
		proc_trace(PCIE_PROC_ERR, "make ax_pcie_proc dir failed!");
		return -1;
	}

	memset(&pcie_proc_items, 0x00, sizeof(pcie_proc_items));
	memset(&policy_proc_items, 0x00, sizeof(policy_proc_items));

	ret = ax_pcie_proc_create(entry_name);
	if (ret < 0) {
		proc_trace(PCIE_PROC_ERR, "ax pcie proc create failded");
		return -1;
	}

	return 0;
}

void ax_pcie_proc_exit(char *entry_name)
{
	ax_pcie_proc_remove(entry_name);
	if (ax_pcie_proc)
		remove_proc_entry(PROC_DIR_NAME, NULL);
#ifdef IS_THIRD_PARTY_PLATFORM
	if (ax_proc)
		remove_proc_entry(AX_PROC_NAME, NULL);
#endif
}
