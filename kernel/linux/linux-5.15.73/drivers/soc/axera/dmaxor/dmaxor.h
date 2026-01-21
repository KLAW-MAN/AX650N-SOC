/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_DMAXOR_H__
#define __AX_DMAXOR_H__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <asm/uaccess.h>
#include <linux/of_platform.h>
#include <linux/miscdevice.h>
#include <linux/poll.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/async_tx.h>

#include "../../../dma/axera-axi-dmaxor/axera-axi-dmaxor-platform.h"

#define MAX_DMA_CHAN 16
#define MAX_XOR_CNT 16

typedef struct {
	volatile unsigned char issync;
	unsigned long int pthread_id;
} ax_dmaxor_file_msg_t;

typedef struct {
	struct dma_chan *chan;
	ax_dmaxor_file_msg_t file_msg;
	wait_queue_head_t event_wait;
	struct mutex mutex_sync;
} ax_dmaxor_chn_t;

typedef struct {
	u64 xor_cali;
	s32 id;
	volatile u32 stat;
} ax_dmaxor_xfer_stat_t;

typedef struct {
	ax_dmaxor_xfer_stat_t xfer_stat;
	void (*pFun) (void *, void *);
	void *arg;
} ax_dmaxor_stat_t;

typedef struct {
	spinlock_t lock;
	struct proc_dir_entry *file;
	ax_dmaxor_xfer_stat_t xfer_stat;
	struct list_head dmaxor_start_list;
	struct list_head dmaxor_complite_list;
	struct list_head dmaxor_free_list;
} ax_dmaxor_info_t;

typedef struct {
	u32 init_val;
	u32 xfer_len;
	u64 dst_addr;
} ax_dmaxor_mem_chn_cfg_t;

typedef struct {
	u64 src_addr_buf[MAX_XOR_CNT];
	u64 dst_addr;
	u32 xfer_len;
	u8 src_cnt;
} ax_dmaxor_xor_chn_cfg_t;

typedef struct {
	u32 desc_num;
	void *dma_chn_cfg;
	void (*pFun) (void *, void *);
	void *arg;
	ax_dmaxor_xfer_mode_t dma_mode;
} ax_dmaxor_msg_t;

typedef struct {
	u8 chan_id;
	u32 size;
	u32 desc_num;
	u64 cali;
	u64 type;
	ktime_t time_start;
	ktime_t ktime_start;
	ktime_t ktime_run;
	ktime_t ktime_end;
	struct file *filp;
	struct dma_chan *chan;
	ax_dmaxor_msg_t dmaxor_msg;
	ax_dmaxor_xfer_stat_t xfer_stat;
	struct list_head element;
} ax_dmaxor_xfer_info_t;

typedef struct {
	u32 msg_id;
	u32 chan_id;
	u32 size;
	u64 start;
	u64 wait;
	u64 run;
	ax_dmaxor_xfer_mode_t dma_mode;
} ax_dmaxor_dbg_info_t;

typedef struct {
	u8 last;
	ax_dmaxor_dbg_info_t info[8];
	atomic64_t total;
	atomic64_t chan_total[MAX_DMA_CHAN];
} ax_dmaxor_dbg_t;

#define AX_DMAXOR_MAGIC			   ('x'+'o'+'r')
#define AX_DMAXOR_CFG_CHN_CMD		   _IOWR(AX_DMAXOR_MAGIC, 1, ax_dmaxor_msg_t)
#define AX_DMAXOR_START_CHN_CMD		   _IOWR(AX_DMAXOR_MAGIC, 2, unsigned int)
#define AX_DMAXOR_GET_CALLBACK		   _IOWR(AX_DMAXOR_MAGIC, 3, ax_dmaxor_msg_t)
#define AX_DMAXOR_XFER_CHN_CMD		   _IOWR(AX_DMAXOR_MAGIC, 4, ax_dmaxor_msg_t)

#define AXDMAXOR			   (0x4158444D41584F52)

#endif
