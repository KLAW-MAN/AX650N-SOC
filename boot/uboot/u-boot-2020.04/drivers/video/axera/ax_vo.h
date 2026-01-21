/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_VO_H
#define __AX_VO_H

#include "ax_vo_common.h"
#include "ax650_vo_common.h"

struct display_info {
	u32 img_width;
	u32 img_height;
	u32 img_fmt;
	u32 img_stride;
	u64 img_addr;

	u32 display_x;
	u32 display_y;
	u64 display_addr;

	void *priv;
};

struct draw_task {
	u32 src_w: 16;
	u32 src_h: 16;
	u32 src_fmt;
	u32 src_stride_y: 16;
	u32 src_stride_c: 16;
	phys_addr_t src_phy_addr_y;
	phys_addr_t src_phy_addr_c;

	u32 dst_x: 16;
	u32 dst_y: 16;
	u32 dst_w: 16;
	u32 dst_h: 16;
	u32 dst_fmt;
	u32 dst_stride_y: 16;
	u32 dst_stride_c: 16;
	phys_addr_t dst_phy_addr_y;
	phys_addr_t dst_phy_addr_c;

	phys_addr_t cmd_buf_phys;

	u32 matrix_csc_type;
	u32 bk_pixel;

	void *data;
};

struct dpu_hw_ops {
	int (*dpu_init)(struct dpu_hw_device *hdev);
	void (*dpu_deinit)(struct dpu_hw_device *hdev);

	void (*dpu_reset)(struct dpu_hw_device *hdev);

	void (*dispc_set_out_mode)(struct dpu_hw_device *hdev, struct dispc_out_mode out_mode);
	int (*dispc_config)(struct dpu_hw_device *hdev, struct ax_disp_mode *mode);
	void (*dispc_enable)(struct dpu_hw_device *hdev);
	void (*dispc_disable)(struct dpu_hw_device *hdev);
	void (*dispc_set_buffer)(struct dpu_hw_device *hdev, u64 addr_y, u64 addr_uv,
				 u32 stride_y, u32 stride_uv);

	void (*draw_enable)(struct dpu_hw_device *hdev);
	void (*draw_disable)(struct dpu_hw_device *hdev);

	int (*task_valid)(struct draw_task *task);
	int (*task_prepare)(struct draw_task *task);
	int (*draw_start)(struct draw_task *task);
	int (*draw_done)(struct draw_task *task);
};

struct ax_bridge_device {
	int id;

	int (*bridge_init)(u32 devid, struct display_timing *timing,
			   struct display_timing *adjusted_timing, void *data);
	int (*bridge_enable)(u32 devid, const struct display_timing *timing);
	void *data;
};

struct ax_dpu_device {
	int id;

	struct ax_disp_mode mode;
	struct display_timing timing;

	struct dpu_hw_device hdev;

	struct dpu_hw_ops *ops;
};

enum {
	BRIDGE_INDEX_HDMI = 0,
	BRIDGE_INDEX_DSI,
	BRIDGE_INDEX_BUTT,
};

struct ax_vo_pipe {
	int id;

	u32 type;
	u32 sync;

	struct simple_panel_cfg *panel_cfg;

	struct ax_dpu_device ddev;
	struct ax_bridge_device bridge[BRIDGE_INDEX_BUTT];

	int (*pixel_clk_set)(u32 devid, u32 clk);
};

int ax_start_vo(u32 dev, u32 type, u32 sync, struct display_info *dp_info);
int fdt_fixup_vo_init_mode(int dev, void *fdt);

#endif /* __AX_VO_H */
