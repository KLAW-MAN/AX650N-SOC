/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX650A_VO_COMMON_H
#define __AX650A_VO_COMMON_H

#define PYHS_ADDR_HIGH(x)	(((x) >> 32) & 0x1F)
#define PYHS_ADDR_LOW(x)	((x) & 0xFFFFFFFF)

#define LAYER_TYPE(x)		((x) << 7)
#define RB_SWAP(x)		((x) << 6)
#define FBDC_EN(x)		((x) << 5)
#define PIXEL_FMT(x)		((x) & 0x1F)
#define ZERO_BITS(x)		((x) & 0x1)

#define SUPPORTED_CHNS_MAX	64
#define SUPPORTED_LAYER_MAX	32
#define DRAW_V0_REG_NUM		14
#define DRAW_G0_REG_NUM		18
#define DRAW_CSC_REG_NUM	25
#define DRAW_OTHER_REG_NUM	24
#define DRAW_CDMA_EXTRA_SIZE	(((SUPPORTED_CHNS_MAX * 16) + 0xFF) & (~0xFF))
#define DRAW_CDMA_QUEUE_H_SIZE	(128)	/* Related to cdma-mode0 format */
#define DRAW_MULTI_CFG_IOMEM_SIZE (DRAW_CDMA_EXTRA_SIZE + DRAW_CDMA_QUEUE_H_SIZE + (DRAW_CSC_REG_NUM + DRAW_V0_REG_NUM + DRAW_G0_REG_NUM + DRAW_OTHER_REG_NUM) * 8)
#define DRAW_SINGLE_CFG_IOMEM_SIZE ((SUPPORTED_CHNS_MAX * (DRAW_CDMA_QUEUE_H_SIZE + DRAW_G0_REG_NUM + DRAW_OTHER_REG_NUM) + DRAW_CSC_REG_NUM) * 8 + DRAW_CDMA_EXTRA_SIZE)

#define DRAW_ADDR_ALIGN			(256)
#define DRAW_ALIGNED_BYTES		(16)
#define DRAW_FBCDC_ALIGNED_PIXEL	(128)
#define DRAW_WIDTH_MIN			(2)
#define DRAW_HEIGHT_MIN			(2)
#define DRAW_WIDTH_MAX			(0x2000)
#define DRAW_HEIGHT_MAX			(0x2000)
#define DRAW_STRIDE_MAX			(0x10000)

#define COLORKEY_VAL_FIX_POINT_SHIFT	(2)
#define SET_HW_COLORKEY_VAL(val)    (((((val >> 16) & 0xFF) << COLORKEY_VAL_FIX_POINT_SHIFT) << 20) \
				  | ((((val >> 8) & 0xFF) << COLORKEY_VAL_FIX_POINT_SHIFT) << 10) \
				  | ((val & 0xFF) << COLORKEY_VAL_FIX_POINT_SHIFT))

/* u2.16, 1/360 value */
#define DISPC_INV_HUE_MAX		(182)
/* u5.18, 1/1.0 value */
#define DISPC_INV_SATU_MAX		(262144)
#define HSV_H_LUT			(33)
#define HSV_V_LUT			(33)
#define HSV_LUT_H_ACC			(7)
#define HSV_LUT_S_ACC			(15)

#define DPU_CLK_RATE			(594000000)

#define DISPC_RESO_HEIGHT_SHIFT		(16)
#define DISPC_H_SHIFT			(16)
#define RXCDPHY_NUM                          4

enum {
	FORMAT_ARGB1555 = 0,
	FORMAT_ARGB4444 = 1,
	FORMAT_ARGB8565 = 2,
	FORMAT_ARGB8888 = 3,
	FORMAT_RGBA5551 = 4,
	FORMAT_RGBA4444 = 5,
	FORMAT_RGBA5658 = 6,
	FORMAT_RGBA8888 = 7,
	FORMAT_RGB565 = 8,
	FORMAT_RGB888 = 9,
	FORMAT_YUV420_8 = 10,
	FORMAT_YUV420_10 = 11,
	FORMAT_YUV420_16 = 12,
	FORMAT_YUV422_8 = 13,
	FORMAT_YUV422_10 = 14,
	FORMAT_YUV422_16 = 15,

	/*unsupport */
	FORMAT_BYTE = 16,
	/*unsupport */
	FORMAT_HALF_WORD = 17,

	FORMAT_BIT_MAP = 31,
	FORMAT_NO_SUPPORT,
};

enum {
	VO_COMPRESS_LEVEL_0,	/* lossless */
	VO_COMPRESS_LEVEL_1,
	VO_COMPRESS_LEVEL_2,
	VO_COMPRESS_LEVEL_3,
	VO_COMPRESS_LEVEL_4,
	VO_COMPRESS_LEVEL_5,
	VO_COMPRESS_LEVEL_6,
	VO_COMPRESS_LEVEL_7,
	VO_COMPRESS_LEVEL_8,
	VO_COMPRESS_LEVEL_9,	/* for yuv 10bit */
	VO_COMPRESS_LEVEL_10,	/* for yuv 10bit */
	VO_COMPRESS_LEVEL_11,	/* for yuv 10bit */
	VO_COMPRESS_LEVEL_BUTT,
};

struct dispc_dither {
	u32 dither_r_seed;
	u32 dither_r_pmask;
	u32 dither_r_acc;
	u32 dither_g_seed;
	u32 dither_g_pmask;
	u32 dither_g_acc;
	u32 dither_b_seed;
	u32 dither_b_pmask;
	u32 dither_b_acc;
};

struct csc0_reg {
	u32 en;
	u32 matrix[3][3];
	u32 offset[2][3];
	u32 decimation_h[7];
	u32 c_offbin_en;
	u32 uv_seq_sel;
};

struct csc1_reg {
	u32 en;
	u32 decimation;
	u32 c_offbin_en;
};

struct csc_reg {
	struct csc0_reg csc0;
	struct csc1_reg csc1;
};

struct dither_reg {
	u32 en;
	u32 up;
	u32 seed;
	u32 pmask;
	u32 acc;
	u32 out_acc;
};

struct yuv2rgb_regs {
	u32 en;
	u32 uv_seq;
	u32 matrix[3][3];
	u32 offset[3];
};

enum out_mode {
	OUT_MODE_BT601 = 0,
	OUT_MODE_BT656,
	OUT_MODE_BT1120,
	OUT_MODE_DPI,
	OUT_MODE_DSI_VIDEO,
	OUT_MODE_DSI_CMD,
	OUT_MODE_HDMI,
};

enum dispc_format_in {
	FMT_IN_ARGB1555 = 0,
	FMT_IN_ARGB4444 = 1,
	FMT_IN_ARGB8565 = 2,
	FMT_IN_ARGB8888 = 3,
	FMT_IN_RGBA5551 = 4,
	FMT_IN_RGBA4444 = 5,
	FMT_IN_RGBA5658 = 6,
	FMT_IN_RGBA8888 = 7,
	FMT_IN_RGB565 = 8,
	FMT_IN_RGB888 = 9,
	FMT_IN_YUV420_8 = 10,
	FMT_IN_YUV420_10 = 11,
};

enum dispc_format_out {
	FMT_OUT_RGB565 = 0,
	FMT_OUT_RGB666 = 1,
	FMT_OUT_RGB666LP = 2,
	FMT_OUT_RGB888 = 3,
	FMT_OUT_RGB101010 = 4,
	FMT_OUT_YUV422 = 5,
	FMT_OUT_YUV422_10 = 6,
};

enum dispc_scan_mode {
	DISPC_SCAN_MODE_FRAME = 0,
	DISPC_SCAN_MODE_FIELD = 1,
};

struct dispc_out_mode {
	enum dispc_format_in fmt_in;
	enum dispc_format_out fmt_out;
	enum out_mode mode;
};

struct fbcdc_comp_level {
	u16 level;
	u16 size;
};

struct draw_cmd_node {
	u16 coordinate_x;
	u16 coordinate_y;
	u16 width;
	u16 height;
	u8 config;
	u8 comp_level;
	u8 alp;
	u32 addr_luma_l;
	u8 addr_luma_h;
	u16 stride_luma;
	u32 addr_chro_l;
	u8 addr_chro_h;
	u16 stride_chro;
	u16 crop_pos_x;
	u16 crop_pos_y;
	u8 zero_bits_cnt_y:1;
	u8 zero_bits_cnt_c:1;
	u8 pad[2];
} __attribute__ ((packed));

struct draw_reg {
	u32 offs;
	u32 val;
};

struct cdma_mode {
	u64 cdma_mode_vaddr;
	u64 cdma_mode_paddr;
};

struct draw_hw_cfg {
	u32 use_cdma;

	u32 start_cnt;

	u64 regs_base_phys;

	u64 hw_cfg_phys;

	u64 hw_cfg_end_vaddr;

	u32 cmd_index;
	u32 cmd_step;
	u32 cmd_num;
	u64 cmd_buf_phys;

	u32 reg_wr_index;
	u32 reg_num;

	u32 cdma_mode_num;
	u64 cdma_modes_vaddr;
	u64 cdma_modes_paddr;

	u64 cdma_queue_vaddr;
	u64 cdma_queue_paddr;

	struct draw_cmd_node cmd_nodes[SUPPORTED_CHNS_MAX] __attribute__ ((aligned(16)));

	char io_mem[0];
};

struct color_space_cfg {
	struct yuv2rgb_regs yuv2rgb_cfg;
	struct csc_reg csc;
};

struct dpu_hw_device {
	int id;

	u64 phy_offs;
	u64 regs_base_phys;

	void __iomem *regs;
	void __iomem *mm_top_regs;

	unsigned long dpu_clk_rate;

	void *dpu_clk;
	void *dpi_clk;
	void *cdma_clk;
	void *isp_ife_clk;
	void *rx_pinmux_clk[RXCDPHY_NUM];
	void *cdphy_rx_cfg_clk[RXCDPHY_NUM];
	void *cdphy_rx_pclk[RXCDPHY_NUM];

	void *rst_ctrl;
	void *cdma_rst_ctrl;
	void *cdphy_rx_rst_ctrl[RXCDPHY_NUM];
	void *rx_pinmux_rst_ctrl[RXCDPHY_NUM];
	void *isp_axim_rst_ctrl;
	void *isp_ife_rst_ctrl;

	const struct yuv2rgb_regs *dispc_matrix;
	const struct csc_reg *dispc_csc;

	bool is_online;

	struct ax_disp_mode mode;

	struct dispc_out_mode out_mode;

	void *data;
};

enum mouse_mode {
	MOUSE_MODE_ARGB = 0,
	MOUSE_MODE_RGB,
	MOUSE_MODE_BITMAP,
};

static inline void dpu_writel(void __iomem *regs, u32 offset, u32 val)
{
	__raw_writel(val, regs + offset);
}

static inline u32 dpu_readl(void __iomem *regs, u32 offset)
{
	return __raw_readl(regs + offset);
}

const struct color_space_cfg *get_csc_cfg(u32 cs_type);
int display_out_mode_convert(struct ax_disp_mode *mode, struct dispc_out_mode *dispc_out);
u32 vo_fmt2hw_fmt(u32 format);

#endif /* __AX650A_VO_H */
