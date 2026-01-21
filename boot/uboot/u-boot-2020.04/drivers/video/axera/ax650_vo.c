/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/dma-mapping.h>
#include <common.h>
#include <asm/io.h>

#include "ax650_vo_reg.h"
#include "ax_vo.h"

enum {
	HW_LAYER_TYPE_VIDEO,
	HW_LAYER_TYPE_GRAPHIC,
	HW_LAYER_TYPE_BUTT,
};

static inline void dpu_intr_mask0(struct dpu_hw_device *hdev, u32 mask)
{
	u32 val;

	val = dpu_readl(hdev->regs, DPU_INT_MASK0);
	val |= mask;
	vo_writel(hdev->regs, DPU_INT_MASK0, val);
}

static inline void dpu_intr_mask1(struct dpu_hw_device *hdev, u32 mask)
{
	u32 val;

	val = dpu_readl(hdev->regs, DPU_INT_MASK1);
	val |= mask;
	vo_writel(hdev->regs, DPU_INT_MASK1, val);
}

static inline void dpu_intr_unmask0(struct dpu_hw_device *hdev, u32 mask)
{
	u32 val;

	val = dpu_readl(hdev->regs, DPU_INT_MASK0);
	val &= (~mask);
	vo_writel(hdev->regs, DPU_INT_MASK0, val);
}

static inline void dpu_intr_unmask1(struct dpu_hw_device *hdev, u32 mask)
{
	u32 val;

	val = dpu_readl(hdev->regs, DPU_INT_MASK1);
	val &= (~mask);
	vo_writel(hdev->regs, DPU_INT_MASK1, val);
}

static void dpu_intr_mask_all(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_INT_MASK0, 0x7);
	vo_writel(hdev->regs, DPU_INT_MASK1, 0xF);
	vo_writel(hdev->regs, DPU_INT_DSP_MASK0, 0x7);
	vo_writel(hdev->regs, DPU_INT_DSP_MASK1, 0xF);
}

static void dpu_set_mode(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_TOP_CTRL, hdev->is_online ? 0x0 : 0x1);
}

void dispc_update_lock(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_DISP_UP, 0);
}

void dispc_update_unlock(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_DISP_UP, 1);
}

static void dispc_enable(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_DISP_EN, 0x1);
}

static void dispc_disable(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_DISP_EN, 0x0);
}

static void dispc_set_out_mode(struct dpu_hw_device *hdev, u32 mode)
{
	vo_writel(hdev->regs, DPU_OUT_MODE, mode);
}

static void dispc_set_format_in(struct dpu_hw_device *hdev, u32 fmt)
{
	vo_writel(hdev->regs, DPU_READ_FORMAT, fmt);
}

static void dispc_set_format_out(struct dpu_hw_device *hdev, u32 fmt)
{
	vo_writel(hdev->regs, DPU_DISP_FORMAT, fmt);
}

static void dispc_set_bt_mode(struct dpu_hw_device *hdev, u32 mode)
{
	vo_writel(hdev->regs, DPU_BT_MODE, mode);
}

static void dispc_set_disp_reso(struct dpu_hw_device *hdev, u32 reso)
{
	vo_writel(hdev->regs, DPU_DISP_RESO, reso);
}

static void dispc_set_timings(struct dpu_hw_device *hdev, struct ax_disp_mode *mode)
{
	u32 hp_pol, vp_pol, de_pol;
	u32 hbp, hfp, vbp, vfp, hpw, vpw;
	u32 hhalf = 0;

	hp_pol = mode->hp_pol;
	vp_pol = mode->vp_pol;
	de_pol = mode->de_pol;

	VO_INFO("hp_pol = %d, vp_pol = %d, de_pol = %d, out_mode:%d\n", hp_pol, vp_pol, de_pol, hdev->out_mode.mode);

	hpw = mode->hsync_end - mode->hsync_start;
	vpw = mode->vsync_end - mode->vsync_start;

	hbp = mode->htotal - mode->hsync_end;
	hfp = mode->hsync_start - mode->hdisplay;

	vbp = mode->vtotal - mode->vsync_end;
	vfp = mode->vsync_start - mode->vdisplay;
	if (mode->flags & MODE_FLAG_INTERLACE) {
		vbp /= 2;
		vfp /= 2;
	}

	hhalf = mode->htotal / 2;

	if (hdev->out_mode.mode == OUT_MODE_BT1120) {
		hpw = mode->htotal - mode->hdisplay - 4;
		vpw = mode->vtotal - mode->vsync_start;
	} else if (hdev->out_mode.mode == OUT_MODE_BT656) {
		hpw = mode->htotal - mode->hdisplay - 2;
		vpw = mode->vtotal - mode->vsync_start;
	}
	if (mode->flags & MODE_FLAG_INTERLACE)
		vpw /= 2;

	VO_INFO("hbp = %d, hfp = %d, hpw = %d\n", hbp, hfp, hpw);
	VO_INFO("vbp = %d, vfp = %d, vpw = %d\n", vbp, vfp, vpw);

	vo_writel(hdev->regs, DPU_DISP_SYNC, (hpw << 16) | vpw);
	vo_writel(hdev->regs, DPU_DISP_HSYNC, (hbp << 16) | hfp);
	vo_writel(hdev->regs, DPU_DISP_VSYNC, (vbp << 16) | vfp);
	vo_writel(hdev->regs, DPU_DISP_HHALF, hhalf);

	if (mode->flags & MODE_FLAG_INTERLACE)
		vo_writel(hdev->regs, DPU_DISP_VTOTAL, !(mode->vtotal & 1));

	vo_writel(hdev->regs, DPU_DISP_POLAR, de_pol << 2 | hp_pol << 1 | vp_pol);
}

static void dispc_set_yuv2rgb_matrix(struct dpu_hw_device *hdev, const struct yuv2rgb_regs *m)
{
	int i, j;
	u32 reg_offs = DPU_DISP_2RGB_MATRIX_00;
	enum dispc_format_in fmt_in = hdev->out_mode.fmt_in;

	if (fmt_in == FMT_IN_YUV420_8 || fmt_in == FMT_IN_YUV420_10) {
		vo_writel(hdev->regs, DPU_DISP_UV_SEQ, m->uv_seq);

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				vo_writel(hdev->regs, reg_offs, m->matrix[i][j]);
				reg_offs += 4;
			}
		}

		vo_writel(hdev->regs, DPU_DISP_2RGB_OFFSET_0, m->offset[0]);
		vo_writel(hdev->regs, DPU_DISP_2RGB_OFFSET_1, m->offset[1]);
		vo_writel(hdev->regs, DPU_DISP_2RGB_OFFSET_2, m->offset[2]);

		vo_writel(hdev->regs, DPU_DISP_2RGB_EN, 1);

	} else {
		vo_writel(hdev->regs, DPU_DISP_2RGB_EN, 0);
	}
}

static void dispc_set_csc(struct dpu_hw_device *hdev, const struct csc_reg *csc)
{
	int i, j;
	u32 reg_offs = DPU_DISP_CSC0_MATRIX_00;
	enum dispc_format_out fmt_out = hdev->out_mode.fmt_out;

	/* rgb888 to yuv422 */
	if (fmt_out == FMT_OUT_YUV422 || fmt_out == FMT_OUT_YUV422_10) {
		vo_writel(hdev->regs, DPU_DISP_CSC0_EN, 1);

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				vo_writel(hdev->regs, reg_offs, csc->csc0.matrix[i][j]);
				reg_offs += 4;
			}
		}

		vo_writel(hdev->regs, DPU_DISP_CSC0_OFFSET_0_0, csc->csc0.offset[0][0]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_OFFSET_0_1, csc->csc0.offset[0][1]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_OFFSET_0_2, csc->csc0.offset[0][2]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_OFFSET_1_0, csc->csc0.offset[1][0]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_OFFSET_1_1, csc->csc0.offset[1][1]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_OFFSET_1_2, csc->csc0.offset[1][2]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H0, csc->csc0.decimation_h[0]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H1, csc->csc0.decimation_h[1]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H2, csc->csc0.decimation_h[2]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H3, csc->csc0.decimation_h[3]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H4, csc->csc0.decimation_h[4]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H5, csc->csc0.decimation_h[5]);
		vo_writel(hdev->regs, DPU_DISP_CSC0_DECIMATION_H6, csc->csc0.decimation_h[6]);
	} else {
		vo_writel(hdev->regs, DPU_DISP_CSC0_EN, 0);
	}
}

static void draw_disable(struct dpu_hw_device *hdev)
{
	vo_writel(hdev->regs, DPU_DRAW_EN, 0x0);
}

static bool src_format_valid(u32 vo_fmt)
{
	switch (vo_fmt) {
	case AX_VO_FORMAT_NV12:
	case AX_VO_FORMAT_NV21:
	case AX_VO_FORMAT_ARGB1555:
	case AX_VO_FORMAT_ARGB4444:
	case AX_VO_FORMAT_RGBA5658:
	case AX_VO_FORMAT_ARGB8888:
	case AX_VO_FORMAT_RGB565:
	case AX_VO_FORMAT_RGB888:
	case AX_VO_FORMAT_RGBA4444:
	case AX_VO_FORMAT_RGBA5551:
	case AX_VO_FORMAT_RGBA8888:
	case AX_VO_FORMAT_ARGB8565:
	case AX_VO_FORMAT_P010:
	case AX_VO_FORMAT_P016:
	case AX_VO_FORMAT_NV16:
	case AX_VO_FORMAT_P210:
	case AX_VO_FORMAT_P216:
	case AX_VO_FORMAT_BITMAP:
		return true;
	}

	return false;
}

static void fmt2bytes_per_pixel(u32 vo_fmt, u32 *bytes_per_pixel, u32 *nplanes)
{
	switch (vo_fmt) {
	case AX_VO_FORMAT_NV12:
	case AX_VO_FORMAT_NV21:
	case AX_VO_FORMAT_NV16:
		*bytes_per_pixel = 1;
		*nplanes = 2;
		break;
	case AX_VO_FORMAT_ARGB1555:
	case AX_VO_FORMAT_ARGB4444:
	case AX_VO_FORMAT_RGBA4444:
	case AX_VO_FORMAT_RGBA5551:
	case AX_VO_FORMAT_RGB565:
		*bytes_per_pixel = 2;
		*nplanes = 1;
		break;
	case AX_VO_FORMAT_RGBA5658:
	case AX_VO_FORMAT_ARGB8565:
	case AX_VO_FORMAT_RGB888:
		*bytes_per_pixel = 3;
		*nplanes = 1;
		break;
	case AX_VO_FORMAT_ARGB8888:
	case AX_VO_FORMAT_RGBA8888:
		*bytes_per_pixel = 4;
		*nplanes = 1;
		break;
	case AX_VO_FORMAT_P010:
	case AX_VO_FORMAT_P016:
	case AX_VO_FORMAT_P210:
	case AX_VO_FORMAT_P216:
		*bytes_per_pixel = 2;
		*nplanes = 2;
		break;
	case AX_VO_FORMAT_BITMAP:
		*bytes_per_pixel = 1;
		*nplanes = 1;
		break;
	}
}

static int geometry_valid(struct draw_task *task)
{
	u16 x, y, w, h, stride_y, stride_c;
	u32 supported_width_max, supported_height_max;
	u32 bytes_per_pixel = 1, nplanes = 1;
	u32 format;

	supported_width_max = task->dst_w;
	supported_height_max = task->dst_h;

	x = task->dst_x;
	y = task->dst_y;
	w = task->src_w;
	h = task->src_h;
	stride_y = task->src_stride_y;
	stride_c = task->src_stride_c;

	format = task->src_fmt;

	if (!src_format_valid(format)) {
		VO_ERROR("draw task src format(%d) invalid\n", format);
		return -EINVAL;
	}

	fmt2bytes_per_pixel(format, &bytes_per_pixel, &nplanes);

	if (w < 2) {
		VO_ERROR("draw task src width(%d) invalid\n", w);
		return -EINVAL;
	}

	if (h < 2) {
		VO_ERROR("draw task src height(%d) invalid\n", h);
		return -EINVAL;
	}

	if (x + w > supported_width_max) {
		w = supported_width_max - x;
		task->src_w = w;
	}

	if (y + h > supported_height_max) {
		h = supported_height_max - y;
		task->src_h = h;
	}

	if ((stride_y < w * bytes_per_pixel) || (stride_y >= DRAW_STRIDE_MAX) ||
	    (stride_y & (DRAW_ALIGNED_BYTES - 1))) {
		VO_ERROR("draw task src stride_y(%d) invalid\n", stride_y);
		return -EINVAL;
	}

	if ((nplanes > 1) && ((stride_c < w * bytes_per_pixel) || (stride_c >= DRAW_STRIDE_MAX) ||
			       (stride_c & (DRAW_ALIGNED_BYTES - 1)))) {
		VO_ERROR("draw task src stride_c(%d) invalid\n", stride_c);
		return -EINVAL;
	}

	if (x >= supported_width_max) {
		VO_ERROR("draw task dst_x(%d) invalid\n", x);
		return -EINVAL;
	}

	if (y >= supported_height_max) {
		VO_ERROR("draw task dst_y(%d) invalid\n", y);
		return -EINVAL;
	}

	return 0;
}

static int ax650_draw_task_valid(struct draw_task *task)
{
	int ret;
	u32 stride, bytes_per_pixel = 1, nplanes = 1;

	if (!task->data) {
		VO_ERROR("draw task data-field invalid\n");
		return -EINVAL;
	}

	if (!task->cmd_buf_phys || ((u64)task->cmd_buf_phys & 0xF)) {
		VO_ERROR("draw task cmd_buf_phys(0x%llx) invalid\n", task->cmd_buf_phys);
		return -EINVAL;
	}

	/* The layer output format check */
	if (task->dst_fmt > AX_VO_FORMAT_P010) {
		VO_ERROR("draw task dst_fmt(%d) invalid\n", task->dst_fmt);
		return -EINVAL;
	}

	fmt2bytes_per_pixel(task->dst_fmt, &bytes_per_pixel, &nplanes);

	if ((task->dst_w < DRAW_WIDTH_MIN) || (task->dst_w > DRAW_WIDTH_MAX) || (task->dst_w & 0x1)) {
		VO_ERROR("draw task dst_w(%d) invalid\n", task->dst_w);
		return -EINVAL;
	}

	if ((task->dst_h < DRAW_HEIGHT_MIN) || (task->dst_h > DRAW_WIDTH_MAX) || (task->dst_h & 0x1)) {
		VO_ERROR("draw task dst_h(%d) invalid\n", task->dst_h);
		return -EINVAL;
	}

	stride = task->dst_stride_y;
	if ((stride % bytes_per_pixel) || ((stride / bytes_per_pixel) < task->dst_w) ||
	    (stride >= DRAW_STRIDE_MAX) || (stride & (DRAW_ALIGNED_BYTES - 1))) {
		VO_ERROR("draw task dst stride_y(%d) invalid\n", stride);
		return -EINVAL;
	}

	stride = task->dst_stride_c;
	if (stride && ((stride % bytes_per_pixel) || ((stride / bytes_per_pixel) < task->dst_w) ||
	    (stride >= DRAW_STRIDE_MAX) || (stride & (DRAW_ALIGNED_BYTES - 1)))) {
		VO_ERROR("draw task dst stride_c(%d) invalid\n", stride);
		return -EINVAL;
	}

	ret = geometry_valid(task);
	if (ret)
		return ret;

	return 0;
}

static u32 ci_default_type(u32 format)
{
	switch (format) {
	case AX_VO_FORMAT_ARGB1555:
	case AX_VO_FORMAT_ARGB4444:
	case AX_VO_FORMAT_RGBA5658:
	case AX_VO_FORMAT_ARGB8888:
	case AX_VO_FORMAT_RGBA4444:
	case AX_VO_FORMAT_RGBA5551:
	case AX_VO_FORMAT_RGBA8888:
	case AX_VO_FORMAT_ARGB8565:
		return HW_LAYER_TYPE_GRAPHIC;

	default:
		return HW_LAYER_TYPE_VIDEO;
	}
}

static void draw_set_cmd(struct dpu_hw_device *hdev, struct draw_task *task, struct draw_cmd_node *cmd)
{
	u32 fmt, type;

	fmt = vo_fmt2hw_fmt(task->src_fmt);
	type = ci_default_type(task->src_fmt);

	cmd->coordinate_x = task->dst_x;
	cmd->coordinate_y = task->dst_y;
	cmd->width = task->src_w;
	cmd->height = task->src_h;
	cmd->config = LAYER_TYPE(type) | RB_SWAP(0) |
		      FBDC_EN(0) | PIXEL_FMT(fmt);
	cmd->comp_level = 0;
	cmd->alp = 0;

	cmd->addr_luma_l = PYHS_ADDR_LOW(task->src_phy_addr_y);
	cmd->addr_luma_h = PYHS_ADDR_HIGH(task->src_phy_addr_y);

	cmd->stride_luma = task->src_stride_y;

	cmd->addr_chro_l = PYHS_ADDR_LOW(task->src_phy_addr_c);
	cmd->addr_chro_h = PYHS_ADDR_HIGH(task->src_phy_addr_c);

	cmd->stride_chro = task->src_stride_c;

	cmd->crop_pos_x = 0;
	cmd->crop_pos_y = 0;
	cmd->zero_bits_cnt_y = 0;
	cmd->zero_bits_cnt_c = 0;
	cmd->pad[0] = 0;
	cmd->pad[1] = 0;
}

static int draw_color_space_cfg(struct draw_task *task)
{
	int i, j, k;
	u32 reg_offs;
	const struct color_space_cfg *cs_cfg;
	const struct yuv2rgb_regs *yuv2rgb_cfg;
	const struct csc_reg *csc;
	struct dpu_hw_device *hdev = (struct dpu_hw_device *)task->data;

	cs_cfg = get_csc_cfg(task->matrix_csc_type);
	if (!cs_cfg)
		return -EINVAL;

	yuv2rgb_cfg = &cs_cfg->yuv2rgb_cfg;
	csc = &cs_cfg->csc;

	/* yuv2rgb config */
	for (k = 0; k < 2; k++) {
		if (k == 0)
			reg_offs = DPU_V0_2RGB_MATRIX_00;
		else
			reg_offs = DPU_G0_2RGB_MATRIX_00;

		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				vo_writel(hdev->regs, reg_offs, yuv2rgb_cfg->matrix[i][j]);
				reg_offs += 4;
			}
		}

		vo_writel(hdev->regs, reg_offs, yuv2rgb_cfg->offset[0]);
		reg_offs += 4;

		vo_writel(hdev->regs, reg_offs, yuv2rgb_cfg->offset[1]);
		reg_offs += 4;

		vo_writel(hdev->regs, reg_offs, yuv2rgb_cfg->offset[2]);
		reg_offs += 4;

	}

	/* csc config */
	vo_writel(hdev->regs, DPU_DRAW_CSC0_EN, 1);

	reg_offs = DPU_DRAW_CSC0_MATRIX_00;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			vo_writel(hdev->regs, reg_offs, csc->csc0.matrix[i][j]);
			reg_offs += 4;
		}
	}

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			vo_writel(hdev->regs, reg_offs, csc->csc0.offset[i][j]);
			reg_offs += 4;
		}
	}

	for (i = 0; i < 7; i++) {
		vo_writel(hdev->regs, reg_offs, csc->csc0.decimation_h[i]);
		reg_offs += 4;
	}

	/* skip draw_csc0_uv_seq_sel register. */
	reg_offs += 4;
	vo_writel(hdev->regs, reg_offs, csc->csc1.decimation);

	return 0;
}

static void draw_out_cfg(struct draw_task *task)
{
	struct dpu_hw_device *hdev = (struct dpu_hw_device *)task->data;

	vo_writel(hdev->regs, DPU_INFO_ADDR_H, PYHS_ADDR_HIGH(task->cmd_buf_phys));
	vo_writel(hdev->regs, DPU_INFO_ADDR_L, PYHS_ADDR_LOW(task->cmd_buf_phys));
	vo_writel(hdev->regs, DPU_INFO_NUM, 1);
	flush_dcache_range(task->cmd_buf_phys & (~(CONFIG_SYS_CACHELINE_SIZE - 1)),
	                   ALIGN(task->cmd_buf_phys + sizeof(struct draw_cmd_node), CONFIG_SYS_CACHELINE_SIZE));

	vo_writel(hdev->regs, DPU_DRAW_RESO, (task->dst_h << 16) | task->dst_w);
	vo_writel(hdev->regs, DPU_DRAW_FORMAT, vo_fmt2hw_fmt(task->dst_fmt));

	vo_writel(hdev->regs, DPU_DRAW_ADDR_Y_H, PYHS_ADDR_HIGH(task->dst_phy_addr_y));
	vo_writel(hdev->regs, DPU_DRAW_ADDR_Y_L, PYHS_ADDR_LOW(task->dst_phy_addr_y));
	vo_writel(hdev->regs, DPU_DRAW_STRIDE_Y, task->dst_stride_y);

	vo_writel(hdev->regs, DPU_DRAW_ADDR_C_H, PYHS_ADDR_HIGH(task->dst_phy_addr_c));
	vo_writel(hdev->regs, DPU_DRAW_ADDR_C_L, PYHS_ADDR_LOW(task->dst_phy_addr_c));
	vo_writel(hdev->regs, DPU_DRAW_STRIDE_C, task->dst_stride_c);

	vo_writel(hdev->regs, DPU_DRAW_FBC_EN, 0);

	VO_DEBUG("dpu%d reso: %dx%d, fmt: %d, stride: [%d,%d], phy_addr: [0x%llx,0x%llx,0x%llx]\n", hdev->id,
		 task->dst_w, task->dst_h, task->dst_fmt,
		 task->dst_stride_y, task->dst_stride_c,
		 task->dst_phy_addr_y, task->dst_phy_addr_c, task->cmd_buf_phys);
}

static int ax650_draw_start(struct draw_task *task)
{
	u32 back_pixel, blend = 0, dst_fmt = task->dst_fmt;
	struct dpu_hw_device *hdev = (struct dpu_hw_device *)task->data;

	back_pixel = task->bk_pixel;

	/* 8bit needs to be extended to 10bit */
	if (dst_fmt != AX_VO_FORMAT_P010 && dst_fmt != AX_VO_FORMAT_P016 &&
	    dst_fmt != AX_VO_FORMAT_P210 && dst_fmt != AX_VO_FORMAT_P216)
	    back_pixel = ((((task->bk_pixel >> 16) & 0xFF) << 2) << 20) |
			 ((((task->bk_pixel >> 8) & 0xFF) << 2) << 10) |
			 (((task->bk_pixel & 0xFF) << 2) << 0);

	if (vo_fmt2hw_fmt(dst_fmt) <= FORMAT_RGBA8888)
		blend = 0x1;

	vo_writel(hdev->regs, DPU_DRAW_EN, 0);
	vo_writel(hdev->regs, DPU_DRAW_UP, 0);

	vo_writel(hdev->regs, DPU_V0_BACK_PIXEL, back_pixel);
	vo_writel(hdev->regs, DPU_V0_BACK_ALPHA, 0x0);

	if (draw_color_space_cfg(task)) {
		VO_ERROR("failed to dpu%d draw csc\n", hdev->id);
		return -EINVAL;
	}

	draw_set_cmd(hdev, task, (struct draw_cmd_node *)task->cmd_buf_phys);

	draw_out_cfg(task);

	vo_writel(hdev->regs, DPU_G_BLENDING, blend);

	vo_writel(hdev->regs, DPU_DRAW_UP, 1);
	vo_writel(hdev->regs, DPU_DRAW_EN, 1);

	return 0;
}

static void ax650_dispc_enable(struct dpu_hw_device *hdev)
{
	dispc_enable(hdev);
}

static void ax650_dispc_disable(struct dpu_hw_device *hdev)
{
	dispc_disable(hdev);
}

static void ax650_dispc_set_out_mode(struct dpu_hw_device *hdev, struct dispc_out_mode dispc_out)
{
	u32 out_mode = 0;

	hdev->out_mode = dispc_out;

	switch (dispc_out.mode) {
	case OUT_MODE_BT601:
	case OUT_MODE_BT656:
	case OUT_MODE_BT1120:
		out_mode |= (0x1 << 2);
		break;

	case OUT_MODE_DPI:
		out_mode |= (0x1 << 1);
		break;

	case OUT_MODE_DSI_VIDEO:
		out_mode |= (0x1 << 3);
		break;

	case OUT_MODE_DSI_CMD:
		out_mode |= (0x1 << 4);
		break;

	case OUT_MODE_HDMI:
		out_mode |= (0x1 << 0);
		break;
	}

	dispc_set_format_in(hdev, dispc_out.fmt_in);
	dispc_set_format_out(hdev, dispc_out.fmt_out);
	dispc_set_out_mode(hdev, out_mode);
}

static void dispc_dither(struct dpu_hw_device *dev)
{
	vo_writel(dev->regs, DPU_DITHER_EN, 1);
	vo_writel(dev->regs, DPU_DITHER_UP, 1);

#if 0
	vo_writel(dev->regs, DPU_ADJ_SATU_EN, 0);
	vo_writel(dev->regs, DPU_SATURATION, 0x400);

	vo_writel(dev->regs, DPU_LUT_EN, 0);
	vo_writel(dev->regs, DPU_ADJ_BC_EN, 0);
	vo_writel(dev->regs, DPU_BRIGHTNESS, 0xff);
	vo_writel(dev->regs, DPU_CONTRAST, 0x400);
#endif
}

static int ax650_dispc_config(struct dpu_hw_device *hdev, struct ax_disp_mode *mode)
{
	int tmp, ret = 0;
	u32 reso, bt_mode, clk;
	struct dispc_out_mode dispc_out;

	hdev->mode = *mode;

	dispc_disable(hdev);

	ret = display_out_mode_convert(mode, &dispc_out);
	if (ret)
		goto exit;

	ax650_dispc_set_out_mode(hdev, dispc_out);

	if (hdev->mode.flags & MODE_FLAG_INTERLACE)
		reso = ((mode->vdisplay / 2 << DISPC_RESO_HEIGHT_SHIFT) | mode->hdisplay);
	else
		reso = ((mode->vdisplay << DISPC_RESO_HEIGHT_SHIFT) | mode->hdisplay);

	dispc_set_disp_reso(hdev, reso);

	dispc_set_yuv2rgb_matrix(hdev, hdev->dispc_matrix);
	dispc_set_csc(hdev, hdev->dispc_csc);
	dispc_set_timings(hdev, &hdev->mode);

	VO_INFO("mode: %d, fmt_in: %d, fmt_out: %d\n", dispc_out.mode, dispc_out.fmt_in, dispc_out.fmt_out);

	clk = mode->clock * 1000;
	tmp = dispc_out.mode;
	switch (tmp) {
	case OUT_MODE_BT601:
	case OUT_MODE_BT656:
	case OUT_MODE_BT1120:
		if (dispc_out.mode == OUT_MODE_BT656)
			bt_mode = 0;
		else if (dispc_out.mode == OUT_MODE_BT601)
			bt_mode = 1;
		else
			bt_mode = 2;

		if (hdev->mode.flags & MODE_FLAG_INTERLACE) {
			if (tmp == OUT_MODE_BT1120)
				clk /= 2;
			/* set field mode */
			bt_mode |= (1 << 3);
			/*hdev->mode.flags |= MODE_FLAG_FIELD1_FIRST;
			bt_mode |= (1 << 4); //set field 1 at first.*/
		} else if (tmp == OUT_MODE_BT601 || tmp == OUT_MODE_BT656) {
			clk *= 2;
		}

		/* select lower 8bits */
		bt_mode |= 0x4;
		dispc_set_bt_mode(hdev, bt_mode);
		break;
	}

	dispc_dither(hdev);

	vo_writel(hdev->regs, DPU_DISP_CLK, 0);

exit:
	VO_INFO("done, ret = %d\n", ret);

	return ret;
}

static void ax650_dispc_set_buffer(struct dpu_hw_device *hdev, u64 addr_y, u64 addr_uv, u32 stride_y, u32 stride_uv)
{
	VO_DEBUG("dpu%d addr: [0x%llx, 0x%llx], stride: [%d, %d]\n", hdev->id,
		 addr_y, addr_uv, stride_y, stride_uv);

	dispc_update_lock(hdev);

	vo_writel(hdev->regs, DPU_DISP_ADDR_Y_L, PYHS_ADDR_LOW(addr_y));
	vo_writel(hdev->regs, DPU_DISP_ADDR_Y_H, PYHS_ADDR_HIGH(addr_y));
	vo_writel(hdev->regs, DPU_DISP_ADDR_C_L, PYHS_ADDR_LOW(addr_uv));
	vo_writel(hdev->regs, DPU_DISP_ADDR_C_H, PYHS_ADDR_HIGH(addr_uv));
	vo_writel(hdev->regs, DPU_DISP_STRIDE_Y, stride_y);
	vo_writel(hdev->regs, DPU_DISP_STRIDE_C, stride_uv);

	dispc_update_unlock(hdev);
}

static int ax650_hw_init(struct dpu_hw_device *hdev)
{
	const struct color_space_cfg *cs_cfg;

	cs_cfg = get_csc_cfg(CSC_MATRIX_BT601_TO_BT601);
	if (!cs_cfg)
		return -EINVAL;

	hdev->dispc_matrix = &cs_cfg->yuv2rgb_cfg;
	hdev->dispc_csc = &cs_cfg->csc;

	hdev->is_online = false;

	ax650_dpu_reset(hdev->id);

	draw_disable(hdev);
	dispc_disable(hdev);

	dpu_set_mode(hdev);

	dpu_intr_mask_all(hdev);

	dispc_update_unlock(hdev);

	vo_writel(hdev->regs, DPU_ADJ_SATU_EN, 0x0);
	vo_writel(hdev->regs, DPU_LUT_EN, 0x0);
	vo_writel(hdev->regs, DPU_ADJ_BC_EN, 0x0);

	vo_writel(hdev->regs, DPU_AXI_EN, 0x1);

	VO_INFO("dpu%d hw init success\n", hdev->id);

	return 0;
}

static void ax650_hw_deinit(struct dpu_hw_device *hdev)
{
	dispc_update_lock(hdev);

	draw_disable(hdev);
	dispc_disable(hdev);

	vo_writel(hdev->regs, DPU_AXI_EN, 0);
}

static int ax650_dpu_init(struct dpu_hw_device *hdev)
{
	return ax650_hw_init(hdev);
}

static void ax650_dpu_deinit(struct dpu_hw_device *hdev)
{
	ax650_hw_deinit(hdev);
}

struct dpu_hw_ops ax650_dpu_hw_ops = {
	.dpu_init = ax650_dpu_init,
	.dpu_deinit = ax650_dpu_deinit,
	.dispc_config = ax650_dispc_config,
	.dispc_enable = ax650_dispc_enable,
	.dispc_disable = ax650_dispc_disable,
	.dispc_set_buffer = ax650_dispc_set_buffer,
	.task_valid = ax650_draw_task_valid,
	.draw_start = ax650_draw_start,
};

