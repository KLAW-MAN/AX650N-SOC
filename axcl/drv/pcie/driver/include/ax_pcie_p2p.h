/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_PCIE_P2P__
#define __AX_PCIE_P2P__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/kern_levels.h>
#include <linux/ioctl.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/kthread.h>
#include <linux/kfifo.h>

#define AXERA_MAX_MAP_DEV		0x100
#define NORMAL_MESSAGE_SIZE		128
#define MAXIMAL_MESSAGE_SIZE		0x1000
#define P2P_BIND_DATA_OFFSET		0x10
#define P2P_BIND0_OFFSET		0x00
#define P2P_BIND1_OFFSET		0x04
#define P2P_KFIFO_DATA_OFFSET		0x80
#define P2P_KFIFO_RP_OFFSET		0x40
#define P2P_KFIFO_WP_OFFSET		0x60
#define P2P_DEVICE_NUMBER_MAX		8
#define P2P_REGNO_INVALID		0xffffffff
#define P2P_BAR_MASK			0xFFFFFFF0
#define P2P_FIFO_MAX_CAPACITY		16
#define P2P_BAR1_SIZE			0x10000
#define P2P_BWT_MAGIC_DATA		0x56575859

#define AX_P2P_IOC_BASE			'P'
#define AX_P2P_IOC_CONFIG_SET		_IOW(AX_P2P_IOC_BASE, 1, p2p_device_config_t)
#define AX_P2P_IOC_CONFIG_GET		_IOR(AX_P2P_IOC_BASE, 2, p2p_device_config_t)
#define AX_P2P_IOC_START_BWT		_IOWR(AX_P2P_IOC_BASE, 3, p2p_bwt_infos_t)
#define AX_P2P_IOC_ITEM_SEND		_IOR(AX_P2P_IOC_BASE, 5, p2p_item_send_t)
#define AX_P2P_IOC_ITEM_GET		_IOR(AX_P2P_IOC_BASE, 6, p2p_item_get_t)
#define AX_P2P_IOC_DISABLE		_IO(AX_P2P_IOC_BASE, 7)
#define AX_P2P_IOC_CONNECT		_IOW(AX_P2P_IOC_BASE, 11, ax_pcie_p2p_attr_t)
#define AX_P2P_IOC_P2P_INIT		_IOW(AX_P2P_IOC_BASE, 12, ax_pcie_p2p_attr_t)
#define AX_P2P_IOC_PCIE_STOP		_IOW(AX_P2P_IOC_BASE, 13, ax_pcie_p2p_attr_t)
#define AX_P2P_IOC_GET_LOCAL_ID		_IOR(AX_P2P_IOC_BASE, 14, int)
#define AX_P2P_IOC_MAILBOX_OUTBOUND	_IOW(AX_P2P_IOC_BASE, 15, unsigned long long)

#define P2P_DEBUG	4
#define P2P_INFO	3
#define P2P_ERR		2
#define P2P_FATAL	1
#define P2P_DBG_LEVEL	2

#define p2p_trace(level, fmt, ...) do { if (level <= P2P_DBG_LEVEL)		    \
	printk(KERN_INFO "[%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);   \
} while (0)

#define p2p_info(fmt, ...) //printk(KERN_INFO "[I][%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);
#define p2p_err(fmt, ...) printk(KERN_ERR "[E][%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);

#define P2P_POINTER_CHECK(p)                        \
	do {	                                    \
		if (!p) {                           \
			p2p_err("null pointer");    \
			return -EINVAL;             \
		}                                   \
	 }  while (0)
#define P2P_RET_CHECK(_statement)                               \
	{                                                       \
		int _ret = _statement;                          \
		if (_ret < 0) {                                 \
			p2p_err("error result:%d", _ret);       \
			return _ret;                            \
		}                                               \
	}
#define P2P_RET_CHECK_V2(_statement)                            \
	{                                                       \
		int _ret = _statement;                          \
		if (_ret) {                                     \
			p2p_err("error result:%d", _ret);       \
			return -EINVAL;                         \
		}                                               \
	}

/*****************PUBLIC*****************/
typedef struct p2p_device_info {
	unsigned int target_id;			// EP bus number
	unsigned long long dma_pci_addr;	// dma pci address
	unsigned long long mbox_pci_addr;	// mailbox pci address
	unsigned long long cmm_phy_addr;	// cmm physical address
	unsigned long long cmm_size;		// cmm space size
} p2p_device_info_t;

typedef struct p2p_device_config {
	unsigned int device_num;		// device number
	p2p_device_info_t devices[P2P_DEVICE_NUMBER_MAX];
} p2p_device_config_t;

typedef struct p2p_bwt_info {
	unsigned int target_id;			// bus number of the target device
	unsigned int loop_count;		// test count
	unsigned int loop_interval;		// time between transmissions, us unit
	unsigned long chunk_size;		// size of the bytes sent, which is less than the allocated CMM size
} p2p_bwt_info_t;

typedef struct p2p_bwt_infos {
	unsigned int target_num;		// target device number
	p2p_bwt_info_t infos[P2P_DEVICE_NUMBER_MAX - 1];

	struct {
		unsigned long long max;
		unsigned long long min;
		unsigned long long avg;
	} result;
} p2p_bwt_infos_t;

typedef struct p2p_item_send {
	int timeout;
	u32 time_consume; 	/* unit is us */
	u64 src_phy_addr;
	u64 dst_phy_addr;
	u32 transfer_size;
	u32 dst_target_id; 	/* bus number */
	u32 customer_data[7]; 	/* 32 bytes send */
} p2p_item_send_t;

typedef struct p2p_item_get {
	mailbox_msg_t msg;
} p2p_item_get_t;

/*****************GLOBAL*****************/
typedef enum p2p_opt_status {
	P2P_OPT_DEINIT = 0,
	P2P_OPT_INIT,
	P2P_OPT_BWT_START,
} p2p_opt_status_e;

typedef struct p2p_item_send_pack {
	u8 device_index;
	p2p_item_send_t send;

	int timeout;
	int done;
	wait_queue_head_t wait;

	u64 start_tm;
	u64 end_tm;
} p2p_item_send_pack_t;

typedef struct p2p_ep_info {
	unsigned long bar1_virt; /* mailmox int, notify another ep */
	wait_queue_head_t usr_wait;

	p2p_device_config_t dev_cfg;
	DECLARE_KFIFO(p2p_in, mailbox_msg_t, P2P_FIFO_MAX_CAPACITY);
	DECLARE_KFIFO(p2p_out, p2p_item_get_t, P2P_FIFO_MAX_CAPACITY);
	bool hw_started;
	spinlock_t hw_lock;
	p2p_opt_status_e status;
	unsigned long long mbox_pci_addr; /* current mailbox pci address for outbound */
} p2p_ep_info_t;

#endif /* __AX_PCIE_P2P__ */
