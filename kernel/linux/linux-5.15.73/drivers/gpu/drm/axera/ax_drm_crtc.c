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
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/memblock.h>
#include <linux/of_fdt.h>
#include <linux/of_reserved_mem.h>
#include <linux/sort.h>
#include <linux/clk.h>

#include <drm/drm_print.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_flip_work.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_vblank.h>
#include <drm/drm_modeset_helper_vtables.h>

#include "ax_drm_ioctl.h"
#include "ax_drm_crtc.h"
#include "ax_drm_plane.h"
#include "ax_drm_gem.h"
#include "ax_drm_virt_connector.h"

struct logo_mem_reserved boot_logo_mem[AX_DISPLAY_MAX];

static char *crtc_stat[] = {
	"init", "configured", "enabled", "disabled"
};

static char *crtc_mode[] = {
	"offline", "online"
};

int pll_fbk_mode_set(struct clk *clk_pll, u32 mode);
void pll_ref_set(struct clk *clk_pll, struct clk *clk_pll_ref);
int pll_cfg_get(struct clk *clk_pll, u32 *fref, u32 *pre_div, u32 *fvco,
		   u32 *post_div, u32 *fbk_int, u32 *fbk_fra);
int pll_rate_adj(struct clk *clk_pll, u32 fbk_int, u32 fbk_fra);

int ax_pclk_adj_param_set_ioctl(struct drm_device *dev, void *data, struct drm_file *filp)
{
	u32 fbk_int, fbk_fra;
	struct ax_drm_mgr *ax_drm_mgr = dev->dev_private;
	struct drm_crtc *crtc = ax_drm_mgr->crtc;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_pclk_adj_param *pclk_adj = (struct ax_pclk_adj_param *)data;
	struct clk *clk_pll = NULL;

	fbk_int = pclk_adj->param0;
	fbk_fra = pclk_adj->param1;

	if (pclk_adj->type == VPLL_ADJ_TYPE_FREF) {
		struct clk *parent = ax_crtc->vpll_ref_clk;

		for (parent = ax_crtc->vpll_ref_clk; parent != NULL; parent = clk_get_parent(parent)) {
			clk_pll = parent;
		}

	} else {
		clk_pll = ax_crtc->vpll_clk;
	}

	if (pclk_adj->mode == VPLL_ADJ_MODE_INT)
		fbk_fra = ~0;
	else if (pclk_adj->mode == VPLL_ADJ_MODE_FRA)
		fbk_int = 0;

	return pll_rate_adj(clk_pll, fbk_int, fbk_fra);
}

static inline struct clk *clk_get_root(struct clk *_clk)
{
	struct clk *root = NULL, *parent;

	for (parent = _clk; parent != NULL; parent = clk_get_parent(parent))
		root = parent;

	return root;
}

int ax_pclk_adj_param_get_ioctl(struct drm_device *dev, void *data, struct drm_file *filp)
{
	int ret;
	u32 rate;
	struct ax_drm_mgr *ax_drm_mgr = dev->dev_private;
	struct drm_crtc *crtc = ax_drm_mgr->crtc;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_pclk_adj_param *pclk_adj = (struct ax_pclk_adj_param *)data;
	struct clk *clk_pll;

	clk_pll = clk_get_root(ax_crtc->vpll_ref_clk);

	clk_get_rate(clk_pll);

	rate = clk_get_rate(ax_crtc->vpll_ref_clk);

	if (pclk_adj->type != VPLL_ADJ_TYPE_FREF) {
		clk_pll = ax_crtc->vpll_clk;
		rate = clk_get_rate(ax_crtc->pclk);
	}

	ret = pll_cfg_get(clk_pll, &pclk_adj->fref, &pclk_adj->pre_div,
			  &pclk_adj->fvco, &pclk_adj->post_div,
			  &pclk_adj->param0, &pclk_adj->param1);

	if (rate)
		pclk_adj->post_div = (pclk_adj->fvco + (rate >> 1)) / rate;
	else
		pclk_adj->post_div = (1 << pclk_adj->post_div);

	pclk_adj->pre_div = (1 << pclk_adj->pre_div);

	return ret;
}

static int ax_crtc_enable_vblank(struct drm_crtc *crtc)
{
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;

	DRM_DEBUG_DRIVER("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);

	if (!dp_funs) {
		DRM_WARN("crtc%d display funcs invalid\n", ax_crtc->id);
		return -EINVAL;
	}

	if (dp_funs->dispc_int_unmask)
		dp_funs->dispc_int_unmask(dp_dev->data);

	return 0;
}

static void ax_crtc_disable_vblank(struct drm_crtc *crtc)
{
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;

	DRM_DEBUG_DRIVER("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);

	if (!dp_funs) {
		DRM_WARN("crtc%d display funcs invalid\n", ax_crtc->id);
		return;
	}

	if (dp_funs->dispc_int_mask)
		dp_funs->dispc_int_mask(dp_dev->data);
}

static int compare_z_coordinate(const void *a, const void *b)
{
	struct ax_fb *fb1 = (struct ax_fb *)a;
	struct ax_fb *fb2 = (struct ax_fb *)b;

	return AX_FB_GET_PLANE_TYPE(fb1->type) - AX_FB_GET_PLANE_TYPE(fb2->type);
}

static void composer(struct drm_crtc *crtc, struct drm_atomic_state *state)
{
	int i;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct drm_crtc_state *crtc_state = crtc->state;
	struct drm_crtc_state *old_crtc_state = drm_atomic_get_old_crtc_state(state, crtc);
	struct ax_composer *composer = &ax_crtc->composer;
	struct ax_plane *ax_plane;
	struct drm_plane *plane;
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;
	struct ax_composer_info composer_info = {0,};

	DRM_DEBUG("state:%d-%d, old_state:%d-%d, num_active_planes: %d\n",
		  crtc_state->enable, crtc_state->active,
		  old_crtc_state->enable, old_crtc_state->active,
		  composer->num_active_planes);

	if (!dp_funs) {
		DRM_WARN("crtc%d display funcs invalid\n", ax_crtc->id);
		return;
	}

	for (i = 0, ax_crtc->fb_nr = 0; i < composer->num_active_planes; i++) {
		if (composer->active_planes[i]) {
			plane = composer->active_planes[i]->plane;
			ax_plane = to_ax_plane(plane);
			DRM_DEBUG("plane = 0x%p, type = %d\n", plane, plane->type);
			ax_crtc->fb[ax_crtc->fb_nr] = ax_plane->fb;
			ax_crtc->fb_nr += 1;
		}
	}

	if (ax_crtc->fb_nr == 0) {
		DRM_DEBUG("no active planes\n");
		return;
	}

	sort(ax_crtc->fb, ax_crtc->fb_nr, sizeof(ax_crtc->fb[0]), compare_z_coordinate, NULL);

	composer_info.reso_w = crtc->mode.hdisplay;
	composer_info.reso_h = crtc->mode.vdisplay;

	composer_info.props = ax_crtc->disp_props;
	ax_crtc->disp_props.flags = 0;

	if (dp_funs->dispc_fb_commit) {
		composer_info.fb_nr = ax_crtc->fb_nr;
		composer_info.fb = ax_crtc->fb;
		dp_funs->dispc_fb_commit(dp_dev->data, &composer_info);
	}
}

static int ax_crtc_atomic_set_property(struct drm_crtc *crtc,
				       struct drm_crtc_state *state,
				       struct drm_property *property, uint64_t val)
{
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);

	if (property == ax_crtc->props.brightness) {
		ax_crtc->disp_props.brightness = (u32)val;
		ax_crtc->disp_props.flags |= DISP_PROP_FLAG_BRIGHTNESS;
	} else if (property == ax_crtc->props.contrast) {
		ax_crtc->disp_props.contrast = (u32)val;
		ax_crtc->disp_props.flags |= DISP_PROP_FLAG_CONTRAST;
	} else if (property == ax_crtc->props.saturation) {
		ax_crtc->disp_props.saturation = (u32)val;
		ax_crtc->disp_props.flags |= DISP_PROP_FLAG_SATURATION;
	} else if (property == ax_crtc->props.hue) {
		ax_crtc->disp_props.hue = (u32)val;
		ax_crtc->disp_props.flags |= DISP_PROP_FLAG_HUE;
	} else if (property == ax_crtc->props.csc) {
		ax_crtc->disp_props.csc = (u32)val;
		ax_crtc->disp_props.flags |= DISP_PROP_FLAG_CSC;
	} else if (property == ax_crtc->props.work_mode) {
		ax_crtc->work_mode = (u32)val;
	} else if (property == ax_crtc->props.ctrl) {
		ax_crtc->ctrl_action = (u32)val;
		if (ax_crtc->ctrl_action == CRTC_CTRL_ACTION_ENABLING) {
			struct ax_display_dev *dp_dev = ax_crtc->display_dev;
			struct ax_display_funcs *dp_funs = dp_dev->display_funs;

			ax_crtc->ctrl_action = CRTC_CTRL_ACTION_INVALID;

			if (ax_crtc->status == CRTC_STATUS_DISABLED) {
				if (dp_funs && dp_funs->dispc_enable) {
					ax_crtc->status = CRTC_STATUS_ENABLED;
					dp_funs->dispc_enable(dp_dev->data);
				}
			}
		}

	} else if (property == ax_crtc->props.vpll_config) {
		u32 fbk_mode;
		ulong old_rate, new_rate, old_ref_rate, new_ref_rate;

		ax_crtc->vpll_config = (u32)val;

		fbk_mode = (ax_crtc->vpll_config & VPLL_FBK_REF_BIT) >> VPLL_FBK_REF_SHIFT;

		old_rate = clk_get_rate(ax_crtc->vpll_clk);

		new_ref_rate = (fbk_mode == VPLL_FBK_INT_REF_21M) ? VPLL_REF_RATE_21M : VPLL_REF_RATE_27M;
		old_ref_rate = clk_get_rate(ax_crtc->vpll_ref_clk);
		if (new_ref_rate != old_ref_rate) {
			clk_set_rate(ax_crtc->vpll_ref_clk, new_ref_rate);
			DRM_DEBUG_DRIVER("change ref-rate %lu to %lu\n", old_ref_rate, new_ref_rate);
		}

		pll_fbk_mode_set(ax_crtc->vpll_clk, fbk_mode);

		new_rate = clk_get_rate(ax_crtc->vpll_clk);
		if (old_rate != new_rate) {
			clk_set_rate(ax_crtc->vpll_clk, old_rate);
			clk_get_rate(clk_get_root(ax_crtc->vpll_ref_clk));
		}

	} else if (property == ax_crtc->props.intf_type) {
		ax_crtc->intf_type = val;
		ax_crtc->mode.type = ax_crtc->intf_type;
	} else if (property == ax_crtc->props.fmt_out) {
		ax_crtc->fmt_out = val;
		ax_crtc->mode.fmt_out = ax_crtc->fmt_out;
	}

	return 0;
}

static int ax_crtc_atomic_get_property(struct drm_crtc *crtc,
				       const struct drm_crtc_state *state,
				       struct drm_property *property, uint64_t *val)
{
	int ret = 0;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;
	struct ax_disp_props disp_props = {0,};

	if (property == ax_crtc->props.id) {
		*val = ax_crtc->id;
		return 0;
	} else if (property == ax_crtc->props.work_mode) {
		*val = ax_crtc->work_mode;
		return 0;
	} else if (property == ax_crtc->props.ctrl) {
		*val = ax_crtc->ctrl_action;
		return 0;
	} else if (property == ax_crtc->props.vpll_config) {
		*val = ax_crtc->vpll_config;
		return 0;
	} else if (property == ax_crtc->props.intf_type) {
		*val = ax_crtc->intf_type;
		return 0;
	} else if (property == ax_crtc->props.fmt_out) {
		*val = ax_crtc->fmt_out;
		return 0;
	}

	if (property == ax_crtc->props.brightness)
		disp_props.flags |= DISP_PROP_FLAG_BRIGHTNESS;
	else if (property == ax_crtc->props.contrast)
		disp_props.flags |= DISP_PROP_FLAG_CONTRAST;
	else if (property == ax_crtc->props.saturation)
		disp_props.flags |= DISP_PROP_FLAG_SATURATION;
	else if (property == ax_crtc->props.hue)
		disp_props.flags |= DISP_PROP_FLAG_HUE;
	else if (property == ax_crtc->props.csc)
		disp_props.flags |= DISP_PROP_FLAG_CSC;

	if (dp_funs->dispc_get_props) {
		ret = dp_funs->dispc_get_props(dp_dev->data, &disp_props);
		if (!ret) {
			if (property == ax_crtc->props.brightness)
				*val = disp_props.brightness;
			else if (property == ax_crtc->props.contrast)
				*val = disp_props.contrast;
			else if (property == ax_crtc->props.saturation)
				*val = disp_props.saturation;
			else if (property == ax_crtc->props.hue)
				*val = disp_props.hue;
			else if (property == ax_crtc->props.csc)
				*val = disp_props.csc;
		}
	}

	return ret;
}

static const struct drm_crtc_funcs ax_crtc_funcs = {
	.atomic_destroy_state = drm_atomic_helper_crtc_destroy_state,
	.atomic_duplicate_state = drm_atomic_helper_crtc_duplicate_state,
	.destroy = drm_crtc_cleanup,
	.page_flip = drm_atomic_helper_page_flip,
	.reset = drm_atomic_helper_crtc_reset,
	.set_config = drm_atomic_helper_set_config,
	.atomic_set_property = ax_crtc_atomic_set_property,
	.atomic_get_property = ax_crtc_atomic_get_property,
	.enable_vblank = ax_crtc_enable_vblank,
	.disable_vblank = ax_crtc_disable_vblank,
};

static void ax_dispc_irq_handler(void *param)
{
	unsigned long flags;
	struct ax_crtc *ax_crtc = (struct ax_crtc *)param;
	struct ax_drm_mgr *drm_mgr = ax_crtc->drm_mgr;
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;

	/* Ignoring vsync events after dispc disabled */
	if (ax_crtc->status == CRTC_STATUS_DISABLED) {
		DRM_INFO("crtc-%d ignoring vsync events\n", ax_crtc->id);
		return;
	}

	if (ax_crtc->ctrl_action == CRTC_CTRL_ACTION_DISABLING) {
		ax_crtc->ctrl_action = CRTC_CTRL_ACTION_INVALID;
		if (dp_funs->dispc_disable) {
			dp_funs->dispc_disable(dp_dev->data, AX_DISP_OFF_TYPE_LOW_LATENCY);
			ax_crtc->status = CRTC_STATUS_DISABLED;
		}
	}

	drm_crtc_handle_vblank(&ax_crtc->base);

	spin_lock_irqsave(&drm_mgr->drm_dev->event_lock, flags);
	if (ax_crtc->event) {
		drm_crtc_send_vblank_event(&ax_crtc->base, ax_crtc->event);
		drm_crtc_vblank_put(&ax_crtc->base);
		ax_crtc->event = NULL;
	}
	spin_unlock_irqrestore(&drm_mgr->drm_dev->event_lock, flags);
}

static int ax_crtc_atomic_set_gamma(struct ax_crtc *ax_crtc,
				    struct drm_crtc_state *state)
{
	if (!state->color_mgmt_changed || !state->gamma_lut)
		return 0;

	ax_crtc->disp_props.gamma_blob = (u64)state->gamma_lut->data;
	ax_crtc->disp_props.flags |= DISP_PROP_FLAG_HSV_LUT;
	return 0;
}

static void ax_crtc_atomic_enable(struct drm_crtc *crtc, struct drm_atomic_state *state)
{
	int ret = 0;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct logo_mem_reserved *logo_mem = &boot_logo_mem[ax_crtc->id];
	struct drm_crtc_state *crtc_state = crtc->state;
	struct drm_crtc_state *old_crtc_state = drm_atomic_get_old_crtc_state(state, crtc);
	struct drm_display_mode *adjusted_mode = &crtc_state->adjusted_mode;
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;
	struct ax_drm_mgr *drm_mgr = ax_crtc->drm_mgr;

	DRM_DEBUG_DRIVER("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);
	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(&crtc_state->mode));
	DRM_DEBUG_DRIVER("adjusted_mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(adjusted_mode));

	if (!dp_funs) {
		DRM_WARN("crtc%d display funcs invalid\n", ax_crtc->id);
		return;
	}

	mutex_lock(&drm_mgr->sleep_lock);
	/* System resume from sleep. */
	if (drm_mgr->sleep_state == AX_DISP_SLEEP) {
		if (dp_funs->dpu_resume)
			dp_funs->dpu_resume(dp_dev->data);

	} else if (!old_crtc_state->enable && !old_crtc_state->active) { /* Dpms-on no reset */
		if (dp_funs->dpu_reset)
			dp_funs->dpu_reset(dp_dev->data);
	}
	mutex_unlock(&drm_mgr->sleep_lock);

	ax_crtc_atomic_set_gamma(ax_crtc, crtc_state);

	if (logo_mem->base && logo_mem->size) {
		DRM_DEBUG_DRIVER("free display%d boot logo, base: 0x%px, size: 0x%px\n", logo_mem->id,
				 (void *)logo_mem->base, (void *)logo_mem->size);
		memblock_free(logo_mem->base, logo_mem->size);
		logo_mem->base = 0;
		logo_mem->size = 0;
	}

	if (dp_funs->dispc_set_irq_callback)
		dp_funs->dispc_set_irq_callback(dp_dev->data, ax_dispc_irq_handler, ax_crtc);

	ax_crtc->mode.fmt_in = AX_VO_FORMAT_NV12;
	ax_crtc->mode.hdisplay = adjusted_mode->hdisplay;
	ax_crtc->mode.hsync_start = adjusted_mode->hsync_start;
	ax_crtc->mode.hsync_end = adjusted_mode->hsync_end;
	ax_crtc->mode.htotal = adjusted_mode->htotal;
	ax_crtc->mode.vdisplay = adjusted_mode->vdisplay;
	ax_crtc->mode.vsync_start = adjusted_mode->vsync_start;
	ax_crtc->mode.vsync_end = adjusted_mode->vsync_end;
	ax_crtc->mode.vtotal = adjusted_mode->vtotal;
	ax_crtc->mode.clock = (adjusted_mode->flags & DRM_MODE_FLAG_DBLCLK) ? (adjusted_mode->clock * 2) : adjusted_mode->clock;
	ax_crtc->mode.vrefresh = drm_mode_vrefresh(adjusted_mode);
	/* dpu registor hs vs polarity bit is 0 for low active,
	 * 1 for high active. For history reason, Invert hs vs
	 * setting . */
	ax_crtc->mode.hp_pol = (adjusted_mode->flags & DRM_MODE_FLAG_PHSYNC) ? 0 : 1;
	ax_crtc->mode.vp_pol = (adjusted_mode->flags & DRM_MODE_FLAG_PVSYNC) ? 0 : 1;

	if (dp_funs->dispc_config) {
		ret = dp_funs->dispc_config(dp_dev->data, ax_crtc->work_mode, &ax_crtc->mode);
		if (!ret) {
			ax_crtc->status = CRTC_STATUS_CONFIGURED;
			DRM_DEBUG_DRIVER("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);
		} else {
			DRM_ERROR("crtc%d %s config failed\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode]);
		}

		dp_dev->status = AX_DISP_DEV_STATUS_USED;
	}

	drm_crtc_vblank_on(crtc);
}

static void ax_crtc_atomic_disable(struct drm_crtc *crtc, struct drm_atomic_state *state)
{
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;
	struct ax_drm_mgr *drm_mgr = ax_crtc->drm_mgr;

	DRM_DEBUG_DRIVER("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);

	if (!dp_funs) {
		DRM_WARN("crtc%d display funcs invalid\n", ax_crtc->id);
		return;
	}

	if (dp_funs->dispc_disable) {
		dp_funs->dispc_disable(dp_dev->data, AX_DISP_OFF_TYPE_NORMAL);
		dp_dev->status = AX_DISP_DEV_STATUS_UNUSED;
	}

	drm_crtc_vblank_off(crtc);

	if (crtc->state->event && !crtc->state->active) {
		spin_lock_irq(&crtc->dev->event_lock);
		drm_crtc_send_vblank_event(crtc, crtc->state->event);
		spin_unlock_irq(&crtc->dev->event_lock);

		crtc->state->event = NULL;
	}

	mutex_lock(&drm_mgr->sleep_lock);
	/* System suspend for sleep. */
	if (drm_mgr->sleep_state == AX_DISP_SLEEP) {
		if (dp_funs->dpu_suspend) {
			dp_funs->dpu_suspend(dp_dev->data);
		}
	}
	mutex_unlock(&drm_mgr->sleep_lock);

	ax_crtc->status = CRTC_STATUS_DISABLED;
}

static void ax_crtc_atomic_begin(struct drm_crtc *crtc, struct drm_atomic_state *state)
{
	unsigned long flags;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);

	DRM_DEBUG("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);

	if (crtc->state->event) {
		WARN_ON(drm_crtc_vblank_get(crtc) != 0);

		spin_lock_irqsave(&crtc->dev->event_lock, flags);
		ax_crtc->event = crtc->state->event;
		spin_unlock_irqrestore(&crtc->dev->event_lock, flags);
		crtc->state->event = NULL;
	}
}

static void ax_crtc_atomic_flush(struct drm_crtc *crtc, struct drm_atomic_state *state)
{
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_display_dev *dp_dev = ax_crtc->display_dev;
	struct ax_display_funcs *dp_funs = dp_dev->display_funs;

	DRM_DEBUG("crtc%d %s %s\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode], crtc_stat[ax_crtc->status]);

	composer(crtc, state);

	if ((ax_crtc->status == CRTC_STATUS_CONFIGURED) || (ax_crtc->status == CRTC_STATUS_DISABLED)) {
		if (dp_funs && dp_funs->dispc_enable) {
			ax_crtc->status = CRTC_STATUS_ENABLED;
			dp_funs->dispc_enable(dp_dev->data);
		}
	}
}

static int ax_crtc_atomic_check_gamma(struct drm_crtc *crtc,
				      struct drm_crtc_state *state)
{
	size_t lut_size;

	if (!state->color_mgmt_changed || !state->gamma_lut)
		return 0;

	if (crtc->state->gamma_lut &&
	    (crtc->state->gamma_lut->base.id == state->gamma_lut->base.id))
		return 0;

	if (state->gamma_lut->length % sizeof(struct drm_color_lut))
		return -EINVAL;

	lut_size = state->gamma_lut->length / sizeof(struct drm_color_lut);
	if (lut_size != DISP_GAMMA_SIZE)
		return -EINVAL;

	return 0;
}

static int ax_crtc_atomic_check(struct drm_crtc *crtc,
				struct drm_atomic_state *state)
{
	int ret = 0;
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc);
	struct ax_composer *composer = &ax_crtc->composer;
	struct drm_crtc_state *crtc_state = drm_atomic_get_new_crtc_state(state, crtc);
	struct drm_plane *plane;
	struct drm_plane_state *plane_state;

	composer->num_active_planes = 0;
	drm_for_each_plane_mask(plane, crtc->dev, crtc_state->plane_mask) {
		plane_state = drm_atomic_get_new_plane_state(state, plane);
		if (plane_state && plane_state->visible) {
			composer->active_planes[composer->num_active_planes] = plane_state;
			composer->num_active_planes += 1;
		}
	}

	DRM_DEBUG("crtc%d %s %s, active_planes:%d\n", ax_crtc->id, crtc_mode[ax_crtc->work_mode],
		  crtc_stat[ax_crtc->status],
		  composer->num_active_planes);

	ret = ax_crtc_atomic_check_gamma(crtc, crtc_state);

	return ret;
}

static const struct drm_crtc_helper_funcs ax_crtc_helper_funcs = {
	.atomic_check = ax_crtc_atomic_check,
	.atomic_begin = ax_crtc_atomic_begin,
	.atomic_flush = ax_crtc_atomic_flush,
	.atomic_enable = ax_crtc_atomic_enable,
	.atomic_disable = ax_crtc_atomic_disable,
};

static int ax_crtc_create(struct device *dev, u32 id, struct ax_drm_mgr *drm_mgr)
{
	struct ax_crtc *ax_crtc;
	struct drm_crtc *crtc;
	struct platform_device *pdev = to_platform_device(dev);
	struct drm_device *drm_dev = drm_mgr->drm_dev;
	int ret;

	ax_crtc = devm_kzalloc(drm_dev->dev, sizeof(*ax_crtc), GFP_KERNEL);
	if (!ax_crtc)
		return -ENOMEM;

	ax_crtc->id = id;
	ax_crtc->drm_mgr = drm_mgr;

	ax_crtc->vpll_clk = devm_clk_get(dev, "clk_vpll");
	if (IS_ERR(ax_crtc->vpll_clk)) {
		DRM_INFO("%pOF/clk_vpll not found\n", dev->of_node);
		ax_crtc->vpll_clk = NULL;
	}

	ax_crtc->vpll_ref_clk = devm_clk_get(dev, "clk_vpll_ref");
	if (IS_ERR(ax_crtc->vpll_ref_clk)) {
		DRM_INFO("%pOF/clk_vpll_ref not found\n", dev->of_node);
		ax_crtc->vpll_ref_clk = NULL;
	}

	ax_crtc->pclk = devm_clk_get(dev, "pclk");
	if (IS_ERR(ax_crtc->pclk)) {
		DRM_INFO("%pOF/pclk not found\n", dev->of_node);
		ax_crtc->pclk = NULL;
	}

	/* Each crtc only supports three planes, video plane, graph plane and cursor plane */
	ax_crtc->num_planes = AX_PLANES_MAX;
	crtc = &ax_crtc->base;

	crtc->port = of_get_next_child(dev->of_node, NULL);
	of_node_put(dev->of_node);
	if (IS_ERR(crtc->port)) {
		DRM_ERROR("%pOF/port not found\n", dev->of_node);
		ret = PTR_ERR(crtc->port);
		return ret;
	}

	drm_mgr->crtc = crtc;
	drm_mgr->id = id;
	ax_crtc->display_dev = &drm_mgr->dp_mgr->display_dev[id];

	ret = ax_plane_create(crtc);
	if (ret) {
		DRM_ERROR("%pOF: failed to create plane\n", dev->of_node);
		return ret;
	}

	ret = drm_crtc_init_with_planes(drm_dev, crtc, ax_crtc->primary_plane, NULL, &ax_crtc_funcs, "ax_crtc");
	if (ret) {
		DRM_ERROR("%pOF: failed to init crtc\n", dev->of_node);
		return ret;
	}

	ax_crtc->props.id = drm_property_create_range(drm_dev, DRM_MODE_PROP_ATOMIC, "DEV_ID", 0, AX_CRTC_MAX);
	if (!ax_crtc->props.id)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.id, id);

	ax_crtc->props.work_mode = drm_property_create_range(drm_dev, DRM_MODE_PROP_ATOMIC, "WORK_MODE", AX_DISP_MODE_OFFLINE, AX_DISP_MODE_BUTT);
	if (!ax_crtc->props.work_mode)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.work_mode, AX_DISP_MODE_OFFLINE);

	ax_crtc->props.ctrl = drm_property_create_range(drm_dev, DRM_MODE_PROP_ATOMIC, "CTRL", CRTC_CTRL_ACTION_DISABLING, CRTC_CTRL_ACTION_INVALID);
	if (!ax_crtc->props.ctrl)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.ctrl, CRTC_CTRL_ACTION_INVALID);
	ax_crtc->ctrl_action = CRTC_CTRL_ACTION_INVALID;

	ax_crtc->props.vpll_config = drm_property_create_range(drm_dev, DRM_MODE_PROP_ATOMIC, "VPLL_CONFIG", 0, 0xFFFFFFFF);
	if (!ax_crtc->props.vpll_config)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.vpll_config, 0);

	ax_crtc->props.intf_type = drm_property_create_range(drm_dev, DRM_MODE_PROP_ATOMIC, "INTF_TYPE",
							     AX_DISP_OUT_MODE_BT601, AX_DISP_OUT_MODE_HDMI);
	if (!ax_crtc->props.intf_type)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.intf_type, AX_DISP_OUT_MODE_DPI);

	ax_crtc->intf_type = AX_DISP_OUT_MODE_DPI;

	ax_crtc->props.fmt_out = drm_property_create_range(drm_dev, DRM_MODE_PROP_ATOMIC, "FMT_OUT",
							   AX_DISP_OUT_FMT_RGB565, AX_DISP_OUT_FMT_YUV422_10);
	if (!ax_crtc->props.fmt_out)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.fmt_out, AX_DISP_OUT_FMT_RGB565);

	ax_crtc->fmt_out = AX_DISP_OUT_FMT_RGB565;

	ax_crtc->props.brightness = drm_property_create_range(drm_dev, 0, "BRIGHTNESS", 0, DISP_USER_BRIGHTNESS_MAX);
	if (!ax_crtc->props.brightness)
		return -ENOMEM;

	ax_crtc->props.contrast = drm_property_create_range(drm_dev, 0, "CONTRAST", 0, DISP_USER_CONTRAST_MAX);
	if (!ax_crtc->props.contrast)
		return -ENOMEM;

	ax_crtc->props.saturation = drm_property_create_range(drm_dev, 0, "SATURATION", 0, DISP_USER_SATURATION_MAX);
	if (!ax_crtc->props.saturation)
		return -ENOMEM;

	ax_crtc->props.hue = drm_property_create_range(drm_dev, 0, "HUE", 0, DISP_USER_HUE_MAX);
	if (!ax_crtc->props.hue)
		return -ENOMEM;

	ax_crtc->props.csc = drm_property_create_range(drm_dev, 0, "CSC", 0, AX_VO_CSC_MATRIX_BUTT - 1);
	if (!ax_crtc->props.csc)
		return -ENOMEM;

	drm_object_attach_property(&crtc->base, ax_crtc->props.brightness, 0);
	drm_object_attach_property(&crtc->base, ax_crtc->props.contrast, 0);
	drm_object_attach_property(&crtc->base, ax_crtc->props.saturation, 0);
	drm_object_attach_property(&crtc->base, ax_crtc->props.hue, 0);
	drm_object_attach_property(&crtc->base, ax_crtc->props.csc, 0);

	drm_mode_crtc_set_gamma_size(crtc, DISP_GAMMA_SIZE);
	/* No inverse-gamma, No ctm. */
	drm_crtc_enable_color_mgmt(crtc, 0, false, DISP_GAMMA_SIZE);

	drm_crtc_helper_add(crtc, &ax_crtc_helper_funcs);

	ax_crtc->status = CRTC_STATUS_INIT;

	platform_set_drvdata(pdev, ax_crtc);

	return 0;
}

static int ax_crtc_bind(struct device *dev, struct device *master, void *data)
{
	int ret;
	u32 id;
	struct drm_device *drm_dev = (struct drm_device *)data;
	struct ax_drm_mgr *ax_drm_mgr = drm_dev->dev_private;
	struct platform_device *pdev = to_platform_device(dev);
	struct ax_crtc *ax_crtc;

	ret = of_property_read_u32(dev->of_node, "id", &id);
	if (ret || (id >= AX_CRTC_MAX)) {
		DRM_ERROR("%pOF: id invalid\n", dev->of_node);
		goto err0;
	}

	ret = ax_crtc_create(dev, id, ax_drm_mgr);
	if (ret) {
		DRM_ERROR("%pOF: failed to create crtc\n", dev->of_node);
		goto err0;
	}

	ax_crtc = platform_get_drvdata(pdev);
	pll_ref_set(ax_crtc->vpll_clk, ax_crtc->vpll_ref_clk);

	DRM_INFO("%pOF bind success\n", dev->of_node);

	return 0;

err0:
	DRM_ERROR("%pOF bind failure(%d)\n", dev->of_node, ret);

	return ret;
}

static void ax_crtc_unbind(struct device *dev, struct device *master, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct ax_crtc *ax_crtc;

	ax_crtc = platform_get_drvdata(pdev);
	pll_ref_set(ax_crtc->vpll_clk, NULL);

	DRM_INFO("%pOF unbind done\n", dev->of_node);
}

const struct component_ops crtc_component_ops = {
	.bind = ax_crtc_bind,
	.unbind = ax_crtc_unbind,
};

static int ax_crtc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	if (!dev->of_node)
		return -ENODEV;

	return component_add(dev, &crtc_component_ops);
}

static int ax_crtc_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	component_del(dev, &crtc_component_ops);

	return 0;
}

static const struct of_device_id crtc_drm_dt_ids[] = {
	{
	 .compatible = "axera,crtc",
	 },
	{},
};

MODULE_DEVICE_TABLE(of, crtc_drm_dt_ids);

struct platform_driver crtc_platform_driver = {
	.probe = ax_crtc_probe,
	.remove = ax_crtc_remove,
	.driver = {
		   .name = "dpu-drv",
		   .of_match_table = of_match_ptr(crtc_drm_dt_ids),
		   },
};

static int __init reserved_mem_boot_logo(struct reserved_mem *rmem)
{
	u32 devid, len;
	const __be32 *prop;

	if (rmem == NULL)
		return -EINVAL;

	prop = of_get_flat_dt_prop(rmem->fdt_node, "id", &len);
	if (!prop)
		return -ENOENT;

	if (len != sizeof(__be32)) {
		pr_err("invalid devid property in boot_logo_reserved node\n");
		return -EINVAL;
	}

	devid = be32_to_cpu(*prop);
	if (devid >= AX_DISPLAY_MAX) {
		pr_err("invalid devid(%d)\n", devid);
		return -EINVAL;
	}

	boot_logo_mem[devid].id = devid;
	boot_logo_mem[devid].base = rmem->base;
	boot_logo_mem[devid].size = rmem->size;

	pr_info("display%d boot logo memory at 0x%px, size is 0x%px\n",
		devid, (void *)rmem->base, (void *)rmem->size);

	return 0;
}

RESERVEDMEM_OF_DECLARE(boot_logo_reserved, "boot_logo", reserved_mem_boot_logo);

MODULE_DESCRIPTION("axera dpu driver");
MODULE_LICENSE("GPL v2");
