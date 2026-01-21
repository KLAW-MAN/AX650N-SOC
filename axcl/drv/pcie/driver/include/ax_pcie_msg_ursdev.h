/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_PCIE_MSG_URSDEV__
#define __AX_PCIE_MSG_URSDEV__

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/ioctl.h>
#include <linux/wait.h>
#include <linux/kern_levels.h>
#include <linux/ioctl.h>
#include "ax_pcie_msg_transfer.h"

#define MSG_USR_MODULE_NAME		"MSG_USR"
#define AXERA_MAX_MAP_DEV		0x100
#define NORMAL_MESSAGE_SIZE		128
#define MAXIMAL_MESSAGE_SIZE		0x1000

struct ax_pcie_msg_attr {
	int target_id;
	int port;
	int remote_id[AXERA_MAX_MAP_DEV];
};

#define	AX_IOC_MSG_BASE			'M'

#define AX_MSG_IOC_CONNECT		_IOW(AX_IOC_MSG_BASE, 1, struct ax_pcie_msg_attr)
#define AX_MSG_IOC_CHECK		_IOW(AX_IOC_MSG_BASE, 2, struct ax_pcie_msg_attr)
#define AX_MSG_IOC_GET_LOCAL_ID		_IOW(AX_IOC_MSG_BASE, 4, struct ax_pcie_msg_attr)
#define AX_MSG_IOC_GET_REMOTE_ID	_IOW(AX_IOC_MSG_BASE, 5, struct ax_pcie_msg_attr)
#define AX_MSG_IOC_ATTR_INIT		_IOW(AX_IOC_MSG_BASE, 6, struct ax_pcie_msg_attr)
#define AX_MSG_IOC_RESET_DEVICE		_IOW(AX_IOC_MSG_BASE, 7, struct ax_pcie_msg_attr)
#define AX_MSG_IOC_PCIE_STOP		_IOW(AX_IOC_MSG_BASE, 8, struct ax_pcie_msg_attr)

#define MSG_DEBUG	4
#define MSG_INFO	3
#define MSG_ERR		2
#define MSG_FATAL	1
#define MSG_DBG_LEVEL	2
#define msg_trace(level, s, params...) do { if (level <= MSG_DBG_LEVEL)\
	printk(KERN_INFO "[%s, %d]: " s "\n", __FUNCTION__, __LINE__, ##params);\
} while (0)

struct ax_pcie_msg_handle {
	unsigned long pci_handle;
	struct list_head mem_list;
	spinlock_t msg_lock;
	int stop_flag;
	atomic_t	event;
	wait_queue_head_t wait;
};
typedef struct ax_pcie_msg_handle ax_pcie_msg_handle_t;

#endif /* __AX_PCIE_MSG_URSDEV__ */
