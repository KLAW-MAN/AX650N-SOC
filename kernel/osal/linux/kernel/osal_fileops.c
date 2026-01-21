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
#include <asm/uaccess.h>
#include "osal_ax.h"

AX_VOID *AX_OSAL_FS_filp_open(const AX_S8 * filename, AX_S32 flags, AX_S32 mode)
{
	struct file *filp = filp_open(filename, flags, mode);
	return (IS_ERR(filp)) ? NULL : filp;
}

EXPORT_SYMBOL(AX_OSAL_FS_filp_open);

AX_VOID AX_OSAL_FS_filp_close(AX_VOID * filp)
{
	if (filp) {
		filp_close(filp, NULL);
	}
	return;
}

EXPORT_SYMBOL(AX_OSAL_FS_filp_close);

AX_S32 AX_OSAL_FS_filp_write(AX_S8 * buf, AX_S32 len, AX_VOID * k_file)
{
	int writelen = 0;
	struct file *filp;

	if (k_file == NULL) {
		return -ENOENT;
	}

	filp = (struct file *)k_file;

	writelen = kernel_write(filp, buf, len, &filp->f_pos);
	return writelen;
}

EXPORT_SYMBOL(AX_OSAL_FS_filp_write);

AX_S32 AX_OSAL_FS_filp_read(AX_S8 * buf, AX_S32 len, AX_VOID * k_file)
{
	int readlen = 0;
	struct file *filp;

	if (k_file == NULL) {
		return -ENOENT;
	}

	filp = (struct file *)k_file;

	readlen = kernel_read(filp, buf, len, &filp->f_pos);
	return readlen;
}

EXPORT_SYMBOL(AX_OSAL_FS_filp_read);

AX_S32 AX_OSAL_FS_FILP_llseek(AX_S32 whence, AX_S32 offset, AX_VOID * k_file)
{
	int loff = 0;
	struct file *filp;

	if (k_file == NULL) {
		return -ENOENT;
	}

	filp = (struct file *)k_file;

	loff = vfs_llseek(filp, offset, whence);

	return loff;
}

EXPORT_SYMBOL(AX_OSAL_FS_FILP_llseek);
