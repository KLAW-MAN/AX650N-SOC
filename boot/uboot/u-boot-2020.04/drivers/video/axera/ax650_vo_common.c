/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <common.h>
#include <asm/io.h>

#include "ax_vo.h"
#include "ax650_vo_csc.h"

#define PYHS_ADDR_HIGH(x)	(((x) >> 32) & 0x1F)
#define PYHS_ADDR_LOW(x)	((x) & 0xFFFFFFFF)

static const struct color_space_cfg cs_cfgs[CSC_MATRIX_BUTT] = {
	AX650_VO_CSC_DATA
};

const struct color_space_cfg *get_csc_cfg(u32 cs_type)
{
	if (cs_type >= CSC_MATRIX_BUTT) {
		VO_ERROR("unsupported color space type:%d\n", cs_type);
		return NULL;
	}

	return &cs_cfgs[cs_type];
}

int display_out_mode_convert(struct ax_disp_mode *mode, struct dispc_out_mode *dispc_out)
{
	int fmt_in = mode->fmt_in;
	int fmt_out = mode->fmt_out;

	VO_INFO("mode fmt_in: %d, fmt_out: %d\n", fmt_in, fmt_out);

	switch (mode->type) {
	case AX_DISP_OUT_MODE_BT601:
		dispc_out->mode = OUT_MODE_BT601;
		break;
	case AX_DISP_OUT_MODE_BT656:
		dispc_out->mode = OUT_MODE_BT656;
		break;
	case AX_DISP_OUT_MODE_BT1120:
		dispc_out->mode = OUT_MODE_BT1120;
		break;
	case AX_DISP_OUT_MODE_DPI:
		dispc_out->mode = OUT_MODE_DPI;
		break;
	case AX_DISP_OUT_MODE_DSI_VIDEO:
		dispc_out->mode = OUT_MODE_DSI_VIDEO;
		break;
	case AX_DISP_OUT_MODE_DSI_CMD:
		dispc_out->mode = OUT_MODE_DSI_CMD;
		break;
	case AX_DISP_OUT_MODE_HDMI:
		dispc_out->mode = OUT_MODE_HDMI;
		break;
	default:
		VO_ERROR("unsupported mode type, mode = %d\n", mode->type);
		return -EINVAL;
	}

	switch (fmt_in) {
	case AX_VO_FORMAT_ARGB1555:
		dispc_out->fmt_in = FMT_IN_ARGB1555;
		break;
	case AX_VO_FORMAT_ARGB4444:
		dispc_out->fmt_in = FMT_IN_ARGB4444;
		break;
	case AX_VO_FORMAT_ARGB8565:
		dispc_out->fmt_in = FMT_IN_ARGB8565;
		break;
	case AX_VO_FORMAT_ARGB8888:
		dispc_out->fmt_in = FMT_IN_ARGB8888;
		break;
	case AX_VO_FORMAT_RGBA5551:
		dispc_out->fmt_in = FMT_IN_RGBA5551;
		break;
	case AX_VO_FORMAT_RGBA4444:
		dispc_out->fmt_in = FMT_IN_RGBA4444;
		break;
	case AX_VO_FORMAT_RGBA5658:
		dispc_out->fmt_in = FMT_IN_RGBA5658;
		break;
	case AX_VO_FORMAT_RGBA8888:
		dispc_out->fmt_in = FMT_IN_RGBA8888;
		break;
	case AX_VO_FORMAT_RGB565:
		dispc_out->fmt_in = FMT_IN_RGB565;
		break;
	case AX_VO_FORMAT_RGB888:
		dispc_out->fmt_in = FMT_IN_RGB888;
		break;
	case AX_VO_FORMAT_NV12:
		dispc_out->fmt_in = FMT_IN_YUV420_8;
		break;
	case AX_VO_FORMAT_P010:
		dispc_out->fmt_in = FMT_IN_YUV420_10;
		break;
	default:
		VO_ERROR("unsupported fmt_in, fmt_in = %d\n", fmt_in);
		return -EINVAL;
	}

	switch (fmt_out) {
	case AX_DISP_OUT_FMT_RGB565:
		dispc_out->fmt_out = FMT_OUT_RGB565;
		break;
	case AX_DISP_OUT_FMT_RGB666:
		dispc_out->fmt_out = FMT_OUT_RGB666;
		break;
	case AX_DISP_OUT_FMT_RGB666LP:
		dispc_out->fmt_out = FMT_OUT_RGB666LP;
		break;
	case AX_DISP_OUT_FMT_RGB888:
		dispc_out->fmt_out = FMT_OUT_RGB888;
		break;
	case AX_DISP_OUT_FMT_RGB101010:
		dispc_out->fmt_out = FMT_OUT_RGB101010;
		break;
	case AX_DISP_OUT_FMT_YUV422:
		dispc_out->fmt_out = FMT_OUT_YUV422;
		break;
	case AX_DISP_OUT_FMT_YUV422_10:
		dispc_out->fmt_out = FMT_OUT_YUV422_10;
		break;
	default:
		VO_ERROR("unsupported fmt_out, fmt_out = %d\n", fmt_out);
		return -EINVAL;
	}

	return 0;
}

u32 vo_fmt2hw_fmt(u32 format)
{
	u32 hw_fmt = FORMAT_NO_SUPPORT;

	switch (format) {
	case AX_VO_FORMAT_NV12:
	case AX_VO_FORMAT_NV21:
		hw_fmt = FORMAT_YUV420_8;
		break;
	case AX_VO_FORMAT_ARGB1555:
		hw_fmt = FORMAT_ARGB1555;
		break;
	case AX_VO_FORMAT_ARGB4444:
		hw_fmt = FORMAT_ARGB4444;
		break;
	case AX_VO_FORMAT_RGBA5658:
		hw_fmt = FORMAT_RGBA5658;
		break;
	case AX_VO_FORMAT_ARGB8888:
		hw_fmt = FORMAT_ARGB8888;
		break;
	case AX_VO_FORMAT_RGB565:
		hw_fmt = FORMAT_RGB565;
		break;
	case AX_VO_FORMAT_RGB888:
		hw_fmt = FORMAT_RGB888;
		break;
	case AX_VO_FORMAT_RGBA4444:
		hw_fmt = FORMAT_RGBA4444;
		break;
	case AX_VO_FORMAT_RGBA5551:
		hw_fmt = FORMAT_RGBA5551;
		break;
	case AX_VO_FORMAT_RGBA8888:
		hw_fmt = FORMAT_RGBA8888;
		break;
	case AX_VO_FORMAT_ARGB8565:
		hw_fmt = FORMAT_ARGB8565;
		break;
	case AX_VO_FORMAT_P010:
		hw_fmt = FORMAT_YUV420_10;
		break;
	case AX_VO_FORMAT_P016:
		hw_fmt = FORMAT_YUV420_16;
		break;
	case AX_VO_FORMAT_NV16:
		hw_fmt = FORMAT_YUV422_8;
		break;
	case AX_VO_FORMAT_P210:
		hw_fmt = FORMAT_YUV422_10;
		break;
	case AX_VO_FORMAT_P216:
		hw_fmt = FORMAT_YUV422_16;
		break;
	case AX_VO_FORMAT_BITMAP:
		hw_fmt = FORMAT_BIT_MAP;
		break;
	default:
		VO_ERROR("unsupported format, format = %d\n", format);
		break;
	}

	return hw_fmt;
}

