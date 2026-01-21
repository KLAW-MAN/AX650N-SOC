#define pr_fmt(fmt) KBUILD_MODNAME ":%s:%d: " fmt, __func__, __LINE__

#include "axera-axi-dmaxor-platform.h"
#include "../../soc/axera/dmaxor/dmaxor.h"

#define AX_DMAXOR_DRV  "axera_dmaxor"

#define AX_DMAXOR_BUSWIDTHS		  \
	(DMA_SLAVE_BUSWIDTH_1_BYTE	| \
	DMA_SLAVE_BUSWIDTH_2_BYTES	| \
	DMA_SLAVE_BUSWIDTH_4_BYTES	| \
	DMA_SLAVE_BUSWIDTH_8_BYTES)

static void kfree_set_null(void *p)
{
	kfree(p);
	p = NULL;
}

static inline void
ax_dmaxor_iowrite32(struct ax_dmaxor_chip *chip, u32 reg, u32 val)
{
	iowrite32(val, chip->regs + reg);
}

static inline u32 ax_dmaxor_ioread32(struct ax_dmaxor_chip *chip, u32 reg)
{
	return ioread32(chip->regs + reg);
}

static inline void
ax_chan_iowrite32(struct ax_dmaxor_chan *chan, u32 reg, u32 val)
{
	iowrite32(val, chan->chan_regs + reg);
}

static inline u32 ax_chan_ioread32(struct ax_dmaxor_chan *chan, u32 reg)
{
	return ioread32(chan->chan_regs + reg);
}

static inline void
ax_chan_iowrite64(struct ax_dmaxor_chan *chan, u32 reg, u64 val)
{
	/*
	 * We split one 64 bit write for two 32 bit write as some HW doesn't
	 * support 64 bit access.
	 */
	iowrite32(lower_32_bits(val), chan->chan_regs + reg);
	iowrite32(upper_32_bits(val), chan->chan_regs + reg + 4);
}

static inline void ax_dmaxor_irq_disable(struct ax_dmaxor_chip *chip)
{
	ax_dmaxor_iowrite32(chip, AX_DMAXOR_REG_INT_GLB_MASK, 0x00);
}

static inline void ax_dmaxor_irq_enable(struct ax_dmaxor_chip *chip)
{
	ax_dmaxor_iowrite32(chip, AX_DMAXOR_REG_INT_GLB_MASK, 0xFFFF);
}

static inline void ax_chan_irq_disable(struct ax_dmaxor_chan *chan)
{
	ax_chan_iowrite32(chan, AX_DMAXOR_CHAN_INT_EN, 0x00);
}

static inline void ax_chan_irq_set(struct ax_dmaxor_chan *chan, u32 irq_mask)
{
	ax_chan_iowrite32(chan, AX_DMAXOR_CHAN_INT_EN, irq_mask);
}

static inline void ax_chan_irq_clear(struct ax_dmaxor_chan *chan, u32 irq_mask)
{
	ax_chan_iowrite32(chan, AX_DMAXOR_CHAN_INT_CLR, irq_mask);
}

static inline u32 ax_chan_irq_read(struct ax_dmaxor_chan *chan)
{
	return ax_chan_ioread32(chan, AX_DMAXOR_CHAN_INT_STA);
}

static inline void ax_set_cur_id(struct ax_dmaxor_chan *chan)
{
	chan->chip->cur_id = BIT(chan->id);
}

static inline void ax_clr_cur_id(struct ax_dmaxor_chip *chip)
{
	chip->cur_id = 0;
}

static inline unsigned int ax_get_cur_id(struct ax_dmaxor_chip *chip)
{
	return chip->cur_id;
}

static inline void ax_chan_enable(struct ax_dmaxor_chan *chan)
{
	ax_dmaxor_iowrite32(chan->chip, AX_DMAXOR_REG_CTRL,
			    (AX_DMAXOR_LLI_SIZE /
			     128 & AX_DMAXOR_REG_CTRL_TYPE_MASK)
			    << AX_DMAXOR_REG_CTRL_TYPE_SHIFT);
	ax_set_cur_id(chan);
	ax_dmaxor_iowrite32(chan->chip, AX_DMAXOR_REG_START, 1);
	mb();
	ax_dmaxor_iowrite32(chan->chip, AX_DMAXOR_REG_TRIG, 0);	//chan->id -> 0
	mb();
}

static inline void ax_chan_dsiable(struct ax_dmaxor_chan *chan)
{
	ax_clr_cur_id(chan->chip);	//disable all
}

static inline bool ax_chan_hw_is_enable(struct ax_dmaxor_chan *chan)
{
	return !!ax_get_cur_id(chan->chip);
}

static inline void ax_dmaxor_reset(struct ax_dmaxor_chip *chip)
{
	reset_control_assert(chip->rst);
	reset_control_deassert(chip->rst);
	ax_clr_cur_id(chip);
}

static void ax_dmaxor_hw_init(struct ax_dmaxor_chip *chip)
{
	ax_dmaxor_reset(chip);
	ax_dmaxor_irq_disable(chip);
	ax_chan_irq_disable(&chip->axdmaxor->chan[0]);
}

static u32 ax_chan_get_xfer_width(struct ax_dmaxor_chan *chan, size_t len)
{
	u32 max_width = chan->chip->axdmaxor->hdata->m_data_width;

	return __ffs(len | BIT(max_width));
}

static inline const char *ax_chan_name(struct ax_dmaxor_chan *chan)
{
	return dma_chan_name(&chan->vc.chan);
}

static struct ax_dmaxor_desc *ax_desc_alloc(struct ax_dmaxor_chip *chip, u32 num)
{
	struct ax_dmaxor_desc *desc = NULL;

	desc = ax_kmem_get(chip->desc_kmem);
	if (!desc)
		return NULL;
	atomic_set(&desc->hw_descs_allocated, 0);
	desc->hw_desc = kcalloc(num, sizeof(*desc->hw_desc),
				GFP_NOWAIT | __GFP_NOWARN);
	if (!desc->hw_desc) {
		desc->hw_desc = kcalloc(num, sizeof(*desc->hw_desc), GFP_ATOMIC);
		if (!desc->hw_desc) {
			ax_kmem_put(chip->desc_kmem, desc);
			return NULL;
		}
	}
	return desc;
}

static struct ax_dmaxor_lli *ax_desc_pool_get(struct ax_dmaxor_chan *chan,
					      struct ax_dmaxor_desc *desc,
					      dma_addr_t * addr)
{
	struct ax_dmaxor_lli *lli;
	dma_addr_t phys;

	lli = dma_pool_zalloc(chan->chip->desc_pool, GFP_NOWAIT, &phys);
	if (unlikely(!lli)) {
		dev_err(chan2dev(chan),
			"%s: not enough descriptors available\n",
			ax_chan_name(chan));
		return NULL;
	}
	atomic_inc(&desc->hw_descs_allocated);
	*addr = phys;
	return lli;
}

static void ax_desc_put(struct ax_dmaxor_desc *desc)
{
	struct ax_dmaxor_chan *chan = desc->chan;
	int count = atomic_read(&desc->hw_descs_allocated);
	struct ax_dmaxor_hw_desc *hw_desc;
	int descs_put;

	for (descs_put = 0; descs_put < count; descs_put++) {
		hw_desc = &desc->hw_desc[descs_put];
		dma_pool_free(chan->chip->desc_pool, hw_desc->lli, hw_desc->llp);
	}
	atomic_sub(descs_put, &desc->hw_descs_allocated);
	dev_vdbg(chan2dev(chan), "%s: %d descs put, %d still allocated\n",
		 ax_chan_name(chan), descs_put,
		 atomic_read(&desc->hw_descs_allocated));
	kfree_set_null(desc->hw_desc);
	ax_kmem_put(chan->chip->desc_kmem, desc);
}

static void vchan_desc_put(struct virt_dma_desc *vdesc)
{
	ax_desc_put(vd_to_ax_desc(vdesc));
}

static enum dma_status
ax_dmaxor_chan_tx_status(struct dma_chan *dchan, dma_cookie_t cookie,
			 struct dma_tx_state *txstate)
{
	return dma_cookie_status(dchan, cookie, txstate);
}

static void
ax_dmaxor_write_desc_llp(struct ax_dmaxor_hw_desc *desc, dma_addr_t adr)
{
	desc->lli->llp = cpu_to_le64(adr);
}

static void
ax_dmaxor_write_chan_llp(struct ax_dmaxor_chan *chan, dma_addr_t adr)
{
	ax_dmaxor_iowrite32(chan->chip, AX_DMAXOR_REG_LLI_BASE_H, adr >> 32);
	ax_dmaxor_iowrite32(chan->chip, AX_DMAXOR_REG_LLI_BASE_L,
			    adr & 0xFFFFFFFF);
}

static void ax_dmaxor_chan_block_xfer_start(struct ax_dmaxor_chan *chan,
					    struct ax_dmaxor_desc *first)
{
	ax_dmaxor_xfer_info_t *xfer_info = NULL;
	if (ax_chan_hw_is_enable(chan)) {
		pr_err("hw is non-idle");
		return;
	}

	if(!IS_ERR_OR_NULL(first->vd.tx.callback_param)) {
		xfer_info = (ax_dmaxor_xfer_info_t *)first->vd.tx.callback_param;
		if (xfer_info->type == AXDMAXOR && !xfer_info->ktime_run) {
			xfer_info->ktime_run = ktime_get();
			xfer_info->chan_id = chan->id;
		}
	}
	chan->chip->desc_starting = first;
	ax_dmaxor_write_chan_llp(chan, first->hw_desc[0].llp);
	ax_dmaxor_irq_enable(chan->chip);
	ax_chan_irq_set(chan, AX_DMAXOR_CHAN_IRQ_MASK);
	ax_chan_enable(chan);
}

static void ax_dmaxor_chan_start_first_queued(struct ax_dmaxor_chan *chan)
{
	struct ax_dmaxor_desc *desc;
	struct virt_dma_desc *vd;
	vd = vchan_next_desc(&chan->vc);
	if (!vd) {
		pr_err("err vd null\n");
		chan->chip->start = 0;
		return;
	}

	desc = vd_to_ax_desc(vd);
	dev_vdbg(chan2dev(chan), "%s: started %u\n", ax_chan_name(chan),
		 vd->tx.cookie);
	ax_dmaxor_chan_block_xfer_start(chan, desc);
}

/* from chan->alloc_list checkout vc.desc_submitted to chan->pending_list */
static void ax_dmaxor_alloc_submit2pending(struct ax_dmaxor_chan *chan)
{
	struct ax_dmaxor_desc *desc = NULL;
	struct virt_dma_desc *vd = NULL, *__vd = NULL;

	list_for_each_entry_safe(vd, __vd, &chan->vc.desc_submitted, node) {
		desc = vd_to_ax_desc(vd);
		list_move_tail(&desc->desc_node, &chan->chip->pending_list);
	}
	return;
}

static void ax_dmaxor_chan_issue_pending(struct dma_chan *dchan)
{
	struct ax_dmaxor_chan *chan = dchan_to_ax_dmaxor_chan(dchan);
	struct ax_dmaxor_desc *desc;
	unsigned long flags, __flags;

	spin_lock_irqsave(&chan->chip->lock, __flags);
	spin_lock_irqsave(&chan->vc.lock, flags);
	ax_dmaxor_alloc_submit2pending(chan);
	vchan_issue_pending(&chan->vc);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
	if (chan->chip->start == 0) {
		desc = list_first_entry_or_null(&chan->chip->pending_list,
						struct ax_dmaxor_desc,
						desc_node);
		if (!desc) {
			spin_unlock_irqrestore(&chan->chip->lock, __flags);
			return;
		}
		chan->chip->start = 1;
		spin_lock(&desc->chan->vc.lock);
		ax_dmaxor_chan_start_first_queued(desc->chan);
		spin_unlock(&desc->chan->vc.lock);

		spin_unlock_irqrestore(&chan->chip->lock, __flags);
	} else {
		spin_unlock_irqrestore(&chan->chip->lock, __flags);
	}
}

static int ax_dmaxor_chan_alloc_chan_resources(struct dma_chan *dchan)
{
	return 0;
}

static void ax_dmaxor_chan_free_chan_resources(struct dma_chan *dchan)
{
	struct ax_dmaxor_chan *chan = dchan_to_ax_dmaxor_chan(dchan);
	struct ax_dmaxor_desc *desc = NULL, *__desc = NULL;
	unsigned long flags;

	spin_lock_irqsave(&chan->chip->lock, flags);
	list_for_each_entry_safe(desc, __desc, &chan->chip->pending_list,
				 desc_node) {
		if (desc && desc->chan == chan)
			list_del(&desc->desc_node);
	}
	INIT_LIST_HEAD(&chan->alloc_list);
	spin_unlock_irqrestore(&chan->chip->lock, flags);

	vchan_free_chan_resources(&chan->vc);
}

static void ax_dmaxor_set_desc_last(struct ax_dmaxor_hw_desc *hw_desc)
{
	hw_desc->lli->ctrl |= BIT(AX_DMAXOR_LAST_SHIFT);
}

static void ax_dmaxor_write_desc_sar(struct ax_dmaxor_hw_desc *hw_desc,
				     dma_addr_t * src, unsigned int src_cnt,
				     size_t step)
{
	hw_desc->lli->src = cpu_to_le64(src[0]) + step;
	while (--src_cnt) {
		hw_desc->lli->sar_buf[src_cnt - 1] =
		    cpu_to_le64(src[src_cnt]) + step;
	}
}

static void ax_dmaxor_write_desc_dar(struct ax_dmaxor_hw_desc *hw_desc,
				     dma_addr_t adr, size_t step)
{
	hw_desc->lli->dst = cpu_to_le64(adr) + step;
}

static void ax_dmaxor_chan_dump_lli(struct ax_dmaxor_chan *chan,
				    struct ax_dmaxor_hw_desc *desc)
{
	int i;
	dev_err(dchan2dev(&chan->vc.chan),
		"sar: 0x%llx dar: 0x%llx xorch_and_size 0x%llx ctrl: 0x%llx llp: 0x%llx\n",
		le64_to_cpu(desc->lli->src), le64_to_cpu(desc->lli->dst),
		le64_to_cpu(desc->lli->xorch_and_size),
		le64_to_cpu(desc->lli->ctrl), le64_to_cpu(desc->lli->llp));
	for (i = 0; i < 15; i++) {
		dev_err(dchan2dev(&chan->vc.chan), "sar[%d]: 0x%llx ", i + 1,
			le64_to_cpu(desc->lli->sar_buf[i]));
	}
	dev_err(dchan2dev(&chan->vc.chan), "\n");
}

static void ax_dmaxor_chan_list_dump_lli(struct ax_dmaxor_chan *chan,
					 struct ax_dmaxor_desc *desc_head)
{
	int i;
	int count = atomic_read(&desc_head->hw_descs_allocated);

	for (i = 0; i < count; i++)
		ax_dmaxor_chan_dump_lli(chan, &desc_head->hw_desc[i]);
}

static void
ax_dmaxor_set_ctrl_data(unsigned long data, unsigned long long *ctrl_data,
			unsigned int shift, unsigned long mask)
{
	*ctrl_data = ((*ctrl_data) & (~(mask << shift))) | (data << shift);
}

static unsigned long long
ax_dmaxor_set_lli_ctrl(ax_dmaxor_xfer_mode_t dma_mode, int xor_ch,
		       int xfer_width)
{
	unsigned long long ctrl_data;
	ctrl_data = AX_DMAXOR_DEFAULT_CTRL;
	switch (dma_mode) {
	case AX_DMAXOR_XOR_CALI:
		ax_dmaxor_set_ctrl_data(0, &ctrl_data, AX_DMAXOR_WB_SHIFT,
					AX_DMAXOR_WB_MASK);
		ax_dmaxor_set_ctrl_data(1, &ctrl_data, AX_DMAXOR_CHKSUM_SHIFT,
					AX_DMAXOR_CHKSUM_MASK);
		ax_dmaxor_set_ctrl_data(xor_ch, &ctrl_data,
					AX_DMAXOR_XOR_NUM_SHIFT,
					AX_DMAXOR_XOR_NUM_MASK);
		ax_dmaxor_set_ctrl_data(AX_DMAXOR_XOR_WB, &ctrl_data,
					AX_DMAXOR_TYPE_SHIFT,
					AX_DMAXOR_TYPE_MASK);
		break;
	case AX_DMAXOR_XOR_WB:
		ax_dmaxor_set_ctrl_data(xor_ch, &ctrl_data,
					AX_DMAXOR_XOR_NUM_SHIFT,
					AX_DMAXOR_XOR_NUM_MASK);
		ax_dmaxor_set_ctrl_data(1, &ctrl_data, AX_DMAXOR_WB_SHIFT,
					AX_DMAXOR_WB_MASK);
		ax_dmaxor_set_ctrl_data(0, &ctrl_data, AX_DMAXOR_CHKSUM_SHIFT,
					AX_DMAXOR_CHKSUM_MASK);
		ax_dmaxor_set_ctrl_data(dma_mode, &ctrl_data,
					AX_DMAXOR_TYPE_SHIFT,
					AX_DMAXOR_TYPE_MASK);
		break;
	case AX_DMAXOR_MEMORY_INIT:
		ax_dmaxor_set_ctrl_data(1, &ctrl_data, AX_DMAXOR_WB_SHIFT,
					AX_DMAXOR_WB_MASK);
		ax_dmaxor_set_ctrl_data(0, &ctrl_data, AX_DMAXOR_CHKSUM_SHIFT,
					AX_DMAXOR_CHKSUM_MASK);
		ax_dmaxor_set_ctrl_data(dma_mode, &ctrl_data,
					AX_DMAXOR_TYPE_SHIFT,
					AX_DMAXOR_TYPE_MASK);
		ax_dmaxor_set_ctrl_data(AX_DMAXOR_AXILEN_32BIT, &ctrl_data,
					AX_DMAXOR_ARLEN_SHIFT,
					AX_DMAXOR_ARLEN_MASK);
		ax_dmaxor_set_ctrl_data(AX_DMAXOR_AXILEN_32BIT, &ctrl_data,
					AX_DMAXOR_AWLEN_SHIFT,
					AX_DMAXOR_AWLEN_MASK);
		break;
	default:
		break;
	}
	ax_dmaxor_set_ctrl_data(0, &ctrl_data, AX_DMAXOR_LAST_SHIFT,
				AX_DMAXOR_LAST_MASK);
	ax_dmaxor_set_ctrl_data(xfer_width, &ctrl_data,
				AX_DMAXOR_SRC_TR_WIDTH_SHIFT,
				AX_DMAXOR_SRC_TR_WIDTH_MASK);
	ax_dmaxor_set_ctrl_data(xfer_width, &ctrl_data,
				AX_DMAXOR_DST_TR_WIDTH_SHIFT,
				AX_DMAXOR_DST_TR_WIDTH_MASK);
	return ctrl_data;
}

static struct
dma_async_tx_descriptor *ax_dmaxor_chan_prep_mem_to_mem(struct dma_chan *dchan,
							dma_addr_t dst_addr,
							dma_addr_t * src,
							unsigned int src_cnt,
							size_t len,
							enum sum_check_flags
							*result,
							unsigned long flags,
							ax_dmaxor_xfer_mode_t
							dma_mode)
{
	struct ax_dmaxor_chan *chan = dchan_to_ax_dmaxor_chan(dchan);
	size_t block_ts, xfer_len, tmp_size;
	struct ax_dmaxor_hw_desc *hw_desc = NULL;
	struct ax_dmaxor_desc *desc = NULL;
	struct dma_async_tx_descriptor *tx = NULL;
	u32 xfer_width, num, __num;
	u64 llp = 0;
	size_t step = 0;
	unsigned long __flags;

	dev_vdbg(dchan2dev(dchan),
		 "%s: dma_mode %d :src_cnt %d src0: 0x%llx dest: 0x%llx length: %zd flags: %#lx",
		 ax_chan_name(chan), dma_mode, src_cnt, src[0], dst_addr, len,
		 flags);

	num = len >> 23;
	tmp_size = len & 0x7FFFFF;
	num += tmp_size / DMAXOR_TR_2B_MAXSIZE;
	tmp_size %= DMAXOR_TR_2B_MAXSIZE;
	num += DIV_ROUND_UP(tmp_size, DMAXOR_TR_1B_MAXSIZE);
	tmp_size = len;
	desc = ax_desc_alloc(chan->chip, num);
	if (unlikely(!desc))
		goto err_xor_desc_get;
	desc->chan = chan;
	desc->xor_check_result = result;
	__num = 0;
	desc->length = 0;
	while (len) {
		if (__num < num - 1) {
			if (tmp_size > DMAXOR_TR_4B_MAXSIZE) {
				xfer_len = DMAXOR_TR_4B_MAXSIZE;
			} else if (tmp_size > DMAXOR_TR_2B_MAXSIZE) {
				xfer_len = DMAXOR_TR_2B_MAXSIZE;
			} else if (tmp_size > DMAXOR_TR_1B_MAXSIZE) {
				xfer_len = DMAXOR_TR_1B_MAXSIZE;
			} else {
				xfer_len = tmp_size;
			}
			tmp_size -= xfer_len;
		} else {
			xfer_len = len;
		}
		xfer_width = ax_chan_get_xfer_width(chan, xfer_len);
		block_ts = xfer_len >> xfer_width;
		hw_desc = &desc->hw_desc[__num];
		hw_desc->lli = ax_desc_pool_get(chan, desc, &hw_desc->llp);
		if (unlikely(!hw_desc->lli))
			goto err_xor_desc_get;

		if (dma_mode == AX_DMAXOR_MEMORY_INIT) {
			ax_dmaxor_write_desc_sar(hw_desc, src, src_cnt, 0);
			hw_desc->lli->xorch_and_size = block_ts;
		} else {
			ax_dmaxor_write_desc_sar(hw_desc, src, src_cnt, step);
			hw_desc->lli->xorch_and_size = block_ts |
			    GENMASK_ULL(LLI_CHAN_EN_SHIFT + src_cnt - 1,
					LLI_CHAN_EN_SHIFT);
		}
		ax_dmaxor_write_desc_dar(hw_desc, dst_addr, step);
		hw_desc->len = xfer_len;

		hw_desc->lli->ctrl = ax_dmaxor_set_lli_ctrl(dma_mode, src_cnt,
							    xfer_width);
		dev_vdbg(dchan2dev(dchan),
			 "%s lli->ctrl 0x%llx xorch_and_size 0x%llx\n",
			 ax_chan_name(chan), hw_desc->lli->ctrl,
			 hw_desc->lli->xorch_and_size);
		desc->length += hw_desc->len;
		/* update the length and addresses for the next loop cycle */
		step += xfer_len;
		len -= xfer_len;
		__num++;
	}
	/* Set end-of-link to the last link descriptor of list */
	ax_dmaxor_set_desc_last(&desc->hw_desc[__num - 1]);
	/* Managed transfer list */
	do {
		hw_desc = &desc->hw_desc[--__num];
		ax_dmaxor_write_desc_llp(hw_desc, llp);
		llp = hw_desc->llp;
	} while (__num);

	spin_lock_irqsave(&chan->chip->lock, __flags);
	list_add_tail(&desc->desc_node, &chan->alloc_list);

	tx = vchan_tx_prep(&chan->vc, &desc->vd, flags);
	spin_unlock_irqrestore(&chan->chip->lock, __flags);
	return tx;

err_xor_desc_get:
	dev_err(dchan2dev(dchan), "%s err: err_xor_desc_get\n",
		ax_chan_name(chan));
	if (desc) {
		ax_desc_put(desc);
	}
	return NULL;
}

static struct
dma_async_tx_descriptor *ax_dmaxor_chan_prep_dma_memset(struct dma_chan *dchan,
							dma_addr_t dst_addr,
							int value,
							size_t len,
							unsigned long flags)
{
	dma_addr_t set_val = value;
	/* dmaengine only support 32bit,copy 32bit */
	set_val = (set_val << 32) | (set_val & 0xFFFFFFFF);
	return ax_dmaxor_chan_prep_mem_to_mem(dchan, dst_addr, &set_val, 1, len,
					      NULL, flags,
					      AX_DMAXOR_MEMORY_INIT);
}

static struct
dma_async_tx_descriptor *ax_dmaxor_chan_prep_dma_xor(struct dma_chan *dchan,
						     dma_addr_t dst_addr,
						     dma_addr_t * src,
						     unsigned int src_cnt,
						     size_t len,
						     unsigned long flags)
{
	if (len & 0x7) {
		return NULL;
	}
	return ax_dmaxor_chan_prep_mem_to_mem(dchan, dst_addr, src, src_cnt,
					      len, NULL, flags,
					      AX_DMAXOR_XOR_WB);
}

static struct
dma_async_tx_descriptor *ax_dmaxor_chan_prep_dma_xor_val(struct dma_chan *dchan,
							 dma_addr_t * src,
							 unsigned int src_cnt,
							 size_t len,
							 enum sum_check_flags
							 *result,
							 unsigned long flags)
{
	*result = 0;
	return ax_dmaxor_chan_prep_mem_to_mem(dchan, 0, src, src_cnt, len,
					      result, flags,
					      AX_DMAXOR_XOR_CALI);
}

static noinline void ax_dmaxor_chan_handle_err(struct ax_dmaxor_chan *chan)
{
	struct virt_dma_desc *vd;
	unsigned long flags, __flags;
	struct ax_dmaxor_desc *desc = NULL;

	spin_lock_irqsave(&chan->chip->lock, __flags);
	spin_lock_irqsave(&chan->vc.lock, flags);
	/* The bad descriptor currently is in the head of vc list */
	vd = vchan_next_desc(&chan->vc);
	desc = vd_to_ax_desc(vd);
	desc->stat = DMA_ERROR;
	if (desc->xor_check_result) {
		dev_err(dchan2dev(&chan->vc.chan), "%s\n", ax_chan_name(chan));
		chan->vc.chan.private = desc->xor_check_result;
		*desc->xor_check_result |= SUM_CHECK_P_RESULT;
	}
	/* Remove the completed descriptor from issued list */
	list_del(&vd->node);
	/* WARN about bad descriptor */
	dev_vdbg(dchan2dev(&chan->vc.chan),
		 "Bad descriptor submitted for %s, cookie: %d, irq: 0x%08x\n",
		 ax_chan_name(chan), vd->tx.cookie, chan->irq_status);

	ax_dmaxor_chan_list_dump_lli(chan, vd_to_ax_desc(vd));
	vchan_cookie_complete(vd);

	list_del(&desc->desc_node);
	ax_chan_dsiable(chan);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
	desc = list_first_entry_or_null(&chan->chip->pending_list,
					struct ax_dmaxor_desc, desc_node);
	if (!desc) {
		chan->chip->start = 0;
		spin_unlock_irqrestore(&chan->chip->lock, __flags);
		return;
	}
	chan->chip->start = 1;
	spin_lock(&desc->chan->vc.lock);
	ax_dmaxor_chan_start_first_queued(desc->chan);
	spin_unlock(&desc->chan->vc.lock);

	spin_unlock_irqrestore(&chan->chip->lock, __flags);
}

static noinline void
ax_dmaxor_chan_block_xfer_complete(struct ax_dmaxor_chan *chan)
{
	struct virt_dma_desc *vd;
	unsigned long flags, __flags;
	struct ax_dmaxor_desc *desc = NULL;

	spin_lock_irqsave(&chan->chip->lock, __flags);
	spin_lock_irqsave(&chan->vc.lock, flags);
	/* The completed descriptor currently is in the head of vc list */
	vd = vchan_next_desc(&chan->vc);
	desc = vd_to_ax_desc(vd);
	desc->stat = DMA_COMPLETE;

	if (desc->xor_check_result
	    && chan->irq_status & AX_DMAXOR_XOR_CHECK_ERR) {
		chan->vc.chan.private = desc->xor_check_result;
		*desc->xor_check_result |= SUM_CHECK_P_RESULT;
	}
	/* Remove the completed descriptor from issued list before completing */
	list_del(&vd->node);
	vchan_cookie_complete(vd);

	list_del(&desc->desc_node);	//del from pending_list
	ax_chan_dsiable(chan);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
	desc = list_first_entry_or_null(&chan->chip->pending_list,
					struct ax_dmaxor_desc, desc_node);
	if (!desc) {
		chan->chip->start = 0;
		spin_unlock_irqrestore(&chan->chip->lock, __flags);
		return;
	}

	spin_lock(&desc->chan->vc.lock);
	ax_dmaxor_chan_start_first_queued(desc->chan);
	spin_unlock(&desc->chan->vc.lock);

	spin_unlock_irqrestore(&chan->chip->lock, __flags);
}

static irqreturn_t ax_dmaxor_interrupt(int irq, void *dev_id)
{
	struct ax_dmaxor_chip *chip = dev_id;
	struct ax_dmaxor *axdmaxor = chip->axdmaxor;
	struct ax_dmaxor_chan *chan;
	u32 chan_id;

	chan_id = __fls(ax_get_cur_id(chip));
	chan = &axdmaxor->chan[chan_id];
	chan->irq_status = ax_chan_irq_read(chan);
	ax_chan_irq_clear(chan, AX_DMAXOR_IRQ_ALL);
	ax_chan_irq_disable(chan);
	dev_vdbg(dchan2dev(&chan->vc.chan), "%s %d IRQ status: 0x%08x\n",
		 ax_chan_name(chan), chan_id, chan->irq_status);
	if (chan->irq_status & AX_DMAXOR_XFER_SUCCESS ||
	    chan->irq_status & AX_DMAXOR_XOR_CHECK_ERR) {
		ax_dmaxor_chan_block_xfer_complete(chan);
	} else if (chan->irq_status & AX_DMAXOR_ALL_ERR) {
		ax_dmaxor_chan_handle_err(chan);
	}
	return IRQ_HANDLED;
}

static int ax_dmaxor_chan_terminate_all(struct dma_chan *dchan)
{
	struct ax_dmaxor_chan *chan = dchan_to_ax_dmaxor_chan(dchan);
	struct ax_dmaxor_chip *chip = chan->chip;
	unsigned long flags, __flags;
	struct ax_dmaxor_desc *desc = NULL;
	struct ax_dmaxor_desc *__desc = NULL;
	struct ax_dmaxor_desc *first_desc = NULL;
	LIST_HEAD(head);

	spin_lock_irqsave(&chan->chip->lock, __flags);
	spin_lock_irqsave(&chan->vc.lock, flags);
	first_desc = list_first_entry_or_null(&chan->chip->pending_list,
					      struct ax_dmaxor_desc, desc_node);
	list_for_each_entry_safe(desc, __desc, &chan->chip->pending_list,
				 desc_node) {
		if (desc && desc->chan == chan)
			list_del(&desc->desc_node);
	}
	INIT_LIST_HEAD(&chan->alloc_list);
	if (first_desc && first_desc == chip->desc_starting &&
	    first_desc->chan == chan) {
		ax_dmaxor_reset(chip);
		desc = list_first_entry_or_null(&chip->pending_list,
						struct ax_dmaxor_desc,
						desc_node);
		if (desc) {
			chip->start = 1;
			spin_lock(&desc->chan->vc.lock);
			ax_dmaxor_chan_start_first_queued(desc->chan);
			spin_unlock(&desc->chan->vc.lock);
		}
	}
	vchan_get_all_descriptors(&chan->vc, &head);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
	spin_unlock_irqrestore(&chan->chip->lock, __flags);
	vchan_dma_desc_free_list(&chan->vc, &head);
	dev_vdbg(dchan2dev(dchan), "terminated: %s\n", ax_chan_name(chan));

	return 0;
}

static int __maybe_unused ax_dmaxor_suspend(struct device *dev)
{
	struct ax_dmaxor_chip *chip = dev_get_drvdata(dev);

	reset_control_assert(chip->rst);
	clk_disable_unprepare(chip->core_clk);
	ax_clr_cur_id(chip);
	return 0;
}

static int __maybe_unused ax_dmaxor_resume(struct device *dev)
{
	struct ax_dmaxor_chip *chip = dev_get_drvdata(dev);
	int ret;

	ret = clk_prepare_enable(chip->core_clk);
	if (ret)
		return ret;
	ret = reset_control_deassert(chip->rst);
	if (ret)
		return ret;
	ax_clr_cur_id(chip);
	return 0;
}

static struct dma_chan *ax_dmaxor_of_xlate(struct of_phandle_args *dma_spec,
					   struct of_dma *ofdma)
{
	struct ax_dmaxor *axdmaxor = ofdma->of_dma_data;
	struct ax_dmaxor_chan *chan;
	struct dma_chan *dchan;

	dchan = dma_get_any_slave_channel(&axdmaxor->dma);
	if (!dchan)
		return NULL;

	chan = dchan_to_ax_dmaxor_chan(dchan);
	return dchan;
}

static int ax_dmaxor_parse_device_properties(struct ax_dmaxor_chip *chip)
{
	struct device *dev = chip->dev;
	u32 tmp;
	int ret;
	ret = device_property_read_u32(dev, "dma-channels", &tmp);
	if (ret)
		return ret;
	if (tmp == 0 || tmp > DMAC_MAX_CHANNELS)
		return -EINVAL;

	chip->axdmaxor->hdata->nr_channels = tmp;

	ret = device_property_read_u32(dev, "ax,data-width", &tmp);
	if (ret)
		return ret;
	if (tmp > AXIDMAXOR_TRANS_WIDTH_MAX)
		return -EINVAL;

	chip->axdmaxor->hdata->m_data_width = tmp;

	ret = device_property_read_u32(dev, "ax,max-burst-len", &tmp);
	if (!ret) {
		if (tmp > AXIDMAXOR_ARWLEN_MAX + 1)
			return -EINVAL;
		if (tmp < AXIDMAXOR_ARWLEN_MIN + 1)
			return -EINVAL;

		chip->axdmaxor->hdata->axi_rw_burst_len = tmp - 1;
	}
	return 0;
}

static int ax_dmaxor_probe(struct platform_device *pdev)
{
	struct ax_dmaxor_chip *chip;
	struct resource *mem;
	struct ax_dmaxor *axdmaxor;
	struct ax_dmaxor_hcfg *hdata;
	u32 i;
	int ret;

	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip) {
		pr_err("\n");
		goto err_probe;
	}

	axdmaxor = devm_kzalloc(&pdev->dev, sizeof(*axdmaxor), GFP_KERNEL);
	if (!axdmaxor) {
		pr_err("\n");
		goto err_probe;
	}

	hdata =
	    devm_kzalloc(&pdev->dev, sizeof(struct ax_dmaxor_hcfg), GFP_KERNEL);
	if (!hdata) {
		pr_err("\n");
		goto err_probe;
	}

	chip->axdmaxor = axdmaxor;
	chip->dev = &pdev->dev;
	chip->axdmaxor->hdata = hdata;

	/* Set the dma mask bits */
	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	of_dma_configure(&pdev->dev, chip->dev->of_node, true);

	chip->irq = platform_get_irq(pdev, 0);
	if (chip->irq < 0) {
		pr_err("\n");
		goto err_probe;
	}

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	chip->regs = devm_ioremap_resource(chip->dev, mem);
	if (IS_ERR(chip->regs)) {
		pr_err("\n");
		goto err_probe;
	}

	chip->rst = devm_reset_control_get_optional(&pdev->dev, "dmaxor-rst");
	if (IS_ERR(chip->rst)) {
		pr_err("Err\n");
		return PTR_ERR(chip->rst);
	}

	chip->core_clk = devm_clk_get(chip->dev, "dmaxor-clk");
	if (IS_ERR(chip->core_clk)) {
		pr_err("\n");
		goto err_probe;
	}

	ret = ax_dmaxor_parse_device_properties(chip);
	if (ret) {
		pr_err("\n");
		goto err_probe;
	}

	axdmaxor->chan = devm_kcalloc(chip->dev, hdata->nr_channels,
				      sizeof(*axdmaxor->chan), GFP_KERNEL);
	if (!axdmaxor->chan) {
		pr_err("\n");
		goto err_probe;
	}

	chip->start = 0;
	spin_lock_init(&chip->lock);
	INIT_LIST_HEAD(&chip->pending_list);
	ret = devm_request_irq(chip->dev, chip->irq, ax_dmaxor_interrupt,
			       IRQF_SHARED, KBUILD_MODNAME, chip);
	if (ret) {
		pr_err("\n");
		goto err_probe;
	}

	INIT_LIST_HEAD(&axdmaxor->dma.channels);
	for (i = 0; i < hdata->nr_channels; i++) {
		struct ax_dmaxor_chan *chan = &axdmaxor->chan[i];
		INIT_LIST_HEAD(&chan->alloc_list);
		chan->chip = chip;
		chan->id = i;
		chan->chan_regs = chip->regs + AX_DMAXOR_CHAN_REG_BASE;
		chan->vc.desc_free = vchan_desc_put;
		vchan_init(&chan->vc, &axdmaxor->dma);
	}

	/* Set capabilities */
	dma_cap_set(DMA_XOR, axdmaxor->dma.cap_mask);
	dma_cap_set(DMA_XOR_VAL, axdmaxor->dma.cap_mask);
	dma_cap_set(DMA_MEMSET, axdmaxor->dma.cap_mask);

	/* DMA capabilities */
	axdmaxor->dma.chancnt = hdata->nr_channels;
	axdmaxor->dma.max_burst = hdata->axi_rw_burst_len;
	axdmaxor->dma.src_addr_widths = AX_DMAXOR_BUSWIDTHS;
	axdmaxor->dma.dst_addr_widths = AX_DMAXOR_BUSWIDTHS;
	axdmaxor->dma.directions = BIT(DMA_MEM_TO_MEM);
	axdmaxor->dma.dev = chip->dev;
	axdmaxor->dma.device_tx_status = ax_dmaxor_chan_tx_status;
	axdmaxor->dma.device_issue_pending = ax_dmaxor_chan_issue_pending;
	axdmaxor->dma.device_terminate_all = ax_dmaxor_chan_terminate_all;
	axdmaxor->dma.device_alloc_chan_resources =
	    ax_dmaxor_chan_alloc_chan_resources;
	axdmaxor->dma.device_free_chan_resources =
	    ax_dmaxor_chan_free_chan_resources;
	axdmaxor->dma.device_prep_dma_memset = ax_dmaxor_chan_prep_dma_memset;
	axdmaxor->dma.max_xor = AX_DMAXOR_MAX_XOR_CNT;
	axdmaxor->dma.xor_align = AX_DMAXOR_ALIGN;
	axdmaxor->dma.device_prep_dma_xor = ax_dmaxor_chan_prep_dma_xor;
	axdmaxor->dma.device_prep_dma_xor_val = ax_dmaxor_chan_prep_dma_xor_val;
	axdmaxor->dma.dev->dma_parms = &axdmaxor->dma_parms;
	platform_set_drvdata(pdev, chip);

	/*
	 * We can't just call pm_runtime_get here instead of
	 * pm_runtime_get_noresume + ax_dmaxor_resume because we need
	 * driver to work also without Runtime PM.
	 */
	ret = ax_dmaxor_resume(chip->dev);
	if (ret < 0) {
		pr_err("\n");
		goto err_probe;
	}
	ax_dmaxor_hw_init(chip);

	/* LLI address must be aligned to a 64-byte boundary */
	chip->desc_pool = dmam_pool_create(dev_name(chip->dev), chip->dev,
					  AX_DMAXOR_LLI_SIZE, 64, 0);
	if (!chip->desc_pool) {
		dev_err(chip->dev, "No memory for descriptors\n");
		goto err_init;
	}

	/* Register DMA */
	ret = dmaenginem_async_device_register(&axdmaxor->dma);
	if (ret) {
		pr_err("\n");
		goto err_init;
	}

	chip->desc_kmem =
		ax_kmem_creat(DMAXOR_ALLOC_SIZE_1M, sizeof(struct ax_dmaxor_desc), GFP_ATOMIC | __GFP_ZERO);
	if (!chip->desc_kmem) {
		pr_err("ax_kmem_creat err alloc size %d Byte\n", DMAXOR_ALLOC_SIZE_1M);
		goto err_init;
	}

	/* Register with OF helpers for DMA lookups */
	ret = of_dma_controller_register(pdev->dev.of_node,
					 ax_dmaxor_of_xlate, axdmaxor);
	if (ret < 0)
		dev_warn(&pdev->dev, "Failed to register OF DMA controller");
	dev_info(chip->dev, "Axera AXI DMAXOR Controller, %d channels\n",
		 axdmaxor->hdata->nr_channels);
	return 0;
err_init:
	ax_dmaxor_suspend(&pdev->dev);
err_probe:
	return ret;
}

static int ax_dmaxor_remove(struct platform_device *pdev)
{
	struct ax_dmaxor_chip *chip = platform_get_drvdata(pdev);
	struct ax_dmaxor *axdmaxor = chip->axdmaxor;
	struct ax_dmaxor_chan *chan, *_chan;

	ax_dmaxor_irq_disable(chip);
	ax_chan_irq_disable(&chip->axdmaxor->chan[0]);
	ax_dmaxor_suspend(chip->dev);

	devm_free_irq(chip->dev, chip->irq, chip);

	of_dma_controller_free(chip->dev->of_node);

	list_for_each_entry_safe(chan, _chan, &axdmaxor->dma.channels,
				 vc.chan.device_node) {
		list_del(&chan->vc.chan.device_node);
		tasklet_kill(&chan->vc.task);
	}
	ax_kmem_destroy(chip->desc_kmem);
	return 0;
}

static const struct dev_pm_ops ax_dmaxor_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(ax_dmaxor_suspend, ax_dmaxor_resume)
};

static const struct of_device_id ax_dmaxor_of_id_table[] = {
	{.compatible = "axera,axi-dmaxor"},
	{}
};

MODULE_DEVICE_TABLE(of, ax_dmaxor_of_id_table);

static struct platform_driver ax_dmaxor_driver = {
	.probe = ax_dmaxor_probe,
	.remove = ax_dmaxor_remove,
	.driver = {
		   .name = AX_DMAXOR_DRV,
		   .of_match_table = of_match_ptr(ax_dmaxor_of_id_table),
		   .pm = &ax_dmaxor_pm_ops,
		   },
};

static int __init ax_dmaxor_init(void)
{
	return platform_driver_register(&ax_dmaxor_driver);
}
subsys_initcall_sync(ax_dmaxor_init);

static void __exit ax_dmaxor_exit(void)
{
	platform_driver_unregister(&ax_dmaxor_driver);
}
module_exit(ax_dmaxor_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Axera AXI DMAXOR Controller platform driver");
MODULE_AUTHOR("Axera");
