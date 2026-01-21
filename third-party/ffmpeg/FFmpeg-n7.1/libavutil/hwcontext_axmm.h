/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVUTIL_HWCONTEXT_AXMM_H
#define AVUTIL_HWCONTEXT_AXMM_H

#include "ax_sys_api.h"

/**
 * @file
 * An API-specific header for AV_HWDEVICE_TYPE_AXMM.
 */

/**
 * This struct is allocated as AVHWDeviceContext.hwctx
 */
typedef struct AVAXMMDeviceContext {
} AVAXMMDeviceContext;

/**
 * This struct is allocated as AVHWFramesContext.hwctx
 */
typedef struct AVAXMMFramesContext {
    unsigned devid;
    unsigned blk_id;
} AVAXMMFramesContext;

typedef struct {
    AX_S32(*sys_init)(AX_VOID);
    AX_S32(*sys_deinit)(AX_VOID);
    AX_S32(*sys_mmap)(AX_U64 phyaddr, AX_U32 size);
	AX_S32(*sys_munmap)(AX_VOID* pviraddr, AX_U32 size);

    AX_S32(*pool_init)(AX_VOID);
    AX_S32(*pool_exit)(AX_VOID);
    AX_POOL(*pool_create)(AX_POOL_CONFIG_T *cfg);
    AX_S32(*pool_destroy)(AX_POOL pool_id);
    AX_BLK(*pool_get_block)(AX_POOL pool_id, AX_U64 blk_size, const AX_S8 *partition_name);
    AX_S32(*pool_release_block)(AX_BLK blk_id);
    AX_BLK(*pool_paddr2handle)(AX_U64 paddr);
    AX_U64(*pool_handle2paddr)(AX_BLK blk_id);
    AX_VOID *(*pool_handle2vaddr)(AX_BLK blk_id);
    AX_S32(*pool_ref_get)(AX_BLK blk_id);
    AX_S32(*pool_ref_put)(AX_BLK blk_id);
} AXMMDeviceFuncs;


typedef struct {
    int alloc_blk;

    void *mm_sys_lib;

    AXMMDeviceFuncs mmdev_funcs;
} AXMMDeviceContext;

typedef struct {
    /* The public AVAXMMFramesContext. See hwcontext_axmm.h for it */
    AVAXMMFramesContext p;

    int alloc_blk;

    AX_POOL pool_id;
    AX_POOL_CONFIG_T pool_cfg;
} AXMMFramesContext;

#endif /* AVUTIL_HWCONTEXT_AXMM_H */

