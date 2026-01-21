/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/iopoll.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/mfd/syscon.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_of.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>
#include <video/mipi_display.h>

#include "ax_drm_crtc.h"
#include "ax_mipi_dsi.h"

extern struct i2c_driver m31_dphy_i2c_driver;

/* Table A-4 High-Speed Transition Times */
static struct hs_transition_times hstt_table[] = {
	HSTT(80, 0x00, 21, 17, 15, 10),
	HSTT(90, 0x10, 23, 17, 16, 10),
	HSTT(100, 0x20, 22, 17, 16, 10),
	HSTT(110, 0x30, 25, 18, 17, 11),
	HSTT(120, 0x01, 26, 20, 18, 11),
	HSTT(130, 0x11, 27, 19, 19, 11),
	HSTT(140, 0x21, 27, 19, 19, 11),
	HSTT(150, 0x31, 28, 20, 20, 12),
	HSTT(160, 0x02, 30, 21, 22, 13),
	HSTT(170, 0x12, 30, 21, 23, 13),
	HSTT(180, 0x22, 31, 21, 23, 13),
	HSTT(190, 0x32, 32, 22, 24, 13),
	HSTT(205, 0x03, 35, 22, 25, 13),
	HSTT(220, 0x13, 37, 26, 27, 15),
	HSTT(235, 0x23, 38, 28, 27, 16),
	HSTT(250, 0x33, 41, 29, 30, 17),
	HSTT(275, 0x04, 43, 29, 32, 18),
	HSTT(300, 0x14, 45, 32, 35, 19),
	HSTT(325, 0x25, 48, 33, 36, 18),
	HSTT(350, 0x35, 51, 35, 40, 20),
	HSTT(400, 0x05, 59, 37, 44, 21),
	HSTT(450, 0x16, 65, 40, 49, 23),
	HSTT(500, 0x26, 71, 41, 54, 24),
	HSTT(550, 0x37, 77, 44, 57, 26),
	HSTT(600, 0x07, 82, 46, 64, 27),
	HSTT(650, 0x18, 87, 48, 67, 28),
	HSTT(700, 0x28, 94, 52, 71, 29),
	HSTT(750, 0x39, 99, 52, 75, 31),
	HSTT(800, 0x09, 105, 55, 82, 32),
	HSTT(850, 0x19, 110, 58, 85, 32),
	HSTT(900, 0x29, 115, 58, 88, 35),
	HSTT(950, 0x3A, 120, 62, 93, 36),
	HSTT(1000, 0x0A, 128, 63, 99, 38),
	HSTT(1050, 0x1A, 132, 65, 102, 38),
	HSTT(1100, 0x2A, 138, 67, 106, 39),
	HSTT(1150, 0x3B, 146, 69, 112, 42),
	HSTT(1200, 0x0B, 151, 71, 117, 43),
	HSTT(1250, 0x1B, 153, 74, 120, 45),
	HSTT(1300, 0x2B, 160, 73, 124, 46),
	HSTT(1350, 0x3C, 165, 76, 130, 47),
	HSTT(1400, 0x0C, 172, 78, 134, 49),
	HSTT(1450, 0x1C, 177, 80, 138, 49),
	HSTT(1500, 0x2C, 183, 81, 143, 52),
	HSTT(1550, 0x3D, 191, 84, 147, 52),
	HSTT(1600, 0x0D, 194, 85, 152, 52),
	HSTT(1650, 0x1D, 201, 86, 155, 53),
	HSTT(1700, 0x2E, 208, 88, 161, 53),
	HSTT(1750, 0x3E, 212, 89, 165, 53),
	HSTT(1800, 0x0E, 220, 90, 171, 54),
	HSTT(1850, 0x1E, 223, 92, 175, 54),
	HSTT(1900, 0x2F, 231, 91, 180, 55),
	HSTT(1950, 0x3F, 236, 95, 185, 56),
	HSTT(2000, 0x0F, 243, 97, 190, 56),
	HSTT(2050, 0x40, 248, 99, 194, 58),
	HSTT(2100, 0x41, 252, 100, 199, 59),
	HSTT(2150, 0x42, 259, 102, 204, 61),
	HSTT(2200, 0x43, 266, 105, 210, 62),
	HSTT(2250, 0x44, 269, 109, 213, 63),
	HSTT(2300, 0x45, 272, 109, 217, 65),
	HSTT(2350, 0x46, 281, 112, 225, 66),
	HSTT(2400, 0x47, 283, 115, 226, 66),
	HSTT(2450, 0x48, 282, 115, 226, 67),
	HSTT(2500, 0x49, 281, 118, 227, 67),
};

/* Table 3-13 Charge-pump Programmability */
struct pll_parameter pp_table[] = {
	PLL_PARAM(55, 0x3f, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(82, 0x37, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(110, 0x2f, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(165, 0x27, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(220, 0x1f, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(330, 0x17, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(440, 0x0f, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(660, 0x07, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(1149, 0x03, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(1152, 0x01, 0x10, 0x00, 0x00, 0x08),
	PLL_PARAM(1250, 0x01, 0x10, 0x00, 0x00, 0x09),
};

static const struct of_device_id ax_mipi_dsi_dt_ids[] = {
	{
	 .compatible = "axera,dsi",
	 },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, ax_mipi_dsi_dt_ids);

static const struct drm_encoder_funcs ax_dsi_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static void ax_dsi_encoder_disable(struct drm_encoder *encoder)
{
	struct ax_mipi_dsi *ax_dsi = container_of(encoder, struct ax_mipi_dsi, encoder);
	struct mipi_dsi_priv *dsi_priv = &ax_dsi->dsi_priv;

	DRM_DEBUG_DRIVER("enter, encoder = 0x%px\n", encoder);

	clk_disable_unprepare(dsi_priv->ivideo_clk);
}

static bool ax_dsi_encoder_mode_fixup(struct drm_encoder *encoder,
	const struct drm_display_mode *mode, struct drm_display_mode *adj_mode)
{
	adj_mode->flags &= ~(DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_NHSYNC |
			     DRM_MODE_FLAG_PVSYNC | DRM_MODE_FLAG_NVSYNC);
	adj_mode->flags |= (DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_PVSYNC);

	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(mode));
	DRM_DEBUG_DRIVER("adj_mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(adj_mode));

	return true;
}

static void ax_dsi_encoder_mode_set(struct drm_encoder *encoder,
				    struct drm_display_mode *mode,
				    struct drm_display_mode *adj_mode)
{
	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(mode));
	DRM_DEBUG_DRIVER("adj_mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(adj_mode));
}

static void ax_dsi_encoder_mode_enable(struct drm_encoder *encoder)
{
	int ret;
	struct ax_mipi_dsi *ax_dsi = container_of(encoder, struct ax_mipi_dsi, encoder);
	struct mipi_dsi_priv *dsi_priv = &ax_dsi->dsi_priv;

	DRM_DEBUG_DRIVER("enter, encoder = 0x%px\n", encoder);

	ret = clk_prepare_enable(dsi_priv->ivideo_clk);
	if (ret)
		DRM_ERROR("enable ivideo clk failed, ret = %d\n", ret);
}

static const struct drm_encoder_helper_funcs ax_dsi_encoder_helper_funcs = {
	.mode_fixup = ax_dsi_encoder_mode_fixup,
	.mode_set = ax_dsi_encoder_mode_set,
	.enable = ax_dsi_encoder_mode_enable,
	.disable = ax_dsi_encoder_disable,
};

static int dphy_init(void *priv_data)
{
	u32 val;
	struct mipi_dsi_priv *dsi_priv = (struct mipi_dsi_priv *)priv_data;
	struct pll_parameter *pll_param = &dsi_priv->pll_param;

	DRM_DEBUG_DRIVER("enter\n");

	dphy_write(dsi_priv, DPHYTX_BASE_DIR_0 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_TURN_DISABLE_0 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_FORCE_RX_MODE_0 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_FORCE_TX_STOP_MODE_0 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_FORCE_TX_STOP_MODE_1 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_FORCE_TX_STOP_MODE_2 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_FORCE_TX_STOP_MODE_3 + CLR_OFFS, 0x1);
	dphy_write(dsi_priv, DPHYTX_CFG_CLK_FREQ_RANGE + CLR_OFFS, ~0x0);
	dphy_write(dsi_priv, DPHYTX_HS_CLK_FREQ_RANGE + CLR_OFFS, ~0x0);
	dphy_write(dsi_priv, DPHYTX_PLL_CFG0 + CLR_OFFS, ~0x0);
	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + CLR_OFFS, ~0x0);

	dphy_write(dsi_priv, DPHYTX_CFG_CLK_FREQ_RANGE + SET_OFFS, CFG_CLK_RANGE(pll_param->cfg_freq_range));
	dphy_write(dsi_priv, DPHYTX_HS_CLK_FREQ_RANGE + SET_OFFS, HS_CLK_RANGE(pll_param->hs_freq_range));

	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + CLR_OFFS, PLL_SHADOW_CTRL(0x1));
	dphy_write(dsi_priv, DPHYTX_PLL_CFG0 + SET_OFFS, PLL_CLK_SEL(0x1));
	dphy_write(dsi_priv, DPHYTX_PLL_CFG0 + CLR_OFFS, PLL_CLK_EXT(0x1));

	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + SET_OFFS, PLL_SHADOW_CLR(0x1));
	udelay(1);
	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + CLR_OFFS, PLL_SHADOW_CLR(0x1));

	val = PLL_INT_CTRL(pll_param->int_cntrl) | PLL_GP_CLK_EN(0x1) |
	      PLL_GMP_CTRL(pll_param->gmp_cntrl) | PLL_CPBIAS_CTRL(pll_param->cpbias_cntrl);
	dphy_write(dsi_priv, DPHYTX_PLL_CFG0 + SET_OFFS, val);

	val = PLL_VCO_CTRL(pll_param->vco_cntrl) | PLL_PROP_CTRL(pll_param->prop_cntrl) |
	      PLL_N(pll_param->pll_n) | PLL_M(pll_param->pll_m);
	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + SET_OFFS, val);

	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + SET_OFFS, PLL_UPDATE_PLL(0x1));
	udelay(1);
	dphy_write(dsi_priv, DPHYTX_PLL_CFG1 + CLR_OFFS, PLL_UPDATE_PLL(0x1));

	return 0;
}

static u32 dphy_get_hs_range(u32 max_freq)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(hstt_table); i++)
		if (max_freq < hstt_table[i].max_freq * USEC_PER_SEC)
			return hstt_table[i].hs_freq_range;

	return 0;
}

static int dphy_get_pll_params(struct mipi_dsi_priv *dsi_priv, u64 lane_bps)
{
	int i;
	unsigned int min_prediv, max_prediv;
	unsigned int _prediv0 = 1, _prediv, best_prediv = 0;
	unsigned long best_freq = 0;
	unsigned long fvco_min, fvco_max, fin, fout, fvco;
	unsigned long f_multi, best_multi;
	unsigned long min_delta = ULONG_MAX;
	struct pll_parameter *pll_param = &dsi_priv->pll_param;

	fin = clk_get_rate(dsi_priv->pll_ref_clk);
	if (!fin) {
		DRM_ERROR("Can not get pll ref clk for DPHY\n");
		return -EINVAL;
	}

	fout = lane_bps / 2;

	DRM_DEBUG_DRIVER("fin:%ld, fout:%ld, lane_bps:%lld\n", fin, fout, lane_bps);

	/* constraint: 2Mhz <= Fref / N <= 8MHz */
	min_prediv = DIV_ROUND_UP(fin, 8 * USEC_PER_SEC);
	max_prediv = fin / (2 * USEC_PER_SEC);

	/* constraint: 320MHz <= Fvco <= 1250Mhz */
	fvco_min = 320 * USEC_PER_SEC;
	fvco_max = 1250 * USEC_PER_SEC;

	if (fout * 4 < fvco_min)
		_prediv0 = 8;
	else if (fout * 2 < fvco_min)
		_prediv0 = 4;
	else if (fout < fvco_min)
		_prediv0 = 2;

	fvco = fout * _prediv0;

	for (_prediv = min_prediv; _prediv <= max_prediv; _prediv++) {
		u64 tmp;
		u32 delta;

		/* Fvco = Fref * M / N */
		tmp = (u64)fvco * _prediv;
		f_multi = DIV_ROUND_UP(tmp, fin);

		DRM_DEBUG_DRIVER("f_multi:%lu, tmp:%llu, fvco:%lu, _prediv:%u\n",
				 f_multi, tmp, fvco, _prediv);
		/* M range */
		if (f_multi < 64 || f_multi > 625)
			continue;

		tmp = (u64)f_multi * fin;
		do_div(tmp, _prediv);
		if (tmp < fvco_min || tmp > fvco_max || ((tmp % 500000) != 0)) {
			DRM_DEBUG_DRIVER("invalid fvco(%llu)\n", tmp);
			continue;
		}

		delta = abs(fvco - tmp);
		if (delta < min_delta) {
			best_prediv = _prediv;
			best_multi = f_multi;
			min_delta = delta;
			best_freq = tmp / _prediv0;
		}
	}

	DRM_DEBUG_DRIVER("fout:%ld, _prediv0:%d, best_prediv:%d\n", fout, _prediv0, best_prediv);

	if (best_freq) {
		pll_param->pll_n = best_prediv - 1;
		pll_param->pll_m = best_multi - 2;
		pll_param->actual_freq = best_freq;
	} else {
		DRM_ERROR("Can not find best_freq for DPHY\n");
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(pp_table); i++) {
		if (pll_param->actual_freq <= pp_table[i].max_freq * USEC_PER_SEC) {
			pll_param->max_freq = pp_table[i].max_freq * USEC_PER_SEC;
			/* cfgclkfreqrange[5:0] = round[(Fcfg_clk(MHz)-17)*4] */
			pll_param->cfg_freq_range = (fin / USEC_PER_SEC - 17) * 4;
			pll_param->hs_freq_range = dphy_get_hs_range(pll_param->actual_freq * 2);
			pll_param->vco_cntrl = pp_table[i].vco_cntrl;
			pll_param->cpbias_cntrl = pp_table[i].cpbias_cntrl;
			pll_param->gmp_cntrl = pp_table[i].gmp_cntrl;
			pll_param->int_cntrl = pp_table[i].int_cntrl;
			pll_param->prop_cntrl = pp_table[i].prop_cntrl;
			break;
		}
	}

	DRM_DEBUG_DRIVER("max_freq:%d, actual_freq:%d, pll_n:%d, pll_m:%d, i:%d\n",
			 pll_param->max_freq, pll_param->actual_freq,
			 pll_param->pll_n, pll_param->pll_m, i);

	return 0;
}

static int dphy_get_lane_mbps(void *priv_data,
				const struct drm_display_mode *mode,
				unsigned long mode_flags,
				u32 lanes, u32 format, unsigned int *lane_mbps)
{
	int ret, bpp;
	u64 tmp;
	struct mipi_dsi_priv *dsi_priv = (struct mipi_dsi_priv *)priv_data;
	struct drm_crtc_state *crtc_state = container_of(mode, struct drm_crtc_state, adjusted_mode);
	struct ax_crtc *ax_crtc = to_ax_crtc(crtc_state->crtc);

	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(mode));
	DRM_DEBUG_DRIVER("lanes: %d, mode_flags: 0x%lx, format: %d\n", lanes, mode_flags, format);

	if (!lanes)
		lanes = dsi_priv->plat_data.max_data_lanes;

	dsi_priv->mode_flags = mode_flags;
	dsi_priv->lanes = lanes;
	dsi_priv->format = format;

	if (mode_flags & MIPI_DSI_MODE_VIDEO)
		ax_crtc->mode.type = AX_DISP_OUT_MODE_DSI_VIDEO;
	else
		ax_crtc->mode.type = AX_DISP_OUT_MODE_DSI_CMD;

	switch (format) {
	case MIPI_DSI_FMT_RGB666:
		ax_crtc->mode.fmt_out = AX_DISP_OUT_FMT_RGB666;
		bpp = 24;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		ax_crtc->mode.fmt_out = AX_DISP_OUT_FMT_RGB666LP;
		bpp = 18;
		break;
	case MIPI_DSI_FMT_RGB565:
		ax_crtc->mode.fmt_out = AX_DISP_OUT_FMT_RGB565;
		bpp = 16;
		break;
	default:
		ax_crtc->mode.fmt_out = AX_DISP_OUT_FMT_RGB888;
		bpp = 24;
	}

	tmp = (u64)mode->clock * 1000;

	if (mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
		tmp = tmp * (dsi_priv->burst_factor + 100) / 100;

	tmp = tmp * 8 * (mode->hdisplay * bpp / 8 + 2 * LONG_PKT_OVERHEAD + 1);
	tmp = DIV_ROUND_UP(tmp, (mode->hdisplay * lanes));

	ret = dphy_get_pll_params(dsi_priv, tmp);
	if (ret)
		return ret;

	*lane_mbps = DIV_ROUND_UP(dsi_priv->pll_param.actual_freq * 2, USEC_PER_SEC);

	return 0;
}

static int dphy_get_hs_transition_timing(void *priv_data, unsigned int lane_mbps,
			     struct dw_mipi_dsi_dphy_timing *timing)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(hstt_table); i++)
		if (lane_mbps < hstt_table[i].max_freq)
			break;

	if (i == ARRAY_SIZE(hstt_table))
		i--;

	*timing = hstt_table[i].timing;

	DRM_DEBUG_DRIVER("lane_mbps: %d, c_lp2hs: %d, c_hs2lp: %d, d_lp2hs: %d, d_hs2lp: %d\n",
			 lane_mbps, timing->clk_lp2hs, timing->clk_hs2lp,
			 timing->data_lp2hs, timing->data_lp2hs);

	return 0;
}

struct dw_mipi_dsi_phy_ops dphy_ops = {
	.init = dphy_init,
	.get_lane_mbps = dphy_get_lane_mbps,
	.get_timing = dphy_get_hs_transition_timing,
};

static int ax_mipi_dsi_bind(struct device *dev, struct device *master, void *data)
{
	int ret = 0;
	unsigned int max_data_lanes, burst_factor;
	struct platform_device *pdev = to_platform_device(dev);
	struct drm_device *drm_dev = (struct drm_device *)data;
	struct ax_mipi_dsi *ax_dsi;
	struct mipi_dsi_priv *dsi_priv;
	struct drm_encoder *encoder;
	struct resource *res;
	struct reset_control *txbyte_dskew_rst;

	ax_dsi = devm_kzalloc(&pdev->dev, sizeof(*ax_dsi), GFP_KERNEL);
	if (!ax_dsi) {
		ret = -ENODEV;
		goto exit0;
	}

	dsi_priv = &ax_dsi->dsi_priv;

	ret = of_property_read_u32(pdev->dev.of_node, "max-lanes", &max_data_lanes);
	if (ret) {
		DRM_WARN("%pOF: max-lanes not defined, default max-lanes is 4\n", dev->of_node);
		max_data_lanes = 4;
	}
	dsi_priv->plat_data.max_data_lanes = max_data_lanes;

	ret = of_property_read_u32(pdev->dev.of_node, "burst-factor", &burst_factor);
	if (ret)
		burst_factor = 25;

	dsi_priv->burst_factor = burst_factor;

	DRM_INFO("%pOF: burst_factor:%d\n", dev->of_node, burst_factor);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	dsi_priv->mm_sys_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(dsi_priv->mm_sys_base)) {
		DRM_ERROR("%pOF: failed to get mm_sys register base\n", dev->of_node);
		ret = PTR_ERR(dsi_priv->mm_sys_base);
		goto exit0;
	}

	writel(0x1, dsi_priv->mm_sys_base + 0xAC);

	dsi_priv->plat_data.phy_ops = &dphy_ops;
	dsi_priv->plat_data.priv_data = dsi_priv;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	dsi_priv->dphy_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(dsi_priv->dphy_base)) {
		DRM_ERROR("%pOF: failed to get dphy register base\n", dev->of_node);
		ret = PTR_ERR(dsi_priv->dphy_base);
		goto exit0;
	}

	dsi_priv->pll_ref_clk = devm_clk_get(dev, "dphy_ref");
	if (IS_ERR(dsi_priv->pll_ref_clk)) {
		DRM_ERROR("%pOF: failed to get dphy ref clk\n", dev->of_node);
		ret = PTR_ERR(dsi_priv->pll_ref_clk);
		goto exit0;
	}

	ret = clk_prepare_enable(dsi_priv->pll_ref_clk);
	if (ret) {
		DRM_ERROR("%pOF: failed to enable dphy ref clk\n", dev->of_node);
		goto exit0;
	}

	dsi_priv->ivideo_clk = devm_clk_get(dev, "ivideo_clk");
	if (IS_ERR(dsi_priv->ivideo_clk)) {
		DRM_ERROR("%pOF: failed to get ivideo clk\n", dev->of_node);
		ret = PTR_ERR(dsi_priv->ivideo_clk);
		goto exit1;
	}

	encoder = &ax_dsi->encoder;
	encoder->possible_crtcs = drm_of_find_possible_crtcs(drm_dev, dev->of_node);
	if (encoder->possible_crtcs == 0) {
		DRM_ERROR("%pOF: can not find crtcs for encoder\n", dev->of_node);
		ret = -EPROBE_DEFER;
		goto exit1;
	}

	drm_encoder_helper_add(encoder, &ax_dsi_encoder_helper_funcs);
	drm_encoder_init(drm_dev, encoder, &ax_dsi_encoder_funcs, DRM_MODE_ENCODER_DSI, NULL);

	platform_set_drvdata(pdev, ax_dsi);

	dsi_priv->data = dw_mipi_dsi_probe(pdev, &dsi_priv->plat_data);
	if (IS_ERR(dsi_priv->data)) {
		DRM_ERROR("%pOF: failed to probe\n", dev->of_node);
		ret = -ENODEV;
		goto exit2;
	}

	ret = dw_mipi_dsi_bind((struct dw_mipi_dsi *)dsi_priv->data, encoder);
	if (ret)
		goto exit3;

	txbyte_dskew_rst = devm_reset_control_get_optional_exclusive(dev, "byte_dskew_rst");
	if (IS_ERR(txbyte_dskew_rst)) {
		ret = PTR_ERR(txbyte_dskew_rst);
		if (ret != -EPROBE_DEFER)
			DRM_ERROR("%pOF: failed to get dskew reset\n", dev->of_node);

		goto exit3;
	}

	if (txbyte_dskew_rst)
		reset_control_deassert(txbyte_dskew_rst);

	DRM_INFO("%pOF bind success\n", dev->of_node);

	return 0;

exit3:
	dw_mipi_dsi_remove((struct dw_mipi_dsi *)dsi_priv->data);
exit2:
	drm_encoder_cleanup(encoder);
exit1:
	clk_disable_unprepare(dsi_priv->pll_ref_clk);
exit0:
	DRM_ERROR("%pOF bind failure(%d)\n", dev->of_node, ret);

	return ret;
}

static void ax_mipi_dsi_unbind(struct device *dev, struct device *master,
			       void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct ax_mipi_dsi *ax_dsi = (struct ax_mipi_dsi *)platform_get_drvdata(pdev);
	struct mipi_dsi_priv *dsi_priv = &ax_dsi->dsi_priv;

	dw_mipi_dsi_unbind((struct dw_mipi_dsi *)dsi_priv->data);
	dw_mipi_dsi_remove((struct dw_mipi_dsi *)dsi_priv->data);
	clk_disable_unprepare(dsi_priv->pll_ref_clk);
	drm_encoder_cleanup(&ax_dsi->encoder);

	DRM_INFO("%pOF unbind done\n", dev->of_node);
}

static const struct component_ops ax_mipi_dsi_ops = {
	.bind = ax_mipi_dsi_bind,
	.unbind = ax_mipi_dsi_unbind,
};

static int ax_mipi_dsi_probe(struct platform_device *pdev)
{
	if (!pdev->dev.of_node)
		return -ENODEV;

	return component_add(&pdev->dev, &ax_mipi_dsi_ops);
}

static int ax_mipi_dsi_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &ax_mipi_dsi_ops);

	return 0;
}

struct platform_driver ax_mipi_dsi_driver = {
	.probe = ax_mipi_dsi_probe,
	.remove = ax_mipi_dsi_remove,
	.driver = {
		   .of_match_table = ax_mipi_dsi_dt_ids,
		   .name = "mipi-dsi-drv",
	},
};

MODULE_AUTHOR("zhengwanhu@axera-tech.com");
MODULE_DESCRIPTION("Axera dw dsi driver");
MODULE_LICENSE("GPL v2");
