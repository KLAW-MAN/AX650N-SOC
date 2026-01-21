
#ifndef _AX_MIPI_DSI_H_
#define _AX_MIPI_DSI_H_

#include <drm/bridge/dw_mipi_dsi.h>

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

struct mipi_dsi_priv {
	u32 lanes;
	u32 format;
	u32 burst_factor;
	unsigned long mode_flags;

	struct pll_parameter pll_param;

	void __iomem *mm_sys_base;
	void __iomem *dphy_base;

	struct clk *ivideo_clk;
	struct clk *pll_ref_clk;

	struct dw_mipi_dsi_plat_data plat_data;

	void *data;
};

struct ax_mipi_dsi {
	struct mipi_dsi_priv dsi_priv;
	struct drm_encoder encoder;
};

static inline void dphy_write(struct mipi_dsi_priv *priv, u32 reg, u32 val)
{
	writel(val, priv->dphy_base + reg);
}

static inline u32 dsi_read(struct mipi_dsi_priv *priv, u32 reg)
{
	return readl(priv->dphy_base + reg);
}

#endif /* end _AX_MIPI_DSI_H_ */
