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
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/time.h>
#include <linux/time64.h>
#include <uapi/linux/time.h>
#include "../include/ax_pcie_msg_transfer.h"
#include "../include/ax_pcie_msg_ursdev.h"
#include "../include/ax_pcie_dev.h"
#include "../include/ax_pcie_proc.h"

static struct ax_pcie_shared_memory_map ax_slave_info_map;
static struct ax_pcie_shared_memory_map ax_host_info_map[MAX_PCIE_DEVICES];

static spinlock_t s_msg_recv_lock;

static int __ax_pcie_kfifo_is_empty(struct pcie_kfifo *ax_pcie_kfifo)
{
	return ax_pcie_kfifo->in == ax_pcie_kfifo->out;
}

static int ax_pcie_kfifo_is_empty(void *dev)
{
	struct ax_pcie_shmem *p;
	struct axera_dev *pdev = (struct axera_dev *)dev;
	unsigned long flags;

	if (!g_axera_pcie_opt->is_host()) {
		struct ax_pcie_shared_memory_map *pinfo = &ax_slave_info_map;
		p = &pinfo->recv.shmem;
	} else {
		struct ax_pcie_shared_memory_map *pinfo
		    = &ax_host_info_map[pdev->slot_index];
		p = &pinfo->recv.shmem;
	}

	if (p->ax_pcie_kfifo == NULL) {
		trans_trace(TRANS_ERR, "kfifo is null\n");
		return -1;
	}

	ax_pcie_data_lock(&flags);
	p->ax_pcie_kfifo->out = *((volatile unsigned long long *)p->rp_addr);
	if (p->ax_pcie_kfifo->out == PCIE_BUS_ERROR) {
		trans_trace(TRANS_ERR, "PCIe bus error");
		ax_pcie_data_unlock(&flags);
		return -1;
	}
	p->ax_pcie_kfifo->in = *((volatile unsigned long long *)p->wp_addr);
	if (p->ax_pcie_kfifo->in == PCIE_BUS_ERROR) {
		trans_trace(TRANS_ERR, "PCIe bus error");
		ax_pcie_data_unlock(&flags);
		return -1;
	}
	ax_pcie_data_unlock(&flags);

	trans_trace(TRANS_DEBUG, "rp 0x%llx, wp 0x%llx",
		    p->ax_pcie_kfifo->out,
		    p->ax_pcie_kfifo->in);

	return __ax_pcie_kfifo_is_empty(p->ax_pcie_kfifo);
}

static int ax_pcie_kfifo_is_full(struct ax_pcie_shmem *p)
{
	return (p->ax_pcie_kfifo->in - p->ax_pcie_kfifo->out) >
					(p->ax_pcie_kfifo->mask);
}

static int __ax_pcie_kfifo_init(struct ax_pcie_shmem *p, void *buffer,
				unsigned int size)
{
	trans_trace(TRANS_DEBUG, "buffer = %p, size = %x",
		    buffer, size);

	p->ax_pcie_kfifo->in = 0;
	p->ax_pcie_kfifo->out = 0;
	p->ax_pcie_kfifo->data = buffer;

	if (size < 2) {
		p->ax_pcie_kfifo->mask = 0;
		return -EINVAL;
	}
	p->ax_pcie_kfifo->mask = size - 1;

	return 0;
}

static unsigned int ax_pcie_kfifo_init(struct ax_pcie_shmem *p)
{
	int ret;
	unsigned int size = p->end_addr - p->base_addr;

	p->ax_pcie_kfifo = kmalloc(sizeof(struct pcie_kfifo), GFP_KERNEL);
	if (!(p->ax_pcie_kfifo)) {
		trans_trace(TRANS_ERR, "pcie kfifo kmalloc failed");
		return -1;
	}

	ret = __ax_pcie_kfifo_init(p, (void *)p->base_addr, size);
	if (ret < 0) {
		trans_trace(TRANS_ERR, "ax pcie kfifo init fail");
		kfree(p->ax_pcie_kfifo);
	}

	printk("base_addr: 0x%lx, size: 0x%x\n", p->base_addr, size);
	return ret;
}

static void pcie_kfifo_free(struct ax_pcie_shmem *p)
{
	kfree(p->ax_pcie_kfifo);
}

static inline void ax_pcie_flush_post_write(struct ax_pcie_shmem *p)
{
	unsigned long wp;
	wp = *(volatile unsigned long *)p->wp_addr;
}

static unsigned int ax_pcie_kfifo_in(struct ax_pcie_shmem *p, const void *buf,
				     const void *head, unsigned int size)
{
	unsigned int fixlen;
	unsigned int aliagn = (size + HEAD_SIZE) & MEM_ALIAGN_VERS;
	unsigned int next_wp;
	unsigned int free_space;
	unsigned int l;
	unsigned long long fifo_size = p->ax_pcie_kfifo->mask + 1;
	unsigned long long off = p->ax_pcie_kfifo->in;

	fixlen = MEM_ALIAGN + MEM_ALIAGN - aliagn;
	next_wp = size + fixlen + HEAD_SIZE;

	trans_trace(TRANS_DEBUG, "fixlen = %x next_wp = %x fifo_size = %llx",
		    fixlen, next_wp, fifo_size);

	free_space = fifo_size - (p->ax_pcie_kfifo->in -
			 p->ax_pcie_kfifo->out);

	if (next_wp > free_space) {
		trans_trace(TRANS_ERR, "free space too small " "free_space = %x in = %llx  out= %llx",
		    free_space, p->ax_pcie_kfifo->in, p->ax_pcie_kfifo->out);
		return -2;
	}

	off %= fifo_size;
	trans_trace(TRANS_DEBUG, "size + head_size = %lx, fifo_size - off = %llx",
		    size + HEAD_SIZE, fifo_size - off);

	l = min((unsigned long long)(size + (unsigned int)HEAD_SIZE), fifo_size - off);

	if (l < HEAD_SIZE) {
		/* If the free space at the bottom is less than HEAD_SIZE, jump to the top to write */
		memcpy_toio(p->ax_pcie_kfifo->data + HEAD_SIZE, buf,
			      size);
		memcpy_toio(p->ax_pcie_kfifo->data, head, HEAD_SIZE);

		/* make sure post memory write is done */
		ax_pcie_flush_post_write(p);

		p->ax_pcie_kfifo->in += (next_wp + l);
	} else {
		memcpy_toio(p->ax_pcie_kfifo->data + off + HEAD_SIZE, buf,
			      l - HEAD_SIZE);
		memcpy_toio(p->ax_pcie_kfifo->data, buf + l - HEAD_SIZE,
			      size - (l - HEAD_SIZE));
		memcpy_toio(p->ax_pcie_kfifo->data + off, head, HEAD_SIZE);

		/* make sure post memory write is done */
		ax_pcie_flush_post_write(p);

		p->ax_pcie_kfifo->in += next_wp;
	}

	return size;
}

static unsigned int __ax_pcie_kfifo_out(struct ax_pcie_shmem *p, void *dst,
					unsigned int len)
{
	unsigned int l;
	unsigned long long size = p->ax_pcie_kfifo->mask + 1;
	unsigned long long off = p->ax_pcie_kfifo->out;
	unsigned long flags = 0;

	l = p->ax_pcie_kfifo->in - p->ax_pcie_kfifo->out;
	/* Judge whether the length is out of bounds */
	if ((len + HEAD_SIZE) > l)
		len = l;

	off %= size;

	ax_pcie_data_lock(&flags);
	if ((size - off) < HEAD_SIZE) {
		memcpy_fromio(dst, p->ax_pcie_kfifo->data + HEAD_SIZE, len);
	} else {
		l = min((unsigned long long)len, size - off - (unsigned int)HEAD_SIZE);
		memcpy_fromio(dst, p->ax_pcie_kfifo->data + off + HEAD_SIZE, l);
		memcpy_fromio(dst + l, p->ax_pcie_kfifo->data, len - l);
	}
	ax_pcie_data_unlock(&flags);

	return len;
}

static int ax_pcie_kfifo_out(void *dev, void *dst, unsigned int size)
{
	struct ax_pcie_shmem *p;
	struct axera_dev *pdev = (struct axera_dev *)dev;

	if (!g_axera_pcie_opt->is_host()) {
		struct ax_pcie_shared_memory_map *pinfo = &ax_slave_info_map;
		p = &pinfo->recv.shmem;
	} else {
		struct ax_pcie_shared_memory_map *pinfo
		    = &ax_host_info_map[pdev->slot_index];
		p = &pinfo->recv.shmem;
	}

	return __ax_pcie_kfifo_out(p, dst, size);

}

static inline void ax_update_kfifo_wrpointer(unsigned long long value,
					     unsigned long int addr)
{
	unsigned long long tmp_rp;
	unsigned int times = 0, time_max = 100;

	do {
		*(volatile unsigned long long *)addr = value;
		tmp_rp = *(volatile unsigned long long *)addr;
		if (tmp_rp == value)
			return;
		times++;
	} while (times < time_max);

	trans_trace(TRANS_ERR, "Update msg read/write pointer failed!");
}

static void *read_data_head(struct ax_pcie_shmem *p)
{
	struct ax_pci_transfer_head *head = NULL;
	unsigned int head_check = 0;
	unsigned int try_times = 10;
	unsigned int try = 0;
	unsigned long long off = p->ax_pcie_kfifo->out;
	unsigned long long fifo_size = p->ax_pcie_kfifo->mask + 1;

	off %= fifo_size;

	if ((fifo_size - off) < HEAD_SIZE) {
		/* The free space at the bottom is less than the head_Size, skip to the top to read */
		head = (struct ax_pci_transfer_head *)(p->ax_pcie_kfifo->data);
	} else {
		head = (struct ax_pci_transfer_head *)(off + p->ax_pcie_kfifo->data);
	}

	trans_trace(TRANS_DEBUG, "head 0x%lx head->check 0x%x head->magic 0x%x",
		    (unsigned long int)head, head->check, head->magic);

	head_check = (unsigned int)(head->check);

	while (head_check != HEAD_CHECK_FINISHED) {

		if (unlikely((++try) >= try_times)) {
			trans_trace(TRANS_ERR, "PCIe message head_check error "
				    "[0x%x/0x%x]!", head_check,
				    HEAD_CHECK_FINISHED);
			trans_trace(TRANS_ERR, "out[0x%llx], in[0x%llx]",
				    p->ax_pcie_kfifo->out,
				    p->ax_pcie_kfifo->in);
			p->ax_pcie_kfifo->out =
			    *((volatile unsigned long long *)p->rp_addr);
			p->ax_pcie_kfifo->in =
			    *((volatile unsigned long long *)p->wp_addr);
			return NULL;
		}

		udelay(10);

		head_check = (unsigned int)(head->check);
	}

	return head;
}

static void *ax_pcie_get_head(void *dev)
{
	struct ax_pcie_shmem *p;
	struct axera_dev *pdev = (struct axera_dev *)dev;

	if (!g_axera_pcie_opt->is_host()) {
		struct ax_pcie_shared_memory_map *pinfo = &ax_slave_info_map;
		p = &pinfo->recv.shmem;
	} else {
		struct ax_pcie_shared_memory_map *pinfo
		    = &ax_host_info_map[pdev->slot_index];
		p = &pinfo->recv.shmem;
	}

	return read_data_head(p);
}

static void _update_read_pointer(struct ax_pcie_shmem *p, unsigned int len)
{
	unsigned int aliag = (len + HEAD_SIZE) & MEM_ALIAGN_VERS;
	unsigned int fixlen = MEM_ALIAGN + MEM_ALIAGN - aliag;
	unsigned int next_rp;
	unsigned long long fifo_size = p->ax_pcie_kfifo->mask + 1;
	unsigned long long off = p->ax_pcie_kfifo->out;
	unsigned long flags = 0;

	next_rp = len + HEAD_SIZE + fixlen;

	off %= fifo_size;
	if ((fifo_size - off) < HEAD_SIZE) {
		p->ax_pcie_kfifo->out += (next_rp + (fifo_size - off));
	} else {
		p->ax_pcie_kfifo->out += next_rp;
	}

	trans_trace(TRANS_DEBUG, "next_rp %x, len + fixlen 0x%x", next_rp,
		    len + fixlen);

	trans_trace(TRANS_DEBUG, "p->rp_addr 0x%lx, kfifo->out = %llx",
		    p->rp_addr, p->ax_pcie_kfifo->out);

	ax_pcie_data_lock(&flags);
	ax_update_kfifo_wrpointer(p->ax_pcie_kfifo->out, p->rp_addr);
	ax_pcie_data_unlock(&flags);

	smp_mb();
}

static void update_kfifo_read_pointer(void *dev, void *data, unsigned int len)
{
	struct ax_pcie_shmem *p = NULL;
	struct axera_dev *pdev = (struct axera_dev *)dev;

	if (!g_axera_pcie_opt->is_host()) {
		struct ax_pcie_shared_memory_map *pinfo = &ax_slave_info_map;
		p = &pinfo->recv.shmem;
	} else {
		struct ax_pcie_shared_memory_map *pinfo
		    = &ax_host_info_map[pdev->slot_index];
		p = &pinfo->recv.shmem;
	}

	_update_read_pointer(p, len);

}

static int send_data_to_shmem_buffer(struct ax_pcie_shmem *p,
				     const void *buf,
				     unsigned int len,
				     struct ax_pci_transfer_head *head)
{
	unsigned int ret;

	p->ax_pcie_kfifo->in = *((volatile unsigned long long *)(p->wp_addr));
	if (p->ax_pcie_kfifo->in == PCIE_BUS_ERROR) {
		trans_trace(TRANS_ERR, "PCIe bus error");
		return -4;
	}
	p->ax_pcie_kfifo->out = *((volatile unsigned long long *)(p->rp_addr));
	if (p->ax_pcie_kfifo->out == PCIE_BUS_ERROR) {
		trans_trace(TRANS_ERR, "PCIe bus error");
		return -4;
	}

	if (ax_pcie_kfifo_is_full(p)) {
		trans_trace(TRANS_DEBUG, "Kfifo is full " "in = %llx out = %llx",
			p->ax_pcie_kfifo->in, p->ax_pcie_kfifo->out);
		return -1;
	}

	ret = ax_pcie_kfifo_in(p, buf, head, len);
	if (ret < 0)
		return ret;

	smp_wmb();
	trans_trace(TRANS_DEBUG, "wp = %llx", p->ax_pcie_kfifo->in);
	ax_update_kfifo_wrpointer(p->ax_pcie_kfifo->in, p->wp_addr);
	//smp_mb();
	return ret;
}

static int ax_send_data_irq(void *dev,
			    const void *buf,
			    unsigned int len, struct ax_pci_transfer_head *head)
{
	struct ax_pcie_shmem *p;
	int ret;
	int size;
	unsigned long flags = 0;
	struct axera_dev *pdev = (struct axera_dev *)dev;
	// volatile unsigned long long host_recvirq_count;
	// volatile unsigned long long slave_sendirq_count;
	// unsigned int count;

	if (!g_axera_pcie_opt->is_host()) {
		struct ax_pcie_shared_memory_map *pinfo = &ax_slave_info_map;
		p = &pinfo->send.shmem;
	} else {
		struct ax_pcie_shared_memory_map *pinfo
		    = &ax_host_info_map[pdev->slot_index];
		p = &pinfo->send.shmem;
	}

	ax_pcie_data_lock(&flags);
	size = send_data_to_shmem_buffer(p, buf, len, head);
	if (size < 0) {
		ax_pcie_data_unlock(&flags);
		return size;
	}

#if 0
	if (!g_axera_pcie_opt->is_host()) {
		do {
			host_recvirq_count = *(volatile unsigned long long *)(pdev->shm_base_virt + 0x8);
			slave_sendirq_count = *(volatile unsigned long long *)(pdev->shm_base_virt + 0x10);
			count = (slave_sendirq_count - host_recvirq_count);
		} while (count > 0);
	}
#endif

	ret = g_axera_pcie_opt->trigger_msg_irq(pdev, 0); //use msi vector 0
	if (ret < 0) {
		ax_pcie_data_unlock(&flags);
		return ret;
	}

	/* record master request mailbox irq count */
	if (g_axera_pcie_opt->is_host()) {
		*(volatile unsigned long long *)(pdev->shm_base_virt + 0x20) += 1;
	}

	ax_pcie_data_unlock(&flags);

	return size;
}

static void clear_proc_record_info(struct axera_dev *ax_dev)
{
	int i;
	if (pcie_proc.proc_level) {
		for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
			if (g_axera_dev_map[i] == ax_dev)
				break;
		}

		((struct msg_number *)pcie_proc.msg_number.data)->target[i].irq_send = 0;
		((struct msg_number *)pcie_proc.msg_number.data)->target[i].irq_recv = 0;
		((struct msg_send_cost *)pcie_proc.msg_send_cost.data)->cur_msg_cost = 0;
		((struct msg_send_cost *)pcie_proc.msg_send_cost.data)->max_msg_cost = 0;
		((struct kmalloc_msg_info *)pcie_proc.kmalloc_msg_info.data)->kmalloc_size = 0;
		((struct kmalloc_msg_info *)pcie_proc.kmalloc_msg_info.data)->free_size = 0;
		((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->total_data = 0;
		((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->total_time = 0;
		((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->send_bandwidth = 0;
		((struct msg_len *)pcie_proc.msg_send_len.data)->cur_irq_send = 0;
		((struct msg_len *)pcie_proc.msg_send_len.data)->max_irq_send = 0;
	}
}

static void init_meminfo(void *start, void *end, struct ax_pcie_shmem *p)
{
	sema_init(&p->sem, 1);

	trans_trace(TRANS_DEBUG,
		    "          section start: 0x%p, section end: 0x%p", start,
		    end);

	p->base_addr = (unsigned long int)start;
	p->end_addr = (unsigned long int)end;
	trans_trace(TRANS_DEBUG, "meminfo: base_addr = %lx", p->base_addr);
	trans_trace(TRANS_DEBUG, "meminfo: end_addr = %lx", p->end_addr);

	p->rp_addr = (unsigned long int)(start - 64);
	p->wp_addr = (unsigned long int)(start - 32);
	trans_trace(TRANS_DEBUG, "meminfo: rp_addr = %lx", p->rp_addr);
	trans_trace(TRANS_DEBUG, "meminfo: wp_addr = %lx", p->wp_addr);
}

static void init_mem(struct axera_dev *ax_dev, void *start, void *end, struct ax_pcie_shmem_info *p)
{
	unsigned int size = end - start;

	trans_trace(TRANS_DEBUG, "     data_base: 0x%p, data_end: 0x%p", start,
		    end);

#ifdef SHMEM_FROM_MASTER
	if (g_axera_pcie_opt->is_host()) {
#else
	if (!g_axera_pcie_opt->is_host()) {
#endif
		memset((start-64), 0, size);
	}

	init_meminfo(start, end, &p->shmem);

	if (ax_dev->kfifo_init_checked != DEVICE_KFIFO_INIT)
		ax_pcie_kfifo_init(&p->shmem);

	p->shmem.ax_pcie_kfifo->in = 0;
	p->shmem.ax_pcie_kfifo->out = 0;

	if (!g_axera_pcie_opt->is_host()) {
		writel(p->shmem.ax_pcie_kfifo->out,
		       (void *)(p->shmem.rp_addr));
		writel(p->shmem.ax_pcie_kfifo->in,
		       (void *)(p->shmem.wp_addr));
		trans_trace(TRANS_DEBUG, "rp_addr[0x%lx], wp_addr[0x%lx].",
			    (unsigned long int)p->shmem.rp_addr,
			    (unsigned long int)p->shmem.wp_addr);
	}
#ifdef MSG_PROC_ENABLE
	clear_proc_record_info(ax_dev);
#endif
}

static int ax_shared_mem_init(void *dev)
{
	u64 first_half_base, first_half_end;
	u64 second_half_base, second_half_end;
	struct axera_dev *ax_dev = (struct axera_dev *)dev;
	struct ax_pcie_shared_memory_map *pinfo = NULL;
	u64 opt_address_base = 0;
	u64 opt_address_end = 0;

	spin_lock_init(&s_msg_recv_lock);

	if (!g_axera_pcie_opt->is_host()) {
		pinfo = &ax_slave_info_map;
	} else {
		pinfo = &ax_host_info_map[ax_dev->slot_index];
	}

	opt_address_base = (u64)ax_dev->shm_base_virt;
	opt_address_end = (u64)ax_dev->shm_end_virt;

	trans_trace(TRANS_DEBUG, "mem init: shm_base_virt = %p",
		    ax_dev->shm_base_virt);
	trans_trace(TRANS_DEBUG, "mem init: shm_end_virt = %p",
		    ax_dev->shm_end_virt);

	first_half_base = opt_address_base + 4 * MEM_ALIAGN;
	first_half_end = opt_address_base + ((ax_dev->shm_size) / 2) - 6 * MEM_ALIAGN;

	trans_trace(TRANS_DEBUG,
		    "first_half_base[0x%llx], first_half_end[0x%llx]",
		    first_half_base, first_half_end);

	opt_address_base += ((ax_dev->shm_size) / 2);

	second_half_base = opt_address_base + 4 * MEM_ALIAGN;
	second_half_end = opt_address_end - 10 * MEM_ALIAGN;

	trans_trace(TRANS_DEBUG,
		    "second_half_base[0x%llx], second_half_end[0x%llx]",
		    second_half_base, second_half_end);

	if (!g_axera_pcie_opt->is_host()) {
		trans_trace(TRANS_DEBUG, "    First half:");
		init_mem(ax_dev, (void *)first_half_base, (void *)first_half_end,
			 &pinfo->recv);

		trans_trace(TRANS_DEBUG, "    Second half:");
		init_mem(ax_dev, (void *)second_half_base, (void *)second_half_end,
			 &pinfo->send);

	} else {
		trans_trace(TRANS_DEBUG, "    First half:");
		init_mem(ax_dev, (void *)first_half_base, (void *)first_half_end,
			 &pinfo->send);

		trans_trace(TRANS_DEBUG, "    Second half:");
		init_mem(ax_dev, (void *)second_half_base, (void *)second_half_end,
			 &pinfo->recv);
	}

	return 0;
}

void update_proc_rp_wp(void)
{
	struct ax_pcie_shared_memory_map *pinfo;
	unsigned int i;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		if (DEVICE_KFIFO_INIT
		    != g_axera_dev_map[i]->kfifo_init_checked)
			continue;
		if (g_axera_pcie_opt->is_host()) {
			int dev_slot;
			dev_slot = g_axera_dev_map[i]->slot_index;
			pinfo = &ax_host_info_map[dev_slot];
		} else {
			pinfo = &ax_slave_info_map;
		}

		((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_send_rp
		    = *(unsigned long long *)pinfo->send.shmem.rp_addr;
		((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_send_wp
		    = *(unsigned long long *)pinfo->send.shmem.wp_addr;
		((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_recv_rp
		    = *(unsigned long long *)pinfo->recv.shmem.rp_addr;
		((struct rp_wp *)pcie_proc.rp_wp.data)->target[i].irq_recv_wp
		    = *(unsigned long long *)pinfo->recv.shmem.wp_addr;
	}
}

EXPORT_SYMBOL(update_proc_rp_wp);

static void ax_pcie_recv_irq_mem(struct axera_dev *ax_dev)
{
	struct axera_dev *dev = ax_dev;
	struct ax_transfer_handle *handle = NULL;
	struct ax_pci_transfer_head *head = NULL;
	unsigned int ret;
	unsigned int len;
	unsigned int port;
	unsigned int magic;
	unsigned int slot;
	void *data;
	// unsigned long flag;
	unsigned long msg_flags;
	int i;

	// spin_lock_irqsave(&s_msg_recv_lock, flag);

	while (!ax_pcie_kfifo_is_empty(dev)) {
		ax_pcie_data_lock(&msg_flags);
		head = ax_pcie_get_head(dev);
		if (head == NULL) {
			trans_trace(TRANS_ERR, "Read msg head error.");
			ax_pcie_data_unlock(&msg_flags);
			goto ext;
		}

		data = (void *)head + HEAD_SIZE;
		len = head->length;
		magic = head->magic;
		slot = head->slot;
		port = head->port;
		ax_pcie_data_unlock(&msg_flags);
		trans_trace(TRANS_DEBUG, "head->magic 0x%x head 0x%lx len 0x%x.",
			    magic, (unsigned long int)head, len);

		trans_trace(TRANS_DEBUG, "head->slot[0x%x] " "head->port[0x%x]",
			    slot, port);

		handle = find_device_handle(slot, port);
		if (handle == NULL)
			goto ext;

		trans_trace(TRANS_DEBUG, "handle 0x%lx",
			    (unsigned long int)handle);

#ifdef MSG_PROC_ENABLE
		if (pcie_proc.proc_level) {
			for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
				if (g_axera_dev_map[i]->slot_index ==
				    handle->target_id)
					break;
			}

			((struct msg_number *)pcie_proc.msg_number.data)->
			    target[i].irq_recv++;
		}
#endif

		if (handle && handle->msg_notifier_recvfrom) {

			trans_trace(TRANS_DEBUG, "handle 0x%lx notify recv",
				    (unsigned long int)handle);

			ret =
			    handle->msg_notifier_recvfrom(handle, data,
							  len);
			if (ret) {
				trans_trace(TRANS_ERR,
					    "hanlde 0x%lx "
					    "notify recv data failed!",
					    (unsigned long int)handle);
				// spin_unlock_irqrestore(&s_msg_recv_lock, flag);
				return;
			}
		} else {
			if (handle) {
				trans_trace(TRANS_ERR,
					    "handler[0x%lx: 0x%x/0x%x] recv_notifier is NULL!",
					    (unsigned long int)handle,
					    slot, port);
			} else {
				if (!g_axera_pcie_opt->is_host())
					    goto ext;
				trans_trace(TRANS_ERR,
					    "handler is NULL[port/target : 0x%x/0x%x!]",
					    port, slot);
			}
			trans_trace(TRANS_INFO,
				    "No handle handles this message!");
		}
ext:
		update_kfifo_read_pointer(dev, data, len);
	}
	// spin_unlock_irqrestore(&s_msg_recv_lock, flag);
}

void slave_message_irq_handler(int fromid, int regno, int count)
{
	unsigned int status = 0;
	struct axera_dev *ax_dev = g_axera_dev_map[0];
	struct mailbox_msg msg;
	unsigned long flags;

	trans_trace(TRANS_DEBUG, "slave: message_handler");

	if (ax_dev->shm_base_virt == NULL) {
		return;
	}

	status = g_axera_pcie_opt->clear_msg_irq(regno, count, &msg);
	if (status) {
		ax_pcie_data_lock(&flags);
		/* slave clean mailbox irq count */
		*(volatile unsigned long long *)(ax_dev->shm_base_virt + 0x28) += 1;
		ax_pcie_data_unlock(&flags);
		ax_pcie_recv_irq_mem(ax_dev);
	}
}

EXPORT_SYMBOL(slave_message_irq_handler);

irqreturn_t host_message_irq_handler(int irq, void *id)
{
	struct axera_dev *ax_dev;
	trans_trace(TRANS_DEBUG, "host: message_handler");

	if (!id) {
		printk("id is NULL, exit\n");
		return -1;
	}

	ax_dev = (struct axera_dev *)(id);
	if (ax_dev->shm_base_virt == NULL)
		return 0;

	// *(volatile unsigned long long *)(ax_dev->shm_base_virt + 0x8) += 1;
	ax_pcie_recv_irq_mem(ax_dev);

	return IRQ_HANDLED;
}

EXPORT_SYMBOL(host_message_irq_handler);

static unsigned int get_time_cost_us(struct timespec64 *start,
				     struct timespec64 *end)
{
	return (end->tv_sec - start->tv_sec) * 1000000	//usec
	    + (end->tv_nsec - start->tv_nsec) / 1000;	//usec
}

static int host_handshake_slave_step0(struct axera_dev *ax_dev)
{
	int handshake_timeout = 120000;
	ax_dev->shm_base = ax_dev->bar[0];
	ax_dev->shm_end = ax_dev->shm_base + ax_dev->shm_size;
	trans_trace(TRANS_DEBUG, "shm base 0x%llx", ax_dev->shm_base);
	trans_trace(TRANS_DEBUG, "shm end 0x%llx", ax_dev->shm_end);

	ax_dev->shm_base_virt = ax_dev->bar_virt[0];
	ax_dev->shm_end_virt = ax_dev->bar_virt[0] + ax_dev->shm_size;
	trans_trace(TRANS_DEBUG, "shm base virt = 0x%p", ax_dev->shm_base_virt);
	trans_trace(TRANS_DEBUG, "shm end virt =  0x%p", ax_dev->shm_end_virt);

#ifdef SHMEM_FROM_MASTER
	trans_trace(TRANS_DEBUG, "ob base= %llx", ax_dev->ob_base);
	trans_trace(TRANS_DEBUG, "ob size =  %llx", ax_dev->ob_size);
	trans_trace(TRANS_DEBUG, "ob base virt = %p", ax_dev->ob_base_virt);
#endif

	while (1) {
		*(volatile unsigned int *)ax_dev->shm_base_virt = DEVICE_CHECKED_FLAG;
#ifdef SHMEM_FROM_MASTER
		*(volatile unsigned long *)(ax_dev->shm_base_virt + 0x10) = ax_dev->ob_base;
		*(volatile unsigned long *)(ax_dev->shm_base_virt + 0x18) = ax_dev->ob_size;
#endif

		if (*(volatile unsigned int *)(ax_dev->shm_base_virt + 4) ==
		    DEVICE_HANDSHAKE_FLAG) {
			/* clear share memory last shandshake flag */
			memset32(ax_dev->shm_base_virt, 0, 8);
#ifndef SHMEM_FROM_MASTER
			trans_trace(TRANS_INFO, "[step0]: host check slave success\n");
#endif
			break;
		}

		if (0 == handshake_timeout--) {
			trans_trace(TRANS_ERR, "[step0]: Handshake timeout!");
			return -1;
		}
		trans_trace(TRANS_DEBUG, "[step0]: slave is not ready yet!, wait...");
		msleep(1);
	}
	return 0;
}

#ifdef SHMEM_FROM_MASTER
static int host_handshake_slave_step1(struct axera_dev *ax_dev)
{
	int handshake_timeout = 120000;

	while (1) {
		if (*(volatile unsigned int *)(ax_dev->shm_base_virt + 4) ==
		    DEVICE_HANDSHAKE_FLAG) {
			/* clear share memory last shandshake flag */
			memset32(ax_dev->shm_base_virt, 0, 8);
			trans_trace(TRANS_INFO, "[step1]: host check slave success\n");
			break;
		}

		if (0 == handshake_timeout--) {
			trans_trace(TRANS_ERR, "[step1]: Handshake timeout!");
			return -1;
		}
		trans_trace(TRANS_DEBUG, "step1: slave is not ready yet!, wait...");
		msleep(1);
	}
	smp_mb();
	*(volatile unsigned int *)ax_dev->shm_base_virt = DEVICE_CHECKED_FLAG;
	msleep(100);
	return 0;
}
#endif

static int host_check_slv(struct axera_dev *ax_dev)
{
	int ret;
	ret = host_handshake_slave_step0(ax_dev);
	if (ret < 0)
		return ret;

#ifdef SHMEM_FROM_MASTER
	ax_dev->shm_base = ax_dev->ob_base;
	ax_dev->shm_end = ax_dev->shm_base + ax_dev->ob_size;
	ax_dev->shm_size = ax_dev->ob_size;
	ax_dev->shm_base_virt = ax_dev->ob_base_virt;
	ax_dev->shm_end_virt = ax_dev->ob_base_virt + ax_dev->ob_size;
	trans_trace(TRANS_DEBUG, "ob base 0x%llx", ax_dev->shm_base);
	trans_trace(TRANS_DEBUG, "ob end 0x%llx", ax_dev->shm_end);
	trans_trace(TRANS_DEBUG, "shm base virt = 0x%p", ax_dev->shm_base_virt);
	trans_trace(TRANS_DEBUG, "shm end virt =  0x%p", ax_dev->shm_end_virt);
#endif
	ax_shared_mem_init(ax_dev);

#ifdef SHMEM_FROM_MASTER
	ret = host_handshake_slave_step1(ax_dev);
	if (ret < 0)
		return ret;
#endif

	return 0;
}

static int slave_handshake_host_step0(struct axera_dev *ax_dev)
{
	int handshake_timeout = 120000;
	ax_dev->shm_end = ax_dev->shm_base + ax_dev->shm_size;
	ax_dev->shm_base_virt = ax_dev->bar_info[BAR_0].addr;
	ax_dev->shm_end_virt = ax_dev->shm_base_virt + ax_dev->shm_size;
	trans_trace(TRANS_DEBUG, "shm base 0x%llx", ax_dev->shm_base);
	trans_trace(TRANS_DEBUG, "shm end 0x%llx", ax_dev->shm_end);

	while (1) {
		if (*(volatile unsigned int *)ax_dev->shm_base_virt ==
		    DEVICE_CHECKED_FLAG) {
			trans_trace(TRANS_INFO, "[step0]: slave check host success!");
			break;
		}

		if (0 == handshake_timeout--) {
			trans_trace(TRANS_ERR, "[step0]: Handshake timeout!");
			return -1;
		}
		trans_trace(TRANS_DEBUG, "[step0]: host is not ready yet!, wait...");
		msleep(1);
	}

#ifdef SHMEM_FROM_MASTER
	*(volatile unsigned int *)(ax_dev->shm_base_virt + 4) = DEVICE_HANDSHAKE_FLAG;
#endif
	return 0;
}

static void slave_handshake_host_step1(struct axera_dev *ax_dev)
{
#ifdef SHMEM_FROM_MASTER
	int handshake_timeout = 120000;
#endif

	*(volatile unsigned int *)(ax_dev->shm_base_virt + 4) = DEVICE_HANDSHAKE_FLAG;
#ifdef SHMEM_FROM_MASTER
	while (1) {
		if (*(volatile unsigned int *)ax_dev->shm_base_virt ==
		    DEVICE_CHECKED_FLAG) {
			/* clear share memory last shandshake flag */
			memset32(ax_dev->shm_base_virt, 0, 8);
			trans_trace(TRANS_INFO, "[step1]: slave check host success!");
			break;
		}

		if (0 == handshake_timeout--) {
			trans_trace(TRANS_ERR, "[step1]: Handshake timeout!");
			return;
		}
		trans_trace(TRANS_DEBUG, "[step1]: host is not ready yet!, wait...");
		msleep(1);
	}
#endif
}

static int slave_check_host(struct axera_dev *ax_dev)
{
	int ret;
	ax_dev->shm_end = ax_dev->shm_base + ax_dev->shm_size;
	trans_trace(TRANS_DEBUG, "shm base 0x%llx", ax_dev->shm_base);
	trans_trace(TRANS_DEBUG, "shm end 0x%llx", ax_dev->shm_end);

	ret = slave_handshake_host_step0(ax_dev);
	if (ret < 0)
		return ret;

#ifdef SHMEM_FROM_MASTER
	ax_dev->ob_base = *(volatile unsigned long *)(ax_dev->shm_base_virt + 0x10);
	ax_dev->ob_size = *(volatile unsigned long *)(ax_dev->shm_base_virt + 0x18);
	trans_trace(TRANS_DEBUG, "ob base 0x%llx", ax_dev->ob_base);
	trans_trace(TRANS_DEBUG, "ob end 0x%llx", ax_dev->ob_size);
	ax_dev->shm_base = ax_dev->ob_base;
	ax_dev->shm_size = ax_dev->ob_size;
	g_axera_pcie_opt->start_ob_map(PCIE_SPACE_SHMEM_BASE, ax_dev->shm_base, OUTBOUND_INDEX0, ax_dev->shm_size);
	ax_dev->shm_base_virt = ax_dev->ob_base_virt;
	ax_dev->shm_end_virt = ax_dev->ob_base_virt + ax_dev->shm_size;
#endif

	ax_shared_mem_init(ax_dev);

	slave_handshake_host_step1(ax_dev);

	return 0;
}

static void ax_pcie_shmem_kfifo_free(void *dev)
{
	struct axera_dev *ax_dev = (struct axera_dev *)dev;
	struct ax_pcie_shared_memory_map *pinfo = NULL;

	if (!g_axera_pcie_opt->is_host()) {
		pinfo = &ax_slave_info_map;
	} else {
		pinfo = &ax_host_info_map[ax_dev->slot_index];
	}

	pcie_kfifo_free(&pinfo->recv.shmem);
	pcie_kfifo_free(&pinfo->send.shmem);
}

static int ax_pcie_send_message(struct ax_transfer_handle *handle,
				const void *buf, unsigned int len)
{
	struct ax_pci_transfer_head head;
	struct axera_dev *dev;
	unsigned int retrys = 10;
	unsigned int retry_irq = 10;
	unsigned long flags;
	int ret = 0;
	int i = 0;

#ifdef MSG_PROC_ENABLE
	unsigned int *cur_cost;
	unsigned int *max_cost;
	unsigned int *cur_len;
	unsigned int *max_len;
	unsigned long *total_data;
	unsigned long *total_time;
	unsigned int *bandwidth;
	struct timespec64 msg_start;
	struct timespec64 msg_done;
#endif

	dev = g_axera_pcie_opt->slot_to_axdev(handle->target_id);
	if (!dev) {
		trans_trace(TRANS_ERR, "Target dev[0x%x] is NULL!",
			    handle->target_id);
		return -1;
	}

	memset(&head, 0, sizeof(struct ax_pci_transfer_head));
	head.target_id = handle->target_id;
	head.port = handle->port;
	head.slot = g_axera_pcie_opt->local_slotid;
	head.length = len;
	head.check = HEAD_CHECK_FINISHED;

	trans_trace(TRANS_DEBUG, "target 0x%x, slot 0x%x, port 0x%x",
		    head.target_id, head.slot, head.port);

#ifdef MSG_PROC_ENABLE
	if (pcie_proc.proc_level)
		ktime_get_ts64(&msg_start);
#endif
	while ((ret = ax_send_data_irq(dev, buf, len, &head)) < 0) {
		if (retrys == 0 || ret == -4) {
			trans_trace(TRANS_ERR, "ax send data failed %d\n", ret);
			break;
		}

		if (ret == -3) {
			while (1) {
				if (retry_irq == 0) {
					ret = len;
					break;
				}

				ax_pcie_data_lock(&flags);
				ret = g_axera_pcie_opt->trigger_msg_irq(dev, 0);
				if (ret >= 0) {
					if (g_axera_pcie_opt->is_host()) {
						*(volatile unsigned long long *)(dev->shm_base_virt + 0x20) += 1;
					}
					ret = len;
					ax_pcie_data_unlock(&flags);
					break;
				}
				ax_pcie_data_unlock(&flags);
				retry_irq--;
				msleep(1);
			}
			break;
		}
		retrys--;
		msleep(1);
	}

#ifdef MSG_PROC_ENABLE
	if (pcie_proc.proc_level) {
		ktime_get_ts64(&msg_done);
		for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
			if (g_axera_dev_map[i]->slot_index == handle->target_id)
				break;
		}
		((struct msg_number *)pcie_proc.msg_number.data)->target[i].
		    irq_send++;

		cur_cost =
		    &((struct msg_send_cost *)pcie_proc.msg_send_cost.data)->
		    cur_msg_cost;
		max_cost =
		    &((struct msg_send_cost *)pcie_proc.msg_send_cost.data)->
		    max_msg_cost;
		*cur_cost = get_time_cost_us(&msg_start, &msg_done);
		if ((*cur_cost > *max_cost) && (*cur_cost < 0x10000000))
			*max_cost = *cur_cost;

		cur_len =
		    &((struct msg_len *)pcie_proc.msg_send_len.data)->
		    cur_irq_send;
		max_len =
		    &((struct msg_len *)pcie_proc.msg_send_len.data)->
		    max_irq_send;
		*cur_len = len;
		if (*cur_len > *max_len)
			*max_len = *cur_len;

		/* send msg bandwidth count */
		total_data = &((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->total_data;
		total_time = &((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->total_time;
		bandwidth = &((struct msg_bandwidth *)pcie_proc.msg_bandwidth.data)->send_bandwidth;
		*total_data += len;
		*total_time += *cur_cost;
		*bandwidth = (*total_data *1000000) / *total_time / 1024 / 1024;
	}
#endif

	return ret;
}

int ax_pcie_msg_close(struct ax_transfer_handle *handle)
{
	struct axera_dev *ax_dev;

	if (NULL == handle) {
		trans_trace(TRANS_ERR, "The handler to close is NULL!");
		return -1;
	}

	ax_dev = g_axera_pcie_opt->slot_to_axdev(handle->target_id);
	ax_dev->ax_handle_table[handle->port] = 0;
	ax_dev->msg_checked_success = 0;
	kfree(handle);
	return 0;
}

EXPORT_SYMBOL(ax_pcie_msg_close);

struct ax_transfer_handle *find_device_handle(int target_id, int port)
{
	struct axera_dev *ax_dev;
	struct ax_transfer_handle *handle = NULL;

	if (port >= MAX_MSG_PORTS) {
		trans_trace(TRANS_ERR, "target 0x%x, port %d out of range",
			    target_id, port);
		return NULL;
	}

	ax_dev = g_axera_pcie_opt->slot_to_axdev(target_id);
	if (!ax_dev) {
		trans_trace(TRANS_ERR, "cannot find ax_dev of target_id 0x%x, port %d",
			    target_id, port);
		return NULL;
	}

	handle = ax_dev->ax_handle_table[port];

	return handle;
}

EXPORT_SYMBOL(find_device_handle);

void *ax_pcie_open(int target_id, int port)
{
	struct ax_transfer_handle *handle;
	struct axera_dev *ax_dev;

	if ((target_id >= MAX_DEV_NUMBER)
	    || (target_id == ax_pcie_msg_getlocalid())) {
		trans_trace(TRANS_ERR,
			    "Try to send message to yourself "
			    "or target_id[0x%x] out of range[0 - 0x%x].",
			    target_id, MAX_DEV_NUMBER);

		return NULL;
	}

	ax_dev = g_axera_pcie_opt->slot_to_axdev(target_id);
	if (NULL == ax_dev) {
		trans_trace(TRANS_ERR, "Try to open a non-exist dev[0x%x].",
			    target_id);
		return NULL;
	}

	if (port >= MAX_MSG_PORTS) {
		trans_trace(TRANS_ERR, "Port[0x%x] out of range[0 - 0x%x].",
			    port, MAX_MSG_PORTS);

		return NULL;
	}

	handle = find_device_handle(target_id, port);

	if (handle) {
		trans_trace(TRANS_ERR, "transfer handle[target:0x%x,port:0x%x]"
			    " already exists!", target_id, port);

		return handle;
	}

	if (in_interrupt()) {
		handle = kmalloc(sizeof(struct ax_transfer_handle), GFP_ATOMIC);
	} else {
		handle = kmalloc(sizeof(struct ax_transfer_handle), GFP_KERNEL);
	}

	if (handle == NULL) {
		trans_trace(TRANS_ERR, "kmalloc for message handle"
			    "[target:0x%x,port:0x%x] failed!", target_id, port);
		return NULL;
	}

	handle->target_id = target_id & MSG_DEV_BITS;
	handle->port = port & MSG_PORT_BITS;
	handle->msg_notifier_recvfrom = NULL;

	ax_dev->ax_handle_table[port] = handle;

	trans_trace(TRANS_INFO, "handler[target:0x%x,port:0x%x] open success!",
		    handle->target_id, handle->port);

	return handle;
}

EXPORT_SYMBOL(ax_pcie_open);

int ax_pcie_msg_send(struct ax_transfer_handle *handle,
		     const void *buf, unsigned int len)
{
	int ret;

	if (!buf) {
		trans_trace(TRANS_ERR, "Message to be sent is NULL!");
		return -1;
	}
	if (!handle) {
		trans_trace(TRANS_ERR, "Message handle is NULL!");
		return -1;
	}
	if (len >= AX_SHARED_SENDMEM_SIZE) {
		trans_trace(TRANS_ERR, "Data len out of range!");
		return -1;
	}

	ret = ax_pcie_send_message(handle, buf, len);;

	return ret;

}

EXPORT_SYMBOL(ax_pcie_msg_send);

int ax_pcie_msg_getlocalid(void)
{
	return g_axera_pcie_opt->local_slot_number();
}

EXPORT_SYMBOL(ax_pcie_msg_getlocalid);

int ax_pcie_msg_getremoteids(int ids[])
{
	int i = 0;

	if (g_axera_pcie_opt->is_host()) {
		for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
			ids[i] = g_axera_dev_map[i]->slot_index;
		}
	} else {
		ids[0] = 0;
		i = 1;
	}

	return i;
}

EXPORT_SYMBOL(ax_pcie_msg_getremoteids);

int ax_pcie_msg_check_remote(int remote_id)
{
	struct axera_dev *ax_dev;
	int ret = 0;

	trans_trace(TRANS_INFO, "## check device 0x%x", remote_id);

	ax_dev = g_axera_pcie_opt->slot_to_axdev(remote_id);
	if (!ax_dev) {
		trans_trace(TRANS_ERR, "Device[0x%x] does not exist!",
			    remote_id);
		return -1;
	}

	if (ax_dev->msg_checked_success == DEVICE_CHECKED_FLAG) {
		trans_trace(TRANS_ERR, "Device[0x%x] already checked!",
			    remote_id);
		return 0;
	}

	if (g_axera_pcie_opt->is_host())
		ret = host_check_slv(ax_dev);
	else
		ret = slave_check_host(ax_dev);

	if (!ret) {
		ax_dev->msg_checked_success = DEVICE_CHECKED_FLAG;
		ax_dev->kfifo_init_checked = DEVICE_KFIFO_INIT;
	}

	return ret;
}

EXPORT_SYMBOL(ax_pcie_msg_check_remote);

int ax_pcie_msg_data_recv(struct ax_transfer_handle *handle, void *dst,
			  unsigned int len)
{
	struct axera_dev *ax_dev;

	ax_dev = g_axera_pcie_opt->slot_to_axdev(handle->target_id);
	if (!ax_dev) {
		trans_trace(TRANS_ERR, "Device[0x%x] does not exist!",
			    handle->target_id);
		return -1;
	}

	return ax_pcie_kfifo_out(ax_dev, dst, len);
}

EXPORT_SYMBOL(ax_pcie_msg_data_recv);

void ax_pcie_msg_kfifo_free(void)
{
	struct axera_dev *ax_dev;
	int i;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		ax_dev = g_axera_dev_map[i];
		ax_pcie_shmem_kfifo_free(ax_dev);
	}
}

EXPORT_SYMBOL(ax_pcie_msg_kfifo_free);

void host_reset_device(int target_id)
{
	struct axera_dev *ax_dev;

	ax_dev = g_axera_pcie_opt->slot_to_axdev(target_id);
	if (!ax_dev) {
		trans_trace(TRANS_ERR, "Device[0x%x] does not exist!",
			    target_id);
		return;
	}
	ax_dev->msg_checked_success = 0;
	g_axera_pcie_opt->reset_device(ax_dev);
}

EXPORT_SYMBOL(host_reset_device);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
