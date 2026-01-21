/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_PCIE_PROC__
#define __AX_PCIE_PROC__

#define PROC_DIR_NAME		"ax_proc/pcie"

#define MAX_PROC_ENTRIES	32

#define PCIE_PROC_DEBUG	4
#define PCIE_PROC_INFO	3
#define PCIE_PROC_ERR	2
#define PCIE_PROC_FATAL	1
#define PCIE_CURRENT_LEVEL	2
#define proc_trace(level, s, params...) do {if (level <= PCIE_CURRENT_LEVEL)\
	printk("[%s, %d]:" s "\n", __func__, __LINE__, ##params);\
} while (0)

typedef int (*msg_proc_read) (struct seq_file *, void *);
typedef int (*msg_proc_write) (struct file *file,
			       const char __user *buf, size_t count,
			       loff_t *ppos);

enum mcc_proc_message_enable_level {
	PROC_LEVEL_I = 1,
	PROC_LEVEL_II = 2,
};

struct pcie_proc_entry {
	char entry_name[32];
	struct proc_dir_entry *entry;

	msg_proc_read read;
	msg_proc_write write;

	int b_default;
	void *pData;

	void *proc_info;
};

struct int_param {
	char name[64];
	unsigned int count;
	unsigned int handler_cost;
};

struct msg_send_cost {
	unsigned int cur_msg_cost;
	unsigned int max_msg_cost;
};

struct msg_len {
	unsigned int cur_irq_send;
	unsigned int max_irq_send;
};

struct dev_msg_number {
	unsigned int dev_slot;
	unsigned int irq_send;
	unsigned int irq_recv;
};

struct dev_msg_cost {
	unsigned int dev_slot;
	unsigned int cur_cost_port;
	unsigned int cur_link_cost;
	unsigned int max_cost_port;
	unsigned int max_link_cost;
};

struct kmalloc_msg_info {
	unsigned long kmalloc_size;
	unsigned long free_size;
};

struct msg_bandwidth {
	unsigned long total_data;
	unsigned long total_time;
	unsigned int send_bandwidth;
};

struct msg_number {
	struct dev_msg_number target[MAX_PCIE_DEVICES];
	unsigned int dev_number;
};

struct dev_rp_wp {
	unsigned int dev_slot;
	unsigned long long irq_send_rp;
	unsigned long long irq_send_wp;
	unsigned long long irq_recv_rp;
	unsigned long long irq_recv_wp;
};

struct rp_wp {
	struct dev_rp_wp target[MAX_PCIE_DEVICES];
	unsigned int dev_number;
};

struct dma_count {
	unsigned int data_write;
	unsigned int data_read;
};

struct dma_list_count {
	unsigned int data_read_all;
	unsigned int data_read_busy;
	unsigned int data_write_all;
	unsigned int data_write_busy;
};

struct dma_task_info {
	unsigned long src_address;
	unsigned long dest_address;
	unsigned int len;
};

struct dma_info {
	struct dma_task_info read;
	struct dma_task_info write;
};

struct proc_item {
	char name[128];
	struct list_head head;
	void *data;
};

struct proc_message {
	unsigned int proc_level;
	struct proc_item msg_send_cost;
	struct proc_item msg_send_len;
	struct proc_item msg_number;
	struct proc_item dev_msg_cost;
	struct proc_item msg_bandwidth;
	struct proc_item kmalloc_msg_info;
	struct proc_item rp_wp;
	struct proc_item dma_count;
	struct proc_item dma_list;
	struct proc_item dma_info;
	void (*get_rp_wp) (void);
	void (*get_dma_trans_list_info) (void);
};

struct sysdump_control {
	unsigned int size;
	char path[256];
	bool debug;
};

extern struct sysdump_control sysdump_ctrl;

extern struct proc_message pcie_proc;

int ax_pcie_proc_init(char *entry_name);
void ax_pcie_proc_exit(char *entry_name);

#endif
