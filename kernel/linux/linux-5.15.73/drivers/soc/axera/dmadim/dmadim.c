/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#define pr_fmt(fmt) KBUILD_MODNAME ":%s:%d: " fmt, __func__, __LINE__
#include "dmadim.h"
#include <linux/iopoll.h>

#define AX_DMADMA_DEVICE_NAME "ax_dmadim"
#define AX_DMADIM_PROC_NAME "ax_proc/dmadim"

static struct ax_dmadim_device *dmadim;
static s32 ax_dmadim_set_lli_msg(ax_dmadim_xfer_msg_t *dmadim_xfer_msg);
static void ax_dmadim_free_lli(ax_dmadim_xfer_msg_t *dmadim_xfer_msg);
static ax_dmadim_dbg_t g_dbg_buf;
static char *ax_dmadim_mode[6] = {
	"1D      ", "2D      ", "3D      ", "4D      ", "memset  ", "checksum"
};

static void kfree_set_null(void *p)
{
	if (!IS_ERR_OR_NULL(p))
		kfree(p);
	p = NULL;
}


/* need in lock id 1-64 */
static s32 ax_dmadim_get_id(struct ax_dmadim_device *dmadim, void *arg)
{
	int id;
	id = fls64(dmadim->id_pool);
	if (id > 0 && id < 65) {
		dmadim->id_pool &= (~BIT(id - 1));
		dmadim->id_buf[id - 1] = arg;
		return id;
	}
	return 0;
}

/* //need in lock */
static void * ax_dmadim_find_id(struct ax_dmadim_device *dmadim, u32 id)
{
	if (id > 0 && id < 65)
		return (void *)dmadim->id_buf[id - 1];
	else
		return NULL;
}

/* need in lock */
static void ax_dmadim_free_id(struct ax_dmadim_device *dmadim, s32 id)
{
	if (id < 1 || id > 64)
		return ;
	dmadim->id_pool |= (BIT(id - 1));
	dmadim->id_buf[id - 1] = NULL;
}

static u32 ax_dmadim_read(struct ax_dmadim_device *dmadim, u32 reg)
{
	return readl(dmadim->base + reg);
}

static void ax_dmadim_write(struct ax_dmadim_device *dmadim, u32 reg, u32 val)
{
	writel(val, dmadim->base + reg);
}

static void ax_dmadim_rmw(struct ax_dmadim_device *dmadim, u32 reg,
			  u32 set, u32 mask, u32 shift)
{
	u32 val;

	val = ax_dmadim_read(dmadim, reg);
	val &= ~(mask << shift);
	val |= (set << shift);
	ax_dmadim_write(dmadim, reg, val);
}


static int ax_dmadim_suspend(struct ax_dmadim_device *dmadim)
{
	u32 ret, val = 0;
	ax_dmadim_rmw(dmadim, DMADIM_CTRL, 1, DMADIM_CTRL_SUSPEND_MASK,
		      DMADIM_CTRL_SUSPEND_SHIFT);
	ret = readl_poll_timeout_atomic(dmadim->base + DMADIM_STA, val,
					val & DMADIM_SUSPEND_STA, 100, 10000);//timeout 10ms
	if (ret == -ETIMEDOUT) {
		pr_err("ETIMEDOUT\n");
	}
	if (ax_dmadim_read(dmadim, DMADIM_STA) & DMADIM_SUSPEND_STA)
		return 0;
	return ret;
}

static void ax_dmadim_suspend_clr(struct ax_dmadim_device *dmadim)
{
	ax_dmadim_write(dmadim, DMADIM_CLEAR, 1);
	return ;
}

static void ax_dmadim_hw_enable(void)
{
	reset_control_deassert(dmadim->rst);
	clk_prepare_enable(dmadim->core_clk);
}

static void ax_dmadim_hw_disable(void)
{
	reset_control_assert(dmadim->rst);
	clk_disable_unprepare(dmadim->core_clk);
}

static int __maybe_unused ax_dmadim_pm_suspend(struct device *dev)
{
	ax_dmadim_suspend(dmadim);
	ax_dmadim_suspend_clr(dmadim);
	ax_dmadim_hw_disable();
	return 0;
}

static int __maybe_unused ax_dmadim_pm_resume(struct device *dev)
{
	ax_dmadim_hw_enable();
	return 0;
}

static void ax_dmadim_set_base_lli(u64 lli_base)
{
	ax_dmadim_rmw(dmadim, DMADIM_CTRL, 1, DMADIM_CTRL_TYPE_MASK,
		      DMADIM_CTRL_TYPE_SHIFT);
	ax_dmadim_write(dmadim, DMADIM_LLI_BASE_H,
			(u32) ((u64) lli_base >> 32));
	ax_dmadim_write(dmadim, DMADIM_LLI_BASE_L,
			(u32) ((u64) lli_base & 0xFFFFFFFF));
}

static void ax_dmadim_enable_irq(struct ax_dmadim_device *dmadim, bool en)
{
	u64 tmp;
	u64 hwch = 0;
	u64 irq_mask = 0x3D;

	tmp = ax_dmadim_read(dmadim, DMADIM_INT_GLB_MASK);
	if (en) {
		tmp |= BIT(hwch);
		writel(tmp, dmadim->base + DMADIM_INT_GLB_MASK);
		writel(irq_mask, dmadim->base + DMADIM_INT_MASK(hwch));
	} else {
		tmp &= ~BIT(hwch);
		writel(tmp, dmadim->base + DMADIM_INT_GLB_MASK);
		writel(0x0, dmadim->base + DMADIM_INT_MASK(hwch));
	}
}

static inline bool ax_chan_hw_is_enable(struct ax_dmadim_device *dmadim)
{
	return ! !ax_dmadim_read(dmadim, DMADIM_INT_GLB_MASK);
}

static void ax_dmadim_disable_all_irq(struct ax_dmadim_device *dmadim)
{
	u64 i;

	writel(0x0, dmadim->base + DMADIM_INT_GLB_MASK);
	for (i = 0; i < 16; i++) {
		writel(0x0, dmadim->base + DMADIM_INT_MASK(i));
	}
}

static void ax_dmadim_start(ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	if (ax_chan_hw_is_enable(dmadim))
		return;
	dmadim_xfer_msg->xfer_stat.stat = DMADIM_XFER_RUNNING;
	dmadim_xfer_msg->ktime_run = ktime_get();
	ax_dmadim_set_base_lli(dmadim_xfer_msg->xfer.lli_base);
	ax_dmadim_enable_irq(dmadim, 1);
	mb();
	ax_dmadim_write(dmadim, DMADIM_START, 1);
	mb();
	ax_dmadim_write(dmadim, DMADIM_TRIG, 0);
	mb();
	return;
}

u32 ax_dmadim_get_checksum(struct ax_dmadim_device *dmadim)
{
	return ax_dmadim_read(dmadim, DMADIM_CHECKSUM);
}

static s32 ax_dmadim_stat_show(struct seq_file *m, void *v)
{
	s32 i, count;

	seq_printf(m, "The number of user: %d\n",
		   atomic_read(&g_dbg_buf.chan_use));
	seq_printf(m, "The number of all tasks so far: %lld\n",
		   atomic64_read(&g_dbg_buf.total));
	seq_printf(m,
		   "The number of tasks that still need to be processed: %lld\n",
		   atomic64_read(&g_dbg_buf.current_total));
	seq_printf(m, "\n");
	seq_printf(m, "type     \tmsg-id        \tsize           \t"
		   "start          \twait(ns)       \trun(ns)        \t\n");
	for (i = 0; i < 8; i++) {
		count = (g_dbg_buf.last + i + 1) & 0x7;
		if (!g_dbg_buf.info[count].size)
			continue;
		seq_printf(m,
			   "%s\t%-2d         \t%-15d\t%-15llu\t%-15llu\t%-15llu\t\n",
			   ax_dmadim_mode[g_dbg_buf.info[count].dma_mode -
					  AX_DMADIM_MODE_OFFSET],
			   g_dbg_buf.info[count].msg_id,
			   g_dbg_buf.info[count].size,
			   g_dbg_buf.info[count].start,
			   g_dbg_buf.info[count].wait,
			   g_dbg_buf.info[count].run);
	}
	return 0;
}

static s32 ax_dmadim_stat_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_dmadim_stat_show, NULL);
}

static const struct proc_ops ax_dmadim_stat_ops = {
	.proc_open = ax_dmadim_stat_open,
	.proc_read = seq_read,
	.proc_release = seq_release,
};

static s32 ax_dmadim_proc_init(struct ax_dmadim_device *dmadim)
{
	struct proc_dir_entry *p = NULL;
	p = proc_create(AX_DMADIM_PROC_NAME, 0644, NULL, &ax_dmadim_stat_ops);
	if (unlikely(!p)) {
		pr_err("err: Create proc fail!\n");
		return -EFAULT;
	}
	return 0;
}

void ax_dmadim_work_handler(struct work_struct *work)
{
	int ret = 0;
	unsigned long flags;
	ax_dmadim_xfer_msg_t *__dmadim_xfer_msg = NULL;
	ax_dmadim_work_data_t *work_data = container_of(work, ax_dmadim_work_data_t, work);
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = work_data->dmadim_xfer_msg;

	dmadim_xfer_msg->dma_callback(dmadim_xfer_msg->arg,
					dmadim_xfer_msg->xfer_stat.stat);
	ret = ax_dmadim_ioctl(dmadim_xfer_msg->filp,
				AXI_DMADIM_FREE_MSG_BY_ID,
				dmadim_xfer_msg->xfer_stat.id);
	if (ret) {
		pr_err("AXI_DMADIM_FREE_MSG_BY_ID err\n");
	}
	ax_dmadim_release(work_data->inode, work_data->filp);
	kfree_set_null(work_data->filp);
	kfree_set_null(work_data->inode);

	spin_lock_irqsave(&dmadim->lock, flags);
	__dmadim_xfer_msg =
	    list_first_entry_or_null(&dmadim->dmadim_pending_list,
				     ax_dmadim_xfer_msg_t, xfer_node);
	ax_dmadim_enable_irq(dmadim, 0);
	if (__dmadim_xfer_msg) {
		ax_dmadim_start(__dmadim_xfer_msg);
	}
	spin_unlock_irqrestore(&dmadim->lock, flags);

	return ;
}

static irqreturn_t ax_dmadim_interrupt(s32 irq, void *dev_id)
{
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL;
	ax_dmadim_xfer_msg_t *__dmadim_xfer_msg = NULL;
	ax_dmadim_chn_t *chn = NULL;
	unsigned long flags;

	spin_lock_irqsave(&dmadim->lock, flags);
	dmadim->irq_stat = ax_dmadim_read(dmadim, DMADIM_INT_STA(0));
	pr_debug("raw star %d\n", ax_dmadim_read(dmadim, DMADIM_INT_RAW(0)));
	ax_dmadim_write(dmadim, DMADIM_INT_CLR(0), 0x3F);
	dmadim_xfer_msg =
	    list_first_entry_or_null(&dmadim->dmadim_pending_list,
				     ax_dmadim_xfer_msg_t, xfer_node);
	if (!dmadim_xfer_msg) {
		spin_unlock_irqrestore(&dmadim->lock, flags);
		pr_debug("err: raw star %d\n", dmadim->irq_stat);
		return IRQ_HANDLED;
	}

	if (!(dmadim_xfer_msg->xfer_stat.stat & DMADIM_XFER_RUNNING)) {
		spin_unlock_irqrestore(&dmadim->lock, flags);
		return IRQ_HANDLED;
	}

	if (dmadim->irq_stat & DMADIM_AXI_WE_ERR) {
		dmadim_xfer_msg->xfer_stat.stat = DMADIM_AXI_WE_ERR;
	} else if (dmadim->irq_stat & DMADIM_AXI_RD_ERR) {
		dmadim_xfer_msg->xfer_stat.stat = DMADIM_AXI_RD_ERR;
	} else if (dmadim->irq_stat & DMADIM_XFER_SUCCESS) {
		dmadim_xfer_msg->xfer_stat.stat = DMADIM_XFER_SUCCESS;
	}
	if (dmadim_xfer_msg->xfer.dma_mode == AX_DMADIM_CHECKSUM)
		dmadim_xfer_msg->xfer_stat.checksum =
		    ax_dmadim_get_checksum(dmadim);
	if (dmadim_xfer_msg->xfer.dma_mode != AX_DMADIM_CHECKSUM) {
		list_move_tail(&dmadim_xfer_msg->xfer_node,
			       &dmadim->dmadim_complite_list);
		chn = (ax_dmadim_chn_t *)dmadim_xfer_msg->filp->private_data;

		if (dmadim_xfer_msg->kuser) {
			list_move_tail(&dmadim_xfer_msg->xfer_node,
			       &dmadim->dmadim_free_list);
			if (!IS_ERR_OR_NULL(dmadim_xfer_msg->dma_callback)) {
				dmadim->work_data.inode = chn->inode;
				dmadim->work_data.filp = dmadim_xfer_msg->filp;
				dmadim->work_data.dmadim_xfer_msg = dmadim_xfer_msg;
				schedule_work(&dmadim->work_data.work);
				goto end;
			} else {
				complete(&dmadim->complete);
			}
		} else {
			wake_up_interruptible(&chn->event_wait);
		}
		atomic64_dec(&g_dbg_buf.current_total);
	} else {
		list_move_tail(&dmadim_xfer_msg->xfer_node,
			       &dmadim->dmadim_checksum_complite_list);
		__dmadim_xfer_msg =
		    (ax_dmadim_xfer_msg_t *) ax_dmadim_find_id(dmadim, dmadim_xfer_msg->xfer_stat.id);
		chn = (ax_dmadim_chn_t *) __dmadim_xfer_msg-> filp->private_data;
		if (!(--__dmadim_xfer_msg->checksum_num)) {
			list_move_tail(&__dmadim_xfer_msg->xfer_node,
				       &dmadim->dmadim_complite_list);
			atomic64_dec(&g_dbg_buf.current_total);
			wake_up_interruptible(&chn->event_wait);
		}
	}
	__dmadim_xfer_msg =
	    list_first_entry_or_null(&dmadim->dmadim_pending_list,
				     ax_dmadim_xfer_msg_t, xfer_node);
	ax_dmadim_enable_irq(dmadim, 0);
	if (__dmadim_xfer_msg) {
		ax_dmadim_start(__dmadim_xfer_msg);
	}
end:
	spin_unlock_irqrestore(&dmadim->lock, flags);

	return IRQ_HANDLED;
}

static u32 ax_dmadim_get_xfer_width(size_t size)
{
	s32 max_width = 3;
	return __ffs(size | BIT(max_width));
}

static void
ax_dmadim_set_ctrl_data(unsigned long data, unsigned long long *ctrl_data,
			unsigned int shift, unsigned long mask)
{
	*ctrl_data = ((*ctrl_data) & (~(mask << shift))) | (data << shift);
}

static void ax_dmadim_set_lli_ctrl_data(ax_dmadim_xfer_msg_t *dmadim_xfer_msg,
					u64 *ctrl_data,
					ax_dmadim_lli_ctrl_t lli_ctrl)
{
	*ctrl_data = AX_DMADIM_DEFAULT_CTRL;
	if(dmadim_xfer_msg->kuser) {
		ax_dmadim_set_ctrl_data(0, ctrl_data, AWLEN_SHIFT, AWLEN_MASK);
		ax_dmadim_set_ctrl_data(0, ctrl_data, ARLEN_SHIFT, ARLEN_MASK);
	}
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_dst_tr_width, ctrl_data,
				DST_TR_WIDTH_SHIFT, DST_TR_WIDTH_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_src_tr_width, ctrl_data,
				SRC_TR_WIDTH_SHIFT, SRC_TR_WIDTH_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_wb, ctrl_data, WB_SHIFT, WB_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_endian, ctrl_data,
				ENDIAN_SHIFT, ENDIAN_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_chksum, ctrl_data,
				CHKSUM_SHIFT, CHKSUM_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_xor, ctrl_data,
				XOR_NUM_SHIFT, XOR_NUM_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_type, ctrl_data,
				TYPE_SHIFT, TYPE_MASK);
	ax_dmadim_set_ctrl_data(lli_ctrl.lli_last, ctrl_data,
				LAST_SHIFT, LAST_MASK);
}

static u32 ax_dmadim_get_dim_size(ax_dmadim_xfer_msg_t *dmadim_xfer_msg,
			       ax_dmadim_lli_t *lli_list, u32 dma_mode)
{
	u32 size;

	size = lli_list->lli_addr_vir->src_imgw <<
	    lli_list->lli_ctrl.lli_src_tr_width;
	size *= lli_list->lli_addr_vir->src_ntile1;
	if (dma_mode > AX_DMADIM_2D)
		size *= lli_list->lli_addr_vir->src_ntile2;
	if (dma_mode > AX_DMADIM_3D)
		size *= lli_list->lli_addr_vir->src_ntile3;

	return size;
}

static s32 ax_dmadim_lli_write(ax_dmadim_xfer_msg_t *dmadim_xfer_msg,
			       ax_dmadim_lli_t *lli_list,
			       u8 *p_msg_lli, ax_dmadim_xfer_t *xfer)
{
	u32 src_imgw, dst_imgw;
	u64 size, size_width;
	u32 dma_mode = xfer->dma_mode;
	ax_dmadim_desc_t *p_msg_lli_data = (ax_dmadim_desc_t *)p_msg_lli;
	ax_dmadim_xd_desc_t *p_dim_msg_lli_data =
	    (ax_dmadim_xd_desc_t *) p_msg_lli;
	ax_dmadim_lli_pool_t *lli_pool = NULL;
	unsigned long flags;

	lli_pool = (void *)kzalloc(sizeof(ax_dmadim_lli_pool_t), GFP_KERNEL);
	lli_pool->lli_addr = lli_list->lli_addr;
	lli_pool->lli_addr_vir = lli_list->lli_addr_vir;
	spin_lock_irqsave(&dmadim->lock, flags);
	list_add_tail(&lli_pool->lli_node, &dmadim_xfer_msg->dmadim_pool_list);
	spin_unlock_irqrestore(&dmadim->lock, flags);
	lli_list->lli_addr_vir->llp = lli_list->lli_addr_next;
	lli_list->lli_ctrl.lli_type =
	    (dma_mode == AX_DMADIM_CHECKSUM) ? AX_DMADIM_1D : dma_mode;
	lli_list->lli_ctrl.lli_endian = xfer->endian;
	if (lli_list->lli_addr_vir_next == NULL) {
		lli_list->lli_ctrl.lli_last = 1;
	} else {
		lli_list->lli_ctrl.lli_last = 0;
	}
	lli_list->lli_ctrl.lli_wb = 1;
	lli_list->lli_ctrl.lli_chksum = 0;

	if (dma_mode > AX_DMADIM_1D && dma_mode < AX_DMADIM_MEMORY_INIT) {
		src_imgw = p_dim_msg_lli_data->src_info.imgw;
		dst_imgw = p_dim_msg_lli_data->dst_info.imgw;

		lli_list->lli_addr_vir->src =
		    p_dim_msg_lli_data->src_info.paddr;
		lli_list->lli_addr_vir->dst =
		    p_dim_msg_lli_data->dst_info.paddr;

		lli_list->lli_addr_vir->src_stride1 =
		    (u32) (p_dim_msg_lli_data->src_info.stride[0]);
		lli_list->lli_addr_vir->dst_stride1 =
		    (u32) (p_dim_msg_lli_data->dst_info.stride[0]);
		lli_list->lli_addr_vir->src_stride2 =
		    (u32) (p_dim_msg_lli_data->src_info.stride[1]);
		lli_list->lli_addr_vir->dst_stride2 =
		    (u32) (p_dim_msg_lli_data->dst_info.stride[1]);
		lli_list->lli_addr_vir->src_stride3 =
		    (u32) (p_dim_msg_lli_data->src_info.stride[2]);
		lli_list->lli_addr_vir->dst_stride3 =
		    (u32) (p_dim_msg_lli_data->dst_info.stride[2]);
		lli_list->lli_addr_vir->src_ntile1 =
		    (u16) (p_dim_msg_lli_data->ntiles[0]);
		lli_list->lli_addr_vir->dst_ntile1 = src_imgw *
		    lli_list->lli_addr_vir->src_ntile1 / dst_imgw;
		lli_list->lli_addr_vir->src_ntile2 =
		    lli_list->lli_addr_vir->dst_ntile2 =
		    (u16) (p_dim_msg_lli_data->ntiles[1]);
		lli_list->lli_addr_vir->src_ntile3 =
		    lli_list->lli_addr_vir->dst_ntile3 =
		    (u16) (p_dim_msg_lli_data->ntiles[2]);

		lli_list->lli_ctrl.lli_src_tr_width =
		    ax_dmadim_get_xfer_width(src_imgw);

		lli_list->lli_ctrl.lli_dst_tr_width =
		    ax_dmadim_get_xfer_width(dst_imgw);

		lli_list->lli_addr_vir->src_imgw =
		    (u16) (p_dim_msg_lli_data->src_info.imgw >>
			   lli_list->lli_ctrl.lli_src_tr_width);

		lli_list->lli_addr_vir->dst_imgw =
		    (u16) (p_dim_msg_lli_data->dst_info.imgw >>
			   lli_list->lli_ctrl.lli_dst_tr_width);
		if(!lli_list->lli_addr_vir->src_imgw || !lli_list->lli_addr_vir->dst_imgw) {
			pr_err("err\n");
			return -EINVAL;
		}
		dmadim_xfer_msg->size = ax_dmadim_get_dim_size(dmadim_xfer_msg,
							       lli_list,
							       xfer->dma_mode);
	} else {
		if (dma_mode == AX_DMADIM_CHECKSUM) {
			dmadim_xfer_msg->size = p_msg_lli_data->size;
			//chacksum src addr align 32
			if (p_msg_lli_data->src & 0x3) {
				pr_err("err\n");
				return -EINVAL;
			}
			lli_list->lli_ctrl.lli_wb = 0;
			lli_list->lli_ctrl.lli_chksum = 1;
		}
		if (dma_mode == AX_DMADIM_MEMORY_INIT &&
		    (!p_msg_lli_data->size || p_msg_lli_data->size & 0x7)) {
			pr_err("err\n");
			return -EINVAL;
		}
		lli_list->lli_addr_vir->src = p_msg_lli_data->src;
		lli_list->lli_addr_vir->dst = p_msg_lli_data->dst;
		size = p_msg_lli_data->size;
		lli_list->lli_ctrl.lli_src_tr_width =
		    lli_list->lli_ctrl.lli_dst_tr_width =
		    ax_dmadim_get_xfer_width(size);
		lli_list->lli_addr_vir->size =
		    size >> lli_list->lli_ctrl.lli_src_tr_width;
	}
	//detect endian
	if (dma_mode < AX_DMADIM_MEMORY_INIT) {
		if (dma_mode != AX_DMADIM_1D)
			size = src_imgw * lli_list->lli_addr_vir->src_ntile1;
		size_width = ax_dmadim_get_xfer_width(size);
		if (lli_list->lli_ctrl.lli_endian == DMADIM_ENDIAN_16B) {
			if (size_width < DMADIM_TR_2B) {
				pr_err("err: detect endian fail\n");
				return -EINVAL;
			}
		} else if (lli_list->lli_ctrl.lli_endian == DMADIM_ENDIAN_32B) {
			if (size_width < DMADIM_TR_4B) {
				pr_err("err: detect endian fail\n");
				return -EINVAL;
			}
		}
	}
	ax_dmadim_set_lli_ctrl_data(dmadim_xfer_msg,
				    &lli_list->lli_addr_vir->ctrl,
				    lli_list->lli_ctrl);
	return 0;
}

static s32 ax_dmadim_set_lli_ctrl(ax_dmadim_xfer_msg_t *dmadim_xfer_msg,
				  ax_dmadim_lli_t *lli_list, u8 *p_msg_lli,
				  ax_dmadim_xfer_t *xfer)
{
	s32 ret;
	u32 step_size = 0;
	u32 tmp_size = 0;
	u32 dma_mode = xfer->dma_mode;
	ulong flags;
	ax_dmadim_lli_t tmp_lli_list;
	ax_dmadim_desc_t *p_msg_lli_data = (ax_dmadim_desc_t *) p_msg_lli;
	if (dma_mode == DMADIM_1D || dma_mode == DMADIM_MEMORY_INIT) {
		memcpy(&tmp_lli_list, lli_list, sizeof(ax_dmadim_lli_t));
		dmadim_xfer_msg->size += p_msg_lli_data->size;
		while (1) {
			tmp_size = p_msg_lli_data->size;
			if (p_msg_lli_data->size >= DMADIM_TR_4B_MAXSIZE) {
				step_size = DMADIM_TR_4B_MAXSIZE;
			} else if (p_msg_lli_data->size >= DMADIM_TR_2B_MAXSIZE) {
				step_size = DMADIM_TR_2B_MAXSIZE;
			} else if (p_msg_lli_data->size >= DMADIM_TR_1B_MAXSIZE) {
				step_size = DMADIM_TR_1B_MAXSIZE;
			} else {
				step_size = p_msg_lli_data->size;
			}
			if (p_msg_lli_data->size == step_size) {
				return ax_dmadim_lli_write(dmadim_xfer_msg,
							   lli_list, p_msg_lli,
							   xfer);
			}
			p_msg_lli_data->size = step_size;
			if (p_msg_lli_data->size) {
				lli_list->lli_addr_vir_next =
				    dma_pool_zalloc(dmadim->dmadim_pool,
						    GFP_KERNEL,
						    &lli_list->lli_addr_next);
				if (unlikely(!lli_list->lli_addr_vir_next)) {
					spin_lock_irqsave(&dmadim->lock, flags);
					ax_dmadim_free_lli(dmadim_xfer_msg);
					spin_unlock_irqrestore(&dmadim->lock, flags);
					pr_err("err\n");
					return -ENOMEM;
				}

				ret =
				    ax_dmadim_lli_write(dmadim_xfer_msg,
							lli_list, p_msg_lli,
							xfer);
				if (ret) {
					pr_err("err\n");
					return -EINVAL;
				}

				lli_list->lli_addr = lli_list->lli_addr_next;
				lli_list->lli_addr_vir =
				    lli_list->lli_addr_vir_next;
				lli_list->lli_addr_next =
				    tmp_lli_list.lli_addr_next;
				lli_list->lli_addr_vir_next =
				    tmp_lli_list.lli_addr_vir_next;
			}

			p_msg_lli_data->size = tmp_size - step_size;
			p_msg_lli_data->dst += step_size;
			if (dma_mode != DMADIM_MEMORY_INIT)
				p_msg_lli_data->src += step_size;
		}
		return 0;
	} else {
		return ax_dmadim_lli_write(dmadim_xfer_msg, lli_list, p_msg_lli,
					   xfer);
	}
}

static void ax_dmadim_free_xfer_buf(ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	kfree_set_null(dmadim_xfer_msg->dmadim_msg.msg_lli);
	kfree_set_null(dmadim_xfer_msg);
}

static void ax_dmadim_free_xfer_msg(ax_dmadim_xfer_msg_t *dmadim_xfer_msg,
				    bool free_id)
{
	ax_dmadim_free_lli(dmadim_xfer_msg);
	if (free_id)
		ax_dmadim_free_id(dmadim, dmadim_xfer_msg->xfer_stat.id);
	ax_dmadim_free_xfer_buf(dmadim_xfer_msg);
}

static s32 ax_dmadim_check_args(ax_dmadim_msg_t *dmadim_msg)
{
	s32 i;
	ax_dmadim_desc_t *msg_data = (ax_dmadim_desc_t *) dmadim_msg->msg_lli;
	ax_dmadim_xd_desc_t *msg_dim_data =
	    (ax_dmadim_xd_desc_t *) dmadim_msg->msg_lli;

	if (dmadim_msg->lli_num < 1) {
		return -EINVAL;
	}
	switch (dmadim_msg->dma_mode) {
	case AX_DMADIM_4D:
	case AX_DMADIM_3D:
	case AX_DMADIM_2D:
		for (i = 0; i < dmadim_msg->lli_num; i++) {
			if (!msg_dim_data->src_info.paddr ||
			    !msg_dim_data->dst_info.paddr ||
			    !msg_dim_data->src_info.imgw ||
			    !msg_dim_data->dst_info.imgw ||
			    msg_dim_data->src_info.imgw >
			    msg_dim_data->src_info.stride[0] ||
			    msg_dim_data->dst_info.imgw >
			    msg_dim_data->dst_info.stride[0]) {
				pr_err("err\n");
				return -EINVAL;
			}
			msg_dim_data++;
		}
		break;
	case AX_DMADIM_1D:
		for (i = 0; i < dmadim_msg->lli_num; i++) {
			if (!msg_data->size || !msg_data->src || !msg_data->dst) {
				pr_err("err\n");
				return -EINVAL;
			}
			msg_data++;
		}
		break;
	case AX_DMADIM_CHECKSUM:
		for (i = 0; i < dmadim_msg->lli_num; i++) {
			if (!msg_data->size || !msg_data->src) {
				pr_err("err\n");
				return -EINVAL;
			}
			msg_data++;
		}
		break;
	case AX_DMADIM_MEMORY_INIT:
		for (i = 0; i < dmadim_msg->lli_num; i++) {
			if (!msg_data->size || !msg_data->dst) {
				pr_err("err\n");
				return -EINVAL;
			}
			msg_data++;
		}
		break;
	}
	return 0;
}

static void ax_dmadim_free_lli(ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	ax_dmadim_xfer_t *xfer = &dmadim_xfer_msg->xfer;
	ax_dmadim_lli_pool_t *lli_pool = NULL, *_lli_pool = NULL;

	if (list_empty(&dmadim_xfer_msg->dmadim_pool_list))
		return;
	list_for_each_entry_safe(lli_pool, _lli_pool,
				 &dmadim_xfer_msg->dmadim_pool_list, lli_node) {
		if (lli_pool) {
			dma_pool_free(dmadim->dmadim_pool,
				      lli_pool->lli_addr_vir,
				      lli_pool->lli_addr);
			list_del(&lli_pool->lli_node);
			kfree_set_null(lli_pool);
		}
	}
	kfree_set_null(xfer->lli_list);
	return;
}

static s32 ax_dmadim_set_lli_msg(ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	s32 i, ret = 0;
	ulong flags;
	ax_dmadim_msg_t *dmadim_msg = &dmadim_xfer_msg->dmadim_msg;
	ax_dmadim_xfer_t *xfer = &dmadim_xfer_msg->xfer;
	ax_dmadim_lli_t *lli_list = NULL;
	u8 *p_msg_lli = (u8 *) dmadim_msg->msg_lli;
	xfer->endian = dmadim_msg->endian;
	xfer->dma_mode = dmadim_msg->dma_mode;
	xfer->ch = 0;
	xfer->lli_list =
	    (ax_dmadim_lli_t *) kzalloc(sizeof(ax_dmadim_lli_t) *
					dmadim_msg->lli_num, GFP_KERNEL);
	if (unlikely(!xfer->lli_list)) {
		pr_err("err\n");
		return -ENOMEM;
	}
	lli_list = xfer->lli_list;
	lli_list->lli_addr_vir =
	    (ax_dmadim_lli_reg_t *) dma_pool_zalloc(dmadim->dmadim_pool,
						    GFP_KERNEL,
						    &lli_list->lli_addr);
	if (unlikely(!lli_list->lli_addr_vir)) {
		pr_err("err\n");
		goto err_lli_list;
	}

	xfer->lli_base = lli_list->lli_addr - 128 * (xfer->ch);
	xfer->lli_base_vir = (u64) (lli_list->lli_addr_vir) - 128 * (xfer->ch);
	for (i = 0; i < dmadim_msg->lli_num; i++) {
		if (dmadim_msg->lli_num - i == 1) {
			lli_list->lli_addr_vir_next = NULL;
			lli_list->lli_addr_next = 0;
		} else {
			lli_list->lli_addr_vir_next =
			    dma_pool_zalloc(dmadim->dmadim_pool, GFP_KERNEL,
					    &lli_list->lli_addr_next);
			if (unlikely(!lli_list->lli_addr_vir_next)) {
				pr_err("err\n");
				ret = -EINVAL;
				goto err_lli_addr;
			}
		}
		ret =
		    ax_dmadim_set_lli_ctrl(dmadim_xfer_msg, lli_list, p_msg_lli,
					   xfer);
		if (ret) {
			pr_err("err\n");
			ret = -EINVAL;
			goto err_lli_addr;
		}

		if (xfer->dma_mode > AX_DMADIM_1D
		    && xfer->dma_mode < AX_DMADIM_MEMORY_INIT) {
			p_msg_lli += sizeof(ax_dmadim_xd_desc_t) / sizeof(u8);
			pr_debug("XD");
		} else {
			p_msg_lli += sizeof(ax_dmadim_desc_t) / sizeof(u8);
			pr_debug("MEM");
		}
		if (dmadim_msg->lli_num - i > 1) {
			lli_list++;
			lli_list->lli_addr = (lli_list - 1)->lli_addr_next;
			lli_list->lli_addr_vir =
			    (lli_list - 1)->lli_addr_vir_next;
		}
	}
	return 0;
err_lli_addr:
	spin_lock_irqsave(&dmadim->lock, flags);
	ax_dmadim_free_lli(dmadim_xfer_msg);
	spin_unlock_irqrestore(&dmadim->lock, flags);
err_lli_list:
	kfree_set_null(xfer->lli_list);
	return ret;
}

static void ax_dmadim_free_list(struct file *filp, struct list_head *list,
				bool free_id, u32 stat)
{
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL, *__dmadim_xfer_msg = NULL;

	if (list_empty(list))
		return;
	list_for_each_entry_safe(dmadim_xfer_msg, __dmadim_xfer_msg,
				 list, xfer_node) {
		if (dmadim_xfer_msg && dmadim_xfer_msg->filp == filp) {
			list_del(&dmadim_xfer_msg->xfer_node);
			ax_dmadim_free_xfer_msg(dmadim_xfer_msg, free_id);
			if (stat)
				atomic64_dec(&g_dbg_buf.current_total);
		}
	}
}

static s32 ax_dmadim_open(struct inode *inode, struct file *filp)
{
	ax_dmadim_chn_t *chn = NULL;
	atomic_inc(&g_dbg_buf.chan_use);
	chn = (ax_dmadim_chn_t *) kzalloc(sizeof(ax_dmadim_chn_t), GFP_KERNEL);
	if ((long)filp->private_data == 1) {
		chn->kuser = 1;
	} else {
		chn->kuser = 0;
		init_waitqueue_head(&chn->event_wait);
	}
	chn->inode = inode;
	filp->private_data = (void *)chn;
	return 0;
}

static s32 ax_dmadim_release(struct inode *inode, struct file *filp)
{
	ax_dmadim_chn_t *chn = (ax_dmadim_chn_t *)filp->private_data;
	unsigned long flags;
	u32 dma_running = 0;
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL;
	spin_lock_irqsave(&dmadim->lock, flags);
	dma_running = ax_chan_hw_is_enable(dmadim);
	dmadim_xfer_msg = list_first_entry_or_null(&dmadim->dmadim_pending_list,
				     ax_dmadim_xfer_msg_t, xfer_node);
	if (dmadim_xfer_msg) {
		if (dma_running && dmadim_xfer_msg->filp == filp) {
			ax_dmadim_enable_irq(dmadim, 0);
			ax_dmadim_suspend(dmadim);
			ax_dmadim_suspend_clr(dmadim);
		}
	}
	ax_dmadim_free_list(filp, &dmadim->dmadim_cfg_list, 1, 0);
	ax_dmadim_free_list(filp, &dmadim->dmadim_pending_list, 1, 1);
	ax_dmadim_free_list(filp, &dmadim->dmadim_complite_list, 1, 0);
	ax_dmadim_free_list(filp, &dmadim->dmadim_free_list, 1, 0);
	ax_dmadim_free_list(filp, &dmadim->dmadim_checksum_cfg_list, 0, 0);
	ax_dmadim_free_list(filp, &dmadim->dmadim_checksum_complite_list, 0, 0);
	//start another chn
	if (!list_empty(&dmadim->dmadim_pending_list)) {
		dmadim_xfer_msg = list_first_entry_or_null(&dmadim->dmadim_pending_list,
				     ax_dmadim_xfer_msg_t, xfer_node);
		if (dmadim_xfer_msg && dmadim_xfer_msg->xfer_stat.stat == DMADIM_XFER_WAIT) {
			ax_dmadim_start(dmadim_xfer_msg);
		}
	}
	spin_unlock_irqrestore(&dmadim->lock, flags);
	kfree_set_null(chn);
	atomic_dec(&g_dbg_buf.chan_use);
	return 0;
}

static ssize_t ax_dmadim_setsync(struct file *filp, const char __user *buffer,
			   size_t count, loff_t *ppos)
{
	ax_dmadim_chn_t *chn = (ax_dmadim_chn_t *)filp->private_data;

	if (count == 0)
		return 0;

	if (copy_from_user
	    ((void *)&chn->file_msg, buffer, sizeof(ax_dmadim_file_msg_t))) {
		pr_err("copy_from_user\n");
		return -EFAULT;
	}

	return count;
}

static ssize_t ax_dmadim_getsync(struct file *filp, char __user *buffer,
			   size_t count, loff_t *ppos)
{
	ax_dmadim_chn_t *chn = (ax_dmadim_chn_t *)filp->private_data;

	if (count == 0)
		return 0;

	if (copy_to_user(buffer, (void *)&chn->file_msg,
			    sizeof(ax_dmadim_file_msg_t))) {
		pr_err("copy_to_user\n");
		return -EFAULT;
	}
	return 0;
}

static ax_dmadim_xfer_msg_t *ax_dmadim_find_entry_by_filp(struct list_head
							  *list,
							  struct file *filp)
{
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL;
	if (list_empty(list))
		return NULL;
	list_for_each_entry(dmadim_xfer_msg, list, xfer_node) {
		if (dmadim_xfer_msg->filp == filp) {
			return dmadim_xfer_msg;
		}
	}
	return NULL;
}

static ax_dmadim_xfer_msg_t *ax_dmadim_find_entry_by_id(struct list_head
							  *list, s32 id)
{
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL;
	if (list_empty(list))
		return NULL;
	list_for_each_entry(dmadim_xfer_msg, list, xfer_node) {
		if (dmadim_xfer_msg->xfer_stat.id == id) {
			return dmadim_xfer_msg;
		}
	}
	return NULL;
}

static __poll_t ax_dmadim_poll(struct file *filp,
			       struct poll_table_struct *wait)
{
	ax_dmadim_chn_t *chn = (ax_dmadim_chn_t *)filp->private_data;
	__poll_t mask = 0;
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL;
	unsigned long flags;

	poll_wait(filp, &chn->event_wait, wait);
	spin_lock_irqsave(&dmadim->lock, flags);
	dmadim_xfer_msg =
	    ax_dmadim_find_entry_by_filp(&dmadim->dmadim_complite_list, filp);
	if (!dmadim_xfer_msg) {
		spin_unlock_irqrestore(&dmadim->lock, flags);
		return mask;
	}
	if (dmadim_xfer_msg
	    && (dmadim_xfer_msg->xfer_stat.stat & DMADIM_XFER_DONE)) {
		list_move_tail(&dmadim_xfer_msg->xfer_node, &dmadim->dmadim_free_list);
		mask = POLLIN | POLLRDNORM;
	}
	spin_unlock_irqrestore(&dmadim->lock, flags);
	return mask;
}

static int ax_dmadim_create_lli(ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	s32 ret = 0;
	ax_dmadim_msg_t *dmadim_msg = &dmadim_xfer_msg->dmadim_msg;
	if (ax_dmadim_check_args(dmadim_msg)) {
		pr_err("err\n");
		ret = -EINVAL;
		goto err;
	}
	ret = ax_dmadim_set_lli_msg(dmadim_xfer_msg);
	if (ret) {
		pr_err("err\n");
		ret = -EINVAL;
		goto err;
	}
	dmadim_xfer_msg->xfer_stat.stat = DMADIM_XFER_IDLE;
	return 0;
err:
	dmadim_xfer_msg->xfer_stat.stat = -EINVAL;
	return ret;
}

static void ax_dmadim_free_checksum_list_by_id(int id, struct list_head *list)
{
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL, *__dmadim_xfer_msg = NULL;

	if (list_empty(list))
		return ;
	list_for_each_entry_safe(dmadim_xfer_msg, __dmadim_xfer_msg,
				 list, xfer_node) {
		if (dmadim_xfer_msg->xfer_stat.id == id) {
			ax_dmadim_free_xfer_msg(dmadim_xfer_msg, 0);
		}
	}
}

static int ax_dmadim_create_xfer_buf(struct file *filp,
				     ax_dmadim_xfer_msg_t **dmadim_xfer_msg,
				     ax_dmadim_msg_t msg,
				     ax_dmadim_desc_t *msg_lli)
{
	s32 msg_size, ret = 0;
	u64 msg_lli_user;
	ax_dmadim_msg_t *dmadim_msg;
	ax_dmadim_xfer_msg_t *p_dmadim_xfer_msg = NULL;
	ax_dmadim_chn_t *chn = (ax_dmadim_chn_t *)filp->private_data;
	*dmadim_xfer_msg =
	    (void *)kzalloc(sizeof(ax_dmadim_xfer_msg_t), GFP_KERNEL);
	p_dmadim_xfer_msg = *dmadim_xfer_msg;
	INIT_LIST_HEAD(&p_dmadim_xfer_msg->dmadim_pool_list);
	if (unlikely(!p_dmadim_xfer_msg)) {
		pr_err("err: ax_dmadim error\n");
		return -ENOMEM;
	}
	p_dmadim_xfer_msg->filp = filp;
	dmadim_msg = &p_dmadim_xfer_msg->dmadim_msg;
	if (unlikely(IS_ERR_OR_NULL(dmadim_msg))) {
		ret = -EINVAL;
		pr_err("err\n");
		goto xfer_msg;
	}
	memcpy(dmadim_msg, &msg, sizeof(ax_dmadim_msg_t));
	if (dmadim_msg->lli_num < 1) {
		ret = -EINVAL;
		pr_err("err\n");
		goto xfer_msg;
	}
	msg_lli_user = (u64) (dmadim_msg->msg_lli);
	if (dmadim_msg->dma_mode > AX_DMADIM_1D &&
	    dmadim_msg->dma_mode < AX_DMADIM_MEMORY_INIT) {
		msg_size = sizeof(ax_dmadim_xd_desc_t);
	} else {
		msg_size = sizeof(ax_dmadim_desc_t);
	}
	dmadim_msg->msg_lli =
	    kzalloc(msg_size * (dmadim_msg->lli_num), GFP_KERNEL);
	if (unlikely(!dmadim_msg->msg_lli)) {
		pr_err("err\n");
		p_dmadim_xfer_msg->xfer_stat.stat = -EFAULT;
		ret = -ENOMEM;
		goto xfer_msg;
	}
	if (msg_lli) {
		memcpy(dmadim_msg->msg_lli, msg_lli,
			msg_size * dmadim_msg->lli_num);
	} else {
		if (chn->kuser) {
			memcpy((void *)(dmadim_msg->msg_lli),
			      (void *)msg_lli_user,
			      msg_size * dmadim_msg->lli_num);
		} else if(unlikely(copy_from_user
			     ((void *)(dmadim_msg->msg_lli),
			      (void *)msg_lli_user,
			      msg_size * dmadim_msg->lli_num))) {
			pr_err("err\n");
			ret = -EIO;
			p_dmadim_xfer_msg->xfer_stat.stat = -EFAULT;
			goto err_msg_lli;
		}
	}
	return 0;
err_msg_lli:
	kfree_set_null(dmadim_msg->msg_lli);
xfer_msg:
	kfree_set_null(p_dmadim_xfer_msg);
	return ret;
}

static int ax_dmadim_creat_checksum_msg(struct file *filp,
					ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	int ret = 0;
	unsigned long flags;
	s32 num = 0, __num = 0, tmp_size = 0, step = 0;
	ax_dmadim_xfer_msg_t *check_xfer_msg = NULL;
	ax_dmadim_msg_t *dmadim_msg = &dmadim_xfer_msg->dmadim_msg;
	ax_dmadim_desc_t *msg_data = (ax_dmadim_desc_t *) dmadim_msg->msg_lli;
	ax_dmadim_desc_t *__msg_data;

	num = msg_data->size >> 23;
	tmp_size = msg_data->size & 0x7FFFFF;
	num += tmp_size / DMADIM_TR_2B_MAXSIZE;
	tmp_size %= DMADIM_TR_2B_MAXSIZE;
	num += DIV_ROUND_UP(tmp_size, DMADIM_TR_1B_MAXSIZE);
	dmadim_xfer_msg->xfer.sub_num = num - 1;

	tmp_size = msg_data->size;
	while (__num < num - 1) {
		ret =
		    ax_dmadim_create_xfer_buf(filp, &check_xfer_msg,
					      *dmadim_msg, msg_data);
		if (ret) {
			pr_err("err\n");
			return ret;
		}
		__msg_data =
		    (ax_dmadim_desc_t *)check_xfer_msg->dmadim_msg.msg_lli;
		__msg_data->src = msg_data->src + step;
		__msg_data->dst = msg_data->dst + step;
		if (tmp_size > DMADIM_TR_4B_MAXSIZE) {
			__msg_data->size = DMADIM_TR_4B_MAXSIZE;
		} else if (tmp_size > DMADIM_TR_2B_MAXSIZE) {
			__msg_data->size = DMADIM_TR_2B_MAXSIZE;
		} else if (tmp_size > DMADIM_TR_1B_MAXSIZE) {
			__msg_data->size = DMADIM_TR_1B_MAXSIZE;
		} else {
			__msg_data->size = tmp_size;
		}
		check_xfer_msg->xfer_stat.id = dmadim_xfer_msg->xfer_stat.id;
		check_xfer_msg->filp = dmadim_xfer_msg->filp;
		ret = ax_dmadim_create_lli(check_xfer_msg);
		if (ret) {
			pr_err("err\n");
			goto err;
		}
		spin_lock_irqsave(&dmadim->lock, flags);
		list_add_tail(&check_xfer_msg->xfer_node,
			      &dmadim->dmadim_checksum_cfg_list);
		spin_unlock_irqrestore(&dmadim->lock, flags);
		step += __msg_data->size;
		tmp_size -= __msg_data->size;
		__num++;
	}
	msg_data->size = tmp_size;
	msg_data->src += step;
	msg_data->dst += step;
	return 0;
err:
	ax_dmadim_free_xfer_buf(check_xfer_msg);
	spin_lock_irqsave(&dmadim->lock, flags);
	ax_dmadim_free_checksum_list_by_id(dmadim_xfer_msg->xfer_stat.id,
					   &dmadim->dmadim_checksum_cfg_list);
	spin_unlock_irqrestore(&dmadim->lock, flags);
	return ret;
}

static void ax_dmadim_checksum_list_remove_reverse(int id, int count)
{
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL, *__dmadim_xfer_msg = NULL;
	if (list_empty(&dmadim->dmadim_checksum_cfg_list))
		return ;
	if (count) {
		list_for_each_entry_safe_reverse(dmadim_xfer_msg,
						 __dmadim_xfer_msg,
						 &dmadim->dmadim_checksum_cfg_list,
						 xfer_node) {
			if (dmadim_xfer_msg && dmadim_xfer_msg->xfer_stat.id == id) {
				list_del(&dmadim_xfer_msg->xfer_node);
				ax_dmadim_free_xfer_msg(dmadim_xfer_msg, 0);
				if (--count == 0)
					break;
			}
		}
	}
}

static int ax_dmadim_checksum_list_add(int id, int count)
{
	int __count = count;
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL, *__dmadim_xfer_msg = NULL;
	if (list_empty(&dmadim->dmadim_checksum_cfg_list))
		return __count;
	if (__count)
		list_for_each_entry_safe(dmadim_xfer_msg, __dmadim_xfer_msg,
					 &dmadim->dmadim_checksum_cfg_list,
					 xfer_node) {
		if (dmadim_xfer_msg && dmadim_xfer_msg->xfer_stat.id == id) {
			list_move_tail(&dmadim_xfer_msg->xfer_node,
				       &dmadim->dmadim_pending_list);
			if (--__count == 0)
				break;
		}
		}
	if (__count)
		return count - __count;
	else
		return 0;
}

static u32 ax_dmadim_cali_checksum(ax_dmadim_xfer_msg_t *dmadim_xfer_msg)
{
	ax_dmadim_xfer_msg_t *checksum_xfer_msg = NULL, *__checksum_xfer_msg =
	    NULL;
	ax_dmadim_xfer_stat_t *xfer_stat = NULL;
	u32 checksum = dmadim_xfer_msg->xfer_stat.checksum;
	s32 count = dmadim_xfer_msg->xfer.sub_num;

	if (list_empty(&dmadim->dmadim_checksum_complite_list))
		return checksum;
	list_for_each_entry_safe(checksum_xfer_msg, __checksum_xfer_msg,
				 &dmadim->dmadim_checksum_complite_list,
				 xfer_node) {
		xfer_stat = &checksum_xfer_msg->xfer_stat;
		if (xfer_stat->id == dmadim_xfer_msg->xfer_stat.id) {
			checksum += xfer_stat->checksum;
			list_del(&checksum_xfer_msg->xfer_node);
			ax_dmadim_free_xfer_msg(checksum_xfer_msg, 0);
			if (--count == 0)
				break;
		}
	}
	return checksum;
}

static long ax_dmadim_ioctl(struct file *filp, unsigned int cmd,
			    unsigned long arg)
{
	s32 id, ret = 0;
	ax_dmadim_chn_t *chn = (ax_dmadim_chn_t *)filp->private_data;
	ax_dmadim_xfer_msg_t *dmadim_xfer_msg = NULL;
	ax_dmadim_msg_t *dmadim_msg = NULL;
	ax_dmadim_msg_t tmp_dmadim_msg;
	ax_dmadim_stat_t dmadim_stat;
	unsigned long flags;

	switch (cmd) {
	case AXI_DMADIM_CFG_MEM_CHN_CMD:
		if (chn->kuser) {
			memcpy((void *)&tmp_dmadim_msg, (void *)arg,
			       sizeof(ax_dmadim_msg_t));
		} else if (unlikely(copy_from_user
			     ((void *)&tmp_dmadim_msg, (void *)arg,
			      sizeof(ax_dmadim_msg_t)))) {
			pr_err("err: ax_dmadim error\n");
			return -EIO;
		}
		ret =
		    ax_dmadim_create_xfer_buf(filp, &dmadim_xfer_msg,
					      tmp_dmadim_msg, NULL);
		if (ret) {
			pr_err("err\n");
			return ret;
		}
		dmadim_xfer_msg->dma_callback =
		    (int (*)(void *, unsigned int))tmp_dmadim_msg.pFun;
		dmadim_xfer_msg->arg = tmp_dmadim_msg.arg;
		dmadim_xfer_msg->kuser = chn->kuser;
		dmadim_msg = &dmadim_xfer_msg->dmadim_msg;
		spin_lock_irqsave(&dmadim->lock, flags);
		dmadim_xfer_msg->xfer_stat.id =
		    ax_dmadim_get_id(dmadim, dmadim_xfer_msg);
		spin_unlock_irqrestore(&dmadim->lock, flags);
		if (!(dmadim_xfer_msg->xfer_stat.id)) {
			pr_err("err\n");
			ret = -EIO;
			goto err;
		}
		if (dmadim_xfer_msg->dmadim_msg.dma_mode == AX_DMADIM_CHECKSUM) {
			if (dmadim_msg->lli_num != 1) {
				ret = -EINVAL;
				pr_err("err\n");
				goto err_id;
			}
			ret = ax_dmadim_creat_checksum_msg(filp, dmadim_xfer_msg);
			if (ret) {
				pr_err("err\n");
				goto err_id;
			}
		}
		ret = ax_dmadim_create_lli(dmadim_xfer_msg);
		if (ret) {
			pr_err("err\n");
			goto err_checksum;
		}
		ret = dmadim_xfer_msg->xfer_stat.id;
		spin_lock_irqsave(&dmadim->lock, flags);
		list_add_tail(&dmadim_xfer_msg->xfer_node, &dmadim->dmadim_cfg_list);
		spin_unlock_irqrestore(&dmadim->lock, flags);
		break;
err_checksum:
		if (dmadim_xfer_msg->dmadim_msg.dma_mode == AX_DMADIM_CHECKSUM) {
			spin_lock_irqsave(&dmadim->lock, flags);
			ax_dmadim_free_checksum_list_by_id
			    (dmadim_xfer_msg->xfer_stat.id,
			     &dmadim->dmadim_checksum_cfg_list);
			spin_unlock_irqrestore(&dmadim->lock, flags);
		}
err_id:
		spin_lock_irqsave(&dmadim->lock, flags);
		ax_dmadim_free_id(dmadim, dmadim_xfer_msg->xfer_stat.id);
		spin_unlock_irqrestore(&dmadim->lock, flags);
err:
		ax_dmadim_free_xfer_buf(dmadim_xfer_msg);
		pr_err("err\n");
		break;
	case AXI_DMADIM_START_CHN_CMD:
		if (chn->kuser) {
			id = arg;
		} else if (unlikely
			   (copy_from_user
			    ((void *)(&id), (void *)arg, sizeof(s32)))) {
			pr_err("err: ax_dmadim error\n");
			return -EFAULT;
		}
		spin_lock_irqsave(&dmadim->lock, flags);
		dmadim_xfer_msg =
		    (ax_dmadim_xfer_msg_t *) ax_dmadim_find_id(dmadim, id);
		if (dmadim_xfer_msg) {
			atomic64_inc(&g_dbg_buf.total);
			atomic64_inc(&g_dbg_buf.current_total);
			dmadim_xfer_msg->time_start = ktime_get_real();
			dmadim_xfer_msg->ktime_start = ktime_get();
			dmadim_xfer_msg->xfer_stat.stat = DMADIM_XFER_WAIT;
			dmadim->id = id;
			list_move_tail(&dmadim_xfer_msg->xfer_node,
				      &dmadim->dmadim_pending_list);
			if (dmadim_xfer_msg->xfer.dma_mode ==
			    AX_DMADIM_CHECKSUM) {
				ret =
				    ax_dmadim_checksum_list_add(id,
								dmadim_xfer_msg->
								xfer.sub_num);
				if (ret) {
					pr_err("add list count err %d\n", ret);
					ax_dmadim_checksum_list_remove_reverse
					    (id, ret);
					ax_dmadim_free_xfer_msg(dmadim_xfer_msg,
								1);
					spin_unlock_irqrestore(&dmadim->lock,
							       flags);
					ret = -EFAULT;
					break;
				}
			}
			dmadim_xfer_msg->checksum_num =
			    dmadim_xfer_msg->xfer.sub_num + 1;

			ax_dmadim_start(dmadim_xfer_msg);
			spin_unlock_irqrestore(&dmadim->lock, flags);
		} else {
			spin_unlock_irqrestore(&dmadim->lock, flags);
			ret = -EFAULT;
		}
		break;
	case AXI_DMADIM_GET_CALLBACK:
		if (unlikely
		    (copy_from_user((void *)(&dmadim_stat), (void *)arg,
				    sizeof(ax_dmadim_stat_t)))) {
			pr_err("err: ax_dmadim error\n");
			return -EFAULT;
		}
		spin_lock_irqsave(&dmadim->lock, flags);
		if (dmadim_stat.xfer_stat.id)
			dmadim_xfer_msg =
				ax_dmadim_find_entry_by_id(
					&dmadim->dmadim_free_list,
					dmadim_stat.xfer_stat.id);
		else
			dmadim_xfer_msg =
				ax_dmadim_find_entry_by_filp(
					&dmadim->dmadim_free_list,
					filp);
		if (!dmadim_xfer_msg) {
			spin_unlock_irqrestore(&dmadim->lock, flags);
			pr_err("err: ax_dmadim error\n");
			ret = -EFAULT;
			break;
		}
		dmadim_msg = &dmadim_xfer_msg->dmadim_msg;
		if (dmadim_xfer_msg->xfer.dma_mode == AX_DMADIM_CHECKSUM)
			dmadim_stat.xfer_stat.checksum =
				ax_dmadim_cali_checksum(dmadim_xfer_msg);
		dmadim_stat.pFun = dmadim_msg->pFun;
		dmadim_stat.arg = dmadim_msg->arg;
		list_del(&dmadim_xfer_msg->xfer_node);
		g_dbg_buf.last = (g_dbg_buf.last + 1) & 0x7;
		g_dbg_buf.info[g_dbg_buf.last].start =
		    dmadim_xfer_msg->time_start / 1000000000;
		g_dbg_buf.info[g_dbg_buf.last].wait =
		    dmadim_xfer_msg->ktime_run - dmadim_xfer_msg->ktime_start;
		g_dbg_buf.info[g_dbg_buf.last].run =
		    ktime_sub(ktime_get(), dmadim_xfer_msg->ktime_run);
		g_dbg_buf.info[g_dbg_buf.last].msg_id =
		    dmadim_xfer_msg->xfer_stat.id;
		g_dbg_buf.info[g_dbg_buf.last].size = dmadim_xfer_msg->size;
		g_dbg_buf.info[g_dbg_buf.last].dma_mode =
		    dmadim_xfer_msg->xfer.dma_mode;
		ax_dmadim_free_xfer_msg(dmadim_xfer_msg, 1);
		spin_unlock_irqrestore(&dmadim->lock, flags);
		ret = copy_to_user((void __user *)arg, &dmadim_stat,
				   sizeof(ax_dmadim_stat_t));
		if (ret)
			pr_err("err: ax_dmadim error\n");
		break;
	case AXI_DMADIM_FREE_MSG_BY_ID:
		id = arg;
		spin_lock_irqsave(&dmadim->lock, flags);
		dmadim_xfer_msg = ax_dmadim_find_entry_by_id(
				    &dmadim->dmadim_free_list, id);
		list_del(&dmadim_xfer_msg->xfer_node);
		g_dbg_buf.last = (g_dbg_buf.last + 1) & 0x7;
		g_dbg_buf.info[g_dbg_buf.last].start =
		    dmadim_xfer_msg->time_start / 1000000000;
		g_dbg_buf.info[g_dbg_buf.last].wait =
		    dmadim_xfer_msg->ktime_run - dmadim_xfer_msg->ktime_start;
		g_dbg_buf.info[g_dbg_buf.last].run =
		    ktime_sub(ktime_get(), dmadim_xfer_msg->ktime_run);
		g_dbg_buf.info[g_dbg_buf.last].msg_id =
		    dmadim_xfer_msg->xfer_stat.id;
		g_dbg_buf.info[g_dbg_buf.last].size = dmadim_xfer_msg->size;
		g_dbg_buf.info[g_dbg_buf.last].dma_mode =
		    dmadim_xfer_msg->xfer.dma_mode;
		ax_dmadim_free_xfer_msg(dmadim_xfer_msg, 1);
		spin_unlock_irqrestore(&dmadim->lock, flags);
		break;
	default:
		pr_info("cmd err\n");
		break;
	}
	return ret;
}

int ax_dmadim_xfer(u64 src_addr, u64 dest_addr, u32 size,
		   int (*dma_callback) (void *, unsigned int), void *arg)
{
	struct inode *inode = kzalloc(sizeof(struct inode), GFP_KERNEL);
	struct file *file = kzalloc(sizeof(struct file), GFP_KERNEL);
	ax_dmadim_msg_t dmadim_msg;
	ax_dmadim_desc_t msg_lli;
	u32 ret = 0, id = 0;

	if (!dma_callback)
		mutex_lock(&dmadim->kmutex);
	msg_lli.src = src_addr;
	msg_lli.dst = dest_addr;
	msg_lli.size = size;
	dmadim_msg.lli_num = 1;
	dmadim_msg.endian = 0;
	dmadim_msg.dma_mode = DMADIM_1D;
	dmadim_msg.msg_lli = &msg_lli;
	dmadim_msg.pFun =
	    (void (*)(ax_dmadim_xfer_stat_t *, void *))dma_callback;
	dmadim_msg.arg = arg;

	file->private_data = (void *)1;
	ax_dmadim_open(inode, file);
	id = ax_dmadim_ioctl(file, AXI_DMADIM_CFG_MEM_CHN_CMD,
			     (unsigned long)&dmadim_msg);
	if (id > 0) {
		ret = ax_dmadim_ioctl(file, AXI_DMADIM_START_CHN_CMD, id);
		if (ret) {
			pr_err("AXI_DMADIM_START_CHN_CMD err\n");
			ret = -1;
			goto err;
		}
	} else {
		ret = -1;
		pr_err("AXI_DMADIM_CFG_MEM_CHN_CMD err\n");
		goto err;
	}
	if (!dma_callback) {
		if (!wait_for_completion_timeout(&dmadim->complete, msecs_to_jiffies(5000))) {
			pr_err("timed out\n");
			ret = -ETIMEDOUT;
		}
		{
			struct file *filp = NULL;
			ax_dmadim_xfer_msg_t *dmadim_xfer_msg;
			dmadim_xfer_msg = (ax_dmadim_xfer_msg_t *) ax_dmadim_find_id(dmadim, id);
			if (dmadim_xfer_msg) {
				filp = dmadim_xfer_msg->filp;
				ret = ax_dmadim_ioctl(dmadim_xfer_msg->filp,
						AXI_DMADIM_FREE_MSG_BY_ID, id);
				if (ret) {
					pr_err("AXI_DMADIM_FREE_MSG_BY_ID err\n");
				}
				ax_dmadim_release(inode, filp);
				kfree_set_null(filp);
				kfree_set_null(inode);
			}
		}
	}
err:
	if (!dma_callback) {
		mutex_unlock(&dmadim->kmutex);
	}
	return ret;
}
EXPORT_SYMBOL(ax_dmadim_xfer);

int ax_dmadim_xfer_sync(u64 src_addr, u64 dest_addr, u32 size)
{
	return ax_dmadim_xfer(src_addr, dest_addr, size, NULL, NULL);
}
EXPORT_SYMBOL(ax_dmadim_xfer_sync);

int ax_dmadim_xfer_crop(ax_dmadim_xd_desc_t *xd_desc, u32 lli_num,
		    ax_dmadim_xfer_mode_t dma_mode, ax_dmadim_endian_t endian,
		    int (*dma_callback) (void *, unsigned int), void *arg)
{
	struct inode *inode = kzalloc(sizeof(struct inode), GFP_KERNEL);
	struct file *file = kzalloc(sizeof(struct file), GFP_KERNEL);
	ax_dmadim_msg_t dmadim_msg;
	u32 ret = 0, id = 0;

	if (!dma_callback) {
		mutex_unlock(&dmadim->kmutex);
	}
	dmadim_msg.lli_num = lli_num;
	dmadim_msg.endian = endian;
	dmadim_msg.dma_mode = dma_mode;
	dmadim_msg.msg_lli = xd_desc;
	dmadim_msg.pFun =
		(void (*)(ax_dmadim_xfer_stat_t *, void *))dma_callback;
	dmadim_msg.arg = arg;

	file->private_data = (void *)1;
	ax_dmadim_open(inode, file);
	id = ax_dmadim_ioctl(file, AXI_DMADIM_CFG_MEM_CHN_CMD,
			     (unsigned long)&dmadim_msg);
	if (id > 0) {
		ret = ax_dmadim_ioctl(file, AXI_DMADIM_START_CHN_CMD, id);
		if (ret) {
			pr_err("AXI_DMADIM_START_CHN_CMD err\n");
			ret = -1;
			goto err;
		}
	} else {
		ret = -1;
		pr_err("AXI_DMADIM_CFG_MEM_CHN_CMD err\n");
		goto err;
	}
	if (!dma_callback) {
		if (!wait_for_completion_timeout(&dmadim->complete, msecs_to_jiffies(5000))) {
			pr_err("timed out\n");
			ret = -ETIMEDOUT;
		}
		{
			struct file *filp = NULL;
			ax_dmadim_xfer_msg_t *dmadim_xfer_msg;
			dmadim_xfer_msg = (ax_dmadim_xfer_msg_t *) ax_dmadim_find_id(dmadim, id);
			if (dmadim_xfer_msg) {
				filp = dmadim_xfer_msg->filp;
				ret = ax_dmadim_ioctl(dmadim_xfer_msg->filp,
						AXI_DMADIM_FREE_MSG_BY_ID, id);
				if (ret) {
					pr_err("AXI_DMADIM_FREE_MSG_BY_ID err\n");
				}
				ax_dmadim_release(inode, filp);
				kfree_set_null(filp);
				kfree_set_null(inode);
			}
		}
	}
err:
	if (!dma_callback) {
		mutex_unlock(&dmadim->kmutex);
	}
	return ret;
}
EXPORT_SYMBOL(ax_dmadim_xfer_crop);

int ax_dmadim_xfer_crop_sync(ax_dmadim_xd_desc_t *xd_desc, u32 lli_num,
		    ax_dmadim_xfer_mode_t dma_mode, ax_dmadim_endian_t endian)
{
	return ax_dmadim_xfer_crop(xd_desc, lli_num, dma_mode, endian, NULL, NULL);
}
EXPORT_SYMBOL(ax_dmadim_xfer_crop_sync);

static const struct file_operations ax_dmadim_fops = {
	.owner = THIS_MODULE,
	.open = ax_dmadim_open,
	.read = ax_dmadim_getsync,
	.write = ax_dmadim_setsync,
	.release = ax_dmadim_release,
	.unlocked_ioctl = ax_dmadim_ioctl,
	.poll = ax_dmadim_poll,
};

static struct miscdevice ax_dmadim_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = AX_DMADMA_DEVICE_NAME,
	.fops = &ax_dmadim_fops
};

static s32 ax_dmadim_probe(struct platform_device *pdev)
{
	s32 err;
	struct resource *res = NULL;

	dmadim =
	    devm_kzalloc(&pdev->dev, sizeof(struct ax_dmadim_device),
			 GFP_KERNEL);
	if (unlikely(!dmadim))
		return -ENOMEM;
	dmadim->pdev = pdev;
	dmadim->id_pool = ~0UL;
	dmadim->irq = platform_get_irq(pdev, 0);
	if (unlikely(dmadim->irq < 0)) {
		pr_err("err: failed to get IRQ number\n");
		return dmadim->irq;
	}
	INIT_WORK(&dmadim->work_data.work, ax_dmadim_work_handler);
	err = devm_request_irq(&pdev->dev, dmadim->irq,
			       ax_dmadim_interrupt, IRQF_SHARED,
			       dev_name(&pdev->dev), dmadim);
	if (unlikely(err)) {
		pr_err("err: failed to request_irq\n");
		return err;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(!res)) {
		pr_err("err: platform_get_resource error\n");
		return -ENOMEM;
	}

	dmadim->base = devm_ioremap_resource(&pdev->dev, res);
	if (unlikely(IS_ERR(dmadim->base))) {
		pr_err("err\n");
		err = PTR_ERR(dmadim->base);
		return err;
	}

	dmadim->rst = devm_reset_control_get_optional(&pdev->dev, "dmadim-rst");
	if (IS_ERR(dmadim->rst)) {
		pr_err("err\n");
		return PTR_ERR(dmadim->rst);
	}

	dmadim->core_clk = devm_clk_get(&pdev->dev, "dmadim-clk");
	if (IS_ERR(dmadim->core_clk)) {
		pr_err("err\n");
		return PTR_ERR(dmadim->core_clk);
	}

	if (clk_set_rate(dmadim->core_clk, 500000000))
		pr_err("err: set core-clk rate fail\n");

	/* Set the dma mask bits */
	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	dmadim->dmadim_pool = dmam_pool_create(KBUILD_MODNAME, &pdev->dev,
					       128, 64, 0);
	if (unlikely(!dmadim->dmadim_pool)) {
		pr_err("err: No memory for descriptors dma pool\n");
		return -ENOMEM;
	}
	err = ax_dmadim_proc_init(dmadim);
	if (unlikely(err)) {
		pr_err("err\n");
		return err;
	}
	err = misc_register(&ax_dmadim_miscdev);
	if (unlikely(err)) {
		pr_err("err: misc_register fail\n");
		goto err_register;
	}
	ax_dmadim_hw_enable();
	init_completion(&dmadim->complete);
	mutex_init(&dmadim->kmutex);
	INIT_LIST_HEAD(&dmadim->dmadim_cfg_list);
	INIT_LIST_HEAD(&dmadim->dmadim_pending_list);
	INIT_LIST_HEAD(&dmadim->dmadim_complite_list);
	INIT_LIST_HEAD(&dmadim->dmadim_free_list);
	INIT_LIST_HEAD(&dmadim->dmadim_checksum_cfg_list);
	INIT_LIST_HEAD(&dmadim->dmadim_checksum_complite_list);
	spin_lock_init(&dmadim->lock);
	platform_set_drvdata(pdev, dmadim);
	atomic64_set(&g_dbg_buf.total, 0);
	atomic64_set(&g_dbg_buf.current_total, 0);
	pr_info("done\n");
	return 0;
err_register:
	pr_err("Err !\n");
	remove_proc_entry(AX_DMADIM_PROC_NAME, NULL);
	return err;
}

static s32 ax_dmadim_remove(struct platform_device *pdev)
{
	if (unlikely(!dmadim)) {
		return -EINVAL;
	}
	/* Disable DMA interrupt */
	ax_dmadim_disable_all_irq(dmadim);
	synchronize_irq(dmadim->irq);
	/* Disable hardware */
	remove_proc_entry(AX_DMADIM_PROC_NAME, NULL);
	misc_deregister(&ax_dmadim_miscdev);
	dmam_pool_destroy(dmadim->dmadim_pool);
	ax_dmadim_hw_disable();
	kfree_set_null(dmadim);
	return 0;
}

static const struct dev_pm_ops ax_dmadim_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(ax_dmadim_pm_suspend, ax_dmadim_pm_resume)
};

static const struct of_device_id ax_dmadim_of_dev_id[] = {
	{
	 .compatible = "axera,dmadim",
	 },
	{}
};

MODULE_DEVICE_TABLE(of, dmadim_of_dev_id);

static struct platform_driver ax_dmadim_driver = {
	.probe = ax_dmadim_probe,
	.remove = ax_dmadim_remove,
	.driver = {
		   .name = "ax-dmadim",
		   .of_match_table = ax_dmadim_of_dev_id,
		   .pm = &ax_dmadim_pm_ops,
		   },
};

static int __init ax_dmadim_init(void)
{
	return platform_driver_register(&ax_dmadim_driver);
}
subsys_initcall_sync(ax_dmadim_init);

static void __exit ax_dmadim_exit(void)
{
	platform_driver_unregister(&ax_dmadim_driver);
}
module_exit(ax_dmadim_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Axera AXI DMADIM Controller driver");
MODULE_AUTHOR("Axera");
