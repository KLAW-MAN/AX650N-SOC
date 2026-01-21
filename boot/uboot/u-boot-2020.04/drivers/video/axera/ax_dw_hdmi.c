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
#include <media_bus_format.h>

#include "dw_hdmi.h"
#include "ax_vo_common.h"

static struct dw_hdmi ax_dw_hdmi[HDMI_DEV_NR] = {
	{.ioaddr = (ulong)AX_DW_HDMI0_BASE_ADDR},
	{.ioaddr = (ulong)AX_DW_HDMI1_BASE_ADDR},
};

static const struct hdmi_phy_config axera_phy_config[] = {
	{
		.mpixelclock = 148500000,
		.sym_ctr = 0x8088,
		.term = 0x0007,
		.vlev_ctr = 0x0180,
	},
	{
		.mpixelclock = 193250000,
		.sym_ctr = 0x81c8,
		.term = 0x0004,
		.vlev_ctr = 0x0280,
	},
	{
		.mpixelclock = 297000000,
		.sym_ctr = 0x81c8,
		.term = 0x0004,
		.vlev_ctr = 0x0180,
	},
	{
		.mpixelclock = 594000000,
		.sym_ctr = 0x81f3,
		.term = 0x0000,
		.vlev_ctr = 0x0180,
	},
	{
		.mpixelclock = ~0ul,
		.sym_ctr = 0x0000,
		.term = 0x0000,
		.vlev_ctr = 0x0000,
	}
};

static const struct hdmi_mpll_config axera_mpll_cfg[] = {
	{
		.mpixelclock = 25175000,	/* 640x480@60Hz */
		.cpce = 0x0003,
		.gmp = 0x0628,
		.curr = 0x0283,
	},
	{
		.mpixelclock = 27000000,	/* 720x576@50Hz 480p60 */
		.cpce = 0x0003,
		.gmp = 0x0628,
		.curr = 0x0283,
	},
	{
		.mpixelclock = 40000000,	/* 800x600@60 */
		.cpce = 0x0003,
		.gmp = 0x0228,
		.curr = 0x0285,
	},
	{
		.mpixelclock = 65000000,	/* 1024x768@50 */
		.cpce = 0x0002,
		.gmp = 0x0614,
		.curr = 0x1183,
	},
	{
		.mpixelclock = 71000000,	/* 1280x800@60 */
		.cpce = 0x0002,
		.gmp = 0x0614,
		.curr = 0x1183,
	},
	{
		.mpixelclock = 74250000,	/* 1280x720@60 */
		.cpce = 0x0002,
		.gmp = 0x0214,
		.curr = 0x1142,
	},
	{
		.mpixelclock = 79500000,	/* 1280x768@60 */
		.cpce = 0x0002,
		.gmp = 0x0214,
		.curr = 0x1142,
	},
	{
		.mpixelclock = 83500000,	/* 1280x800@60 */
		.cpce = 0x0002,
		.gmp = 0x0214,
		.curr = 0x1142,
	},
	{
		.mpixelclock = 85500000,	/* 1360x768@60 */
		.cpce = 0x0002,
		.gmp = 0x0214,
		.curr = 0x1142,
	},
	{
		.mpixelclock = 106500000,	/* 1440x900@60 */
		.cpce = 0x0001,
		.gmp = 0x060a,
		.curr = 0x20c0,
	},
	{
		.mpixelclock = 108000000,	/* 1280x1024@60 */
		.cpce = 0x0001,
		.gmp = 0x060a,
		.curr = 0x20c0,
	},
	{
		.mpixelclock = 119000000,	/* 1680x1050@60 Reduceed Blanking */
		.cpce = 0x0001,
		.gmp = 0x060a,
		.curr = 0x20c0,
	},
	{
		.mpixelclock = 146250000,	/* 1680x1050@60 */
		.cpce = 0x0001,
		.gmp = 0x020a,
		.curr = 0x2080,
	},
	{
		.mpixelclock = 148500000,	/* 1080p60 */
		.cpce = 0x0001,
		.gmp = 0x020a,
		.curr = 0x2080,
	},
	{
		.mpixelclock = 154000000,	/* 1920x1200@60 */
		.cpce = 0x0001,
		.gmp = 0x010a,
		.curr = 0x3041,
	},
	{
		.mpixelclock = 162000000,	/* 1600x1200@60 */
		.cpce = 0x0001,
		.gmp = 0x020a,
		.curr = 0x2080,
	},
	{
		.mpixelclock = 193250000,	/* 1600x1200@60 */
		.cpce = 0x0000,
		.gmp = 0x0605,
		.curr = 0x3040,
	},
	{
		.mpixelclock = 268500000,	/* 2560x1600@60 Reduceed Blanking */
		.cpce = 0x0000,
		.gmp = 0x0605,
		.curr = 0x3040,
	},
	{
		.mpixelclock = 297000000,	/* 3840x2160@30 */
		.cpce = 0x0000,
		.gmp = 0x0205,
		.curr = 0x3041,
	},
	{
		.mpixelclock = 348500000,	/* 2560x1600@60 */
		.cpce = 0x0640,
		.gmp = 0x0205,
		.curr = 0x3041,
	},
	{
		.mpixelclock = 594000000,	/* 3840x2160@60 */
		.cpce = 0x0640,
		.gmp = 0x0005,
		.curr = 0x3080,
	},
	{
		.mpixelclock = ~0ul,
		.cpce = 0x0051,
		.gmp = 0x0003,
		.curr = 0x0000,
	}
};

int ax_dw_hdmi_enable(u32 devid, const struct display_timing *edid)
{
	struct dw_hdmi *hdmi;

	if (devid >= HDMI_DEV_NR) {
		VO_ERROR("invalid devid, devid:%d\n", devid);
		return -1;
	}

	hdmi = &ax_dw_hdmi[devid];

	return dw_hdmi_enable(hdmi, edid);
}

int ax_dw_hdmi_read_edid(u32 devid, u8 *buf, int buf_size)
{
	struct dw_hdmi *hdmi;

	if (devid >= HDMI_DEV_NR) {
		VO_ERROR("invalid devid, devid:%d\n", devid);
		return -1;
	}

	hdmi = &ax_dw_hdmi[devid];

	return dw_hdmi_read_edid(hdmi, buf, buf_size);
}

static void ax_dw_hdmi_platdata_init(struct dw_hdmi *hdmi)
{
	hdmi->i2c_clk_high = 0x7a;
	hdmi->i2c_clk_low = 0x8d;

	hdmi->mpll_cfg = axera_mpll_cfg;
	hdmi->phy_cfg = axera_phy_config;

	hdmi->reg_io_width = 1;

	hdmi->phy_set = dw_hdmi_phy_cfg;

	hdmi->hdmi_data.enc_out_encoding = V4L2_YCBCR_ENC_709;

	hdmi->hdmi_data.video_mode.mpixelrepetitionoutput = 0;
	hdmi->hdmi_data.video_mode.mpixelrepetitioninput = 0;

	/* TOFIX: Get input format from plat data or fallback to RGB888 */
	hdmi->hdmi_data.enc_in_bus_format = MEDIA_BUS_FMT_RGB888_1X24;

	/* TOFIX: Get input encoding from plat data or fallback to none */
	hdmi->hdmi_data.enc_in_encoding = V4L2_YCBCR_ENC_DEFAULT;

	/* TOFIX: Default to RGB888 output format */
	hdmi->hdmi_data.enc_out_bus_format = MEDIA_BUS_FMT_RGB888_1X24;

	hdmi->hdmi_data.rgb_limited_range = true;

	hdmi->hdmi_data.pix_repet_factor = 0;
	hdmi->hdmi_data.hdcp_enable = 0;
	hdmi->hdmi_data.video_mode.mdataenablepolarity = true;
	hdmi->sink_is_hdmi = true;
	hdmi->mc_clkdis = 0x7f;
	hdmi->phy_mask = (u8) ~(HDMI_PHY_HPD | HDMI_PHY_RX_SENSE);
}

int ax_dw_hdmi_init(u32 devid, struct display_timing *timing, struct display_timing *adjusted_timing, void *data)
{
	int ret;
	struct dw_hdmi *hdmi;

	if (devid >= HDMI_DEV_NR) {
		VO_ERROR("invalid devid, devid:%d\n", devid);
		return -1;
	}

	hdmi = &ax_dw_hdmi[devid];

	ax650_hdmi_reset(devid);

	ax_dw_hdmi_platdata_init(hdmi);

	/* wait for connector */
	ret = dw_hdmi_phy_wait_for_hpd(hdmi);
	if (ret < 0) {
		VO_ERROR("hdmi%d can not get hpd signal\n", devid);
		return -1;
	}

	dw_hdmi_init(hdmi);
	dw_hdmi_phy_init(hdmi);

	adjusted_timing->flags = DISPLAY_FLAGS_HSYNC_HIGH | DISPLAY_FLAGS_VSYNC_HIGH | DISPLAY_FLAGS_DE_HIGH;

	VO_DEBUG("hdmi%d init success\n", devid);

	return 0;
}

