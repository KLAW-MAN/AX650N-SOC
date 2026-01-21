/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXERA_KMEMALLOC_H__
#define __AXERA_KMEMALLOC_H__
struct ax_kmem_info *ax_kmem_creat(size_t size, size_t allocation, gfp_t flags);
void *ax_kmem_get(struct ax_kmem_info *info);
void ax_kmem_put(struct ax_kmem_info *info, void *node);
void ax_kmem_destroy(struct ax_kmem_info *info);
#endif