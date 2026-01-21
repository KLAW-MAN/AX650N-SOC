/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/types.h>

struct ax_kmem_info {
	int in_use;
	gfp_t flags;
	size_t size;
	size_t allocation;
	raw_spinlock_t lock;
	unsigned long long start;
	unsigned long long end;
	void *head;
	void *tail;
};

struct ax_kmem_info *ax_kmem_creat(size_t size, size_t allocation, gfp_t flags)
{
	int i;
	int objnum = size / allocation;
	struct ax_kmem_info *info = NULL;
	void *node;

	if (!objnum)
		return NULL;

	info = kzalloc(sizeof(struct ax_kmem_info), GFP_KERNEL);
	if (IS_ERR_OR_NULL(info))
		return NULL;

	node = kzalloc(size, GFP_KERNEL);
	if (IS_ERR_OR_NULL(node))
		goto err;

	for (i = 0; i < objnum; i++) {
		*(unsigned long long *)(node + i * allocation) =
			(unsigned long long)(node + (i + 1) * allocation);
	}

	info->flags = flags;
	info->in_use = 0;
	info->head = node;
	info->tail = info->head + (objnum - 1) * allocation;
	info->size = size;
	info->allocation = allocation;
	info->start = (unsigned long long)node;
	info->end = info->start + size;
	raw_spin_lock_init(&info->lock);
	return info;
err:
	kfree(info);
	return NULL;
}
EXPORT_SYMBOL(ax_kmem_creat);

void *ax_kmem_get(struct ax_kmem_info *info)
{
	unsigned long flags;
	void *retval = NULL;

	if (IS_ERR_OR_NULL(info))
		return NULL;

	raw_spin_lock_irqsave(&info->lock, flags);
	if (likely(info->head != info->tail)) {
		retval = info->head;
		info->head = (void *)*(unsigned long long *)info->head;
	} else {
		raw_spin_unlock_irqrestore(&info->lock, flags);
		goto kmalloc;
	}
	info->in_use++;
	raw_spin_unlock_irqrestore(&info->lock, flags);

	return retval;
kmalloc:
	retval = kmalloc(info->allocation, info->flags);
	if (retval) {
		raw_spin_lock_irqsave(&info->lock, flags);
		info->in_use++;
		raw_spin_unlock_irqrestore(&info->lock, flags);
	}
	return retval;
}
EXPORT_SYMBOL(ax_kmem_get);

void ax_kmem_put(struct ax_kmem_info *info, void *node)
{
	unsigned long flags;

	if (IS_ERR_OR_NULL(info) || IS_ERR_OR_NULL(node))
		return ;
	if (unlikely((unsigned long long)node > info->end || (unsigned long long)node < info->start)) {
		kfree(node);
		raw_spin_lock_irqsave(&info->lock, flags);
		info->in_use--;
		raw_spin_unlock_irqrestore(&info->lock, flags);
		return ;
	} else {
		if (info->flags & __GFP_ZERO)
			memset(node, 0, info->allocation);
		raw_spin_lock_irqsave(&info->lock, flags);
		*(unsigned long long *)info->tail = (unsigned long long)node;
		info->tail = node;
		info->in_use--;
		raw_spin_unlock_irqrestore(&info->lock, flags);
	}
	return ;
}
EXPORT_SYMBOL(ax_kmem_put);

void ax_kmem_destroy(struct ax_kmem_info *info)
{
	if (info->in_use)
		pr_err("err: mem is in use %d\n", info->in_use);
	kfree(info);
}
EXPORT_SYMBOL(ax_kmem_destroy);