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
#include <div64.h>
#include <media_bus_format.h>

#include "dw_mipi_dsi.h"
#include "ax_vo_common.h"

#define USEC_PER_SEC			1000000L

#define LONG_PKT_OVERHEAD		6

#define SET_OFFS			0x4
#define CLR_OFFS			0x8

/* Defines the offset of the dphy register */
#define DPHYTX_BASE_DIR_0		0x10
#define DPHYTX_TURN_DISABLE_0		0x1C
#define DPHYTX_FORCE_RX_MODE_0		0x28
#define DPHYTX_FORCE_TX_STOP_MODE_0	0x34
#define DPHYTX_FORCE_TX_STOP_MODE_1	0x40
#define DPHYTX_FORCE_TX_STOP_MODE_2	0x4C
#define DPHYTX_FORCE_TX_STOP_MODE_3	0x58

#define DPHYTX_CFG_CLK_FREQ_RANGE	0x64
#define CFG_CLK_RANGE(val)		(((val) & 0x3F) << 0)

#define DPHYTX_HS_CLK_FREQ_RANGE	0x70
#define HS_CLK_RANGE(val)		(((val) & 0x7F) << 0)

#define DPHYTX_PLL_CFG0			0x7C
#define PLL_INT_CTRL(val)		(((val) & 0x3F) << 14)
#define PLL_GP_CLK_EN(val)		(((val) & 0x1) << 13)
#define PLL_GMP_CTRL(val)		(((val) & 0x3) << 11)
#define PLL_FORCE_LOCK(val)		(((val) & 0x1) << 10)
#define PLL_CPBIAS_CTRL(val)		(((val) & 0x7F) << 3)
#define PLL_CLK_SEL(val)		(((val) & 0x3) << 1)
#define PLL_CLK_EXT(val)		(((val) & 0x1) << 0)

#define DPHYTX_PLL_CFG1			0x88
#define PLL_VCO_CTRL(val)		(((val) & 0x3F) << 23)
#define PLL_UPDATE_PLL(val)		(((val) & 0x1) << 22)
#define PLL_SHADOW_CLR(val)		(((val) & 0x1) << 21)
#define PLL_PROP_CTRL(val)		(((val) & 0x3F) << 15)
#define PLL_SHADOW_CTRL(val)		(((val) & 0x1) << 14)
#define PLL_N(val)			(((val) & 0xF) << 10)
#define PLL_M(val)			(((val) & 0x3FF) << 0)

#define DPHYTX_PLL_OBS0			0x94
#define DPHYTX_PLL_OBS1			0x98

struct hs_transition_times {
	u32 max_freq;
	u32 hs_freq_range;
	struct dw_mipi_dsi_dphy_timing timing;
};

#define HSTT(_maxfreq, _range, _c_lp2hs, _c_hs2lp, _d_lp2hs, _d_hs2lp)	\
{					\
	.max_freq = _maxfreq,		\
	.hs_freq_range = _range,	\
	.timing = {			\
		.clk_lp2hs = _c_lp2hs,	\
		.clk_hs2lp = _c_hs2lp,	\
		.data_lp2hs = _d_lp2hs,	\
		.data_hs2lp = _d_hs2lp,	\
	}				\
}

struct pll_parameter {
	u32 max_freq;
	u32 actual_freq;
	u16 cfg_freq_range;
	u16 hs_freq_range;
	u16 pll_n;
	u16 pll_m;
	u8 vco_cntrl;
	u8 cpbias_cntrl;
	u8 gmp_cntrl;
	u8 int_cntrl;
	u8 prop_cntrl;
};

#define PLL_PARAM(_maxfreq, _vco_ctl, _cpbias_ctl, _gmp_ctl, _int_ctl, _prop_ctl) \
{					\
	.max_freq = _maxfreq,		\
	.vco_cntrl = _vco_ctl,		\
	.cpbias_cntrl = _cpbias_ctl,	\
	.gmp_cntrl = _gmp_ctl,		\
	.int_cntrl = _int_ctl,		\
	.prop_cntrl = _prop_ctl,	\
}

struct ax_mipi_dsi {
	unsigned max_data_lanes;
	unsigned pll_ref_clk;
	unsigned burst_factor;

	struct dw_mipi_dsi dw_dsi;
	struct mipi_dsi_device dsi_device;

	struct pll_parameter pll_param;
	const struct mipi_dsi_phy_ops phy_ops;

	void __iomem *mm_sys_base;
	void __iomem *dphy_base;
};

static inline void dphy_write(struct ax_mipi_dsi *dsi, u32 reg, u32 val)
{
	writel(val, dsi->dphy_base + reg);
}

static inline u32 dsi_read(struct ax_mipi_dsi *dsi, u32 reg)
{
	return readl(dsi->dphy_base + reg);
}

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

static int dphy_init(void *priv_data)
{
	u32 val;
	struct mipi_dsi_device *dsi_device = priv_data;
	struct ax_mipi_dsi *dsi = container_of(dsi_device, struct ax_mipi_dsi, dsi_device);
	struct pll_parameter *pll_param = &dsi->pll_param;

	dphy_write(dsi, DPHYTX_BASE_DIR_0 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_TURN_DISABLE_0 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_FORCE_RX_MODE_0 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_FORCE_TX_STOP_MODE_0 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_FORCE_TX_STOP_MODE_1 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_FORCE_TX_STOP_MODE_2 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_FORCE_TX_STOP_MODE_3 + CLR_OFFS, 0x1);
	dphy_write(dsi, DPHYTX_CFG_CLK_FREQ_RANGE + CLR_OFFS, ~0x0);
	dphy_write(dsi, DPHYTX_HS_CLK_FREQ_RANGE + CLR_OFFS, ~0x0);
	dphy_write(dsi, DPHYTX_PLL_CFG0 + CLR_OFFS, ~0x0);
	dphy_write(dsi, DPHYTX_PLL_CFG1 + CLR_OFFS, ~0x0);

	dphy_write(dsi, DPHYTX_CFG_CLK_FREQ_RANGE + SET_OFFS, CFG_CLK_RANGE(pll_param->cfg_freq_range));
	dphy_write(dsi, DPHYTX_HS_CLK_FREQ_RANGE + SET_OFFS, HS_CLK_RANGE(pll_param->hs_freq_range));

	dphy_write(dsi, DPHYTX_PLL_CFG1 + CLR_OFFS, PLL_SHADOW_CTRL(0x1));
	dphy_write(dsi, DPHYTX_PLL_CFG0 + SET_OFFS, PLL_CLK_SEL(0x1));
	dphy_write(dsi, DPHYTX_PLL_CFG0 + CLR_OFFS, PLL_CLK_EXT(0x1));

	dphy_write(dsi, DPHYTX_PLL_CFG1 + SET_OFFS, PLL_SHADOW_CLR(0x1));
	udelay(1);
	dphy_write(dsi, DPHYTX_PLL_CFG1 + CLR_OFFS, PLL_SHADOW_CLR(0x1));

	val = PLL_INT_CTRL(pll_param->int_cntrl) | PLL_GP_CLK_EN(0x1) |
	      PLL_GMP_CTRL(pll_param->gmp_cntrl) | PLL_CPBIAS_CTRL(pll_param->cpbias_cntrl);
	dphy_write(dsi, DPHYTX_PLL_CFG0 + SET_OFFS, val);

	val = PLL_VCO_CTRL(pll_param->vco_cntrl) | PLL_PROP_CTRL(pll_param->prop_cntrl) |
	      PLL_N(pll_param->pll_n) | PLL_M(pll_param->pll_m);
	dphy_write(dsi, DPHYTX_PLL_CFG1 + SET_OFFS, val);

	dphy_write(dsi, DPHYTX_PLL_CFG1 + SET_OFFS, PLL_UPDATE_PLL(0x1));
	udelay(1);
	dphy_write(dsi, DPHYTX_PLL_CFG1 + CLR_OFFS, PLL_UPDATE_PLL(0x1));

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

static int dphy_get_pll_params(struct ax_mipi_dsi *dsi, u64 lane_bps)
{
	int i;
	unsigned int min_prediv, max_prediv;
	unsigned int _prediv0 = 1, _prediv, best_prediv = 0;
	unsigned long best_freq = 0;
	unsigned long fvco_min, fvco_max, fin, fout, fvco;
	unsigned long f_multi, best_multi;
	unsigned long min_delta = ULONG_MAX;
	struct pll_parameter *pll_param = &dsi->pll_param;

	fin = dsi->pll_ref_clk;
	fout = lane_bps / 2;

	VO_DEBUG("fin:%ld, fout:%ld, lane_bps:%lld\n", fin, fout, lane_bps);

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

		VO_DEBUG("f_multi:%lu, tmp:%llu, fvco:%lu, _prediv:%u\n",
			 f_multi, tmp, fvco, _prediv);
		/* M range */
		if (f_multi < 64 || f_multi > 625)
			continue;

		tmp = (u64)f_multi * fin;
		do_div(tmp, _prediv);
		if (tmp < fvco_min || tmp > fvco_max || ((tmp % 500000) != 0)) {
			VO_DEBUG("invalid fvco(%llu)\n", tmp);
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

	VO_DEBUG("fout:%ld, _prediv0:%d, best_prediv:%d\n", fout, _prediv0, best_prediv);

	if (best_freq) {
		pll_param->pll_n = best_prediv - 1;
		pll_param->pll_m = best_multi - 2;
		pll_param->actual_freq = best_freq;
	} else {
		VO_ERROR("Can not find best_freq for DPHY\n");
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

	VO_DEBUG("max_freq:%d, actual_freq:%d, pll_n:%d, pll_m:%d, i:%d\n",
		 pll_param->max_freq, pll_param->actual_freq,
		 pll_param->pll_n, pll_param->pll_m, i);

	return 0;
}

static int dphy_get_hs_transition_timing(unsigned int lane_mbps, struct dw_mipi_dsi_dphy_timing *timing)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(hstt_table); i++)
		if (lane_mbps < hstt_table[i].max_freq)
			break;

	if (i == ARRAY_SIZE(hstt_table))
		i--;

	*timing = hstt_table[i].timing;

	VO_DEBUG("lane_mbps: %d, c_lp2hs: %d, c_hs2lp: %d, d_lp2hs: %d, d_hs2lp: %d\n",
		 lane_mbps, timing->clk_lp2hs, timing->clk_hs2lp,
		 timing->data_lp2hs, timing->data_lp2hs);

	return 0;
}

static int dphy_get_lane_mbps(void *priv_data, struct display_timing *timings,
				u32 lanes, u32 format, unsigned int *lane_mbps)
{
	int ret, bpp;
	u64 tmp;
	struct mipi_dsi_device *dsi_device = priv_data;
	struct ax_mipi_dsi *dsi = container_of(dsi_device, struct ax_mipi_dsi, dsi_device);

	VO_DEBUG("lanes:%d, mode_flags:0x%lx, format:%d\n", lanes, dsi_device->mode_flags, format);

	bpp = mipi_dsi_pixel_format_to_bpp(format);
	if (bpp < 0) {
		VO_ERROR("format(%d) invalid\n", format);
		return -EINVAL;
	}

	tmp = (u64)timings->pixelclock.typ;
	/* Add 25% to pll out to be higher than pixel bandwidth (burst mode only) */
	if (dsi_device->mode_flags & MIPI_DSI_MODE_VIDEO_BURST) {
		VO_DEBUG("burst_factor:%d\n", dsi->burst_factor);
		tmp = tmp * (100 + (dsi->burst_factor ? dsi->burst_factor : 25)) / 100;
	}

	tmp = tmp * 8 * (timings->hactive.typ * bpp / 8 + 2 * LONG_PKT_OVERHEAD + 1);
	tmp = DIV_ROUND_UP(tmp, (timings->hactive.typ * lanes));

	ret = dphy_get_pll_params(dsi, tmp);
	if (ret)
		return ret;

	*lane_mbps = DIV_ROUND_UP(dsi->pll_param.actual_freq * 2, USEC_PER_SEC);

	return dphy_get_hs_transition_timing(*lane_mbps, &dsi->dw_dsi.dphy_timing);
}

static struct ax_mipi_dsi mipi_dsi_arrays[] = {
	[0] = {
		.max_data_lanes = 4,
		.pll_ref_clk = 24000000,

		.dw_dsi = {.base = (void *)AX_DW_MIPI_DSI_BASE_ADDR},
		.phy_ops = {
			.init = dphy_init,
			.get_lane_mbps = dphy_get_lane_mbps,
		},

		.mm_sys_base = (void *)MM_SYS_GLB_BASE_ADDR,
		.dphy_base = (void *)DPHY_SYS_BASE_ADDR,
	},
};

static inline struct ax_mipi_dsi *mipi_dsi_dev_get(u32 devid)
{
	struct ax_mipi_dsi *mipi_dsi;

	if (devid >= sizeof(mipi_dsi_arrays) / sizeof(mipi_dsi_arrays[0])) {
		VO_ERROR("invalid devid(%d)\n", devid);
		return NULL;
	}

	mipi_dsi = &mipi_dsi_arrays[devid];

	return mipi_dsi;
}

int ax_dw_mipi_dsi_enable(u32 devid, const struct display_timing *timing)
{
	struct ax_mipi_dsi *mipi_dsi = mipi_dsi_dev_get(devid);

	if (!mipi_dsi) {
		VO_ERROR("no suitable dsi dev for vo%d\n", devid);
		return -ENODEV;
	}

	return dw_mipi_dsi_enable(&mipi_dsi->dw_dsi);
}

int ax_dw_mipi_dsi_init(u32 devid, struct display_timing *timing, struct display_timing *adjusted_timing, void *data)
{
	int ret;
	struct ax_mipi_dsi *mipi_dsi = mipi_dsi_dev_get(devid);
	struct simple_panel_cfg *panel_cfg = data;
	struct dsi_panel_cfg *dp_cfg = &panel_cfg->dp_cfg;
	struct mipi_dsi_device *dsi_device;

	if (!mipi_dsi) {
		VO_ERROR("no suitable dsi dev for vo%d\n", devid);
		ret = -ENODEV;
		goto exit;
	}

	if (panel_cfg->type != PANEL_TYPE_MIPI) {
		VO_ERROR("panel cfg mismatch for vo%d\n", devid);
		ret = -EINVAL;
		goto exit;
	}

	dsi_device = &mipi_dsi->dsi_device;
	dsi_device->lanes = dp_cfg->lanes;
	dsi_device->format = dp_cfg->format;
	dsi_device->mode_flags = dp_cfg->mode_flags;
	mipi_dsi->burst_factor = dp_cfg->burst_factor;
	dp_cfg->dsi_dev = dsi_device;

	writel(0x1, mipi_dsi->mm_sys_base + 0xAC);

	ax650_dsi_reset(devid);

	ret = dw_mipi_dsi_init(&mipi_dsi->dw_dsi, dsi_device, timing, adjusted_timing,
			       mipi_dsi->max_data_lanes, &mipi_dsi->phy_ops);
	if (ret) {
		VO_ERROR("vo%d dw mipi dsi init failed, ret:%d\n", devid, ret);
		goto exit;
	}

exit:
	VO_INFO("mipi dsi%d init %s\n", devid, ret ? "failed" : "success");

	return ret;
}

