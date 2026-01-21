/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/of.h>
#include <linux/blk-mq.h>
#include <linux/pgtable.h>
#include <linux/capability.h>
#include <linux/compat.h>
#include <linux/export.h>
#include <linux/gfp.h>
#include <linux/blkpg.h>
#include <linux/slab.h>
#include <linux/hdreg.h>
#include <linux/backing-dev.h>
#include <linux/blktrace_api.h>
#include <linux/pr.h>
#include <linux/uaccess.h>


#define AX_DEVICE_NAME "axramdisk"

struct axera_virtual_gendisk {
	unsigned int axbdrv_ma_no;
	char *vir_ramdisk_addr;
	struct gendisk *disk;
	spinlock_t lock;
	u64 ramdisk_size;
	u64 phy_ramdisk_addr;
	struct blk_mq_tag_set tag_set;
};

struct axera_virtual_gendisk *ax_gendisk;

static blk_status_t axera_queue_rq(struct blk_mq_hw_ctx *hctx,const struct blk_mq_queue_data *bd)
{
	struct request *req = bd->rq;
	unsigned int nr_bytes = 0;
    	struct bio_vec bvec;
    	struct req_iterator iter;
	struct axera_virtual_gendisk *gendisk = req->q->queuedata;

    	loff_t pos = blk_rq_pos(req) << SECTOR_SHIFT;
    	loff_t dev_size = gendisk->ramdisk_size;

	blk_status_t status = BLK_STS_OK;
	blk_mq_start_request(req);

	spin_lock(&gendisk->lock);

	rq_for_each_segment(bvec, req, iter) {
		unsigned long b_len = bvec.bv_len;
		void* b_buf = page_address(bvec.bv_page) + bvec.bv_offset;

		if ((pos + b_len) > dev_size)
			b_len = (unsigned long)(dev_size - pos);

		if (rq_data_dir(req))//WRITE
			memcpy(gendisk->vir_ramdisk_addr + pos, b_buf, b_len);
		else//READ
			memcpy(b_buf, gendisk->vir_ramdisk_addr + pos, b_len);
		pos += b_len;
		nr_bytes += b_len;
	}

	if (blk_update_request(req, status, nr_bytes)) {
		spin_unlock(&gendisk->lock);
		BUG();
	}

	spin_unlock(&gendisk->lock);
	blk_mq_end_request(req, BLK_STS_OK);

	return BLK_STS_OK;
}

static int axera_ramdisk_open(struct block_device *bdev, fmode_t mode)
{
	return 0;
}

static void axera_ramdisk_release(struct gendisk *disk, fmode_t mode)
{

}

static int ax_ioctl(struct block_device *bdev, fmode_t mode,
		    unsigned int cmd, unsigned long arg)
{

	struct hd_geometry geo;

	printk("cmd=%d\n", cmd);

	switch (cmd) {
	case HDIO_GETGEO:
		printk("hit HDIO_GETGEO\n");
		geo.cylinders = 1;
		geo.heads = 1;
		geo.sectors = ax_gendisk->ramdisk_size/geo.heads/geo.cylinders/512;

		if (copy_to_user((void __user *)arg, &geo, sizeof(geo)))
			return -EFAULT;

		return 0;
	}
	pr_warn("return -ENOTTY\n");

	return -ENOTTY;
}


static const struct block_device_operations axbdrv_fops = {
	.owner = THIS_MODULE,
	.ioctl = ax_ioctl,
	.open = axera_ramdisk_open,
	.release = axera_ramdisk_release,
};

static const struct blk_mq_ops axera_disk_mq_ops = {
	.queue_rq = axera_queue_rq,
};

static int __init ax_ramdisk_init(void)
{
	int ret;
	int diskmb;

	struct device_node *of_node = of_find_compatible_node(NULL, NULL, "axera, ramdisk");
	if (!of_node) {
		pr_err("%s:can't find ramdisk device tree node\n", __func__);
		return -ENODEV;
	}

	ax_gendisk = kmalloc(sizeof(*ax_gendisk),GFP_KERNEL);
	if(IS_ERR(ax_gendisk)) {
		goto err0;
	}

	memset(ax_gendisk,0,sizeof(*ax_gendisk));

	if (of_property_read_u64(of_node, "addr", &ax_gendisk->phy_ramdisk_addr)) {
		pr_err("%s:can't find 'addr' in ramdisk device tree node\n",__func__);
		goto err1;
	}
	if (of_property_read_u64(of_node, "size", &ax_gendisk->ramdisk_size)) {
		pr_err("%s:can't find  'size' in ramdisk device tree node\n",__func__);
		goto err1;
	}

	printk("ramdisk addr = 0x%llx size = 0x%llx\n",ax_gendisk->phy_ramdisk_addr,ax_gendisk->ramdisk_size);

	diskmb = ax_gendisk->ramdisk_size / 0x100000;
	spin_lock_init(&ax_gendisk->lock);

	ax_gendisk->vir_ramdisk_addr = ioremap_wc(ax_gendisk->phy_ramdisk_addr, ax_gendisk->ramdisk_size);
	if (IS_ERR(ax_gendisk->vir_ramdisk_addr))
		goto err2;


	ax_gendisk->axbdrv_ma_no = register_blkdev(0, AX_DEVICE_NAME);
	if (ax_gendisk->axbdrv_ma_no < 0) {
		pr_err("Failed registering axbdrv, returned %d\n",ax_gendisk->axbdrv_ma_no);
		goto err2;
	}

	ax_gendisk->tag_set.ops = &axera_disk_mq_ops;
	ax_gendisk->tag_set.nr_hw_queues = 1;
	ax_gendisk->tag_set.nr_maps = 1;
	ax_gendisk->tag_set.queue_depth = 16;
	ax_gendisk->tag_set.numa_node = NUMA_NO_NODE;
	ax_gendisk->tag_set.flags = BLK_MQ_F_SHOULD_MERGE;
	ret = blk_mq_alloc_tag_set(&ax_gendisk->tag_set);
	if (ret)
		goto err3;

	ax_gendisk->disk = blk_mq_alloc_disk(&ax_gendisk->tag_set, ax_gendisk);
	if (IS_ERR(ax_gendisk->disk))
		goto err4;

	ax_gendisk->disk->major = ax_gendisk->axbdrv_ma_no;
	ax_gendisk->disk->first_minor = 0;
	ax_gendisk->disk->minors = 1;
	ax_gendisk->disk->fops = &axbdrv_fops;
	strcpy(ax_gendisk->disk->disk_name, AX_DEVICE_NAME);


	set_capacity(ax_gendisk->disk, ax_gendisk->ramdisk_size / SECTOR_SIZE);
	add_disk(ax_gendisk->disk);

	pr_info("axramdisk device successfully registered, Major No. = %d\n",ax_gendisk->axbdrv_ma_no);
	pr_info("Capacity of ram disk is: %d MB\n", diskmb);

	return 0;
err4:
	blk_mq_free_tag_set(&ax_gendisk->tag_set);
err3:
	unregister_blkdev(ax_gendisk->axbdrv_ma_no, AX_DEVICE_NAME);
err2:
	iounmap(ax_gendisk->vir_ramdisk_addr);
err1:
	kfree(ax_gendisk);
err0:
	return -1;
}

static void __exit ax_ramdisk_exit(void)
{
	del_gendisk(ax_gendisk->disk);
	blk_cleanup_queue(ax_gendisk->disk->queue);
	put_disk(ax_gendisk->disk);
	unregister_blkdev(ax_gendisk->axbdrv_ma_no, AX_DEVICE_NAME);
	blk_mq_free_tag_set(&ax_gendisk->tag_set);
	iounmap(ax_gendisk->vir_ramdisk_addr);
	kfree(ax_gendisk);
	pr_info("module successfully unloaded, Major No. = %d\n", ax_gendisk->axbdrv_ma_no);
}

module_init(ax_ramdisk_init);
module_exit(ax_ramdisk_exit);

MODULE_LICENSE("GPL v2");
MODULE_VERSION("0.9");

