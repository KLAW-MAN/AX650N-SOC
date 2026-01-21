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
#include <cpu_func.h>
#include <splash.h>
#include <clk.h>
#include <display.h>
#include <dm.h>
#include <malloc.h>
#include <mapmem.h>
#include <edid.h>
#include <regmap.h>
#include <syscon.h>
#include <video.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <asm/unaligned.h>
#include <linux/err.h>
#include <linux/compiler.h>
#include <power/regulator.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch/ax650.h>

#include "ax_vo.h"

#define DISPLAY_MODE_FLAG_PHSYNC		(1<<0)
#define DISPLAY_MODE_FLAG_NHSYNC		(1<<1)
#define DISPLAY_MODE_FLAG_PVSYNC		(1<<2)
#define DISPLAY_MODE_FLAG_NVSYNC		(1<<3)

struct display_mode {
	u32 clock;

	u16 hdisplay, hsync_start, hsync_end, htotal;
	u16 vdisplay, vsync_start, vsync_end, vtotal;

	u32 vrefresh;

	u32 flags;
	u32 type;
};

static struct display_mode cea_dmt_modes[AX_VO_OUTPUT_USER] = {
	[AX_VO_OUTPUT_576P50] = {27000, 720, 732, 796, 864, 576, 581, 586, 625, 50, 0xa},
	[AX_VO_OUTPUT_480P60] = {27000, 720, 736, 798, 858, 480, 489, 495, 525, 60, 0xa},
	[AX_VO_OUTPUT_720P50] = {74250, 1280, 1720, 1760, 1980, 720, 725, 730, 750, 50, 0x5},
	[AX_VO_OUTPUT_720P60] = {74250, 1280, 1390, 1430, 1650, 720, 725, 730, 750, 60, 0x5},
	[AX_VO_OUTPUT_1080P24] = {74250, 1920, 2558, 2602, 2750, 1080, 1084, 1089, 1125, 24, 0x5},
	[AX_VO_OUTPUT_1080P25] = {74250, 1920, 2448, 2492, 2640, 1080, 1084, 1089, 1125, 25, 0x5},
	[AX_VO_OUTPUT_1080P30] = {74250, 1920, 2008, 2052, 2200, 1080, 1084, 1089, 1125, 30, 0x5},
	[AX_VO_OUTPUT_1080P50] = {148500, 1920, 2448, 2492, 2640, 1080, 1084, 1089, 1125, 50, 0x5},
	[AX_VO_OUTPUT_1080P60] = {148500, 1920, 2008, 2052, 2200, 1080, 1084, 1089, 1125, 60, 0x5},
	[AX_VO_OUTPUT_640x480_60] = {25175, 640, 656, 752, 800, 480, 490, 492, 525, 60, 0xa},
	[AX_VO_OUTPUT_800x600_60] = {40000, 800, 840, 968, 1056, 600, 601, 605, 628, 60, 0x5},
	[AX_VO_OUTPUT_1024x768_60] = {65000, 1024, 1048, 1184, 1344, 768, 771, 777, 806, 60, 0xa},
	[AX_VO_OUTPUT_1280x1024_60] = {108000, 1280, 1328, 1440, 1688, 1024, 1025, 1028, 1066, 60, 0x5},
	[AX_VO_OUTPUT_1366x768_60] = {85500, 1366, 1436, 1579, 1792, 768, 771, 774, 798, 60, 0x5},
	[AX_VO_OUTPUT_1280x800_60] = {83500, 1280, 1352, 1480, 1680, 800, 803, 809, 831, 60, 0x6},
	[AX_VO_OUTPUT_1440x900_60] = {106500, 1440, 1520, 1672, 1904, 900, 903, 909, 934, 60, 0x6},
	[AX_VO_OUTPUT_1600x1200_60] = {162000, 1600, 1664, 1856, 2160, 1200, 1201, 1204, 1250, 60, 0x5},
	[AX_VO_OUTPUT_1680x1050_60] = {146250, 1680, 1784, 1960, 2240, 1050, 1053, 1059, 1089, 60, 0x6},
	[AX_VO_OUTPUT_1920x1200_60] = {154000, 1920, 1968, 2000, 2080, 1200, 1203, 1209, 1235, 60, 0x9},
	[AX_VO_OUTPUT_2560x1600_60] = {348500, 2560, 2752, 3032, 3504, 1600, 1603, 1609, 1658, 60, 0x6},
	[AX_VO_OUTPUT_3840x2160_24] = {297000, 3840, 5116, 5204, 5500, 2160, 2168, 2178, 2250, 24, 0x5},
	[AX_VO_OUTPUT_3840x2160_25] = {297000, 3840, 4896, 4984, 5280, 2160, 2168, 2178, 2250, 25, 0x5},
	[AX_VO_OUTPUT_3840x2160_30] = {297000, 3840, 4016, 4104, 4400, 2160, 2168, 2178, 2250, 30, 0x5},
	[AX_VO_OUTPUT_3840x2160_50] = {594000, 3840, 4896, 4984, 5280, 2160, 2168, 2178, 2250, 50, 0x5},
	[AX_VO_OUTPUT_3840x2160_60] = {594000, 3840, 4016, 4104, 4400, 2160, 2168, 2178, 2250, 60, 0x5},
	[AX_VO_OUTPUT_4096x2160_24] = {297000, 4096, 5116, 5204, 5500, 2160, 2168, 2178, 2250, 24, 0x5},
	[AX_VO_OUTPUT_4096x2160_25] = {297000, 4096, 5064, 5152, 5280, 2160, 2168, 2178, 2250, 25, 0x5},
	[AX_VO_OUTPUT_4096x2160_30] = {297000, 4096, 4184, 4272, 4400, 2160, 2168, 2178, 2250, 30, 0x5},
	[AX_VO_OUTPUT_4096x2160_50] = {594000, 4096, 5064, 5152, 5280, 2160, 2168, 2178, 2250, 50, 0x5},
	[AX_VO_OUTPUT_4096x2160_60] = {594000, 4096, 4184, 4272, 4400, 2160, 2168, 2178, 2250, 60, 0x5},
	[AX_VO_OUTPUT_720x1280_60] = {74250, 720, 802, 816, 936, 1280, 1300, 1310, 1322, 60, 0xa},
	[AX_VO_OUTPUT_1080x1920_60] = {148500, 1080, 1100, 1170, 1200, 1920, 1970, 2020, 2062, 60, 0xa},
};

static int display_mode2timing(u32 sync, struct display_timing *timing)
{
	struct display_mode *dp_mode;

	if ((sync >= AX_VO_OUTPUT_USER) || !timing)
		return -EINVAL;

	dp_mode = &cea_dmt_modes[sync];

	memset(timing, 0, sizeof(*timing));

	timing->pixelclock.typ = dp_mode->clock * 1000;
	timing->hactive.typ = dp_mode->hdisplay;
	timing->hfront_porch.typ = dp_mode->hsync_start - dp_mode->hdisplay;
	timing->hsync_len.typ = dp_mode->hsync_end - dp_mode->hsync_start;
	timing->hback_porch.typ = dp_mode->htotal - dp_mode->hsync_end;

	timing->vactive.typ = dp_mode->vdisplay;
	timing->vfront_porch.typ = dp_mode->vsync_start - dp_mode->vdisplay;
	timing->vsync_len.typ = dp_mode->vsync_end - dp_mode->vsync_start;
	timing->vback_porch.typ = dp_mode->vtotal - dp_mode->vsync_end;

	if (dp_mode->flags & DISPLAY_MODE_FLAG_PHSYNC)
		timing->flags |= DISPLAY_FLAGS_HSYNC_HIGH;
	else
		timing->flags |= DISPLAY_FLAGS_HSYNC_LOW;

	if (dp_mode->flags & DISPLAY_MODE_FLAG_PVSYNC)
		timing->flags |= DISPLAY_FLAGS_VSYNC_HIGH;
	else
		timing->flags |= DISPLAY_FLAGS_VSYNC_LOW;

	/* The default DE polarity is the same as HSYNC */
	timing->flags |= (timing->flags & DISPLAY_FLAGS_HSYNC_HIGH) ? DISPLAY_FLAGS_DE_HIGH : DISPLAY_FLAGS_DE_LOW;

	return 0;
}

extern struct dpu_hw_ops ax650_dpu_hw_ops;

static struct ax_vo_pipe g_vo_pipes[VO_NR] = {
	{
		.id = 0,
		.ddev = {
			.id = 0,
			.hdev = {
				.id = 0,
				.regs = (void __iomem *)AX_VO_DPU0_BASE_ADDR,
			},
			.ops = &ax650_dpu_hw_ops,
		},
		.bridge = {
			[BRIDGE_INDEX_HDMI] = {
				.id = 0,
				.bridge_init = ax_dw_hdmi_init,
				.bridge_enable = ax_dw_hdmi_enable,
			},
			[BRIDGE_INDEX_DSI] = {
				.id = 0,
				.bridge_init = ax_dw_mipi_dsi_init,
				.bridge_enable = ax_dw_mipi_dsi_enable,
			},
		},
		.pixel_clk_set = ax650_pixel_clk_set,
	},
	{
		.id = 1,
		.ddev = {
			.id = 1,
			.hdev = {
				.id = 1,
				.regs = (void __iomem *)AX_VO_DPU1_BASE_ADDR,
			},
			.ops = &ax650_dpu_hw_ops,
		},
		.bridge = {
			[BRIDGE_INDEX_HDMI] = {
				.id = 1,
				.bridge_init = ax_dw_hdmi_init,
				.bridge_enable = ax_dw_hdmi_enable,
			},
		},
		.pixel_clk_set = ax650_pixel_clk_set,
	},
};

int ax_bridge_init(struct ax_bridge_device *bridge_dev, struct display_timing *timing, struct display_timing *adjusted_timing)
{
	int ret = 0;

	if (bridge_dev && bridge_dev->bridge_init)
		ret = bridge_dev->bridge_init(bridge_dev->id, timing, adjusted_timing, bridge_dev->data);

	return ret;
}

int ax_bridge_enable(struct ax_bridge_device *bridge_dev, const struct display_timing *timing)
{
	int ret = 0;

	if (bridge_dev && bridge_dev->bridge_enable)
		ret = bridge_dev->bridge_enable(bridge_dev->id, timing);

	return ret;
}

void ax_dispc_enable(struct ax_dpu_device *ddev)
{
	if (ddev->ops && ddev->ops->dispc_enable)
		ddev->ops->dispc_enable(&ddev->hdev);
}

int ax_dispc_config(struct ax_dpu_device *ddev, struct ax_disp_mode *mode)
{
	int ret = 0;

	if (ddev->ops && ddev->ops->dispc_config)
		ret = ddev->ops->dispc_config(&ddev->hdev, mode);

	return ret;
}

void ax_dispc_set_buffer(struct ax_dpu_device *ddev, u64 addr_y, u64 addr_uv,
                         u32 stride_y, u32 stride_uv)
{
	if (ddev->ops && ddev->ops->dispc_set_buffer)
		ddev->ops->dispc_set_buffer(&ddev->hdev, addr_y, addr_uv, stride_y, stride_uv);
}

int ax_dpu_init(struct ax_dpu_device *ddev)
{
	int ret = 0;

	if (ddev->ops && ddev->ops->dpu_init)
		ret = ddev->ops->dpu_init(&ddev->hdev);

	return ret;
}

int ax_draw_task_commit(struct ax_dpu_device *ddev, struct draw_task *task)
{
	int ret = 0;

	if (ddev->ops && ddev->ops->draw_start)
		ret = ddev->ops->draw_start(task);

	return ret;
}

static int draw_task_check(struct ax_dpu_device *ddev, struct draw_task *task)
{
	int ret = 0;

	if (ddev->ops && ddev->ops->draw_start)
		ret = ddev->ops->task_valid(task);

	return ret;
}

static int ax_vo_display(struct ax_dpu_device *ddev, struct display_info *dp_info)
{
	int ret = 0, src_img_szie;
	struct draw_task task = {0};

	task.src_w = dp_info->img_width;
	task.src_h = dp_info->img_height;
	task.src_fmt = dp_info->img_fmt;

	task.src_stride_y = task.src_w * 3;
	task.src_phy_addr_y = dp_info->img_addr;

	task.dst_w = ddev->mode.hdisplay;
	task.dst_h = ddev->mode.vdisplay;

	if (dp_info->display_x >= task.dst_w)
		task.dst_x = 0;
	else
		task.dst_x = dp_info->display_x & (~0x1);

	if (task.dst_x + task.src_w > task.dst_w)
		task.src_w = task.dst_w - task.dst_x;

	if (dp_info->display_y >= task.dst_h)
		task.dst_y = 0;
	else
		task.dst_y = dp_info->display_y & (~0x1);

	if (task.dst_y + task.src_h > task.dst_h)
		task.src_h = task.dst_h - task.dst_y;

	task.dst_fmt = AX_VO_FORMAT_NV12;
	task.dst_stride_y = (ddev->mode.hdisplay + 0xf) & (~0xf);
	task.dst_stride_c = task.dst_stride_y;
	task.dst_phy_addr_y = dp_info->display_addr;
	task.dst_phy_addr_c = task.dst_phy_addr_y + task.dst_stride_y * task.dst_h;

	task.data = &ddev->hdev;
	task.cmd_buf_phys = (task.dst_phy_addr_y + task.dst_stride_c * task.dst_h * 3 / 2 + 0xF) & (~0xFUL);
	task.matrix_csc_type = CSC_MATRIX_RGB_TO_BT601_TV;

	ret = draw_task_check(ddev, &task);
	if (ret) {
		VO_ERROR("dpu%d draw task illegal parameters\n", ddev->id);
		return ret;
	}

	src_img_szie = task.src_stride_y * task.src_h;
	flush_dcache_range(task.src_phy_addr_y & (~(CONFIG_SYS_CACHELINE_SIZE - 1)),
	                   ALIGN(task.src_phy_addr_y + src_img_szie, CONFIG_SYS_CACHELINE_SIZE));

	ret = ax_draw_task_commit(ddev, &task);
	if (ret) {
		VO_ERROR("dpu%d draw task commit failed, ret = %d\n", ddev->id, ret);
		return ret;
	}

	ax_dispc_set_buffer(ddev, task.dst_phy_addr_y, task.dst_phy_addr_c,
	                    task.dst_stride_y, task.dst_stride_c);

	ax_dispc_enable(ddev);

	return 0;
}

static inline int type2bridge_index(u32 type)
{
	switch (type) {
	case AX_DISP_OUT_MODE_DSI_VIDEO:
	case AX_DISP_OUT_MODE_DSI_CMD:
		return BRIDGE_INDEX_DSI;
	case AX_DISP_OUT_MODE_HDMI:
		return BRIDGE_INDEX_HDMI;
	}

	return BRIDGE_INDEX_BUTT;
}

static int display_timing2disp_mode(struct display_timing *timing, struct ax_disp_mode *mode)
{
	mode->flags = 0;
	mode->clock = timing->pixelclock.typ / 1000;		/* in kHz */
	mode->hdisplay = timing->hactive.typ;
	mode->hsync_start = mode->hdisplay + timing->hfront_porch.typ;
	mode->hsync_end = mode->hsync_start + timing->hsync_len.typ;
	mode->htotal = mode->hsync_end + timing->hback_porch.typ;
	mode->vdisplay = timing->vactive.typ;
	mode->vsync_start = mode->vdisplay + timing->vfront_porch.typ;
	mode->vsync_end = mode->vsync_start + timing->vsync_len.typ;
	mode->vtotal = mode->vsync_end + timing->vback_porch.typ;
	mode->hp_pol = timing->flags & DISPLAY_FLAGS_HSYNC_HIGH ? 0 : 1;
	mode->vp_pol = timing->flags & DISPLAY_FLAGS_VSYNC_HIGH ? 0 : 1;
	mode->de_pol = timing->flags & DISPLAY_FLAGS_DE_HIGH ? 0 : 1;

	mode->vrefresh = (timing->pixelclock.typ + ((mode->vtotal * mode->htotal) >> 1)) /
	                 (mode->vtotal * mode->htotal);

	VO_DEBUG("reso: %dx%d, flags: 0x%x, pclk: %d\n",
	         timing->hactive.typ, timing->vactive.typ, timing->flags,
	         timing->pixelclock.typ);

	return 0;
}

#if 0
static int ax_vo_read_timing(u32 devid, struct display_timing *timing)
{
	int panel_bits_per_colour;
	u8 buf[EDID_EXT_SIZE];
	int ret;

	ret = ax_dw_hdmi_read_edid(devid, buf, sizeof(buf));
	if (ret < 0)
		return ret;

	return edid_get_timing_validate(buf, ret, timing,
	                                &panel_bits_per_colour,
	                                NULL, NULL);
}
#endif

static int ax_vo_pipe_init(struct ax_vo_pipe *pipe)
{
	int ret;
	u32 bridge_index;
	u32 type = pipe->type, sync = pipe->sync;
	struct ax_dpu_device *ddev = &pipe->ddev;
	struct ax_disp_mode *mode = &ddev->mode;
	struct display_timing timing = {0}, adjusted_timing;

	VO_DEBUG("pipe%d\n", pipe->id);

	if (sync < AX_VO_OUTPUT_BUTT) {
		ret = display_mode2timing(sync, &timing);
		if (ret < 0) {
			VO_ERROR("dpu%d sync(%d) invalid\n", ddev->id, sync);
			return ret;
		}
	}

	adjusted_timing = timing;

	VO_DEBUG("timing: %d %d %d %d %d %d %d %d %d 0x%x\n",
		  timing.pixelclock.typ, timing.hactive.typ, timing.hfront_porch.typ,
		  timing.hback_porch.typ, timing.hsync_len.typ, timing.vactive.typ,
		  timing.vfront_porch.typ, timing.vback_porch.typ, timing.vsync_len.typ,
		  timing.flags);

	ret = ax_dpu_init(ddev);
	if (ret) {
		VO_ERROR("failed to init dpu%d\n", ddev->id);
		return ret;
	}

	bridge_index = type2bridge_index(type);

	if (bridge_index < BRIDGE_INDEX_BUTT) {
		struct ax_bridge_device *bridge = &pipe->bridge[bridge_index];

		bridge->data = pipe->panel_cfg;

		ret = ax_bridge_init(bridge, &timing, &adjusted_timing);
		if (ret) {
			VO_ERROR("failed to init vo%d bridge%d\n", pipe->id, bridge_index);
			return ret;
		}
	}

	VO_DEBUG("adjusted_timing: %d %d %d %d %d %d %d %d %d 0x%x\n",
		  adjusted_timing.pixelclock.typ, adjusted_timing.hactive.typ, adjusted_timing.hfront_porch.typ,
		  adjusted_timing.hback_porch.typ, adjusted_timing.hsync_len.typ, adjusted_timing.vactive.typ,
		  adjusted_timing.vfront_porch.typ, adjusted_timing.vback_porch.typ, adjusted_timing.vsync_len.typ,
		  adjusted_timing.flags);

	if (pipe->panel_cfg && pipe->panel_cfg->init) {
		ret = pipe->panel_cfg->init(pipe->panel_cfg);
		if (ret) {
			VO_ERROR("failed to panel init, ret:%d\n", ret);
			return ret;
		}
	}

	if (pipe->pixel_clk_set) {
		ret = pipe->pixel_clk_set(pipe->id, adjusted_timing.pixelclock.typ);
		if (ret) {
			VO_ERROR("failed to set vo%d pixel clock\n", pipe->id);
			return ret;
		}
	}

	mode->type = type;
	mode->fmt_in = AX_VO_FORMAT_NV12;
	mode->fmt_out = AX_DISP_OUT_FMT_RGB888;

	ret = display_timing2disp_mode(&adjusted_timing, mode);
	if (ret) {
		VO_ERROR("failed to get dpu%d mode\n", ddev->id);
		return ret;
	}

	ret = ax_dispc_config(ddev, mode);
	if (ret) {
		VO_ERROR("failed to config dpu%d\n", ddev->id);
		return ret;
	}

	if (bridge_index < BRIDGE_INDEX_BUTT) {
		ret = ax_bridge_enable(&pipe->bridge[bridge_index], &adjusted_timing);
		if (ret) {
			VO_ERROR("failed to enable vo%d bridge%d\n", pipe->id, bridge_index);
			return ret;
		}
	}

	ddev->timing = adjusted_timing;

	VO_DEBUG("pipe%d success\n", pipe->id);

	return 0;
}

int fdt_fixup_vo_init_mode(int devid, void *fdt)
{
	int ret, offset, parent_offset;
	uint32_t val;
	char path[128], *name = "init-mode";
	struct ax_dpu_device *ddev = &g_vo_pipes[devid].ddev;
	struct display_timing *timing = &ddev->timing;

	if (!timing->pixelclock.typ)
		return -EPERM;

	sprintf(path, "/soc/drm@%d", devid);

	parent_offset = fdt_path_offset(fdt, path);
	if (parent_offset < 0) {
		VO_ERROR("%s not found\n", path);
		return -EINVAL;
	}

	offset = fdt_add_subnode(fdt, parent_offset, name);
	if (offset < 0) {
		VO_ERROR("add %s to %s failed, ret = %d\n", name, path, offset);
		return offset;
	}

	val = cpu_to_fdt32(ddev->mode.type);
	ret = fdt_setprop(fdt, offset, "interface-type", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set interface-type to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->pixelclock.typ);
	ret = fdt_setprop(fdt, offset, "clock-frequency", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set clock-frequency to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->hactive.typ);
	ret = fdt_setprop(fdt, offset, "hactive", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set hactive to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->hfront_porch.typ);
	ret = fdt_setprop(fdt, offset, "hfront-porch", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set hfront-porch to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->hsync_len.typ);
	ret = fdt_setprop(fdt, offset, "hsync-len", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set hsync-len to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->hback_porch.typ);
	ret = fdt_setprop(fdt, offset, "hback-porch", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set hback-porch to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->vactive.typ);
	ret = fdt_setprop(fdt, offset, "vactive", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set vactive to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->vfront_porch.typ);
	ret = fdt_setprop(fdt, offset, "vfront-porch", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set vfront-porch to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->vsync_len.typ);
	ret = fdt_setprop(fdt, offset, "vsync-len", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set vsync-len to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(timing->vback_porch.typ);
	ret = fdt_setprop(fdt, offset, "vback-porch", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set vback-porch to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(!!(timing->flags & DISPLAY_FLAGS_HSYNC_HIGH));
	ret = fdt_setprop(fdt, offset, "hsync-active", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set hsync-active to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(!!(timing->flags & DISPLAY_FLAGS_VSYNC_HIGH));
	ret = fdt_setprop(fdt, offset, "vsync-active", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set vsync-active to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	val = cpu_to_fdt32(!!(timing->flags & DISPLAY_FLAGS_DE_HIGH));
	ret = fdt_setprop(fdt, offset, "de-active", &val, sizeof(val));
	if (ret) {
		VO_ERROR("set de-active to %s failed, ret = %d\n", name, ret);
		goto exit;
	}

	if (!!(timing->flags & DISPLAY_FLAGS_INTERLACED)) {
		ret = fdt_setprop(fdt, offset, "interlaced", NULL, 0);
		if (ret) {
			VO_ERROR("set interlaced to %s failed, ret = %d\n", name, ret);
			goto exit;
		}
	}

	if (!!(timing->flags & DISPLAY_FLAGS_DOUBLESCAN)) {
		ret = fdt_setprop(fdt, offset, "doublescan", NULL, 0);
		if (ret) {
			VO_ERROR("set doublescan to %s failed, ret = %d\n", name, ret);
			goto exit;
		}
	}

exit:
	if (ret)
		fdt_del_node(fdt, offset);

	VO_INFO("add %s node %s\n", name, ret ? "failed" : "success");

	return ret;
}

int ax_start_vo(u32 devid, u32 type, u32 sync, struct display_info *dp_info)
{
	int ret;
	struct ax_vo_pipe *pipe;

	/* Before relocation we don't need to do anything */
	if (!(gd->flags & GD_FLG_RELOC))
		return 0;

	if (devid >= VO_NR) {
		VO_ERROR("invalid devid(%d)\n", devid);
		return -EINVAL;
	}

	pipe = &g_vo_pipes[devid];

	if (type >= AX_DISP_OUT_MODE_BUT) {
		VO_ERROR("invalid type(%d)\n", type);
		return -EINVAL;
	}

	pipe->type = type;
	pipe->sync = sync;
	pipe->panel_cfg = dp_info->priv;

	ret = ax_vo_pipe_init(pipe);
	if (ret)
		return ret;

	return ax_vo_display(&pipe->ddev, dp_info);
}
