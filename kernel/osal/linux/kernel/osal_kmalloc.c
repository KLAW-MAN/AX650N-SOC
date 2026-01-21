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
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include "osal_ax.h"

AX_VOID *AX_OSAL_MEM_kmalloc(AX_SIZE_T size, AX_U32 osal_gfp_flag)
{
    if (osal_gfp_flag == AX_OSAL_GFP_KERNEL) {
        return kmalloc(size, GFP_KERNEL);
    } else if (osal_gfp_flag == AX_OSAL_GFP_ATOMIC) {
        return kmalloc(size, GFP_ATOMIC);
    }

    return kmalloc(size, GFP_KERNEL);
}
EXPORT_SYMBOL(AX_OSAL_MEM_kmalloc);

AX_VOID *AX_OSAL_MEM_kzalloc(AX_SIZE_T size, AX_U32 osal_gfp_flag)
{
    if (osal_gfp_flag == AX_OSAL_GFP_KERNEL) {
        return kzalloc(size, GFP_KERNEL);
    } else if (osal_gfp_flag == AX_OSAL_GFP_ATOMIC) {
        return kzalloc(size, GFP_ATOMIC);
    }

    return kzalloc(size, GFP_KERNEL);
}
EXPORT_SYMBOL(AX_OSAL_MEM_kzalloc);

AX_VOID AX_OSAL_MEM_kfree(const AX_VOID *addr)
{
    kfree(addr);
}
EXPORT_SYMBOL(AX_OSAL_MEM_kfree);

AX_VOID *AX_OSAL_MEM_vmalloc(AX_SIZE_T size)
{
    return vmalloc(size);
}
EXPORT_SYMBOL(AX_OSAL_MEM_vmalloc);

AX_VOID AX_OSAL_MEM_vfree(const AX_VOID *addr)
{
    vfree(addr);
}
EXPORT_SYMBOL(AX_OSAL_MEM_vfree);

AX_S32 AX_OSAL_MEM_VirtAddrIsValid(AX_ULONG vm_start, AX_ULONG vm_end)
{
    struct vm_area_struct *pvma1;
    struct vm_area_struct *pvma2;

    mmap_read_lock(current->mm);
    pvma1 = find_vma(current->mm, vm_start);
    if (NULL == pvma1) {
        printk("user vaddr 1 is null. user add = 0x%lx\n", vm_start);
        goto badAddr;
    }

    pvma2 = find_vma(current->mm, vm_end - 1);
    if (NULL == pvma2) {
        printk("user vaddr 2 is null. user add = 0x%lx\n", vm_start);
        goto badAddr;
    }

    if (pvma1 != pvma2) {
        printk("user vaddr:[0x%lx,0x%lx) and user vaddr:[0x%lx,0x%lx) are not equal\n",
                       pvma1->vm_start, pvma1->vm_end, pvma2->vm_start, pvma2->vm_end);
        goto badAddr;
    }

    if (!(pvma1->vm_flags & VM_WRITE)) {
        printk("ERROR vma flag:0x%lx\n", pvma1->vm_flags);
        goto badAddr;
    }

    if (pvma1->vm_start > vm_start) {
        printk("cannot find corresponding vma, vm[%lx, %lx], user range[%lx,%lx]\n", pvma1->vm_start, pvma1->vm_end,
                       vm_start, vm_end);
        goto badAddr;
    }

    mmap_read_unlock(current->mm);
    return 0;
badAddr:
    mmap_read_unlock(current->mm);
    return -1;
}
EXPORT_SYMBOL(AX_OSAL_MEM_VirtAddrIsValid);

AX_ULONG AX_OSAL_MEM_AddrMmap(AX_VOID *file, AX_ULONG addr,
	AX_ULONG len, AX_ULONG prot, AX_ULONG flag, AX_ULONG offset)
{
    return vm_mmap((struct file *)file, addr, len, prot, flag, offset);
}
EXPORT_SYMBOL(AX_OSAL_MEM_AddrMmap);

AX_S32 AX_OSAL_MEM_AddrMunmap(AX_ULONG start, AX_SIZE_T size)
{
    return vm_munmap(start, size);
}
EXPORT_SYMBOL(AX_OSAL_MEM_AddrMunmap);
