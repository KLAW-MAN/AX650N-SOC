/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2015 Google, Inc
 * Copyright 2014 Rockchip Inc.
 * Copyright (C) 2011 Freescale Semiconductor, Inc.
 * (C) Copyright 2017 Jernej Skrabec <jernej.skrabec@siol.net>
 */

#include <common.h>
#include <asm/io.h>
#include <media_bus_format.h>

#include "dw_hdmi.h"
#include "ax_vo_common.h"

static const u16 csc_coeff_default[3][4] = {
	{0x2000, 0x0000, 0x0000, 0x0000},
	{0x0000, 0x2000, 0x0000, 0x0000},
	{0x0000, 0x0000, 0x2000, 0x0000}
};

static const u16 csc_coeff_rgb_out_eitu601[3][4] = {
	{0x2000, 0x6926, 0x74fd, 0x010e},
	{0x2000, 0x2cdd, 0x0000, 0x7e9a},
	{0x2000, 0x0000, 0x38b4, 0x7e3b}
};

static const u16 csc_coeff_rgb_out_eitu709[3][4] = {
	{0x2000, 0x7106, 0x7a02, 0x00a7},
	{0x2000, 0x3264, 0x0000, 0x7e6d},
	{0x2000, 0x0000, 0x3b61, 0x7e25}
};

static const u16 csc_coeff_rgb_in_eitu601[3][4] = {
	{0x2591, 0x1322, 0x074b, 0x0000},
	{0x6535, 0x2000, 0x7acc, 0x0200},
	{0x6acd, 0x7534, 0x2000, 0x0200}
};

static const u16 csc_coeff_rgb_in_eitu709[3][4] = {
	{0x2dc5, 0x0d9b, 0x049e, 0x0000},
	{0x62f0, 0x2000, 0x7d11, 0x0200},
	{0x6756, 0x78ab, 0x2000, 0x0200}
};

static const u16 csc_coeff_rgb_full_to_rgb_limited[3][4] = {
	{ 0x1b7c, 0x0000, 0x0000, 0x0020 },
	{ 0x0000, 0x1b7c, 0x0000, 0x0020 },
	{ 0x0000, 0x0000, 0x1b7c, 0x0020 }
};

static void dw_hdmi_write(struct dw_hdmi *hdmi, u8 val, int offset)
{
	switch (hdmi->reg_io_width) {
	case 1:
		writeb(val, hdmi->ioaddr + offset);
		break;
	case 4:
		writel(val, hdmi->ioaddr + (offset << 2));
		break;
	default:
		debug("reg_io_width has unsupported width!\n");
		break;
	}
}

static u8 dw_hdmi_read(struct dw_hdmi *hdmi, int offset)
{
	switch (hdmi->reg_io_width) {
	case 1:
		return readb(hdmi->ioaddr + offset);
	case 4:
		return readl(hdmi->ioaddr + (offset << 2));
	default:
		debug("reg_io_width has unsupported width!\n");
		break;
	}
	return 0;
}

static u8(*hdmi_read)(struct dw_hdmi *hdmi, int offset) = dw_hdmi_read;
static void (*hdmi_write)(struct dw_hdmi *hdmi, u8 val, int offset) =
        dw_hdmi_write;

static void hdmi_mod(struct dw_hdmi *hdmi, unsigned reg, u8 mask, u8 data)
{
	u8 val = hdmi_read(hdmi, reg) & ~mask;

	val |= data & mask;
	hdmi_write(hdmi, val, reg);
}

static bool hdmi_bus_fmt_is_rgb(unsigned int bus_format)
{
	switch (bus_format) {
	case MEDIA_BUS_FMT_RGB888_1X24:
	case MEDIA_BUS_FMT_RGB101010_1X30:
	case MEDIA_BUS_FMT_RGB121212_1X36:
	case MEDIA_BUS_FMT_RGB161616_1X48:
		return true;

	default:
		return false;
	}
}

static bool hdmi_bus_fmt_is_yuv444(unsigned int bus_format)
{
	switch (bus_format) {
	case MEDIA_BUS_FMT_YUV8_1X24:
	case MEDIA_BUS_FMT_YUV10_1X30:
	case MEDIA_BUS_FMT_YUV12_1X36:
	case MEDIA_BUS_FMT_YUV16_1X48:
		return true;
	default:
		return false;
	}
}

static bool hdmi_bus_fmt_is_yuv422(unsigned int bus_format)
{
	switch (bus_format) {
	case MEDIA_BUS_FMT_UYVY8_1X16:
	case MEDIA_BUS_FMT_UYVY10_1X20:
	case MEDIA_BUS_FMT_UYVY12_1X24:
		return true;

	default:
		return false;
	}
}

/*
 * this submodule is responsible for the video data synchronization.
 * for example, for RGB 4:4:4 input, the data map is defined as
 *			pin{47~40} <==> R[7:0]
 *			pin{31~24} <==> G[7:0]
 *			pin{15~8}  <==> B[7:0]
 */
static void hdmi_video_sample(struct dw_hdmi *hdmi)
{
	int color_format = 0;
	u8 val;

	switch (hdmi->hdmi_data.enc_in_bus_format) {
	case MEDIA_BUS_FMT_RGB888_1X24:
		color_format = 0x01;
		break;
	case MEDIA_BUS_FMT_RGB101010_1X30:
		color_format = 0x03;
		break;
	case MEDIA_BUS_FMT_RGB121212_1X36:
		color_format = 0x05;
		break;
	case MEDIA_BUS_FMT_RGB161616_1X48:
		color_format = 0x07;
		break;

	case MEDIA_BUS_FMT_YUV8_1X24:
	case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
		color_format = 0x09;
		break;
	case MEDIA_BUS_FMT_YUV10_1X30:
	case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
		color_format = 0x0B;
		break;
	case MEDIA_BUS_FMT_YUV12_1X36:
	case MEDIA_BUS_FMT_UYYVYY12_0_5X36:
		color_format = 0x0D;
		break;
	case MEDIA_BUS_FMT_YUV16_1X48:
	case MEDIA_BUS_FMT_UYYVYY16_0_5X48:
		color_format = 0x0F;
		break;

	case MEDIA_BUS_FMT_UYVY8_1X16:
		color_format = 0x16;
		break;
	case MEDIA_BUS_FMT_UYVY10_1X20:
		color_format = 0x14;
		break;
	case MEDIA_BUS_FMT_UYVY12_1X24:
		color_format = 0x12;
		break;

	default:
		return;
	}

	val = HDMI_TX_INVID0_INTERNAL_DE_GENERATOR_ENABLE |
	      ((color_format << HDMI_TX_INVID0_VIDEO_MAPPING_OFFSET) &
	       HDMI_TX_INVID0_VIDEO_MAPPING_MASK);
	hdmi_write(hdmi, val, HDMI_TX_INVID0);

	/* Enable TX stuffing: When DE is inactive, fix the output data to 0 */
	val = HDMI_TX_INSTUFFING_BDBDATA_STUFFING_ENABLE |
	      HDMI_TX_INSTUFFING_RCRDATA_STUFFING_ENABLE |
	      HDMI_TX_INSTUFFING_GYDATA_STUFFING_ENABLE;
	hdmi_write(hdmi, val, HDMI_TX_INSTUFFING);
	hdmi_write(hdmi, 0x0, HDMI_TX_GYDATA0);
	hdmi_write(hdmi, 0x0, HDMI_TX_GYDATA1);
	hdmi_write(hdmi, 0x0, HDMI_TX_RCRDATA0);
	hdmi_write(hdmi, 0x0, HDMI_TX_RCRDATA1);
	hdmi_write(hdmi, 0x0, HDMI_TX_BCBDATA0);
	hdmi_write(hdmi, 0x0, HDMI_TX_BCBDATA1);
}

static int is_color_space_interpolation(struct dw_hdmi *hdmi)
{
	if (!hdmi_bus_fmt_is_yuv422(hdmi->hdmi_data.enc_in_bus_format))
		return 0;

	if (hdmi_bus_fmt_is_rgb(hdmi->hdmi_data.enc_out_bus_format) ||
	    hdmi_bus_fmt_is_yuv444(hdmi->hdmi_data.enc_out_bus_format))
		return 1;

	return 0;
}

static int is_color_space_decimation(struct dw_hdmi *hdmi)
{
	if (!hdmi_bus_fmt_is_yuv422(hdmi->hdmi_data.enc_out_bus_format))
		return 0;

	if (hdmi_bus_fmt_is_rgb(hdmi->hdmi_data.enc_in_bus_format) ||
	    hdmi_bus_fmt_is_yuv444(hdmi->hdmi_data.enc_in_bus_format))
		return 1;

	return 0;
}

static int hdmi_bus_fmt_color_depth(unsigned int bus_format)
{
	switch (bus_format) {
	case MEDIA_BUS_FMT_RGB888_1X24:
	case MEDIA_BUS_FMT_YUV8_1X24:
	case MEDIA_BUS_FMT_UYVY8_1X16:
	case MEDIA_BUS_FMT_UYYVYY8_0_5X24:
		return 8;

	case MEDIA_BUS_FMT_RGB101010_1X30:
	case MEDIA_BUS_FMT_YUV10_1X30:
	case MEDIA_BUS_FMT_UYVY10_1X20:
	case MEDIA_BUS_FMT_UYYVYY10_0_5X30:
		return 10;

	case MEDIA_BUS_FMT_RGB121212_1X36:
	case MEDIA_BUS_FMT_YUV12_1X36:
	case MEDIA_BUS_FMT_UYVY12_1X24:
	case MEDIA_BUS_FMT_UYYVYY12_0_5X36:
		return 12;

	case MEDIA_BUS_FMT_RGB161616_1X48:
	case MEDIA_BUS_FMT_YUV16_1X48:
	case MEDIA_BUS_FMT_UYYVYY16_0_5X48:
		return 16;

	default:
		return 0;
	}
}

static int is_color_space_conversion(struct dw_hdmi *hdmi)
{
	struct hdmi_data_info *hdmi_data = &hdmi->hdmi_data;
	bool is_input_rgb, is_output_rgb;

	is_input_rgb = hdmi_bus_fmt_is_rgb(hdmi_data->enc_in_bus_format);
	is_output_rgb = hdmi_bus_fmt_is_rgb(hdmi_data->enc_out_bus_format);

	return (is_input_rgb != is_output_rgb) ||
	       (is_input_rgb && is_output_rgb && hdmi_data->rgb_limited_range);
}

static bool is_csc_needed(struct dw_hdmi *hdmi)
{
	return is_color_space_conversion(hdmi) ||
	       is_color_space_decimation(hdmi) ||
	       is_color_space_interpolation(hdmi);
}

static inline void hdmi_phy_test_clear(struct dw_hdmi *hdmi, uint bit)
{
	hdmi_mod(hdmi, HDMI_PHY_TST0, HDMI_PHY_TST0_TSTCLR_MASK,
	         bit << HDMI_PHY_TST0_TSTCLR_OFFSET);
}

static int hdmi_phy_wait_i2c_done(struct dw_hdmi *hdmi, u32 msec)
{
	ulong start;
	u32 val;

	start = get_timer(0);
	do {
		val = hdmi_read(hdmi, HDMI_IH_I2CMPHY_STAT0);
		if (val & 0x3) {
			hdmi_write(hdmi, val, HDMI_IH_I2CMPHY_STAT0);
			return 0;
		}

		udelay(100);
	} while (get_timer(start) < msec);

	return 1;
}

static void hdmi_phy_i2c_write(struct dw_hdmi *hdmi, uint data, uint addr)
{
	hdmi_write(hdmi, 0xff, HDMI_IH_I2CMPHY_STAT0);
	hdmi_write(hdmi, addr, HDMI_PHY_I2CM_ADDRESS_ADDR);
	hdmi_write(hdmi, (u8)(data >> 8), HDMI_PHY_I2CM_DATAO_1_ADDR);
	hdmi_write(hdmi, (u8)(data >> 0), HDMI_PHY_I2CM_DATAO_0_ADDR);
	hdmi_write(hdmi, HDMI_PHY_I2CM_OPERATION_ADDR_WRITE,
	           HDMI_PHY_I2CM_OPERATION_ADDR);

	hdmi_phy_wait_i2c_done(hdmi, 1000);
}

static void hdmi_phy_enable_powerdown(struct dw_hdmi *hdmi, uint enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_PDZ_MASK,
	         enable << HDMI_PHY_CONF0_PDZ_OFFSET);
}

static void hdmi_phy_enable_tmds(struct dw_hdmi *hdmi, uint enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_ENTMDS_MASK,
	         enable << HDMI_PHY_CONF0_ENTMDS_OFFSET);
}

static void hdmi_phy_enable_svsret(struct dw_hdmi *hdmi, u8 enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_SVSRET_MASK,
	         enable << HDMI_PHY_CONF0_SVSRET_OFFSET);
}

static void hdmi_phy_gen2_pddq(struct dw_hdmi *hdmi, uint enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_GEN2_PDDQ_MASK,
	         enable << HDMI_PHY_CONF0_GEN2_PDDQ_OFFSET);
}

static void hdmi_phy_gen2_txpwron(struct dw_hdmi *hdmi, uint enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0,
	         HDMI_PHY_CONF0_GEN2_TXPWRON_MASK,
	         enable << HDMI_PHY_CONF0_GEN2_TXPWRON_OFFSET);
}

static void hdmi_phy_sel_data_en_pol(struct dw_hdmi *hdmi, uint enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0,
	         HDMI_PHY_CONF0_SELDATAENPOL_MASK,
	         enable << HDMI_PHY_CONF0_SELDATAENPOL_OFFSET);
}

static void hdmi_phy_sel_interface_control(struct dw_hdmi *hdmi, uint enable)
{
	hdmi_mod(hdmi, HDMI_PHY_CONF0, HDMI_PHY_CONF0_SELDIPIF_MASK,
	         enable << HDMI_PHY_CONF0_SELDIPIF_OFFSET);
}

static void hdmi_phy_reset(struct dw_hdmi *hdmi)
{
	/* PHY reset. The reset signal is active high on Gen2 PHYs. */
	hdmi_write(hdmi, HDMI_MC_PHYRSTZ_PHYRSTZ, HDMI_MC_PHYRSTZ);
	hdmi_write(hdmi, 0, HDMI_MC_PHYRSTZ);
}

static void hdmi_phy_i2c_set_addr(struct dw_hdmi *hdmi, u8 address)
{
	hdmi_phy_test_clear(hdmi, 1);
	hdmi_write(hdmi, address, HDMI_PHY_I2CM_SLAVE_ADDR);
	hdmi_phy_test_clear(hdmi, 0);
}

static void hdmi_phy_power_off(struct dw_hdmi *hdmi)
{
	unsigned int i;
	u16 val;

	if (hdmi->phy_gen == 1) {
		hdmi_phy_enable_tmds(hdmi, 0);
		hdmi_phy_enable_powerdown(hdmi, true);
		return;
	}

	hdmi_phy_gen2_txpwron(hdmi, 0);

	/*
	 * Wait for TX_PHY_LOCK to be deasserted to indicate that the PHY went
	 * to low power mode.
	 */
	for (i = 0; i < 5; ++i) {
		val = hdmi_read(hdmi, HDMI_PHY_STAT0);
		if (!(val & HDMI_PHY_TX_PHY_LOCK))
			break;

		udelay(100);
	}

	if (val & HDMI_PHY_TX_PHY_LOCK)
		debug("PHY failed to power down\n");
	else
		debug("PHY powered down in %u iterations\n", i);

	hdmi_phy_gen2_pddq(hdmi, 1);
}

static int hdmi_phy_power_on(struct dw_hdmi *hdmi)
{
	unsigned int i;
	u8 val;

	if (hdmi->phy_gen == 1) {
		hdmi_phy_enable_powerdown(hdmi, false);

		/* Toggle TMDS enable. */
		hdmi_phy_enable_tmds(hdmi, 0);
		hdmi_phy_enable_tmds(hdmi, 1);
		return 0;
	}

	hdmi_phy_gen2_txpwron(hdmi, 1);
	hdmi_phy_gen2_pddq(hdmi, 0);

	/* Wait for PHY PLL lock */
	for (i = 0; i < 30; ++i) {
		val = hdmi_read(hdmi, HDMI_PHY_STAT0) & HDMI_PHY_TX_PHY_LOCK;
		if (val)
			break;

		udelay(50);
	}

	if (!val) {
		printf("PHY PLL failed to lock\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static void hdmi_tx_hdcp_config(struct dw_hdmi *hdmi)
{
	u8 de;

	if (hdmi->hdmi_data.video_mode.mdataenablepolarity)
		de = HDMI_A_VIDPOLCFG_DATAENPOL_ACTIVE_HIGH;
	else
		de = HDMI_A_VIDPOLCFG_DATAENPOL_ACTIVE_LOW;

	/* disable rx detect */

	hdmi_mod(hdmi, HDMI_A_HDCPCFG0, HDMI_A_HDCPCFG0_RXDETECT_MASK,
	         HDMI_A_HDCPCFG0_RXDETECT_DISABLE);

	hdmi_mod(hdmi, HDMI_A_VIDPOLCFG, HDMI_A_VIDPOLCFG_DATAENPOL_MASK, de);
	hdmi_mod(hdmi, HDMI_A_HDCPCFG1, HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_MASK,
	         HDMI_A_HDCPCFG1_ENCRYPTIONDISABLE_DISABLE);
}

static void hdmi_config_AVI(struct dw_hdmi *hdmi)
{
	u8 val;

	/* AVI Data Byte 5- set up input and output pixel repetition */
	val =
	        (((hdmi->hdmi_data.video_mode.mpixelrepetitioninput +
	           1) << HDMI_FC_PRCONF_INCOMING_PR_FACTOR_OFFSET) &
	         HDMI_FC_PRCONF_INCOMING_PR_FACTOR_MASK) | ((hdmi->hdmi_data.
	                         video_mode.
	                         mpixelrepetitionoutput
	                         <<
	                         HDMI_FC_PRCONF_OUTPUT_PR_FACTOR_OFFSET)
	                         &
	                         HDMI_FC_PRCONF_OUTPUT_PR_FACTOR_MASK);
	hdmi_write(hdmi, val, HDMI_FC_PRCONF);
}

static void hdmi_config_vendor_specific_infoframe(struct dw_hdmi *hdmi)
{

	hdmi_mod(hdmi, HDMI_FC_DATAUTO0, HDMI_FC_DATAUTO0_VSD_MASK, 0);

	/* Packet frame interpolation */
	hdmi_write(hdmi, 1, HDMI_FC_DATAUTO1);
	/* Auto packets per frame and line spacing */
	hdmi_write(hdmi, 0x11, HDMI_FC_DATAUTO2);
	/* Configures the Frame Composer On RDRB mode */
	hdmi_mod(hdmi, HDMI_FC_DATAUTO0, HDMI_FC_DATAUTO0_VSD_MASK,
	         1 << HDMI_FC_DATAUTO0_VSD_OFFSET);
}

static void hdmi_av_composer(struct dw_hdmi *hdmi,
                             const struct display_timing *edid)
{
	bool mdataenablepolarity = true;
	uint inv_val;
	uint hbl;
	uint vbl;

	hbl = edid->hback_porch.typ + edid->hfront_porch.typ +
	      edid->hsync_len.typ;
	vbl = edid->vback_porch.typ + edid->vfront_porch.typ +
	      edid->vsync_len.typ;

	/* set up hdmi_fc_invidconf */
	inv_val = HDMI_FC_INVIDCONF_HDCP_KEEPOUT_INACTIVE;

	inv_val |=
	        (edid->flags & DISPLAY_FLAGS_VSYNC_HIGH ?
	         HDMI_FC_INVIDCONF_VSYNC_IN_POLARITY_ACTIVE_HIGH :
	         HDMI_FC_INVIDCONF_VSYNC_IN_POLARITY_ACTIVE_LOW);

	inv_val |=
	        (edid->flags & DISPLAY_FLAGS_HSYNC_HIGH ?
	         HDMI_FC_INVIDCONF_HSYNC_IN_POLARITY_ACTIVE_HIGH :
	         HDMI_FC_INVIDCONF_HSYNC_IN_POLARITY_ACTIVE_LOW);

	inv_val |=
	        (mdataenablepolarity ? HDMI_FC_INVIDCONF_DE_IN_POLARITY_ACTIVE_HIGH
	         : HDMI_FC_INVIDCONF_DE_IN_POLARITY_ACTIVE_LOW);

	inv_val |=
	        (edid->hdmi_monitor ? HDMI_FC_INVIDCONF_DVI_MODEZ_HDMI_MODE :
	         HDMI_FC_INVIDCONF_DVI_MODEZ_DVI_MODE);

	inv_val |= HDMI_FC_INVIDCONF_R_V_BLANK_IN_OSC_ACTIVE_LOW;

	inv_val |= HDMI_FC_INVIDCONF_IN_I_P_PROGRESSIVE;

	hdmi_write(hdmi, inv_val, HDMI_FC_INVIDCONF);

	/* set up horizontal active pixel width 1920 */
	hdmi_write(hdmi, edid->hactive.typ >> 8, HDMI_FC_INHACTV1);
	hdmi_write(hdmi, edid->hactive.typ, HDMI_FC_INHACTV0);

	/* set up vertical active lines 1080 */
	hdmi_write(hdmi, edid->vactive.typ >> 8, HDMI_FC_INVACTV1);
	hdmi_write(hdmi, edid->vactive.typ, HDMI_FC_INVACTV0);

	/* set up horizontal blanking pixel region width */
	hdmi_write(hdmi, hbl >> 8, HDMI_FC_INHBLANK1);
	hdmi_write(hdmi, hbl, HDMI_FC_INHBLANK0);

	/* set up vertical blanking pixel region width */
	hdmi_write(hdmi, vbl, HDMI_FC_INVBLANK);

	/* set up hsync active edge delay width (in pixel clks) */
	hdmi_write(hdmi, edid->hfront_porch.typ >> 8, HDMI_FC_HSYNCINDELAY1);
	hdmi_write(hdmi, edid->hfront_porch.typ, HDMI_FC_HSYNCINDELAY0);

	/* set up vsync active edge delay (in lines) */
	hdmi_write(hdmi, edid->vfront_porch.typ, HDMI_FC_VSYNCINDELAY);

	/* set up hsync active pulse width (in pixel clks) */
	hdmi_write(hdmi, edid->hsync_len.typ >> 8, HDMI_FC_HSYNCINWIDTH1);
	hdmi_write(hdmi, edid->hsync_len.typ, HDMI_FC_HSYNCINWIDTH0);

	/* set up vsync active edge delay (in lines) */
	hdmi_write(hdmi, edid->vsync_len.typ, HDMI_FC_VSYNCINWIDTH);
}

static void hdmi_update_csc_coeffs(struct dw_hdmi *hdmi)
{
	const u16 (*csc_coeff)[3][4] = &csc_coeff_default;
	bool is_input_rgb, is_output_rgb;
	unsigned i;
	u32 csc_scale = 1;

	is_input_rgb = hdmi_bus_fmt_is_rgb(hdmi->hdmi_data.enc_in_bus_format);
	is_output_rgb = hdmi_bus_fmt_is_rgb(hdmi->hdmi_data.enc_out_bus_format);

	if (!is_input_rgb && is_output_rgb) {
		if (hdmi->hdmi_data.enc_out_encoding == V4L2_YCBCR_ENC_601)
			csc_coeff = &csc_coeff_rgb_out_eitu601;
		else
			csc_coeff = &csc_coeff_rgb_out_eitu709;
	} else if (is_input_rgb && !is_output_rgb) {
		if (hdmi->hdmi_data.enc_out_encoding == V4L2_YCBCR_ENC_601)
			csc_coeff = &csc_coeff_rgb_in_eitu601;
		else
			csc_coeff = &csc_coeff_rgb_in_eitu709;
		csc_scale = 0;
	} else if (is_input_rgb && is_output_rgb &&
		   hdmi->hdmi_data.rgb_limited_range) {
		csc_coeff = &csc_coeff_rgb_full_to_rgb_limited;
	}

	/* The CSC registers are sequential, alternating MSB then LSB */
	for (i = 0; i < ARRAY_SIZE(csc_coeff_default[0]); i++) {
		u16 coeff_a = (*csc_coeff)[0][i];
		u16 coeff_b = (*csc_coeff)[1][i];
		u16 coeff_c = (*csc_coeff)[2][i];

		hdmi_write(hdmi, coeff_a & 0xff, HDMI_CSC_COEF_A1_LSB + i * 2);
		hdmi_write(hdmi, coeff_a >> 8, HDMI_CSC_COEF_A1_MSB + i * 2);
		hdmi_write(hdmi, coeff_b & 0xff, HDMI_CSC_COEF_B1_LSB + i * 2);
		hdmi_write(hdmi, coeff_b >> 8, HDMI_CSC_COEF_B1_MSB + i * 2);
		hdmi_write(hdmi, coeff_c & 0xff, HDMI_CSC_COEF_C1_LSB + i * 2);
		hdmi_write(hdmi, coeff_c >> 8, HDMI_CSC_COEF_C1_MSB + i * 2);
	}

	hdmi_mod(hdmi, HDMI_CSC_SCALE, HDMI_CSC_SCALE_CSCSCALE_MASK, csc_scale);
}

static void hdmi_video_csc(struct dw_hdmi *hdmi)
{
	int color_depth = 0;
	int interpolation = HDMI_CSC_CFG_INTMODE_DISABLE;
	int decimation = 0;

	/* YCC422 interpolation to 444 mode */
	if (is_color_space_interpolation(hdmi))
		interpolation = HDMI_CSC_CFG_INTMODE_CHROMA_INT_FORMULA1;
	else if (is_color_space_decimation(hdmi))
		decimation = HDMI_CSC_CFG_DECMODE_CHROMA_INT_FORMULA3;

	switch (hdmi_bus_fmt_color_depth(hdmi->hdmi_data.enc_out_bus_format)) {
	case 8:
		color_depth = HDMI_CSC_SCALE_CSC_COLORDE_PTH_24BPP;
		break;
	case 10:
		color_depth = HDMI_CSC_SCALE_CSC_COLORDE_PTH_30BPP;
		break;
	case 12:
		color_depth = HDMI_CSC_SCALE_CSC_COLORDE_PTH_36BPP;
		break;
	case 16:
		color_depth = HDMI_CSC_SCALE_CSC_COLORDE_PTH_48BPP;
		break;

	default:
		return;
	}

	/* Configure the CSC registers */
	hdmi_write(hdmi, interpolation | decimation, HDMI_CSC_CFG);
	hdmi_mod(hdmi, HDMI_CSC_SCALE, HDMI_CSC_SCALE_CSC_COLORDE_PTH_MASK,
	         color_depth);

	hdmi_update_csc_coeffs(hdmi);
}

static void hdmi_video_packetize(struct dw_hdmi *hdmi)
{
	unsigned int color_depth = 0;
	unsigned int remap_size = HDMI_VP_REMAP_YCC422_16bit;
	unsigned int output_select = HDMI_VP_CONF_OUTPUT_SELECTOR_PP;
	struct hdmi_data_info *hdmi_data = &hdmi->hdmi_data;
	u8 val, vp_conf;

	if (hdmi_bus_fmt_is_rgb(hdmi->hdmi_data.enc_out_bus_format) ||
	    hdmi_bus_fmt_is_yuv444(hdmi->hdmi_data.enc_out_bus_format)) {
		switch (hdmi_bus_fmt_color_depth
		        (hdmi->hdmi_data.enc_out_bus_format)) {
		case 8:
			color_depth = 4;
			output_select = HDMI_VP_CONF_OUTPUT_SELECTOR_BYPASS;
			break;
		case 10:
			color_depth = 5;
			break;
		case 12:
			color_depth = 6;
			break;
		case 16:
			color_depth = 7;
			break;
		default:
			output_select = HDMI_VP_CONF_OUTPUT_SELECTOR_BYPASS;
		}
	} else if (hdmi_bus_fmt_is_yuv422(hdmi->hdmi_data.enc_out_bus_format)) {
		switch (hdmi_bus_fmt_color_depth
		        (hdmi->hdmi_data.enc_out_bus_format)) {
		case 0:
		case 8:
			remap_size = HDMI_VP_REMAP_YCC422_16bit;
			break;
		case 10:
			remap_size = HDMI_VP_REMAP_YCC422_20bit;
			break;
		case 12:
			remap_size = HDMI_VP_REMAP_YCC422_24bit;
			break;

		default:
			return;
		}
		output_select = HDMI_VP_CONF_OUTPUT_SELECTOR_YCC422;
	} else {
		return;
	}

	/* set the packetizer registers */
	val = ((color_depth << HDMI_VP_PR_CD_COLOR_DEPTH_OFFSET) &
	       HDMI_VP_PR_CD_COLOR_DEPTH_MASK) |
	      ((hdmi_data->
	        pix_repet_factor << HDMI_VP_PR_CD_DESIRED_PR_FACTOR_OFFSET) &
	       HDMI_VP_PR_CD_DESIRED_PR_FACTOR_MASK);
	hdmi_write(hdmi, val, HDMI_VP_PR_CD);

	hdmi_mod(hdmi, HDMI_VP_STUFF, HDMI_VP_STUFF_PR_STUFFING_MASK,
	         HDMI_VP_STUFF_PR_STUFFING_STUFFING_MODE);

	/* Data from pixel repeater block */
	if (hdmi_data->pix_repet_factor > 1) {
		vp_conf = HDMI_VP_CONF_PR_EN_ENABLE |
		          HDMI_VP_CONF_BYPASS_SELECT_PIX_REPEATER;
	} else {		/* data from packetizer block */
		vp_conf = HDMI_VP_CONF_PR_EN_DISABLE |
		          HDMI_VP_CONF_BYPASS_SELECT_VID_PACKETIZER;
	}

	hdmi_mod(hdmi, HDMI_VP_CONF,
	         HDMI_VP_CONF_PR_EN_MASK | HDMI_VP_CONF_BYPASS_SELECT_MASK,
	         vp_conf);

	hdmi_mod(hdmi, HDMI_VP_STUFF, HDMI_VP_STUFF_IDEFAULT_PHASE_MASK,
	         1 << HDMI_VP_STUFF_IDEFAULT_PHASE_OFFSET);

	hdmi_write(hdmi, remap_size, HDMI_VP_REMAP);

	if (output_select == HDMI_VP_CONF_OUTPUT_SELECTOR_PP) {
		vp_conf = HDMI_VP_CONF_BYPASS_EN_DISABLE |
		          HDMI_VP_CONF_PP_EN_ENABLE | HDMI_VP_CONF_YCC422_EN_DISABLE;
	} else if (output_select == HDMI_VP_CONF_OUTPUT_SELECTOR_YCC422) {
		vp_conf = HDMI_VP_CONF_BYPASS_EN_DISABLE |
		          HDMI_VP_CONF_PP_EN_DISABLE | HDMI_VP_CONF_YCC422_EN_ENABLE;
	} else if (output_select == HDMI_VP_CONF_OUTPUT_SELECTOR_BYPASS) {
		vp_conf = HDMI_VP_CONF_BYPASS_EN_ENABLE |
		          HDMI_VP_CONF_PP_EN_DISABLE | HDMI_VP_CONF_YCC422_EN_DISABLE;
	} else {
		return;
	}

	hdmi_mod(hdmi, HDMI_VP_CONF,
	         HDMI_VP_CONF_BYPASS_EN_MASK | HDMI_VP_CONF_PP_EN_ENMASK |
	         HDMI_VP_CONF_YCC422_EN_MASK, vp_conf);

	hdmi_mod(hdmi, HDMI_VP_STUFF,
	         HDMI_VP_STUFF_PP_STUFFING_MASK |
	         HDMI_VP_STUFF_YCC422_STUFFING_MASK,
	         HDMI_VP_STUFF_PP_STUFFING_STUFFING_MODE |
	         HDMI_VP_STUFF_YCC422_STUFFING_STUFFING_MODE);

	hdmi_mod(hdmi, HDMI_VP_CONF, HDMI_VP_CONF_OUTPUT_SELECTOR_MASK,
	         output_select);
}

static int hdmi_get_plug_in_status(struct dw_hdmi *hdmi)
{
	uint val = hdmi_read(hdmi, HDMI_PHY_STAT0) & HDMI_PHY_HPD;

	return ! !val;
}

static int hdmi_ddc_wait_i2c_done(struct dw_hdmi *hdmi, int msec)
{
	u32 val;
	ulong start;

	start = get_timer(0);
	do {
		val = hdmi_read(hdmi, HDMI_IH_I2CM_STAT0);
		if (val & 0x2) {
			hdmi_write(hdmi, val, HDMI_IH_I2CM_STAT0);
			return 0;
		}

		udelay(100);
	} while (get_timer(start) < msec);

	return 1;
}

static void hdmi_ddc_reset(struct dw_hdmi *hdmi)
{
	hdmi_mod(hdmi, HDMI_I2CM_SOFTRSTZ, HDMI_I2CM_SOFTRSTZ_MASK, 0);
}

static int hdmi_read_edid(struct dw_hdmi *hdmi, int block, u8 *buff)
{
	int shift = (block % 2) * 0x80;
	int edid_read_err = 0;
	u32 trytime = 5;
	u32 n;

	/* set ddc i2c clk which devided from ddc_clk to 100khz */
	hdmi_write(hdmi, hdmi->i2c_clk_high, HDMI_I2CM_SS_SCL_HCNT_0_ADDR);
	hdmi_write(hdmi, hdmi->i2c_clk_low, HDMI_I2CM_SS_SCL_LCNT_0_ADDR);
	hdmi_mod(hdmi, HDMI_I2CM_DIV, HDMI_I2CM_DIV_FAST_STD_MODE,
	         HDMI_I2CM_DIV_STD_MODE);

	hdmi_write(hdmi, HDMI_I2CM_SLAVE_DDC_ADDR, HDMI_I2CM_SLAVE);
	hdmi_write(hdmi, HDMI_I2CM_SEGADDR_DDC, HDMI_I2CM_SEGADDR);
	hdmi_write(hdmi, block >> 1, HDMI_I2CM_SEGPTR);

	while (trytime--) {
		edid_read_err = 0;

		for (n = 0; n < HDMI_EDID_BLOCK_SIZE; n++) {
			hdmi_write(hdmi, shift + n, HDMI_I2CM_ADDRESS);

			if (block == 0)
				hdmi_write(hdmi, HDMI_I2CM_OP_RD8,
				           HDMI_I2CM_OPERATION);
			else
				hdmi_write(hdmi, HDMI_I2CM_OP_RD8_EXT,
				           HDMI_I2CM_OPERATION);

			if (hdmi_ddc_wait_i2c_done(hdmi, 10)) {
				hdmi_ddc_reset(hdmi);
				edid_read_err = 1;
				break;
			}

			buff[n] = hdmi_read(hdmi, HDMI_I2CM_DATAI);
		}

		if (!edid_read_err)
			break;
	}

	return edid_read_err;
}

static void hdmi_disable_overflow_interrupts(struct dw_hdmi *hdmi)
{
	hdmi_write(hdmi, HDMI_IH_MUTE_FC_STAT2_OVERFLOW_MASK,
	           HDMI_IH_MUTE_FC_STAT2);
}

static void initialize_hdmi_ih_mutes(struct dw_hdmi *hdmi)
{
	u8 ih_mute;

	/*
	 * Boot up defaults are:
	 * HDMI_IH_MUTE   = 0x03 (disabled)
	 * HDMI_IH_MUTE_* = 0x00 (enabled)
	 *
	 * Disable top level interrupt bits in HDMI block
	 */
	ih_mute = hdmi_read(hdmi, HDMI_IH_MUTE) |
	          HDMI_IH_MUTE_MUTE_WAKEUP_INTERRUPT |
	          HDMI_IH_MUTE_MUTE_ALL_INTERRUPT;

	hdmi_write(hdmi, ih_mute, HDMI_IH_MUTE);

	/* by default mask all interrupts */
	hdmi_write(hdmi, 0xff, HDMI_VP_MASK);
	hdmi_write(hdmi, 0xff, HDMI_FC_MASK0);
	hdmi_write(hdmi, 0xff, HDMI_FC_MASK1);
	hdmi_write(hdmi, 0xff, HDMI_FC_MASK2);
	hdmi_write(hdmi, 0xff, HDMI_PHY_MASK0);
	hdmi_write(hdmi, 0xff, HDMI_PHY_I2CM_INT_ADDR);
	hdmi_write(hdmi, 0xff, HDMI_PHY_I2CM_CTLINT_ADDR);
	hdmi_write(hdmi, 0xff, HDMI_AUD_INT);
	hdmi_write(hdmi, 0xff, HDMI_AUD_SPDIFINT);
	hdmi_write(hdmi, 0xff, HDMI_AUD_HBR_MASK);
	hdmi_write(hdmi, 0xff, HDMI_GP_MASK);
	hdmi_write(hdmi, 0xff, HDMI_A_APIINTMSK);
	hdmi_write(hdmi, 0xff, HDMI_I2CM_INT);
	hdmi_write(hdmi, 0xff, HDMI_I2CM_CTLINT);

	/* Disable interrupts in the IH_MUTE_* registers */
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_FC_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_FC_STAT1);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_FC_STAT2);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_AS_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_PHY_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_I2CM_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_CEC_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_VP_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_I2CMPHY_STAT0);
	hdmi_write(hdmi, 0xff, HDMI_IH_MUTE_AHBDMAAUD_STAT0);

	/* Enable top level interrupt bits in HDMI block */
	ih_mute &= ~(HDMI_IH_MUTE_MUTE_WAKEUP_INTERRUPT |
	             HDMI_IH_MUTE_MUTE_ALL_INTERRUPT);
	hdmi_write(hdmi, ih_mute, HDMI_IH_MUTE);
}

static int hdmi_phy_configure(struct dw_hdmi *hdmi, u32 mpixelclock)
{
	int i;

	if (!hdmi->mpll_cfg || !hdmi->phy_cfg)
		return -1;

	hdmi_phy_power_off(hdmi);

	/* Leave low power consumption mode by asserting SVSRET. */
	if (hdmi->has_svsret)
		hdmi_phy_enable_svsret(hdmi, 1);

	hdmi_phy_reset(hdmi);

	hdmi_write(hdmi, HDMI_MC_HEACPHY_RST_ASSERT, HDMI_MC_HEACPHY_RST);

	hdmi_phy_i2c_set_addr(hdmi, HDMI_PHY_I2CM_SLAVE_ADDR_PHY_GEN2);

	/* pll/mpll cfg - always match on final entry */
	for (i = 0; hdmi->mpll_cfg[i].mpixelclock != (~0ul); i++) {
		if (mpixelclock <= hdmi->mpll_cfg[i].mpixelclock)
			break;
	}

	hdmi_phy_i2c_write(hdmi, hdmi->mpll_cfg[i].cpce, PHY_OPMODE_PLLCFG);
	hdmi_phy_i2c_write(hdmi, hdmi->mpll_cfg[i].gmp, PHY_PLLGMPCTRL);
	hdmi_phy_i2c_write(hdmi, hdmi->mpll_cfg[i].curr, PHY_PLLCURRCTRL);

	for (i = 0; hdmi->phy_cfg[i].mpixelclock != (~0ul); i++) {
		if (mpixelclock <= hdmi->phy_cfg[i].mpixelclock)
			break;
	}

	hdmi_phy_i2c_write(hdmi, hdmi->phy_cfg[i].term, PHY_TXTERM);
	hdmi_phy_i2c_write(hdmi, hdmi->phy_cfg[i].sym_ctr, PHY_CKSYMTXCTRL);
	hdmi_phy_i2c_write(hdmi, hdmi->phy_cfg[i].vlev_ctr, PHY_VLEVCTRL);

	return hdmi_phy_power_on(hdmi);
}

int dw_hdmi_phy_cfg(struct dw_hdmi *hdmi, uint mpixelclock)
{
	int i, ret;

	/* hdmi phy spec says to do the phy initialization sequence twice */
	for (i = 0; i < 2; i++) {
		hdmi_phy_sel_data_en_pol(hdmi, 1);
		hdmi_phy_sel_interface_control(hdmi, 0);

		debug("mpixelclock %d\n", mpixelclock);

		ret = hdmi_phy_configure(hdmi, mpixelclock);
		if (ret) {
			debug("hdmi phy config failure %d\n", ret);
			return ret;
		}
	}

	return 0;
}

int dw_hdmi_phy_wait_for_hpd(struct dw_hdmi *hdmi)
{
	ulong start;

	start = get_timer(0);
	do {
		if (hdmi_get_plug_in_status(hdmi))
			return 0;
		udelay(100);
	} while (get_timer(start) < 300);

	return -1;
}

static void dw_hdmi_phy_setup_hpd(struct dw_hdmi *hdmi)
{
	/*
	 * Configure the PHY RX SENSE and HPD interrupts polarities and clear
	 * any pending interrupt.
	 */
	hdmi_write(hdmi, HDMI_PHY_HPD | HDMI_PHY_RX_SENSE, HDMI_PHY_POL0);
	hdmi_write(hdmi, HDMI_IH_PHY_STAT0_HPD | HDMI_IH_PHY_STAT0_RX_SENSE,
	           HDMI_IH_PHY_STAT0);

	/* Enable cable hot plug irq. */
	hdmi_write(hdmi, hdmi->phy_mask, HDMI_PHY_MASK0);

	/* Clear and unmute interrupts. */
	hdmi_write(hdmi, HDMI_IH_PHY_STAT0_HPD | HDMI_IH_PHY_STAT0_RX_SENSE,
	           HDMI_IH_PHY_STAT0);
	hdmi_write(hdmi, ~(HDMI_IH_PHY_STAT0_HPD | HDMI_IH_PHY_STAT0_RX_SENSE),
	           HDMI_IH_MUTE_PHY_STAT0);
}

void dw_hdmi_phy_init(struct dw_hdmi *hdmi)
{
	/*i2c setup */
	/* enable phy i2cm done irq */
	hdmi_write(hdmi, HDMI_PHY_I2CM_INT_ADDR_DONE_POL,
	           HDMI_PHY_I2CM_INT_ADDR);

	/* enable phy i2cm nack & arbitration error irq */
	hdmi_write(hdmi,
	           HDMI_PHY_I2CM_CTLINT_ADDR_NAC_POL |
	           HDMI_PHY_I2CM_CTLINT_ADDR_ARBITRATION_POL,
	           HDMI_PHY_I2CM_CTLINT_ADDR);

	dw_hdmi_phy_setup_hpd(hdmi);
}

int dw_hdmi_read_edid(struct dw_hdmi *hdmi, u8 *buf, int buf_size)
{
	u32 edid_size = HDMI_EDID_BLOCK_SIZE;
	int ret;

	ret = hdmi_read_edid(hdmi, 0, buf);
	if (ret) {
		printf("failed to read edid \n");
		return -1;
	}

	if (buf[0x7e] != 0) {
		hdmi_read_edid(hdmi, 1, buf + HDMI_EDID_BLOCK_SIZE);
		edid_size += HDMI_EDID_BLOCK_SIZE;
	}

	printf("edid_size:%d\n", edid_size);

	return edid_size;
}

/* HDMI Initialization Step B.4 */
static void dw_hdmi_enable_video_path(struct dw_hdmi *hdmi)
{
	/* control period minimum duration */
	hdmi_write(hdmi, 12, HDMI_FC_CTRLDUR);
	hdmi_write(hdmi, 32, HDMI_FC_EXCTRLDUR);
	hdmi_write(hdmi, 1, HDMI_FC_EXCTRLSPAC);

	/* Set to fill TMDS data channels */
	hdmi_write(hdmi, 0x0B, HDMI_FC_CH0PREAM);
	hdmi_write(hdmi, 0x16, HDMI_FC_CH1PREAM);
	hdmi_write(hdmi, 0x21, HDMI_FC_CH2PREAM);

	/* Enable pixel clock and tmds data path */
	hdmi->mc_clkdis |= HDMI_MC_CLKDIS_HDCPCLK_DISABLE |
	                   HDMI_MC_CLKDIS_CSCCLK_DISABLE |
	                   HDMI_MC_CLKDIS_AUDCLK_DISABLE |
	                   HDMI_MC_CLKDIS_PREPCLK_DISABLE |
			   HDMI_MC_CLKDIS_TMDSCLK_DISABLE;
	hdmi->mc_clkdis &= ~HDMI_MC_CLKDIS_PIXELCLK_DISABLE;
	hdmi_write(hdmi, hdmi->mc_clkdis, HDMI_MC_CLKDIS);

	hdmi->mc_clkdis &= ~HDMI_MC_CLKDIS_TMDSCLK_DISABLE;
	hdmi_write(hdmi, hdmi->mc_clkdis, HDMI_MC_CLKDIS);

	/* Enable csc path */
	if (is_csc_needed(hdmi)) {
		hdmi->mc_clkdis &= ~HDMI_MC_CLKDIS_CSCCLK_DISABLE;
		hdmi_write(hdmi, hdmi->mc_clkdis, HDMI_MC_CLKDIS);

		hdmi_write(hdmi, HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_IN_PATH, HDMI_MC_FLOWCTRL);
	} else {
		hdmi->mc_clkdis |= HDMI_MC_CLKDIS_CSCCLK_DISABLE;
		hdmi_write(hdmi, hdmi->mc_clkdis, HDMI_MC_CLKDIS);

		hdmi_write(hdmi, HDMI_MC_FLOWCTRL_FEED_THROUGH_OFF_CSC_BYPASS, HDMI_MC_FLOWCTRL);
	}
}

/* Workaround to clear the overflow condition */
static void dw_hdmi_clear_overflow(struct dw_hdmi *hdmi)
{
	unsigned int count;
	unsigned int i;
	u8 val;

	switch (hdmi->version) {
	case 0x130a:
		count = 4;
		break;
	case 0x131a:
	case 0x132a:
	case 0x201a:
		count = 1;
		break;
	default:
		return;
	}

	/* TMDS software reset */
	hdmi_write(hdmi, (u8) ~ HDMI_MC_SWRSTZ_TMDSSWRST_REQ, HDMI_MC_SWRSTZ);

	val = hdmi_read(hdmi, HDMI_FC_INVIDCONF);
	for (i = 0; i < count; i++)
		hdmi_write(hdmi, val, HDMI_FC_INVIDCONF);
}

int dw_hdmi_enable(struct dw_hdmi *hdmi, const struct display_timing *edid)
{
	int ret;

	printf("%s, mode info : clock %d hdis %d vdis %d\n",
	       edid->hdmi_monitor ? "hdmi" : "dvi",
	       edid->pixelclock.typ, edid->hactive.typ, edid->vactive.typ);

	hdmi_disable_overflow_interrupts(hdmi);

	hdmi_av_composer(hdmi, edid);

	ret = hdmi->phy_set(hdmi, edid->pixelclock.typ);
	if (ret)
		return ret;

	dw_hdmi_enable_video_path(hdmi);

	/* not for DVI mode */
	if (hdmi->sink_is_hdmi) {
		debug("%s HDMI mode\n", __func__);
		/* HDMI Initialization Step F - Configure AVI InfoFrame */
		hdmi_config_AVI(hdmi);
		hdmi_config_vendor_specific_infoframe(hdmi);
	} else {
		debug("%s DVI mode\n", __func__);
	}
	hdmi_video_packetize(hdmi);
	hdmi_video_csc(hdmi);
	hdmi_video_sample(hdmi);
	hdmi_tx_hdcp_config(hdmi);

	dw_hdmi_clear_overflow(hdmi);

	hdmi_video_sample(hdmi);

	return 0;
}

static int dw_hdmi_detect_phy(struct dw_hdmi *hdmi)
{
	u8 phy_type;

	phy_type = hdmi_read(hdmi, HDMI_CONFIG2_ID);
	//243 DW_HDMI_PHY_DWC_HDMI20_TX_PHY name = "DWC HDMI 2.0 TX PHY",
	printf("%s DWC HDMI 2.0 TX PHY %d\n", __func__, phy_type);
	hdmi->phy_gen = 2;
	hdmi->has_svsret = true;

	return 0;
}

int dw_hdmi_init(struct dw_hdmi *hdmi)
{
	int ret;
	u8 prod_id0;
	u8 prod_id1;

	/* Product and revision IDs */
	hdmi->version =
	        (hdmi_read(hdmi, HDMI_DESIGN_ID) << 8) |
	        (hdmi_read(hdmi, HDMI_REVISION_ID) << 0);

	prod_id0 = hdmi_read(hdmi, HDMI_PRODUCT_ID0);
	prod_id1 = hdmi_read(hdmi, HDMI_PRODUCT_ID1);

	debug("%s prod_id0 = 0x%x, prod_id1 = 0x%x\n", __func__, prod_id0,
	      prod_id1);

	if (prod_id0 != HDMI_PRODUCT_ID0_HDMI_TX ||
	    (prod_id1 & ~HDMI_PRODUCT_ID1_HDCP) != HDMI_PRODUCT_ID1_HDMI_TX) {
		printf("Unsupported HDMI controller (%04x:%02x:%02x)\n",
		       hdmi->version, prod_id0, prod_id1);
		return -1;
	}

	ret = dw_hdmi_detect_phy(hdmi);
	if (ret < 0)
		return -1;

	printf("Detected HDMI TX controller v%x.%03x %s HDCP\n",
	       hdmi->version >> 12, hdmi->version & 0xfff,
	       prod_id1 & HDMI_PRODUCT_ID1_HDCP ? "with" : "without");

	if (hdmi->write_reg)
		hdmi_write = hdmi->write_reg;

	if (hdmi->read_reg)
		hdmi_read = hdmi->read_reg;

	initialize_hdmi_ih_mutes(hdmi);

	/* enable i2c master done irq */
	hdmi_write(hdmi, ~0x04, HDMI_I2CM_INT);

	/* enable i2c client nack % arbitration error irq */
	hdmi_write(hdmi, ~0x44, HDMI_I2CM_CTLINT);

	return 0;
}
