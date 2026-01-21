/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_DRM_IOCTL_H
#define __AX_DRM_IOCTL_H

#if defined(__cplusplus)
extern "C"
{
#endif

struct ax_pclk_adj_param {
	u16 type;
	u16 mode;

	u32 fref;
	u32 pre_div;

	u32 fvco;
	u32 post_div;

	u32 param0;
	u32 param1;
};

#define DRM_AXPCLK_ADJ_PARAM_SET 0x10
#define DRM_AXPCLK_ADJ_PARAM_GET 0x11

#define DRM_IOCTL_AXPCLK_ADJ_PARAM_SET DRM_IOWR(DRM_COMMAND_BASE + DRM_AXPCLK_ADJ_PARAM_SET, struct ax_pclk_adj_param)
#define DRM_IOCTL_AXPCLK_ADJ_PARAM_GET DRM_IOWR(DRM_COMMAND_BASE + DRM_AXPCLK_ADJ_PARAM_GET, struct ax_pclk_adj_param)

#if defined(__cplusplus)
}
#endif

#endif /* __AX_DRM_IOCTL_H */

