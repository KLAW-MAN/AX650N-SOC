/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXERA_VO_COMMON_H__
#define __AXERA_VO_COMMON_H__

#include <asm/io.h>

#include "ax650_mm_ck.h"
#include "ax_dw_hdmi.h"
#include "ax_dw_mipi_dsi.h"

#define VO_LOG_ON
#ifdef VO_LOG_ON
	#define VO_INFO(fmt,...) printf("[VO][I][%s:%d] "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);
	#define VO_DEBUG(fmt,...) printf("[VO][D][%s:%d] "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
	#define VO_INFO(fmt,...)
	#define VO_DEBUG(fmt,...)
#endif

#define VO_WARN(fmt,...) printf("[VO][W][%s:%d] "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define VO_ERROR(fmt,...) printf("[VO][E][%s:%d] "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);

enum {
	AX_DISP_OUT_MODE_BT601 = 0,
	AX_DISP_OUT_MODE_BT656,
	AX_DISP_OUT_MODE_BT1120,
	AX_DISP_OUT_MODE_DPI,
	AX_DISP_OUT_MODE_DSI_VIDEO,
	AX_DISP_OUT_MODE_DSI_CMD,
	AX_DISP_OUT_MODE_HDMI,
	AX_DISP_OUT_MODE_BUT,
};

enum {
	AX_DISP_OUT_FMT_RGB565 = 0,
	AX_DISP_OUT_FMT_RGB666,
	AX_DISP_OUT_FMT_RGB666LP,
	AX_DISP_OUT_FMT_RGB888,
	AX_DISP_OUT_FMT_RGB101010,
	AX_DISP_OUT_FMT_YUV422,
	AX_DISP_OUT_FMT_YUV422_10,
};

enum {
	AX_VO_FORMAT_NV12,
	AX_VO_FORMAT_NV21,
	AX_VO_FORMAT_ARGB1555,
	AX_VO_FORMAT_ARGB4444,
	AX_VO_FORMAT_RGBA5658,
	AX_VO_FORMAT_ARGB8888,
	AX_VO_FORMAT_RGB565,
	AX_VO_FORMAT_RGB888,
	AX_VO_FORMAT_RGBA4444,
	AX_VO_FORMAT_RGBA5551,
	AX_VO_FORMAT_RGBA8888,
	AX_VO_FORMAT_ARGB8565,
	AX_VO_FORMAT_P010,
	AX_VO_FORMAT_P016,
	AX_VO_FORMAT_NV16,
	AX_VO_FORMAT_P210,
	AX_VO_FORMAT_P216,
	AX_VO_FORMAT_BITMAP,
	AX_VO_FORMAT_BUT,
};

enum {
	CSC_MATRIX_IDENTITY = 0,
	CSC_MATRIX_BT601_TO_BT601,
	CSC_MATRIX_BT601_TO_BT709,
	CSC_MATRIX_BT709_TO_BT709,
	CSC_MATRIX_BT709_TO_BT601,
	CSC_MATRIX_BT601_TO_RGB_PC,
	CSC_MATRIX_BT709_TO_RGB_PC,
	CSC_MATRIX_RGB_TO_BT601_PC,
	CSC_MATRIX_RGB_TO_BT709_PC,
	CSC_MATRIX_RGB_TO_BT2020_PC,
	CSC_MATRIX_BT2020_TO_RGB_PC,
	CSC_MATRIX_RGB_TO_BT601_TV,
	CSC_MATRIX_RGB_TO_BT709_TV,
	CSC_MATRIX_BUTT,
};

enum {
	AX_VO_OUTPUT_576P50,                /* 720  x  576 at 50 Hz */
	AX_VO_OUTPUT_480P60,                /* 720  x  480 at 60 Hz */
	AX_VO_OUTPUT_720P50,                /* 1280 x  720 at 50 Hz */
	AX_VO_OUTPUT_720P60,                /* 1280 x  720 at 60 Hz */

	AX_VO_OUTPUT_1080P24,               /* 1920 x 1080 at 24 Hz */
	AX_VO_OUTPUT_1080P25,               /* 1920 x 1080 at 25 Hz */
	AX_VO_OUTPUT_1080P30,               /* 1920 x 1080 at 30 Hz */
	AX_VO_OUTPUT_1080P50,               /* 1920 x 1080 at 50 Hz */
	AX_VO_OUTPUT_1080P60,               /* 1920 x 1080 at 60 Hz */

	AX_VO_OUTPUT_640x480_60,            /* VESA 640 x 480 at 60 Hz (non-interlaced) */
	AX_VO_OUTPUT_800x600_60,            /* VESA 800 x 600 at 60 Hz (non-interlaced) */
	AX_VO_OUTPUT_1024x768_60,           /* VESA 1024 x 768 at 60 Hz (non-interlaced) */
	AX_VO_OUTPUT_1280x1024_60,          /* VESA 1280 x 1024 at 60 Hz (non-interlaced) */
	AX_VO_OUTPUT_1366x768_60,           /* VESA 1366 x 768 at 60 Hz (non-interlaced) */
	AX_VO_OUTPUT_1280x800_60,           /* VESA 1280 x 800 at 60 Hz (non-interlaced) CVT Compliant */
	AX_VO_OUTPUT_1440x900_60,           /* VESA 1440 x 900 at 60 Hz (non-interlaced) CVT Compliant */
	AX_VO_OUTPUT_1600x1200_60,          /* VESA 1600 x 1200 at 60 Hz (non-interlaced) */
	AX_VO_OUTPUT_1680x1050_60,          /* VESA 1680 x 1050 at 60 Hz (non-interlaced) CVT Compliant */
	AX_VO_OUTPUT_1920x1200_60,          /* VESA 1920 x 1600 at 60 Hz (non-interlaced) CVT (Reduced Blanking) */
	AX_VO_OUTPUT_2560x1600_60,          /* VESA 2560 x 1600 at 60 Hz (non-interlaced) CVT Compliant */

	AX_VO_OUTPUT_3840x2160_24,          /* 3840 x 2160 at 24 Hz */
	AX_VO_OUTPUT_3840x2160_25,          /* 3840 x 2160 at 25 Hz */
	AX_VO_OUTPUT_3840x2160_30,          /* 3840 x 2160 at 30 Hz */
	AX_VO_OUTPUT_3840x2160_50,          /* 3840 x 2160 at 50 Hz */
	AX_VO_OUTPUT_3840x2160_60,          /* 3840 x 2160 at 60 Hz */
	AX_VO_OUTPUT_4096x2160_24,          /* 4096 x 2160 at 24 Hz */
	AX_VO_OUTPUT_4096x2160_25,          /* 4096 x 2160 at 25 Hz */
	AX_VO_OUTPUT_4096x2160_30,          /* 4096 x 2160 at 30 Hz */
	AX_VO_OUTPUT_4096x2160_50,          /* 4096 x 2160 at 50 Hz */
	AX_VO_OUTPUT_4096x2160_60,          /* 4096 x 2160 at 60 Hz */

	AX_VO_OUTPUT_720x1280_60,           /* For MIPI DSI Tx 720 x1280 at 60 Hz */
	AX_VO_OUTPUT_1080x1920_60,          /* For MIPI DSI Tx 1080x1920 at 60 Hz */
	AX_VO_OUTPUT_USER,                  /* User timing. */

	AX_VO_OUTPUT_BUTT
};

#define MODE_FLAG_INTERLACE			(1 << 0)	/* 1: interlace */
#define MODE_FLAG_SYNC_TYPE			(1 << 1)	/* 0: internal sync. 1: external sync */
#define MODE_FLAG_FIELD1_FIRST			(1 << 2)

struct ax_disp_mode {
	int type;
	int fmt_in;
	int fmt_out;
	int flags;

	int clock;  /* in kHz */
	int vrefresh;
	int hdisplay;
	int hsync_start;
	int hsync_end;
	int htotal;
	int vdisplay;
	int vsync_start;
	int vsync_end;
	int vtotal;

	int hp_pol;
	int vp_pol;
	int de_pol;
};

struct dsi_panel_cfg {
	u32 lanes;
	u32 format;
	unsigned long mode_flags;

	u32 burst_factor;

	u32 num_init_seqs;
	u8 *init_seq;

	void *dsi_dev;
};

enum {
	PANEL_TYPE_NORMAL,
	PANEL_TYPE_MIPI,
};

#define PANEL_GPIO_SET(GPIO_NUM)	((1 << 16) | ((GPIO_NUM) & 0xFFFF))
#define PANEL_GPIO_GET(VAL)  		((VAL) & 0xFFFF)
#define PANEL_GPIO_VALID(VAL)		!!((1 << 16) & (VAL))

struct simple_panel_cfg {
	u32 type;
	struct dsi_panel_cfg dp_cfg;

	u32 enable_gpio;

	u32 reset_gpio;
	u32 reset_delay_ms;

	int (*init)(struct simple_panel_cfg *panel_cfg);
};

static inline void vo_writel(void __iomem *regs, u32 offset, u32 val)
{
	writel_relaxed(val, regs + offset);
}

static inline u32 vo_readl(void __iomem *regs, u32 offset)
{
	return readl_relaxed(regs + offset);
}

#endif
