/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_PCIE_MSG_TRANSFER__
#define __AX_PCIE_MSG_TRANSFER__

#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/kernel.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/kfifo.h>
#include "ax_pcie_msg_ursdev.h"

#define MAX_DEV_NUMBER			0x100
#define MAX_MSG_PORTS			0x80

#define MSG_DEV_BITS			(MAX_DEV_NUMBER - 1)
#define MSG_PORT_BITS			(MAX_MSG_PORTS - 1)

#define DEV_SHM_SIZE			(768 * 1024)
#define AX_SHARED_RECVMEM_SIZE        (384 * 1024)
#define AX_SHARED_SENDMEM_SIZE        (384 * 1024)
#define AX_SHM_IRQSEND_SIZE		(128 * 1024)
#define AX_SHM_IRQRECV_SIZE		(128 * 1024)

#define HEAD_SIZE (sizeof(struct ax_pci_transfer_head))
#define MEM_ALIAGN_CHECK (MEM_ALIAGN - HEAD_SIZE)

#define MEM_ALIAGN                     (32)	/* 32bytes aliagn */
#define MEM_ALIAGN_VERS                (MEM_ALIAGN - 1)

#define HEAD_CHECK_FINISHED		(0xA65)

#define TRANS_DEBUG	4
#define TRANS_INFO	3
#define TRANS_ERR	2
#define TRANS_FATAL	1
#define TRANS_DBG_LEVEL	2
#define trans_trace(level, s, params...) do { if (level <= TRANS_DBG_LEVEL)\
	printk("[%s, %d]: " s "\n", __FUNCTION__, __LINE__, ##params);\
} while (0)

typedef struct mailbox_msg {
	u32 src_target_id;
	u32 customer_data[7]; /* 32 bytes receive */
} mailbox_msg_t;

struct pcie_kfifo {
	unsigned long long	in;
	unsigned long long	out;
	unsigned long long	mask;
	void		*data;
};

struct ax_transfer_handle {
	unsigned int target_id;

	unsigned int port;
	unsigned int priority;

	int (*msg_notifier_recvfrom) (void *handle, void *buf,
				      unsigned int len);

	unsigned long data;
};

struct ax_pcie_shmem {
	volatile unsigned long int base_addr;
	volatile unsigned long int end_addr;
	volatile unsigned long int buf_len;

	volatile unsigned long int rp_addr;
	volatile unsigned long int wp_addr;
	struct pcie_kfifo *ax_pcie_kfifo;

	struct semaphore sem;
};

struct ax_pcie_shmem_info {
	struct ax_pcie_shmem shmem;
};

struct ax_pcie_shared_memory_map {
	struct ax_pcie_shmem_info recv;
	struct ax_pcie_shmem_info send;
};

struct ax_pci_transfer_head {
	volatile unsigned int target_id;
	volatile unsigned int slot;
	volatile unsigned int port;
	volatile unsigned int magic;
	volatile unsigned int length;
	volatile unsigned int check;
};

typedef int (*msg_notifier_recvfrom) (void *handle,
				      void *buf, unsigned int length);

struct ax_transfer_handle *find_device_handle(int target_id, int port);

extern void ax_pcie_msg_kfifo_free(void);
extern int ax_pcie_msg_data_recv(struct ax_transfer_handle *handle, void *dst,
				 unsigned int len);
extern int ax_pcie_msg_check_remote(int remote_id);
extern int ax_pcie_msg_getremoteids(int ids[]);
extern int ax_pcie_msg_getlocalid(void);
extern int ax_pcie_msg_send(struct ax_transfer_handle *handle, const void *buf,
			    unsigned int len);
extern int ax_pcie_msg_close(struct ax_transfer_handle *handle);
extern irqreturn_t host_message_irq_handler(int irq, void *id);
extern void slave_message_irq_handler(int fromid, int regno, int count);
extern void update_proc_rp_wp(void);
extern void *ax_pcie_open(int target_id, int port);
extern struct ax_transfer_handle *find_device_handle(int target_id, int port);
extern void host_reset_device(int target_id);
#endif
