#define pr_fmt(fmt) KBUILD_MODNAME ":%s:%d: " fmt, __func__, __LINE__

#include "dmaxor.h"

#define DEVICE_NAME "ax_dmaxor"
#define AX_DMAXOR_PROC_NAME "ax_proc/dmaxor"

static DEFINE_IDR(xfer_idr);
static ax_dmaxor_info_t glb_dma_info;
static ax_dmaxor_dbg_t g_dbg_buf;
static char *ax_dmaxor_mode[3] = {
	"mem-set  ", "xor-wb   ", "xor-check"
};

static void kfree_set_null(void *p)
{
	kfree(p);
	p = NULL;
}

//When dma transfer finished,this function will be called.
static void ax_dma_callback_func(void *arg)
{
	unsigned long flags;
	ax_dmaxor_xfer_info_t *xfer_info = (ax_dmaxor_xfer_info_t *) arg;
	ax_dmaxor_msg_t *dmaxor_msg = &xfer_info->dmaxor_msg;
	ax_dmaxor_chn_t *dma_chn =
	    (ax_dmaxor_chn_t *)xfer_info->filp->private_data;

	if (dmaxor_msg->dma_mode == AX_DMAXOR_XOR_CALI)
		xfer_info->cali |= xfer_info->xfer_stat.xor_cali <<
				   (dmaxor_msg->desc_num - xfer_info->desc_num);
	xfer_info->desc_num--;
	if (!(xfer_info->desc_num)) {
		xfer_info->xfer_stat.stat = AX_DMAXOR_XFER_SUCCESS;
		spin_lock_irqsave(&glb_dma_info.lock, flags);
		list_move_tail(&xfer_info->element,
			       &glb_dma_info.dmaxor_complite_list);
		spin_unlock_irqrestore(&glb_dma_info.lock, flags);
		xfer_info->ktime_end = ktime_get();
		wake_up_interruptible(&dma_chn->event_wait);
	}
}

static bool ax_dmaxor_chan_filter(struct dma_chan *chan, void *param)
{
	if (dma_has_cap(DMA_MEMSET, chan->device->cap_mask) &&
	    dma_has_cap(DMA_XOR, chan->device->cap_mask))
		return true;
	return false;
}

static ssize_t ax_dmaxor_setsync(struct file *filp, const char __user *buffer,
				 size_t count, loff_t *ppos)
{
	ax_dmaxor_chn_t *dma_chn = (ax_dmaxor_chn_t *)filp->private_data;

	if (count == 0)
		return 0;

	if (copy_from_user
	    ((void *)&dma_chn->file_msg, buffer, sizeof(ax_dmaxor_file_msg_t)))
		return -EFAULT;

	return count;
}

static ssize_t ax_dmaxor_getsync(struct file *filp, char __user *buffer,
				 size_t count, loff_t *ppos)
{
	ax_dmaxor_chn_t *dma_chn = (ax_dmaxor_chn_t *)filp->private_data;

	if (count == 0)
		return 0;

	return copy_to_user(buffer, (void *)&dma_chn->file_msg,
			    sizeof(ax_dmaxor_file_msg_t));
}

static s32 ax_dma_get_id(ax_dmaxor_info_t *dmaxor_info, void *arg)
{
	s32 id;
	unsigned long flags;
	spin_lock_irqsave(&glb_dma_info.lock, flags);
	id = idr_alloc_cyclic(&xfer_idr, arg, 1, 1024, GFP_KERNEL);
	spin_unlock_irqrestore(&glb_dma_info.lock, flags);
	return id;
}

static void ax_dma_free_id(ax_dmaxor_info_t *dmaxor_info, s32 id)
{
	unsigned long flags;
	spin_lock_irqsave(&glb_dma_info.lock, flags);
	idr_remove(&xfer_idr, id);
	spin_unlock_irqrestore(&glb_dma_info.lock, flags);
}

static ax_dmaxor_xfer_info_t *ax_dmaxor_find_entry_by_filp(struct list_head
							   *list,
							   struct file *filp)
{
	ax_dmaxor_xfer_info_t *xfer_info = NULL;
	list_for_each_entry(xfer_info, list, element) {
		if (xfer_info->filp == filp)
			return xfer_info;
	}
	return NULL;
}

static ax_dmaxor_xfer_info_t *ax_dmaxor_find_entry_by_id(struct list_head
							   *list, s32 id)
{
	ax_dmaxor_xfer_info_t *xfer_info = NULL;
	list_for_each_entry(xfer_info, list, element) {
		if (xfer_info->xfer_stat.id == id)
			return xfer_info;
	}
	return NULL;
}

static void ax_dmaxor_free_xfer_info(ax_dmaxor_xfer_info_t *xfer_info)
{
	ax_dma_free_id(&glb_dma_info, xfer_info->xfer_stat.id);
	kfree_set_null(xfer_info->dmaxor_msg.dma_chn_cfg);
	kfree_set_null(xfer_info);
}

static s32 ax_dmaxor_free_by_id_filp(struct list_head *list,
					struct file *filp, void *arg)
{
	s32 ret = 0;
	unsigned long flags;
	ax_dmaxor_xfer_info_t *xfer_info = NULL;
	struct ax_dmaxor_chan *achan = NULL;
	ax_dmaxor_stat_t dmaxor_stat;

	if (copy_from_user((void *)&dmaxor_stat, (void *)arg,
				sizeof(ax_dmaxor_stat_t))) {
		pr_err("err\n");
		return -EIO;
	}
	spin_lock_irqsave(&glb_dma_info.lock, flags);
	if (dmaxor_stat.xfer_stat.id)
		xfer_info = ax_dmaxor_find_entry_by_id(list,
			      dmaxor_stat.xfer_stat.id);
	else
		xfer_info = ax_dmaxor_find_entry_by_filp(list, filp);
	if (!xfer_info) {
		spin_unlock_irqrestore(&glb_dma_info.lock, flags);
		return -EFAULT;
	}
	list_del(&xfer_info->element);
	achan = dchan_to_ax_dmaxor_chan(xfer_info->chan);
	atomic64_dec(&g_dbg_buf.chan_total[achan->id]);
	g_dbg_buf.last = (g_dbg_buf.last + 1) & 0x7;
	g_dbg_buf.info[g_dbg_buf.last].chan_id = xfer_info->chan_id;
	g_dbg_buf.info[g_dbg_buf.last].msg_id = dmaxor_stat.xfer_stat.id;
	g_dbg_buf.info[g_dbg_buf.last].start = xfer_info->time_start / 1000000000;
	g_dbg_buf.info[g_dbg_buf.last].wait = xfer_info->ktime_run - xfer_info->ktime_start;
	g_dbg_buf.info[g_dbg_buf.last].run = xfer_info->ktime_end - xfer_info->ktime_run;
	g_dbg_buf.info[g_dbg_buf.last].size = xfer_info->size;
	g_dbg_buf.info[g_dbg_buf.last].dma_mode =
	    xfer_info->dmaxor_msg.dma_mode;
	spin_unlock_irqrestore(&glb_dma_info.lock, flags);
	if (arg) {
		dmaxor_stat.xfer_stat.xor_cali = xfer_info->cali;
		dmaxor_stat.pFun = xfer_info->dmaxor_msg.pFun;
		dmaxor_stat.arg = xfer_info->dmaxor_msg.arg;
		ret = copy_to_user((void __user *)arg, &dmaxor_stat,
				    sizeof(ax_dmaxor_stat_t));
	}
	ax_dmaxor_free_xfer_info(xfer_info);

	return ret;
}

static s32 ax_dmaxor_open(struct inode *inode, struct file *filp)
{
	s32 ret = 0;
	dma_cap_mask_t mask;
	ax_dmaxor_chn_t *dma_chn =
	    (ax_dmaxor_chn_t *) kzalloc(sizeof(ax_dmaxor_chn_t), GFP_KERNEL);
	filp->private_data = (void *)dma_chn;

	dma_cap_zero(mask);
	dma_cap_set(DMA_XOR_VAL, mask);
	dma_chn->chan = dma_request_channel(mask, ax_dmaxor_chan_filter, NULL);
	if (!dma_chn->chan) {
		pr_err("err: dma_request_channel failed!\n");
		kfree_set_null(dma_chn);
		ret = -ENOMEM;
	}
	init_waitqueue_head(&dma_chn->event_wait);
	mutex_init(&dma_chn->mutex_sync);
	return ret;
}

static s32 ax_dmaxor_free_list(struct file *filp)
{
	ax_dmaxor_xfer_info_t *xfer_info = NULL, *__msg_list = NULL;
	unsigned long flags;
	spin_lock_irqsave(&glb_dma_info.lock, flags);
	if (!list_empty(&glb_dma_info.dmaxor_start_list)) {
		list_for_each_entry_safe(xfer_info, __msg_list,
					 &glb_dma_info.dmaxor_start_list,
					 element) {
			if (xfer_info && xfer_info->filp == filp) {
				list_del(&xfer_info->element);
				kfree_set_null(xfer_info->
					       dmaxor_msg.dma_chn_cfg);
				kfree_set_null(xfer_info);
			}
		}
	}
	xfer_info = NULL;
	if (!list_empty(&glb_dma_info.dmaxor_complite_list)) {
		list_for_each_entry_safe(xfer_info, __msg_list,
					 &glb_dma_info.dmaxor_complite_list,
					 element) {
			if (xfer_info && xfer_info->filp == filp) {
				list_del(&xfer_info->element);
				kfree_set_null(xfer_info->
					       dmaxor_msg.dma_chn_cfg);
				kfree_set_null(xfer_info);
			}
		}
	}
	xfer_info = NULL;
	if (!list_empty(&glb_dma_info.dmaxor_free_list)) {
		list_for_each_entry_safe(xfer_info, __msg_list,
					 &glb_dma_info.dmaxor_free_list,
					 element) {
			if (xfer_info && xfer_info->filp == filp) {
				list_del(&xfer_info->element);
				kfree_set_null(xfer_info->
					       dmaxor_msg.dma_chn_cfg);
				kfree_set_null(xfer_info);
			}
		}
	}
	spin_unlock_irqrestore(&glb_dma_info.lock, flags);
	return 0;
}

static s32 ax_dmaxor_release(struct inode *inode, struct file *filp)
{
	ax_dmaxor_chn_t *dma_chn = (ax_dmaxor_chn_t *)filp->private_data;

	if (dma_chn->chan) {
		dmaengine_terminate_all(dma_chn->chan);
		dma_release_channel(dma_chn->chan);
	}
	dma_chn->chan = NULL;
	ax_dmaxor_free_list(filp);
	kfree_set_null(dma_chn);
	return 0;
}

static __poll_t ax_dmaxor_poll(struct file *filp,
			       struct poll_table_struct *wait)
{
	unsigned long flags;
	ax_dmaxor_xfer_info_t *xfer_info = NULL;
	ax_dmaxor_chn_t *dma_chn = (ax_dmaxor_chn_t *)filp->private_data;
	__poll_t mask = 0;

	poll_wait(filp, &dma_chn->event_wait, wait);
	spin_lock_irqsave(&glb_dma_info.lock, flags);
	xfer_info =
	    ax_dmaxor_find_entry_by_filp(&glb_dma_info.dmaxor_complite_list,
					 filp);
	if (!xfer_info) {
		spin_unlock_irqrestore(&glb_dma_info.lock, flags);
		return mask;
	}
	if (xfer_info->xfer_stat.stat == AX_DMAXOR_XFER_SUCCESS) {
		list_move_tail(&xfer_info->element,
			       &glb_dma_info.dmaxor_free_list);
		mask = POLLIN | POLLRDNORM;
	}
	spin_unlock_irqrestore(&glb_dma_info.lock, flags);
	if (dma_chn->file_msg.issync)
		mutex_unlock(&dma_chn->mutex_sync);
	return mask;
}

static s32 ax_dmaxor_start(ax_dmaxor_xfer_info_t *xfer_info)
{
	s32 i, ret = 0;
	struct dma_async_tx_descriptor *tx = NULL;
	ax_dmaxor_msg_t *dmaxor_msg = &xfer_info->dmaxor_msg;
	struct dma_device *dev = xfer_info->chan->device;
	dma_cookie_t cookie;
	ax_dmaxor_mem_chn_cfg_t *dma_chn_cfg_mem =
	    (ax_dmaxor_mem_chn_cfg_t *) dmaxor_msg->dma_chn_cfg;
	ax_dmaxor_xor_chn_cfg_t *dma_chn_cfg_xor =
	    (ax_dmaxor_xor_chn_cfg_t *) dmaxor_msg->dma_chn_cfg;

	xfer_info->desc_num = dmaxor_msg->desc_num;
	for (i = 0; i < dmaxor_msg->desc_num; i++) {
		if (dmaxor_msg->dma_mode == AX_DMAXOR_MEMORY_INIT) {
			xfer_info->size += (dma_chn_cfg_mem + i)->xfer_len;
			tx = dev->device_prep_dma_memset(xfer_info->chan,
					(dma_chn_cfg_mem + i)->dst_addr,
					(dma_chn_cfg_mem + i)->init_val,
					(dma_chn_cfg_mem + i)->xfer_len,
					DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
		} else if (dmaxor_msg->dma_mode == AX_DMAXOR_XOR_WB) {
			xfer_info->size += ((dma_chn_cfg_xor + i)->xfer_len *
					    (dma_chn_cfg_xor + i)->src_cnt);
			tx = dev->device_prep_dma_xor(xfer_info->chan,
					(dma_chn_cfg_xor + i)->dst_addr,
					(dma_chn_cfg_xor + i)->src_addr_buf,
					(dma_chn_cfg_xor + i)->src_cnt,
					(dma_chn_cfg_xor + i)->xfer_len,
					DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
		} else if (dmaxor_msg->dma_mode == AX_DMAXOR_XOR_CALI) {
			xfer_info->size += ((dma_chn_cfg_xor + i)->xfer_len *
					    (dma_chn_cfg_xor + i)->src_cnt);
			xfer_info->cali = 0;
			xfer_info->xfer_stat.xor_cali = 0;
			tx = dev->device_prep_dma_xor_val(xfer_info->chan,
					(dma_chn_cfg_xor + i)->src_addr_buf,
					(dma_chn_cfg_xor + i)->src_cnt,
					(dma_chn_cfg_xor + i)->xfer_len,
					(enum sum_check_flags *)&xfer_info->xfer_stat.xor_cali,
					DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
		}
		if (tx) {
			tx->callback = ax_dma_callback_func;	//set call back function
			tx->callback_param = (void *)xfer_info;
			cookie = tx->tx_submit(tx);	//submit the desc
			if (dma_submit_error(cookie)) {
				ret = -EFAULT;
			}
		} else {
			ret = -EFAULT;
		}
	}
	dma_async_issue_pending(xfer_info->chan);	//begin dma transfer

	return ret;
}

static long ax_dmaxor_ioctl(struct file *filp, u32 cmd, unsigned long arg)
{
	ax_dmaxor_msg_t *dmaxor_msg;
	ax_dmaxor_msg_t tmp_cmd_msg;
	ax_dmaxor_xfer_info_t *xfer_info;
	ax_dmaxor_chn_t *dma_chn = (ax_dmaxor_chn_t *)filp->private_data;
	struct ax_dmaxor_chan *achan = NULL;
	unsigned long flags;
	s32 ret = 0, id = 0;
	u32 msg_size;
	u64 __arg;

	switch (cmd) {
	case AX_DMAXOR_CFG_CHN_CMD:
		xfer_info = kzalloc(sizeof(ax_dmaxor_xfer_info_t), GFP_KERNEL);
		if (!xfer_info) {
			ret = -ENOMEM;
			goto cfg_fail;
		}
		xfer_info->filp = filp;
		dmaxor_msg = &xfer_info->dmaxor_msg;
		xfer_info->chan = dma_chn->chan;
		if (copy_from_user((void *)dmaxor_msg, (void *)arg,
				   sizeof(ax_dmaxor_msg_t))) {
			pr_err("err\n");
			ret = -EIO;
			goto cfg_fail;
		}
		memcpy(&tmp_cmd_msg, dmaxor_msg, sizeof(ax_dmaxor_msg_t));
		if (dmaxor_msg->dma_mode > AX_DMAXOR_MEMORY_INIT) {
			msg_size = sizeof(ax_dmaxor_xor_chn_cfg_t);
		} else {
			msg_size = sizeof(ax_dmaxor_mem_chn_cfg_t);
		}
		__arg = (u64) (dmaxor_msg->dma_chn_cfg);
		dmaxor_msg->dma_chn_cfg =
		    kzalloc(msg_size * (dmaxor_msg->desc_num), GFP_KERNEL);
		if (!dmaxor_msg->dma_chn_cfg) {
			pr_err("err\n");
			ret = -ENOMEM;
			goto cfg_fail;
		}
		if (copy_from_user((void *)(dmaxor_msg->dma_chn_cfg),
				   (void *)__arg,
				   msg_size * (dmaxor_msg->desc_num))) {
			pr_err("err\n");
			ret = -EIO;
			goto cfg_fail;
		}
		xfer_info->xfer_stat.stat = AX_DMAXOR_CFG_SUCCESS;
		xfer_info->xfer_stat.id =
		    ax_dma_get_id(&glb_dma_info, xfer_info);
		ret = xfer_info->xfer_stat.id;
		break;
cfg_fail:
		if (dmaxor_msg->dma_chn_cfg)
			kfree_set_null(dmaxor_msg->dma_chn_cfg);
		if (xfer_info)
			kfree_set_null(xfer_info);
		break;
	case AX_DMAXOR_START_CHN_CMD:
		if (unlikely
		    (copy_from_user((void *)(&id), (void *)arg, sizeof(s32)))) {
			pr_err("error\n");
			return -EIO;
		}
		spin_lock_irqsave(&glb_dma_info.lock, flags);
		xfer_info = (ax_dmaxor_xfer_info_t *) idr_find(&xfer_idr, id);
		spin_unlock_irqrestore(&glb_dma_info.lock, flags);
		if (xfer_info) {
			xfer_info->time_start = ktime_get_real();
			xfer_info->ktime_start = ktime_get();
			xfer_info->ktime_run = 0;
			xfer_info->type = AXDMAXOR;
			achan = dchan_to_ax_dmaxor_chan(xfer_info->chan);
			atomic64_inc(&g_dbg_buf.total);
			atomic64_inc(&g_dbg_buf.chan_total[achan->id]);
			if (dma_chn->file_msg.issync)
				mutex_lock(&dma_chn->mutex_sync);
			spin_lock_irqsave(&glb_dma_info.lock, flags);
			list_add_tail(&xfer_info->element,
				      &glb_dma_info.dmaxor_start_list);
			spin_unlock_irqrestore(&glb_dma_info.lock, flags);
			ret = ax_dmaxor_start(xfer_info);
			if (ret) {
				spin_lock_irqsave(&glb_dma_info.lock, flags);
				list_del(&xfer_info->element);
				spin_unlock_irqrestore(&glb_dma_info.lock,
						       flags);
				ax_dmaxor_free_xfer_info(xfer_info);
				if (dma_chn->file_msg.issync)
					mutex_unlock(&dma_chn->mutex_sync);
			}
		} else {
			ret = -EFAULT;
		}
		break;
	case AX_DMAXOR_GET_CALLBACK:
		ret = ax_dmaxor_free_by_id_filp
		    (&glb_dma_info.dmaxor_free_list, filp, (void *)arg);
		break;
	default:
		break;
	}
	return ret;
}

static const struct file_operations ax_dmaxor_fops = {
	.owner = THIS_MODULE,
	.open = ax_dmaxor_open,
	.release = ax_dmaxor_release,
	.read = ax_dmaxor_getsync,
	.write = ax_dmaxor_setsync,
	.unlocked_ioctl = ax_dmaxor_ioctl,
	.poll = ax_dmaxor_poll,
};

static struct miscdevice ax_dmaxor_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &ax_dmaxor_fops
};

static s32 ax_dmaxor_stat_show(struct seq_file *m, void *v)
{
	s32 i, count;

	seq_printf(m, "The number of all tasks so far: %lld\n", atomic64_read(&g_dbg_buf.total));
	seq_printf(m, "\nThe number of tasks that still need to be processed on each channel:\n");
	for (i = 0; i < MAX_DMA_CHAN; i++) {
		seq_printf(m, "chan-%-2d:    %-15lld\t", i, atomic64_read(&g_dbg_buf.chan_total[i]));
		if ((i & 3) == 3)
			seq_printf(m, "\n");
	}
	seq_printf(m, "\n");
	seq_printf(m, "type     \tchan-id\tmsg-id\tsize           \t"
		   "start          \twait(ns)       \trun(ns)        \t\n");
	for (i = 0; i < 8; i++) {
		count = (g_dbg_buf.last + i + 1) & 0x7;
		if (g_dbg_buf.info[count].dma_mode < AX_DMAXOR_MODE_OFFSET ||
		   !g_dbg_buf.info[count].size)
			continue;
		seq_printf(m, "%s\t%-2d     \t%-4d  \t%-15d\t%-15llu\t%-15llu\t%-15llu\t\n",
			   ax_dmaxor_mode[g_dbg_buf.info[count].dma_mode -
					  AX_DMAXOR_MODE_OFFSET],
			   g_dbg_buf.info[count].chan_id,
			   g_dbg_buf.info[count].msg_id,
			   g_dbg_buf.info[count].size,
			   g_dbg_buf.info[count].start,
			   g_dbg_buf.info[count].wait,
			   g_dbg_buf.info[count].run);
	}
	return 0;
}

static s32 ax_dmaxor_stat_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_dmaxor_stat_show, NULL);
}

static const struct proc_ops dmaxor_stat_ops = {
	.proc_open = ax_dmaxor_stat_open,
	.proc_read = seq_read,
	.proc_release = seq_release,
};

static s32 ax_dmaxor_proc_init(ax_dmaxor_info_t *info)
{
	info->file =
	    proc_create(AX_DMAXOR_PROC_NAME, 0644, NULL, &dmaxor_stat_ops);
	if (unlikely(!info->file)) {
		pr_err("err: Create proc fail!\n");
		return -EFAULT;
	}
	return 0;
}

static s32 ax_dmaxor_probe(struct platform_device *pdev)
{
	s32 i, ret;

	INIT_LIST_HEAD(&glb_dma_info.dmaxor_start_list);
	INIT_LIST_HEAD(&glb_dma_info.dmaxor_complite_list);
	INIT_LIST_HEAD(&glb_dma_info.dmaxor_free_list);
	spin_lock_init(&glb_dma_info.lock);
	ret = misc_register(&ax_dmaxor_miscdev);
	if (unlikely(ret)) {
		pr_err("err: %s misc_register failed!\n", __func__);
		goto end;
	}
	glb_dma_info.xfer_stat.stat = AX_DMAXOR_XFER_IDLE;
	ret = ax_dmaxor_proc_init(&glb_dma_info);
	if (unlikely(ret)) {
		pr_err("err\n");
		goto end;
	}
	atomic64_set(&g_dbg_buf.total, 0);
	for (i = 0; i < MAX_DMA_CHAN; i++)
		atomic64_set(&g_dbg_buf.chan_total[i], 0);
	pr_info("done.\n");
end:
	return ret;
}

static s32 ax_dmaxor_remove(struct platform_device *pdev)
{
	if (glb_dma_info.file)
		remove_proc_entry(AX_DMAXOR_PROC_NAME, NULL);
	misc_deregister(&ax_dmaxor_miscdev);
	return 0;
}

static const struct of_device_id ax_dmaxor_of_id_table[] = {
	{.compatible = "axera,dmaxor"},
	{}
};

MODULE_DEVICE_TABLE(of, ax_dmaxor_of_id_table);

static struct platform_driver ax_dmaxor_driver = {
	.probe = ax_dmaxor_probe,
	.remove = ax_dmaxor_remove,
	.driver = {
		   .name = DEVICE_NAME,
		   .of_match_table = ax_dmaxor_of_id_table,
		   },
};

static s32 __init ax_dmaxor_init(void)
{
	return platform_driver_register(&ax_dmaxor_driver);
}

static void ax_dmaxor_exit(void)
{
	platform_driver_unregister(&ax_dmaxor_driver);
}

module_init(ax_dmaxor_init);
module_exit(ax_dmaxor_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Axera DMAXOR driver");
MODULE_AUTHOR("Axera");
