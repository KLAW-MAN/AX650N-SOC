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
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/dma-direct.h>
#include <linux/dma-mapping.h>
#include <linux/of_device.h>
#include <linux/version.h>
#include <asm/cacheflush.h>

#include "../include/ax_pcie_dev.h"

#ifndef MAX_ORDER
#ifdef MAX_PAGE_ORDER
#define MAX_ORDER MAX_PAGE_ORDER
#else
#define MAX_ORDER 11
#endif
#endif

#if defined(CONFIG_ARM64)
extern void ax_dcache_clean_inval_poc(unsigned long start, unsigned long end);
extern void ax_dcache_inval_poc(unsigned long start, unsigned long end);
#endif

#if defined(CONFIG_RISCV)
#include "rv64_cache.h"
#endif

#define MAX_SG_LIST_ENTRY	64

//#define MEM_LIST_PARTITION
#ifdef MEM_LIST_PARTITION
#define MAX_MEM_LIST_ENTRY	2
#define MAX_MEM_ENTRY_SIZE	SZ_2M
#endif

static struct miscdevice ax_mmb_miscdev;

#define AX_IOC_PCIe_BASE           'H'
#define AX_IOC_PCIe_ALLOC_MEMORY   _IOWR(AX_IOC_PCIe_BASE, 4, struct ax_mem_info)
#define AX_IOC_PCIe_ALLOC_MEMCACHED   _IOWR(AX_IOC_PCIe_BASE, 5, struct ax_mem_info)
#define AX_IOC_PCIe_FLUSH_CACHED	_IOW(AX_IOC_PCIe_BASE, 6, struct ax_mem_info)
#define AX_IOC_PCIe_INVALID_CACHED	_IOW(AX_IOC_PCIe_BASE, 7, struct ax_mem_info)
#define AX_IOC_PCIe_SCATTERLIST_ALLOC	_IOW(AX_IOC_PCIe_BASE, 8, struct ax_sg_list)
#ifdef MEM_LIST_PARTITION
#define AX_IOC_PCIe_GET_MEM_ENTRY	_IOW(AX_IOC_PCIe_BASE, 9, struct mem_entry_info)
#define AX_IOC_PCIe_PUT_MEM_ENTRY	_IOW(AX_IOC_PCIe_BASE, 10, struct mem_entry_info)
#endif

#define PCIe_DEBUG  4
#define PCIe_INFO   3
#define PCIe_ERR    2
#define PCIe_FATAL  1
#define PCIe_CURR_LEVEL 2
#define PCIe_PRINT(level, s, params...) do { \
	if (level <= PCIe_CURR_LEVEL)  \
	printk("[%s, %d]: " s "\n", __func__, __LINE__, ##params); \
} while (0)

struct ax_mmb_buf {
	bool is_scatter;
	bool mem_cached;
	bool dma_mapped;

	unsigned int actual_nents;
	struct sg_table sgt;
	struct sg_table sgt_bak;

	struct device *dev;
};

struct ax_mem_entry {
	uint32_t size;
	uint64_t phyaddr;
	uint64_t virtaddr;
};

struct ax_mem_info {
	uint32_t id;
	struct ax_mem_entry mem;
};

struct ax_sg_list {
	uint32_t id;
	uint32_t size;
	uint32_t num;
	struct ax_mem_entry mems[MAX_SG_LIST_ENTRY];
};

#ifdef MEM_LIST_PARTITION
struct mem_entry_info {
	dma_addr_t phyaddr;
	unsigned int target;
};

struct mem_block {
	struct list_head list;
	unsigned long phyaddr;
	void *virtaddr;
};

struct mem_list {
	struct list_head busy_head;
	struct list_head free_head;
	struct mem_block mem_arry[MAX_MEM_LIST_ENTRY];
	unsigned int target;
};
static struct mem_list *g_mem_list[32];
#endif

static int ax_mmb_open(struct inode *inode, struct file *fp)
{
	struct ax_mmb_buf *ax_mmb;

	ax_mmb = kmalloc(sizeof(struct ax_mmb_buf), GFP_KERNEL);
	if (!ax_mmb) {
		PCIe_PRINT(PCIe_ERR, "ax_mmb kmalloc failed");
		return -EINVAL;
	}
	memset((void *)ax_mmb, 0, sizeof(struct ax_mmb_buf));

	fp->private_data = (void *)ax_mmb;

	PCIe_PRINT(PCIe_DEBUG, "open mmb(MultiMedia buffer) dev");

	return 0;
}

#ifdef MEM_LIST_PARTITION
static int ax_memory_list_putentry(struct file *fp, struct mem_entry_info *entry_info)
{
	struct ax_mmb_buf *ax_mmb = (void *)fp->private_data;
	struct list_head *busy_head = NULL;
	struct mem_block *mem_entry = NULL;
	struct list_head *entry, *tmp;
	int i;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		if (g_mem_list[i]->target != entry_info->target)
			continue;
		busy_head = &g_mem_list[i]->busy_head;
		if (!list_empty(busy_head)) {
			list_for_each_safe(entry, tmp, busy_head) {
				mem_entry = list_entry(entry, struct mem_block, list);
				if (mem_entry == NULL)
					continue;
				if (mem_entry->phyaddr == entry_info->phyaddr) {
					list_del(&mem_entry->list);
					list_add_tail(&mem_entry->list, &g_mem_list[i]->free_head);
					ax_mmb->dma_phy_addr = 0;
					return 0;
				}
			}
			PCIe_PRINT(PCIe_DEBUG, "Device %x not such the entry: %llx", entry_info->target, entry_info->phyaddr);
			return -1;
		}
		PCIe_PRINT(PCIe_DEBUG, "Device %x busy list is empty : %llx", entry_info->target, entry_info->phyaddr);
	}
	return -1;
}

static dma_addr_t ax_memory_list_getentry(struct file *fp, struct mem_entry_info *entry_info)
{
	struct ax_mmb_buf *ax_mmb = (void *)fp->private_data;
	struct list_head *list;
	struct list_head *free_head = NULL;
	struct mem_block *mem_entry = NULL;
	int i;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		if (g_mem_list[i]->target != entry_info->target)
			continue;

		free_head = &g_mem_list[i]->free_head;
		if (unlikely(list_empty(free_head))) {
			PCIe_PRINT(PCIe_ERR, "No free llp space");
			return -1;
		}
		list = free_head->next;
		list_del(list);
		mem_entry = list_entry(list, struct mem_block, list);
		list_add_tail(&mem_entry->list, &g_mem_list[i]->busy_head);
		ax_mmb->dma_phy_addr = mem_entry->phyaddr;
	}

	if (i == g_axera_pcie_opt->remote_device_number)
		return -1;

	entry_info->phyaddr = mem_entry->phyaddr;

	return 0;
}

static void ax_memory_list_deinit(void)
{
	int i, j;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		if (g_mem_list[i]) {
			for (j = (MAX_MEM_LIST_ENTRY - 1); j >= 0; j--) {
				if (g_mem_list[i]->mem_arry[j].virtaddr != NULL)
					kfree(g_mem_list[i]->mem_arry[j].virtaddr);
			}
			kfree(g_mem_list[i]);
		}
	}
}

static int ax_memory_list_init(void)
{
	int i, j;
	void *virtaddr;
	unsigned long phyaddr;

	for (i = 0; i < g_axera_pcie_opt->remote_device_number; i++) {
		g_mem_list[i] = kmalloc(sizeof(struct mem_list), GFP_KERNEL);
		if (!g_mem_list[i]) {
			PCIe_PRINT(PCIe_ERR, "mem list kmalloc failed");
			ax_memory_list_deinit();
			return -1;
		}

		INIT_LIST_HEAD(&g_mem_list[i]->busy_head);
		INIT_LIST_HEAD(&g_mem_list[i]->free_head);

		memset(g_mem_list[i]->mem_arry, 0, sizeof(g_mem_list[i]->mem_arry));
		for (j = (MAX_MEM_LIST_ENTRY - 1); j >= 0; j--) {
			virtaddr = kmalloc(MAX_MEM_ENTRY_SIZE, GFP_KERNEL);
			if (!virtaddr) {
				PCIe_PRINT(PCIe_ERR, "virtaddr kmalloc memory failed");
				ax_memory_list_deinit();
				return -1;
			}
			phyaddr = virt_to_phys(virtaddr);
			if (phyaddr < 0) {
				PCIe_PRINT(PCIe_ERR, "virt to phys failed");
				kfree(virtaddr);
				ax_memory_list_deinit();
				return -1;
			}
			g_mem_list[i]->mem_arry[j].phyaddr = phyaddr;
			g_mem_list[i]->mem_arry[j].virtaddr = virtaddr;
			list_add_tail(&g_mem_list[i]->mem_arry[j].list, &g_mem_list[i]->free_head);
		}
	}

	return 0;
}
#endif

static void ax_mmb_sglist_free_mem(struct ax_mmb_buf *mmb)
{
	int i;
	struct scatterlist *sg;

	if (!mmb->sgt.sgl)
		goto exit;

	if (!mmb->is_scatter) {
		sg = mmb->sgt.sgl;
		dma_free_coherent(mmb->dev, sg_dma_len(sg), sg_virt(sg), sg_dma_address(sg));
		goto exit;
	}

	if (mmb->actual_nents) {
		if (mmb->dma_mapped) {
			dma_unmap_sg_attrs(mmb->dev, mmb->sgt.sgl, mmb->actual_nents, 0, 0);
			mmb->dma_mapped = false;
		}
	}

	for (i = 0, sg = mmb->sgt_bak.sgl; i < mmb->sgt_bak.nents; i++) {
		__free_pages(sg_page(sg), get_order(sg_dma_len(sg)));
		sg = sg_next(sg);
	}

exit:
	if (mmb->sgt.sgl)
		sg_free_table(&mmb->sgt);

	if (mmb->sgt_bak.sgl)
		sg_free_table(&mmb->sgt_bak);

	if (mmb->actual_nents)
		mmb->actual_nents = 0;
}

static int ax_mmb_release(struct inode *inode, struct file *fp)
{
	struct ax_mmb_buf *ax_mmb = (void *)fp->private_data;

	PCIe_PRINT(PCIe_DEBUG, "release mmb");

	if (ax_mmb) {
		ax_mmb_sglist_free_mem(ax_mmb);
		kfree(ax_mmb);
	}

	return 0;
}

static struct device *get_mmb_dev(uint32_t id)
{
	return get_pcie_dev(id);
}

static void ax_copy_sg_table(struct sg_table *dst, struct sg_table *src, unsigned int nents)
{
	memcpy(dst->sgl, src->sgl, nents * sizeof(struct scatterlist));
	dst->nents = nents;
}

static int ax_sglist_alloc_mem(struct ax_mmb_buf *mmb, struct ax_sg_list *sgl)
{
	int i, ret = 0;
	uint32_t id = sgl->id, size = sgl->size;
	uint32_t alloc_size;
	int order;
	void *virtaddr;
	struct scatterlist *sg;
	struct page *pg = NULL;

	if (mmb->dev || mmb->sgt.sgl) {
		PCIe_PRINT(PCIe_ERR, "mmb is already allocated");
		ret = -EINVAL;
		goto err0;
	}

	if (!size) {
		PCIe_PRINT(PCIe_ERR, "size invalid");
		ret = -EINVAL;
		goto err0;
	}

	mmb->dev = get_mmb_dev(id);
	if (!mmb->dev) {
		PCIe_PRINT(PCIe_ERR, "id(0x%x) invalid", id);
		ret = -ENODEV;
		goto err0;
	}

	if (mmb->actual_nents > 0) {
		PCIe_PRINT(PCIe_ERR, "sg table is already allocated, actual_nents: %d", mmb->actual_nents);
		ret = -EACCES;
		goto err0;
	}

	mmb->mem_cached = true;
	mmb->is_scatter = true;

	ret = sg_alloc_table(&mmb->sgt, MAX_SG_LIST_ENTRY, GFP_KERNEL);
	if (ret) {
		PCIe_PRINT(PCIe_ERR, "sg_alloc_table failed");
		ret = -ENOMEM;
		goto err0;
	}

	ret = sg_alloc_table(&mmb->sgt_bak, MAX_SG_LIST_ENTRY, GFP_KERNEL);
	if (ret) {
		sg_free_table(&mmb->sgt);
		PCIe_PRINT(PCIe_ERR, "sg_alloc_table backup failed");
		ret = -ENOMEM;
		goto err0;
	} else {
		mmb->sgt_bak.nents = 0;
	}

	for (sg = mmb->sgt.sgl; size > 0; sg = sg_next(sg)) {
		if (!sg) {
			PCIe_PRINT(PCIe_ERR, "there are no free entries");
			ret = -ENOMEM;
			goto err1;
		}

		alloc_size = PAGE_ALIGN(size);
		for (order = min(get_order(alloc_size), MAX_ORDER - 1); order < MAX_ORDER; order -= 1) {
			if (order < 0) {
				PCIe_PRINT(PCIe_ERR, "size: 0x%x, alloc_size: 0x%x, order %d < 0, MAX_ORDER: %d", size, alloc_size, order, MAX_ORDER);
				ret = -EINVAL;
				goto err1;
			}

			pg = alloc_pages(GFP_KERNEL, order);
			if (pg) {
				alloc_size = (1 << order) * PAGE_SIZE;
				break;
			}

			PCIe_PRINT(PCIe_ERR, "alloc size: 0x%x, alloc pages:%d failed", alloc_size, (1 << order));
		}

		if (!pg) {
			PCIe_PRINT(PCIe_ERR, "alloc pages failed");
			ret = -ENOMEM;
			goto err1;
		}

		if (size >= alloc_size) {
			size -= alloc_size;
		} else {
			alloc_size = size;
			size = 0;
		}

		sg_set_page(sg, pg, alloc_size, 0);
		sg_dma_address(sg) = sg_phys(sg);
		sg_dma_len(sg) = alloc_size;

		virtaddr = sg_virt(sg);
		PCIe_PRINT(PCIe_DEBUG, "alloc pages:%d virtaddr:%llx phyaddr:%llx size:%x success",
			   (1 << order), (uint64_t)virtaddr, sg_dma_address(sg), alloc_size);

		mmb->actual_nents += 1;
	}

	PCIe_PRINT(PCIe_INFO, "actual_nents:%d", mmb->actual_nents);
	if (mmb->actual_nents == 0) {
		PCIe_PRINT(PCIe_ERR, "sgl->size: 0x%x, nents is 0", sgl->size);
		ret = -ENOMEM;
		goto err1;
	}

	/* copy to backup table for free */
	ax_copy_sg_table(&mmb->sgt_bak, &mmb->sgt, mmb->actual_nents);

	mmb->actual_nents = dma_map_sg_attrs(mmb->dev, mmb->sgt.sgl, mmb->actual_nents, 0, 0);
	if (mmb->actual_nents == 0) {
		PCIe_PRINT(PCIe_ERR, "dma_map_sg_attrs failed");
		ret = -EIO;
		goto err1;
	} else {
		PCIe_PRINT(PCIe_INFO, "dma_map_sg_attrs actual_nents:%d", mmb->actual_nents);
	}

	mmb->dma_mapped = true;

	sgl->num = mmb->actual_nents;
	for (i = 0, sg = mmb->sgt.sgl; i < mmb->actual_nents; i += 1) {
		sgl->mems[i].size = sg_dma_len(sg);
		sgl->mems[i].phyaddr = sg_dma_address(sg);
		sg = sg_next(sg);
		PCIe_PRINT(PCIe_INFO, "mems[%d] phyaddr:%llx, size:%x",
			   i, sgl->mems[i].phyaddr, sgl->mems[i].size);
	}

	return 0;

err1:
	ax_mmb_sglist_free_mem(mmb);
err0:
	return ret;
}

static int ax_mmb_alloc_mem(struct ax_mmb_buf *mmb, struct ax_mem_info *minfo, bool is_cachecd)
{
	int ret = 0;
	unsigned int size = minfo->mem.size;
	void *buf;
	dma_addr_t dma_addr;
	struct scatterlist *sg;

	if (mmb->dev || mmb->sgt.sgl) {
		PCIe_PRINT(PCIe_ERR, "mmb is already allocated");
		ret = -EINVAL;
		goto exit;
	}

	if (!size) {
		PCIe_PRINT(PCIe_ERR, "size invalid");
		ret = -EINVAL;
		goto exit;
	}

	mmb->dev = get_mmb_dev(minfo->id);
	if (!mmb->dev) {
		PCIe_PRINT(PCIe_ERR, "id(0x%x) invalid", minfo->id);
		ret = -ENODEV;
		goto exit;
	}

	ret = sg_alloc_table(&mmb->sgt, 1, GFP_KERNEL);
	if (ret) {
		PCIe_PRINT(PCIe_ERR, "sg_alloc_table failed");
		goto exit;
	}

	buf = dma_alloc_coherent(mmb->dev, size, &dma_addr, GFP_KERNEL);
	if (!buf) {
		PCIe_PRINT(PCIe_ERR, "dma_alloc_coherent failed");
		sg_free_table(&mmb->sgt);
		ret = -ENOMEM;
		goto exit;
	}

	mmb->actual_nents = 1;
	mmb->is_scatter = false;
	mmb->mem_cached = is_cachecd;

	PCIe_PRINT(PCIe_INFO, "buf:%lx, dma_addr: %llx, size:%x", (long)buf, dma_addr, size);

	sg = mmb->sgt.sgl;
	sg_init_one(sg, buf, size);
	sg_dma_address(sg) = dma_addr;
	sg_dma_len(sg) = size;
	minfo->mem.phyaddr = (uint64_t)dma_addr;

exit:
	return ret;
}

static void _ax_mmb_invalidate_dcache_area(unsigned long addr, unsigned long size)
{
#if defined(CONFIG_ARM64)
	ax_dcache_inval_poc(addr, addr + size);
#elif defined(CONFIG_RISCV)
	riscv_invalidate_dcache_range(addr, size);
#elif defined(CONFIG_X86)
#endif
}

static int ax_mmb_invalidate_dcache_area(unsigned long long virtaddr, unsigned long long size)
{
	_ax_mmb_invalidate_dcache_area((unsigned long)virtaddr, size);
	return 0;
}

static void ax_mmb_flush_dcache_area(unsigned long kvirt, unsigned long length)
{
#if defined(CONFIG_ARM64)
	ax_dcache_clean_inval_poc(kvirt, kvirt + length);
#elif defined(CONFIG_RISCV)
	riscv_flush_dcache_range(kvirt, length);
#elif defined(CONFIG_X86)
#endif
}

static long ax_mmb_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	struct ax_mem_info meminfo;
	struct ax_sg_list *sgl;
#ifdef MEM_LIST_PARTITION
	struct mem_entry_info entry_info;
#endif

	switch (cmd) {
	case AX_IOC_PCIe_ALLOC_MEMORY:
	case AX_IOC_PCIe_ALLOC_MEMCACHED:
		PCIe_PRINT(PCIe_DEBUG, "ax mmb alloc mem-%s", (cmd == AX_IOC_PCIe_ALLOC_MEMORY) ? "nocached" : "cached");

		if (copy_from_user((void *)&meminfo, (void *)arg, sizeof(meminfo))) {
			PCIe_PRINT(PCIe_ERR, "copy data from user failed");
			return -EFAULT;
		}

		ret = ax_mmb_alloc_mem(fp->private_data, &meminfo, (cmd == AX_IOC_PCIe_ALLOC_MEMORY) ? false : true);
		if (ret) {
			PCIe_PRINT(PCIe_ERR, "ax mmb alloc mem-%s failed",
				   (cmd == AX_IOC_PCIe_ALLOC_MEMORY) ? "nocached" : "cached");
			return ret;
		}

		if (copy_to_user((void *)(&((struct ax_mem_info *)arg)->mem.phyaddr), (void *)&meminfo.mem.phyaddr, sizeof(meminfo.mem.phyaddr))) {
			PCIe_PRINT(PCIe_ERR, "copy data to usr space failed");
			return -EFAULT;
		}

		break;
	case AX_IOC_PCIe_INVALID_CACHED:
		if (copy_from_user((void *)&meminfo, (void *)arg, sizeof(struct ax_mem_info))) {
			PCIe_PRINT(PCIe_ERR, "copy data from usr space failed");
			return -EFAULT;
		}
		PCIe_PRINT(PCIe_DEBUG, "invalid: phyaddr = 0x%llx, virtaddr = 0x%llx, size = 0x%x",
			   meminfo.mem.phyaddr, meminfo.mem.virtaddr, meminfo.mem.size);
		ret = ax_mmb_invalidate_dcache_area(meminfo.mem.virtaddr, meminfo.mem.size);
		break;
	case AX_IOC_PCIe_FLUSH_CACHED:
		if (copy_from_user((void *)&meminfo, (void *)arg, sizeof(struct ax_mem_info))) {
			PCIe_PRINT(PCIe_ERR, "copy data from usr space failed");
			return -EFAULT;
		}
		PCIe_PRINT(PCIe_DEBUG, "flush: phyaddr = 0x%llx, virtaddr = 0x%llx, size = 0x%x",
			   meminfo.mem.phyaddr, meminfo.mem.virtaddr, meminfo.mem.size);
		ax_mmb_flush_dcache_area(meminfo.mem.virtaddr, meminfo.mem.size);
		break;
	case AX_IOC_PCIe_SCATTERLIST_ALLOC:
		PCIe_PRINT(PCIe_DEBUG, "ax mmb alloc scatterlist mem");

		sgl = kzalloc(sizeof(*sgl), GFP_KERNEL);
		if (!sgl) {
			PCIe_PRINT(PCIe_ERR, "Ioctl: Scatterlist alloc failed\n");
			return -ENOMEM;
		}

		if (copy_from_user((void *)sgl, (void *)arg, sizeof(*sgl))) {
			PCIe_PRINT(PCIe_ERR, "copy data from usr space failed");
			kfree(sgl);
			return -EFAULT;
		}

		ret = ax_sglist_alloc_mem(fp->private_data, sgl);
		if (ret < 0) {
			PCIe_PRINT(PCIe_ERR, "ax sgl alloc failed\n");
			kfree(sgl);
			return -1;
		}

		if (copy_to_user((void *)arg, (void *)sgl, sizeof(*sgl))) {
			PCIe_PRINT(PCIe_ERR, "copy data to usr space failed");
			kfree(sgl);
			return -EFAULT;
		}

		kfree(sgl);
		break;
#ifdef MEM_LIST_PARTITION
	case AX_IOC_PCIe_GET_MEM_ENTRY:
		if (copy_from_user
		    ((void *)&entry_info, (void *)arg, sizeof(struct mem_entry_info))) {
				PCIe_PRINT(PCIe_ERR, "copy data from usr space failed.");
			return -1;
		}
		ret = ax_memory_list_getentry(fp, &entry_info);
		if (ret < 0) {
			PCIe_PRINT(PCIe_ERR, "memory list getentry failed\n");
			return -1;
		}
		if (copy_to_user
		    ((void *)arg, (void *)&entry_info, sizeof(struct mem_entry_info))) {
			PCIe_PRINT(PCIe_ERR, "copy data to usr space failed");
			return -1;
		}
		break;
	case AX_IOC_PCIe_PUT_MEM_ENTRY:
		if (copy_from_user
		    ((void *)&entry_info, (void *)arg, sizeof(struct mem_entry_info))) {
				PCIe_PRINT(PCIe_ERR, "copy data from usr space failed.");
			return -1;
		}
		ret = ax_memory_list_putentry(fp, &entry_info);
		if (ret < 0) {
			PCIe_PRINT(PCIe_ERR, "memory list putentry failed\n");
			return -1;
		}
		break;
#endif
	default:
		PCIe_PRINT(PCIe_INFO, "unknown ioctl cmd");
		return -ENOIOCTLCMD;
	}

	return ret;
}

static pgprot_t ax_mmb_mmap_pgprot_cached(pgprot_t prot)
{
#if defined(CONFIG_ARM64)
	return __pgprot(pgprot_val(prot)
			| PTE_VALID | PTE_DIRTY | PTE_AF);
#elif defined(CONFIG_X86)
	return __pgprot(pgprot_val(prot));
#elif defined(CONFIG_RISCV)
	return pgprot_writecombine(prot);
#else
	return __pgprot(pgprot_val(prot) | L_PTE_PRESENT
			| L_PTE_YOUNG | L_PTE_DIRTY | L_PTE_MT_DEV_CACHED);
#endif
}

static pgprot_t ax_mmb_mmap_pgprot_nocached(pgprot_t prot)
{
#if defined(CONFIG_ARM64)
	return pgprot_writecombine(prot);
#elif defined(CONFIG_X86)
	return __pgprot(pgprot_val(prot));
#elif defined(CONFIG_RISCV)
	return pgprot_writecombine(prot);
#else
	return pgprot_writecombine(prot);
#endif
}

static int ax_mmb_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long vm_start, pfn_start;
	struct ax_mmb_buf *ax_mmb = (void *)file->private_data;
	struct scatterlist *sg = NULL;
	int i, ret = 0;

	if (!ax_mmb->dev || !ax_mmb->sgt.sgl) {
		PCIe_PRINT(PCIe_ERR, "memory unallocated");
		return -EPERM;
	}

	if (!ax_mmb->is_scatter) {
		sg = ax_mmb->sgt.sgl;

		if (ax_mmb->mem_cached)
			vma->vm_page_prot = ax_mmb_mmap_pgprot_cached(vma->vm_page_prot);
		else
			vma->vm_page_prot = ax_mmb_mmap_pgprot_nocached(vma->vm_page_prot);

		PCIe_PRINT(PCIe_DEBUG, "vm_start:0x%lx, vm_pgoff:0x%lx, virtaddr:0x%lx, dmaaddr:%lx, size:0x%x, mem_cached:%d",
			   vma->vm_start, vma->vm_pgoff,
			   (unsigned long)sg_virt(sg),
			   (unsigned long)sg_dma_address(sg),
			   sg_dma_len(sg),
			   ax_mmb->mem_cached);

#if defined(CONFIG_X86)
		return dma_mmap_attrs(ax_mmb->dev, vma, sg_virt(sg), sg_dma_address(sg), sg_dma_len(sg), 0);
#else
		pfn_start = dma_to_phys(ax_mmb->dev, sg_dma_address(sg)) >> PAGE_SHIFT;
		PCIe_PRINT(PCIe_DEBUG, "pfn_start:0x%lx, phyaddr:%lx", pfn_start,
			   (unsigned long)dma_to_phys(ax_mmb->dev, sg_dma_address(sg)));

		ax_mmb_invalidate_dcache_area((unsigned long long)sg_virt(sg), sg_dma_len(sg));

		return remap_pfn_range(vma, vma->vm_start, pfn_start, sg_dma_len(sg), vma->vm_page_prot);
#endif
	} else {
		sg = ax_mmb->sgt_bak.sgl; /* use backup sg table for mmap */

		for (i = 0, vm_start = vma->vm_start; i < ax_mmb->sgt_bak.nents; i++, sg = sg_next(sg)) {
			pfn_start = sg_phys(sg) >> PAGE_SHIFT;
			PCIe_PRINT(PCIe_INFO, "vm_start:0x%lx, pfn_start:0x%lx, phyaddr:%lx, size:0x%x",
				   vm_start, pfn_start, (unsigned long)sg_phys(sg), sg_dma_len(sg));
			ret = remap_pfn_range(vma, vm_start, pfn_start, sg_dma_len(sg), vma->vm_page_prot);
			if (ret) {
				PCIe_PRINT(PCIe_ERR, "remap_pfn_range failed at [0x%lx  0x%lx]",
					   vm_start, vma->vm_end);
			}

			vm_start += sg_dma_len(sg);
		}
	}

	return ret;
}

static struct file_operations ax_mmb_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ax_mmb_ioctl,
	.open = ax_mmb_open,
	.release = ax_mmb_release,
	.mmap = ax_mmb_mmap,
};

static struct miscdevice ax_mmb_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ax_mmb_dev",
	.fops = &ax_mmb_fops,
};

static int __init ax_mmb_init(void)
{
	int ret;

	PCIe_PRINT(PCIe_DEBUG, "axera mmb(MultiMedia buffer) init");

	ret = misc_register(&ax_mmb_miscdev);
	if (ret) {
		PCIe_PRINT(PCIe_ERR,
			   "Register axera MultiMedia Buffer Misc failed!");
		return -1;
	}

	/* configure dev->archdata.dma_ops = &swiotlb_dma_ops;
	 * or this dev's dma_ops is NULL, dma_alloc_coherent will fail */
	//of_dma_configure(ax_mmb_miscdev.this_device, NULL, 1);
	//arch_setup_dma_ops(ax_mmb_miscdev.this_device, 0, 0, NULL,1);
#ifdef MEM_LIST_PARTITION
	ret = ax_memory_list_init();
	if (ret < 0) {
		PCIe_PRINT(PCIe_ERR, "mem list init failed!");
		return -1;
	}
#endif

	return 0;
}

static void __exit ax_mmb_exit(void)
{
	PCIe_PRINT(PCIe_DEBUG, "axera mmb exit");
#ifdef MEM_LIST_PARTITION
	ax_memory_list_deinit();
#endif
	misc_deregister(&ax_mmb_miscdev);
}

module_init(ax_mmb_init);
module_exit(ax_mmb_exit);

MODULE_AUTHOR("AXERA");
MODULE_DESCRIPTION("AXERA AX650 MultiMedia Buffer");
MODULE_LICENSE("GPL");
