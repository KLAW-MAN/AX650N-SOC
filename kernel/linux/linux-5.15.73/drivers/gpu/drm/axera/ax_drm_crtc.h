/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_DRM_CRTC_H
#define __AX_DRM_CRTC_H

#include "ax_drm_drv.h"

#define AX_PLANES_MAX 3

#define VPLL_REF_RATE_27M	27000000
#define VPLL_REF_RATE_21M	21000000

#define VPLL_FBK_REF_BIT	BIT(1)
#define VPLL_FBK_REF_SHIFT	1

enum {
	VPLL_ADJ_TYPE_FREF,
	VPLL_ADJ_TYPE_FVCO
};

enum {
	VPLL_ADJ_MODE_INT,
	VPLL_ADJ_MODE_FRA,
	VPLL_ADJ_MODE_MIX
};

enum {
	VPLL_FBK_FRA_REF_27M,
	VPLL_FBK_INT_REF_21M
};

struct logo_mem_reserved {
	u32 id;
	phys_addr_t base;
	phys_addr_t size;
};

struct ax_composer {
	int num_active_planes;
	struct drm_plane_state *active_planes[AX_PLANES_MAX];
};

enum {
	CRTC_STATUS_INIT = 0,
	CRTC_STATUS_CONFIGURED,
	CRTC_STATUS_ENABLED,
	CRTC_STATUS_DISABLED,
};

enum {
	CRTC_CTRL_ACTION_DISABLING = 0,
	CRTC_CTRL_ACTION_ENABLING,
	CRTC_CTRL_ACTION_INVALID,
};

/* CRTC definition */
struct ax_crtc {
	u32 id;
	u32 status;
	struct drm_crtc base;
	struct drm_pending_vblank_event *event;
	struct ax_drm_mgr *drm_mgr;

	struct drm_plane *primary_plane;

	struct clk *vpll_clk;
	struct clk *vpll_ref_clk;
	struct clk *pclk;

	int num_planes;
	struct ax_plane *plane;

	u32 fb_nr;
	struct ax_fb fb[AX_PLANES_MAX];

	struct ax_composer composer;

	u32 work_mode;
	u32 ctrl_action;
	u32 vpll_config;
	int intf_type;
	int fmt_out;

	struct ax_disp_mode mode;
	struct ax_display_dev *display_dev;

	struct ax_disp_props disp_props;

	struct {
		struct drm_property *id;
		struct drm_property *work_mode;
		struct drm_property *ctrl;
		struct drm_property *vpll_config;
		struct drm_property *brightness;
		struct drm_property *contrast;
		struct drm_property *saturation;
		struct drm_property *hue;
		struct drm_property *csc;
		struct drm_property *intf_type;
		struct drm_property *fmt_out;
	} props;
};

#define to_ax_crtc(x) container_of(x, struct ax_crtc, base)

int ax_pclk_adj_param_set_ioctl(struct drm_device *dev, void *data, struct drm_file *filp);
int ax_pclk_adj_param_get_ioctl(struct drm_device *dev, void *data, struct drm_file *filp);

#endif /* __AX_DRM_CRTC_H */
