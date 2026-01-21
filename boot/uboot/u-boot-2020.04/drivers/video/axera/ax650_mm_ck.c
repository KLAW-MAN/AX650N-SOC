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
#include <asm/io.h>

#include "ax_vo_common.h"

#define CLK_MUX_0				0x0
#define CLK_MUX_0_DPU_SEL_BIT(ID)		(4 + (ID) * 3)
#define CLK_MUX_0_DPU_SEL_MASK			(0x7)

#define CLK_EB_0				0x4
#define CLK_HDMI_32K_EB_BIT			(1)
#define CLK_MM_DPU_OUT_EB_BIT(ID)		(2 + (ID) * 1)
#define CLK_MM_ISFR_EB_BIT			(5)

#define CLK_EB_1				0x8
#define CLK_DPU_EB_BIT(ID)			(3 + (ID) * 1)
#define CLK_HDMI_ISFR_EB_BIT(ID)		(5 + (ID) * 1)
#define CLK_HDMI_VIDEO_EB_BIT(ID)		(7 + (ID) * 1)
#define PCLK_DSITX_EB_BIT			(12)
#define PCLK_HDMI_EB_BIT(ID)			(13 + (ID) * 1)

#define CLK_DIV_0				0xC
#define CLK_MUX_0_DPU_DIVN_BIT(ID)		(0 + (ID) * 5)
#define CLK_MUX_0_DPU_DIVN_MASK			(0x1F)
#define CLK_MUX_0_DPU_DIVN_UPDATE_BIT(ID)	(4 + (ID) * 5)
#define CLK_MUX_0_DPU_DIVN_UPDATE_MASK		(0x1)

#define SW_RST_0				0x10
#define DPU_SW_RST_BIT(ID)			(4 + (ID) * 1)
#define DSITX_SW_RST_BIT			(6)
#define HDMI_SW_PRST_BIT(ID)			(8 + (ID) * 2)
#define HDMI_SW_RST_BIT(ID)			(9 + (ID) * 2)
#define TXBYTE_DSKEW_SW_RST_BIT			(16)

#define CLK_MUX_0_SET				0x18
#define CLK_MUX_0_CLR				0x1C

#define CLK_EB_0_SET				0x20
#define CLK_EB_0_CLR				0x24

#define CLK_EB_1_SET				0x28
#define CLK_EB_1_CLR				0x2C

#define CLK_DIV_0_SET				0x30
#define CLK_DIV_0_CLR				0x34

#define SW_RST_0_SET				0x38
#define SW_RST_0_CLR				0x3C

#define VPLL_594 594000000
#define VPLL_198 198000000
#define VPLL_118P8 118800000
#define VPLL_108 108000000

#define SRC_CLK_NUM 4
#define CLK_DIV_NUM 16

void __iomem *mm_ck_regs = (void __iomem *)MM_CK_BASE_ADDR;
void __iomem *mm_phy_ck_rst_regs = (void __iomem *)MM_PHY_CK_RST_BASE_ADDR;
void __iomem *pllc_regs = (void __iomem *)AON_SYS_PLL_BASE_ADDR;

#define GRP_PLL_RE_OPEN		0x478
#define GRP_PLL_RDY_STS		0x3E4
#define VPLL0_ON_CFG		0x3F4
#define VPLL0_CFG0		0x214
#define VPLL0_CFG1		0x220
#define VPLL0_STS		0x210
#define VPLL1_ON_CFG		0x400
#define VPLL1_CFG0		0x248
#define VPLL1_CFG1		0x254
#define VPLL1_STS		0x244

#define PLLC_SET_OFFS		0x4
#define PLLC_CLR_OFFS		0x8

#define VPLL_REF_FREQ		27000000
#define VPLL_MIN_RATE		600000000U
#define VPLL_MAX_RATE		1850000000U
#define VPLL_CUR_RATE		(VPLL_MAX_RATE)

#define ON_BIT_OFF		0
#define LOCK_STS_BIT		0
#define FBK_INT_OFF		0
#define FBK_FRA_OFF		2
#define PRE_DIV_OFF		17
#define LDO_STB_X2_EN_OFF	14
#define POST_DIV_OFF		23

struct vpll_cfg {
	const char *name;
	u32 re_open_reg;
	u32 on_reg;
	u32 cfg0_reg;
	u32 cfg1_reg;
	u32 lock_sts_reg;
	u32 rdy_sts_reg;
	u8 re_open_bit;
	u32 ref_rate;
	u32 min_rate;
	u32 max_rate;
	u32 cur_rate;
};

struct vpll_cfg vpll_cfg_array[] = {
	{
		"vpll0",
		GRP_PLL_RE_OPEN,
		VPLL0_ON_CFG,
		VPLL0_CFG0,
		VPLL0_CFG1,
		VPLL0_STS,
		GRP_PLL_RDY_STS,
		1,
		VPLL_REF_FREQ,
		VPLL_MIN_RATE,
		VPLL_MAX_RATE,
		VPLL_CUR_RATE,
	},
	{
		"vpll1",
		GRP_PLL_RE_OPEN,
		VPLL1_ON_CFG,
		VPLL1_CFG0,
		VPLL1_CFG1,
		VPLL1_STS,
		GRP_PLL_RDY_STS,
		2,
		VPLL_REF_FREQ,
		VPLL_MIN_RATE,
		VPLL_MAX_RATE,
		VPLL_CUR_RATE
	},
};

static int vpll_set(u32 id, u32 rate)
{
	int i = 0, val, try_count = 10000;
	u32 pre_div, fbk_fra, ldo_stb_x2_en;
	u32 fbk_int = rate << 1;
	u32 pfd_rate;
	struct vpll_cfg *cfg;

	if (id > 1) {
		VO_ERROR("id(%d) invalid\n", id);
		return -EINVAL;
	}

	cfg = &vpll_cfg_array[id];

	if ((rate < cfg->min_rate) || (rate > cfg->max_rate)) {
		VO_ERROR("vo%d rate(%d) out of range:[%d,%d]\n", id, rate, cfg->min_rate, cfg->max_rate);
		return -EINVAL;
	}

	if (cfg->cur_rate != rate) {
		cfg->cur_rate = rate;
	} else {
		VO_INFO("vpll%d rate(%d) is configured\n", id, cfg->cur_rate);
		return 0;
	}

	pre_div = 0;
	if (fbk_int % cfg->ref_rate)
		pre_div = 0;
	else if (fbk_int % (cfg->ref_rate >> 1))
		pre_div = 1;

	pfd_rate = cfg->ref_rate >> pre_div;
	fbk_fra = do_div(fbk_int, pfd_rate);
	fbk_fra = ((u64)fbk_fra * (1 << 24) + pfd_rate / 2) / pfd_rate;
	ldo_stb_x2_en = (pfd_rate < 20000000) ? 0 : 1;

	VO_INFO("vpll%d fbk_int:%d, fbk_fra:%d, pfd_rate:%d, pre_div:%d\n", id,
		fbk_int, fbk_fra, pfd_rate, pre_div);

	/* re_open set to 0 */
	vo_writel(pllc_regs, cfg->re_open_reg + PLLC_CLR_OFFS, (1 << cfg->re_open_bit));

	/* on set to 0 */
	vo_writel(pllc_regs, cfg->on_reg + PLLC_CLR_OFFS, (1 << ON_BIT_OFF));

	val = (GENMASK(8, 0) | BIT(14) | BIT(17) | BIT(18) | BIT(23) | BIT(24));
	vo_writel(pllc_regs, cfg->cfg1_reg + PLLC_CLR_OFFS, val);
	val = (1 << POST_DIV_OFF) | (fbk_int << FBK_INT_OFF) | (pre_div << PRE_DIV_OFF) |
	      (ldo_stb_x2_en << LDO_STB_X2_EN_OFF);
	vo_writel(pllc_regs, cfg->cfg1_reg + PLLC_SET_OFFS, val);

	vo_writel(pllc_regs, cfg->cfg0_reg + PLLC_CLR_OFFS, GENMASK(25, 2));
	val = fbk_fra << FBK_FRA_OFF;
	vo_writel(pllc_regs, cfg->cfg0_reg + PLLC_SET_OFFS, val);

	/* on set to 1 */
	vo_writel(pllc_regs, cfg->on_reg + PLLC_SET_OFFS, (1 << ON_BIT_OFF));

	udelay(50);

	/* wait locked */
	for (i = 0; i < try_count; i += 1) {
		if (vo_readl(pllc_regs, cfg->lock_sts_reg) & (1 << LOCK_STS_BIT))
			break;
	}

	if (i == try_count)
		VO_ERROR("vpll%d wait locked failed\n", id);

	/* wait ready */
	for (i = 0; i < try_count; i += 1) {
		if (vo_readl(pllc_regs, cfg->rdy_sts_reg) & (1 << cfg->re_open_bit))
			break;
	}

	if (i == try_count)
		VO_ERROR("vpll%d wait ready failed\n", id);

	/*re_open set to 1 */
	vo_writel(pllc_regs, cfg->re_open_reg + PLLC_SET_OFFS, (1 << cfg->re_open_bit));

	return 0;
}

int ax650_pixel_clk_set(u32 id, u32 clk)
{
	int ret;
	int i, j, found = 0;
	u32 sel, div, rate = VPLL_MAX_RATE;
	u32 pre_div, post_div;
	u32 pre_divs[SRC_CLK_NUM] = {11, 10, 6, 2};
	u32 def_src[SRC_CLK_NUM] = {VPLL_108, VPLL_118P8, VPLL_198, VPLL_594};

	for (i = SRC_CLK_NUM - 1; i >= 0; i -= 1) {
		if (clk == def_src[i]) {
			div = 1;
			sel = i;
			found = 1;
			break;
		} else if ((clk < def_src[i]) && !(def_src[i] % clk) && !((def_src[i] / clk) % 2)) {
			div = def_src[i] / clk;
			if (div <= 30) {
				sel = i;
				found = 1;
				break;
			}
		}
	}

	if (found)
		goto skip;

	for (i = SRC_CLK_NUM - 1; (!found && (i >= 0)); i -= 1) {
		pre_div = pre_divs[i];

		for (j = 0; j < CLK_DIV_NUM; j += 1) {
			post_div = (j == 0) ? 1 : (j << 1);
			rate = clk * post_div * pre_div;
			if ((rate >= VPLL_MIN_RATE) && (rate <= VPLL_MAX_RATE)) {
				sel = i;
				div = post_div;
				found = 1;
				break;
			}
		}
	}

skip:
	if (found) {
		VO_INFO("vo%d sel:%d, div:%d, rate:%d\n", id, sel, div, rate);

		ret = vpll_set(id, rate);
		if (ret)
			return ret;

		div = (div == 1) ? (div - 1) : (div >> 1);
		sel = ((id == 0) && (sel == 3)) ? (sel + 1) : sel;

		vo_writel(mm_ck_regs, CLK_MUX_0_CLR, CLK_MUX_0_DPU_SEL_MASK << CLK_MUX_0_DPU_SEL_BIT(id));
		vo_writel(mm_ck_regs, CLK_MUX_0_SET, sel << CLK_MUX_0_DPU_SEL_BIT(id));

		vo_writel(mm_ck_regs, CLK_DIV_0_CLR, CLK_MUX_0_DPU_DIVN_MASK << CLK_MUX_0_DPU_DIVN_BIT(id));
		vo_writel(mm_ck_regs, CLK_DIV_0_SET, (CLK_MUX_0_DPU_DIVN_UPDATE_MASK << CLK_MUX_0_DPU_DIVN_UPDATE_BIT(id)) |
			  (div << CLK_MUX_0_DPU_DIVN_BIT(id)));
	}

	return found ? 0 : -EINVAL;
}

void ax650_dpu_reset(u32 id)
{
	vo_writel(mm_ck_regs, SW_RST_0_SET, 0x1 << DPU_SW_RST_BIT(id));
	udelay(5);
	vo_writel(mm_ck_regs, SW_RST_0_CLR, 0x1 << DPU_SW_RST_BIT(id));

	vo_writel(mm_ck_regs, CLK_EB_1_SET, 0x1 << CLK_DPU_EB_BIT(id));
	vo_writel(mm_ck_regs, CLK_EB_0_SET, 0x1 << CLK_MM_DPU_OUT_EB_BIT(id));
}

void ax650_dsi_reset(u32 id)
{
	vo_writel(mm_ck_regs, SW_RST_0_SET, (0x1 << TXBYTE_DSKEW_SW_RST_BIT) | (0x1 << DSITX_SW_RST_BIT));
	udelay(5);
	vo_writel(mm_ck_regs, SW_RST_0_CLR, (0x1 << TXBYTE_DSKEW_SW_RST_BIT) | (0x1 << DSITX_SW_RST_BIT));

	vo_writel(mm_ck_regs, CLK_EB_1_SET, 0x1 << PCLK_DSITX_EB_BIT);
	vo_writel(mm_ck_regs, CLK_EB_1_SET, 0x1 << CLK_HDMI_VIDEO_EB_BIT(id));

	vo_writel(mm_phy_ck_rst_regs, 0x4, 0x1);
}

void ax650_hdmi_reset(u32 id)
{
	vo_writel(mm_ck_regs, SW_RST_0_SET, (0x1 << HDMI_SW_PRST_BIT(id)) | (0x1 << HDMI_SW_RST_BIT(id)));
	udelay(5);
	vo_writel(mm_ck_regs, SW_RST_0_CLR, (0x1 << HDMI_SW_PRST_BIT(id)) | (0x1 << HDMI_SW_RST_BIT(id)));

	vo_writel(mm_ck_regs, CLK_EB_0_SET, (0x1 << CLK_HDMI_32K_EB_BIT) | (0x1 << CLK_MM_ISFR_EB_BIT));
	vo_writel(mm_ck_regs, CLK_EB_1_SET, (0x1 << CLK_HDMI_ISFR_EB_BIT(id)) | (0x1 << PCLK_HDMI_EB_BIT(id)));
	vo_writel(mm_ck_regs, CLK_EB_1_SET, 0x1 << CLK_HDMI_VIDEO_EB_BIT(id));
}
