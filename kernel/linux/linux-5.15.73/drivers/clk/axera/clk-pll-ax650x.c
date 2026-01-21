/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <dt-bindings/clock/ax650x-clock.h>

#include "clk-axera.h"

#undef CLK_PLL_DEBUG

#ifdef CLK_PLL_DEBUG
#define CLK_PLL_DBG(FMT, ...) pr_info("[%s:%d] "FMT, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define CLK_PLL_DBG(FMT, ...)
#endif
#define CLK_PLL_ERR(FMT, ...) pr_err("[%s:%d] "FMT, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define VPLL0_STS_OFFS		0x210
#define VPLL1_STS_OFFS		0x244
#define NFPLL_STS_OFFS		0x37C
#define LOCKED_BIT		BIT(0)

#define VPLL0_CFG0_OFFS		0x214
#define VPLL1_CFG0_OFFS		0x248
#define NFPLL_CFG0_OFFS		0x380
#define FBK_FRA_MASK		GENMASK(25, 2)
#define FBK_FRA_SHIFT		2
#define FBK_CHG			BIT(1)

#define VPLL0_CFG1_OFFS		0x220
#define VPLL1_CFG1_OFFS		0x254
#define NFPLL_CFG1_OFFS		0x38C
#define POST_DIV_MASK		GENMASK(24, 23)
#define POST_DIV_SHIFT		23
#define PRE_DIV_MASK		GENMASK(18, 17)
#define PRE_DIV_SHIFT		17
#define LDO_STB_X2_EN		BIT(14)
#define LDO_STB_X2_EN_SHIFT	14
#define FBK_INT_MASK		GENMASK(8, 0)
#define FBK_INT_SHIFT		0

#define VPLL0_ON_CFG_OFFS	0x3F4
#define VPLL1_ON_CFG_OFFS	0x400
#define NFPLL_ON_CFG_OFFS	0x448
#define ON_BIT			BIT(0)

#define GRP_PLL_RDY_STS_OFFS	0x3E4
#define GRP_PLL_RE_OPEN_OFFS	0x478

#define SET_ADDR(ADDR)		((ADDR) + 0x4)
#define CLR_ADDR(ADDR)		((ADDR) + 0x8)

#define PLL_FVCO_MIN_RATE	600000000
#define PLL_FVCO_MAX_RATE	3700000000

#define VPLL_REF_RATE_27M	27000000
#define VPLL_REF_RATE_21M	21000000

#define NFPLL_REF_RATE		24000000
#define NFPLL_FIXED_POST_DIV	22

#define DIV_AUTO		0xFFFF
#define _POST_DIV_MASK		(GENMASK(15, 0))
#define _POST_DIV_SHIFT		0
#define POST_DIV_MAX		((POST_DIV_MASK) >> (POST_DIV_SHIFT))
#define POST_DIV_MIN		0
#define POST_DIV(DIV)		(((DIV) << _POST_DIV_SHIFT) & _POST_DIV_MASK)
#define _PRE_DIV_MASK		(GENMASK(31, 16))
#define _PRE_DIV_SHIFT		16
#define PRE_DIV_MAX		((PRE_DIV_MASK) >> (PRE_DIV_SHIFT))
#define PRE_DIV_MIN		0
#define PRE_DIV(DIV)		(((DIV) << _PRE_DIV_SHIFT) & _PRE_DIV_MASK)
#define POST_DIV_GET(DIV)	(((DIV) & _POST_DIV_MASK) >> _POST_DIV_SHIFT)
#define PRE_DIV_GET(DIV)	(((DIV) & _PRE_DIV_MASK) >> _PRE_DIV_SHIFT)

#define PLL_NAME_MAX		32

enum {
    PLL_DIV_FBK_NORMAL,
    PLL_DIV_FBK_INT_ONLY
};

#define to_pll_hw(_hw) container_of(_hw, struct clk_pll_hw, hw)
#define to_pll_hw_nb(nb) container_of(nb, struct clk_pll_hw, clk_nb)

struct clk_pll_cfg {
	u32 id;
	const char *name;
	const char *parent_name;

	ulong flags;

	ulong ref_rate;

	u32 div;

	u32 re_open_reg_offs;
	u32 re_open_bit;
	u32 on_cfg_reg_offs;
	u32 cfg0_reg_offs;
	u32 cfg1_reg_offs;
	u32 lock_sts_reg_offs;
	u32 rdy_sts_reg_offs;
	u32 rdy_bit;
};

struct pll_regs {
	void __iomem *on_cfg_reg;
	void __iomem *cfg0_reg;
	void __iomem *cfg1_reg;
	void __iomem *lock_sts_reg;

	void __iomem *re_open_reg;
	u32 re_open_bit;

	void __iomem *rdy_sts_reg;
	u32 rdy_bit;
};

struct clk_pll_hw {
	u32 id;

	u32 fbk_mode;
	u32 ref_rate;
	u32 cur_ref_rate;
	u32 div;

	ulong min_rate;
	ulong max_rate;
	ulong cur_rate;

	struct clk_hw hw;
	struct clk_hw *ref_hw;
	struct clk_hw *ref_root_hw;

	struct notifier_block clk_nb;

	void __iomem *base;
	struct pll_regs regs;

	spinlock_t lock;

	struct list_head associated_pll_list;

	struct list_head node;
};

static void pll_cfg_get_nolock(struct clk_pll_hw *pll_hw,
		u32 *fref, u32 *pre_div, u32 *fvco,
		u32 *post_div, u32 *fbk_int, u32 *fbk_fra);

static struct clk_pll_cfg ax650_pll_cfgs[] = {
	{
		.id = AX650X_CLK_VPLL0,
		.name = "vpll0",
		.parent_name = NULL,
		.flags = CLK_GET_RATE_NOCACHE,
		.ref_rate = VPLL_REF_RATE_27M,
		.div = PRE_DIV(DIV_AUTO) | POST_DIV(1),
		.re_open_reg_offs = GRP_PLL_RE_OPEN_OFFS,
		.re_open_bit = BIT(1),
		.on_cfg_reg_offs = VPLL0_ON_CFG_OFFS,
		.cfg0_reg_offs = VPLL0_CFG0_OFFS,
		.cfg1_reg_offs = VPLL0_CFG1_OFFS,
		.lock_sts_reg_offs = VPLL0_STS_OFFS,
		.rdy_sts_reg_offs = GRP_PLL_RDY_STS_OFFS,
		.rdy_bit = BIT(1),
	},
	{
		.id = AX650X_CLK_VPLL1,
		.name = "vpll1",
		.parent_name = NULL,
		.flags = CLK_GET_RATE_NOCACHE,
		.ref_rate = VPLL_REF_RATE_27M,
		.div = PRE_DIV(DIV_AUTO) | POST_DIV(1),
		.re_open_reg_offs = GRP_PLL_RE_OPEN_OFFS,
		.re_open_bit = BIT(2),
		.on_cfg_reg_offs = VPLL1_ON_CFG_OFFS,
		.cfg0_reg_offs = VPLL1_CFG0_OFFS,
		.cfg1_reg_offs = VPLL1_CFG1_OFFS,
		.lock_sts_reg_offs = VPLL1_STS_OFFS,
		.rdy_sts_reg_offs = GRP_PLL_RDY_STS_OFFS,
		.rdy_bit = BIT(2),
	},
	{
		.id = AX650X_CLK_NFPLL,
		.name = "nfpll",
		.parent_name = NULL,
		.flags = CLK_GET_RATE_NOCACHE,
		.ref_rate = NFPLL_REF_RATE,
		.div = PRE_DIV(0) | POST_DIV(DIV_AUTO),
		.re_open_reg_offs = GRP_PLL_RE_OPEN_OFFS,
		.re_open_bit = BIT(8),
		.on_cfg_reg_offs = NFPLL_ON_CFG_OFFS,
		.cfg0_reg_offs = NFPLL_CFG0_OFFS,
		.cfg1_reg_offs = NFPLL_CFG1_OFFS,
		.lock_sts_reg_offs = NFPLL_STS_OFFS,
		.rdy_sts_reg_offs = GRP_PLL_RDY_STS_OFFS,
		.rdy_bit = BIT(8),
	},
};

static u32 pll_ref_rate_get(struct clk_pll_hw *pll_hw)
{
	u32 fvco;
	u32 fixed_div, post_div;
	struct clk_pll_hw *ref_pll_hw;

	if (!pll_hw->ref_hw)
		return pll_hw->ref_rate;

	ref_pll_hw = to_pll_hw(pll_hw->ref_root_hw);

	spin_lock(&ref_pll_hw->lock);

	fixed_div = clk_hw_get_rate(pll_hw->ref_root_hw) / clk_hw_get_rate(pll_hw->ref_hw);

	pll_cfg_get_nolock(ref_pll_hw, NULL, NULL, &fvco, &post_div, NULL, NULL);

	spin_unlock(&ref_pll_hw->lock);

	return fvco / ((1 << post_div) * fixed_div);
}

static unsigned long pll_rate_get(struct clk_pll_hw *pll_hw)
{
	ulong rate;
	u32 _cfg0_reg, _cfg1_reg;
	u32 fbk_int, fbk_fra;
	u32 pre_div, post_div;
	u32 ref_rate, pfd_rate;
	u64 fvco;
	struct pll_regs *regs = &pll_hw->regs;

	spin_lock(&pll_hw->lock);

	ref_rate = pll_ref_rate_get(pll_hw);

	_cfg0_reg = readl_relaxed(regs->cfg0_reg);
	_cfg1_reg = readl_relaxed(regs->cfg1_reg);

	spin_unlock(&pll_hw->lock);

	fbk_fra = (_cfg0_reg & FBK_FRA_MASK) >> FBK_FRA_SHIFT;
	fbk_int = _cfg1_reg & FBK_INT_MASK;
	pre_div = (_cfg1_reg & PRE_DIV_MASK) >> PRE_DIV_SHIFT;
	post_div = (_cfg1_reg & POST_DIV_MASK) >> POST_DIV_SHIFT;

	pfd_rate = ref_rate >> pre_div;
	fvco = fbk_int * pfd_rate + ((u64)fbk_fra * pfd_rate + (1 << 23)) / (1 << 24);
	rate = fvco >> post_div;

	CLK_PLL_DBG("%s div:[%d,%d], fbk:[%u,%u], fvco:%llu, ref_rate:%u, rate:%lu\n",
		    clk_hw_get_name(&pll_hw->hw), pre_div, post_div, fbk_int, fbk_fra,
		    fvco, ref_rate, rate);

	return rate;
}

static void pll_cfg_get_nolock(struct clk_pll_hw *pll_hw,
	u32 *fref, u32 *pre_div, u32 *fvco,
	u32 *post_div, u32 *fbk_int, u32 *fbk_fra)
{
	u32 cfg0_reg, cfg1_reg;
	u32 _fbk_int, _fbk_fra;
	u32 _pre_div, _post_div;
	u32 _ref_rate, _pfd_rate;
	u64 _fvco;
	struct pll_regs *regs = &pll_hw->regs;

	_ref_rate = pll_ref_rate_get(pll_hw);

	cfg0_reg = readl_relaxed(regs->cfg0_reg);
	cfg1_reg = readl_relaxed(regs->cfg1_reg);

	_fbk_fra = (cfg0_reg & FBK_FRA_MASK) >> FBK_FRA_SHIFT;
	_fbk_int = cfg1_reg & FBK_INT_MASK;
	_pre_div = (cfg1_reg & PRE_DIV_MASK) >> PRE_DIV_SHIFT;
	_post_div = (cfg1_reg & POST_DIV_MASK) >> POST_DIV_SHIFT;

	_pfd_rate = _ref_rate >> _pre_div;
	_fvco = _fbk_int * _pfd_rate + ((u64)_fbk_fra * _pfd_rate + (1 << 23)) / (1 << 24);

	if (fref)
		*fref = _ref_rate;

	if (pre_div)
		*pre_div = _pre_div;

	if (fvco)
		*fvco = _fvco;

	if (post_div)
		*post_div = _post_div;

	if (fbk_int)
		*fbk_int = _fbk_int;

	if (fbk_fra)
		*fbk_fra = _fbk_fra;

	CLK_PLL_DBG("%s div:[%d,%d], fbk:[%u,%u], fvco:%llu, ref_rate:%u\n",
		    clk_hw_get_name(&pll_hw->hw), _pre_div, _post_div,
		    _fbk_int, _fbk_fra, _fvco, _ref_rate);
}

int pll_cfg_get(struct clk *clk_pll, u32 *fref, u32 *pre_div, u32 *fvco,
		u32 *post_div, u32 *fbk_int, u32 *fbk_fra)
{
	struct clk_pll_hw *pll_hw;

	if (IS_ERR_OR_NULL(clk_pll))
		return -EINVAL;

	pll_hw = to_pll_hw(__clk_get_hw(clk_pll));

	spin_lock(&pll_hw->lock);
	pll_cfg_get_nolock(pll_hw, fref, pre_div, fvco, post_div, fbk_int, fbk_fra);
	spin_unlock(&pll_hw->lock);

	return 0;
}
EXPORT_SYMBOL(pll_cfg_get);

static void pll_rate_adj_nolock(struct clk_pll_hw *pll_hw, u32 fbk_int, u32 fbk_fra)
{
	int i, try_count = 300;
	u32 val;
	struct pll_regs *regs = &pll_hw->regs;

	CLK_PLL_DBG("%s fbk_int:%u, fbk_fra:%u\n", clk_hw_get_name(&pll_hw->hw), fbk_int, fbk_fra);

	if ((fbk_int == 0) && (fbk_fra == ~0))
		return;

	writel_relaxed(FBK_CHG, CLR_ADDR(regs->cfg0_reg));

	val = ((fbk_int << FBK_INT_SHIFT) & FBK_INT_MASK);
	if (val) {
		writel_relaxed(FBK_INT_MASK, CLR_ADDR(regs->cfg1_reg));
		writel_relaxed(val, SET_ADDR(regs->cfg1_reg));
	}

	if (fbk_fra != ~0) {
		writel_relaxed(FBK_FRA_MASK, CLR_ADDR(regs->cfg0_reg));
		writel_relaxed(((fbk_fra << FBK_FRA_SHIFT) & FBK_FRA_MASK), SET_ADDR(regs->cfg0_reg));
	}

	writel_relaxed(FBK_CHG, SET_ADDR(regs->cfg0_reg));

	/* wait locked */
	for (i = 0; i < try_count; i += 1) {
		if (readl_relaxed(regs->lock_sts_reg) & LOCKED_BIT)
			break;

		udelay(2);
	}

	if (i == try_count)
		CLK_PLL_ERR("%s unlocked!\n", clk_hw_get_name(&pll_hw->hw));
}

int pll_rate_adj(struct clk *clk_pll, u32 fbk_int, u32 fbk_fra)
{
	struct clk_pll_hw *pll_hw;

	if (IS_ERR_OR_NULL(clk_pll))
		return -EINVAL;

	pll_hw = to_pll_hw(__clk_get_hw(clk_pll));

	if (pll_hw->fbk_mode == PLL_DIV_FBK_INT_ONLY)
		fbk_fra = ~0;

	spin_lock(&pll_hw->lock);
	pll_rate_adj_nolock(pll_hw, fbk_int, fbk_fra);
	spin_unlock(&pll_hw->lock);

	return 0;
}
EXPORT_SYMBOL(pll_rate_adj);

int pll_fbk_mode_set(struct clk *clk_pll, u32 mode)
{
	struct clk_pll_hw *pll_hw;

	if (IS_ERR_OR_NULL(clk_pll))
		return -EINVAL;

	pll_hw = to_pll_hw(__clk_get_hw(clk_pll));

	mode = (mode == PLL_DIV_FBK_NORMAL) ? PLL_DIV_FBK_NORMAL : PLL_DIV_FBK_INT_ONLY;

	spin_lock(&pll_hw->lock);

	if (mode != pll_hw->fbk_mode) {
		if (mode == PLL_DIV_FBK_INT_ONLY)
			pll_rate_adj_nolock(pll_hw, 0, 0);

		pll_hw->fbk_mode = mode;
	}

	spin_unlock(&pll_hw->lock);

        CLK_PLL_DBG("%s fbk_mode:%d\n", __clk_get_name(clk_pll), mode);

	return 0;
}
EXPORT_SYMBOL(pll_fbk_mode_set);

static void pll_ref_rate_adj(struct clk_pll_hw *pll_hw, unsigned long rate)
{
	u32 fixed_div;
	u32 fbk_int, fbk_fra;
	u32 pre_div = 0, post_div = 0;
	u32 ref_rate, pfd_rate;
	struct clk_pll_hw *ref_root_pll_hw;

	if (!pll_hw->ref_hw || !pll_hw->ref_root_hw)
		return;

	fixed_div = clk_hw_get_rate(pll_hw->ref_root_hw) / clk_hw_get_rate(pll_hw->ref_hw);

        CLK_PLL_DBG("%s %s ref_root_rate:%lu, ref_rate:%lu, fixed_div:%u rate:%lu\n",
		    clk_hw_get_name(pll_hw->ref_root_hw),
		    clk_hw_get_name(pll_hw->ref_hw),
		    clk_hw_get_rate(pll_hw->ref_root_hw),
		    clk_hw_get_rate(pll_hw->ref_hw),
		    fixed_div, rate);

	ref_root_pll_hw = to_pll_hw(pll_hw->ref_root_hw);

	spin_lock(&ref_root_pll_hw->lock);

	pll_cfg_get_nolock(ref_root_pll_hw, &ref_rate, &pre_div, NULL, &post_div, NULL, NULL);

	fbk_int = (rate * fixed_div) << post_div;
	pfd_rate = ref_rate >> pre_div;
	fbk_fra = do_div(fbk_int, pfd_rate);
	fbk_fra = ((u64)fbk_fra * (1 << 24) + pfd_rate / 2) / pfd_rate;

	pll_rate_adj_nolock(ref_root_pll_hw, fbk_int, fbk_fra);

	spin_unlock(&ref_root_pll_hw->lock);
}

static int pll_rate_set(struct clk_pll_hw *pll_hw, unsigned long rate)
{
	int ret = 0;
	int i, try_count = 200;
	u32 val;
	u32 pre_div, post_div;
	u32 ref_rate, pfd_rate;
	u32 ldo_stb_x2_en;
	u32 fbk_int, fbk_fra;
	struct pll_regs *regs = &pll_hw->regs;

	pre_div = PRE_DIV_GET(pll_hw->div);

	post_div = POST_DIV_GET(pll_hw->div);
	if (post_div == DIV_AUTO) {
		for (post_div = POST_DIV_MAX; post_div > 0; post_div -= 1) {
			if ((rate << post_div) <= PLL_FVCO_MAX_RATE)
				break;
		}
	}

	fbk_int = rate << post_div;

	CLK_PLL_DBG("%s rate:%lu, post_div:%u\n", clk_hw_get_name(&pll_hw->hw), rate, post_div);

	spin_lock(&pll_hw->lock);

	if (pll_hw->fbk_mode == PLL_DIV_FBK_NORMAL) {
		ref_rate = pll_ref_rate_get(pll_hw);
		if (pre_div == DIV_AUTO) {
			pre_div = 0;
			if (fbk_int % ref_rate)
				pre_div = 0;
			else if (fbk_int % (ref_rate >> 1))
				pre_div = 1;
		}

		pfd_rate = ref_rate >> pre_div;
		fbk_fra = do_div(fbk_int, pfd_rate);
		fbk_fra = ((u64)fbk_fra * (1 << 24) + pfd_rate / 2) / pfd_rate;

	} else {
		ref_rate = pll_hw->cur_ref_rate;
		if (pre_div == DIV_AUTO)
			pre_div = 0;
		pfd_rate = ref_rate >> pre_div;
		fbk_fra = do_div(fbk_int, pfd_rate);
		fbk_fra = 0;
		pfd_rate = ((rate << post_div) / fbk_int);
		if ((pfd_rate << pre_div) != ref_rate) {
			ref_rate = (pfd_rate << pre_div);
			pll_ref_rate_adj(pll_hw, ref_rate);
		}
	}

	ldo_stb_x2_en = (pfd_rate < 20000000) ? 0 : 1;

	CLK_PLL_DBG("%s rate:%lu, ref_rate:%u, pfd_rate:%u, fbk_int:%u, fbk_fra:%u\n",
		    clk_hw_get_name(&pll_hw->hw), rate, ref_rate, pfd_rate, fbk_int, fbk_fra);

	/* re_open set to 0 */
	writel_relaxed(regs->re_open_bit, CLR_ADDR(regs->re_open_reg));
	/* on set to 0 */
	writel_relaxed(ON_BIT, CLR_ADDR(regs->on_cfg_reg));

	val = POST_DIV_MASK | PRE_DIV_MASK | LDO_STB_X2_EN | FBK_INT_MASK;
	writel_relaxed(val, CLR_ADDR(regs->cfg1_reg));
	val = ((post_div << POST_DIV_SHIFT) & POST_DIV_MASK) | ((pre_div << PRE_DIV_SHIFT) & PRE_DIV_MASK) |
	      ((fbk_int << FBK_INT_SHIFT) & FBK_INT_MASK);
	if (ldo_stb_x2_en)
		val |= LDO_STB_X2_EN;
	writel_relaxed(val, SET_ADDR(regs->cfg1_reg));

	writel_relaxed(FBK_FRA_MASK, CLR_ADDR(regs->cfg0_reg));
	val = (fbk_fra << FBK_FRA_SHIFT) & FBK_FRA_MASK;
	writel_relaxed(val, SET_ADDR(regs->cfg0_reg));

	/* on set to 1 */
	writel_relaxed(ON_BIT, SET_ADDR(regs->on_cfg_reg));

	/* wait locked */
	for (i = 0; i < try_count; i += 1) {
		if (readl_relaxed(regs->lock_sts_reg) & LOCKED_BIT)
			break;

		udelay(5);
	}

	if (i == try_count) {
		CLK_PLL_ERR("%s unlocked!\n", clk_hw_get_name(&pll_hw->hw));
		ret = -EAGAIN;
		goto exit;
	}

	/* wait rdy */
	for (i = 0; i < try_count; i += 1) {
		if (readl_relaxed(regs->rdy_sts_reg) & regs->rdy_bit)
			break;

		udelay(5);
	}

	if (i == try_count) {
		CLK_PLL_ERR("%s not ready!\n", clk_hw_get_name(&pll_hw->hw));
		ret = -EAGAIN;
	}

	/* re_open set to 1 */
	writel_relaxed(regs->re_open_bit, SET_ADDR(regs->re_open_reg));

exit:
	spin_unlock(&pll_hw->lock);

	return ret;
}

void pll_ref_set(struct clk *clk_pll, struct clk *clk_pll_ref)
{
	struct clk *clk_ref_root, *parent;
	struct clk_pll_hw *pll_hw, *pll_ref_root_hw;

	if (IS_ERR_OR_NULL(clk_pll) || IS_ERR(clk_pll_ref))
		return;

	pll_hw = to_pll_hw(__clk_get_hw(clk_pll));

	if (!clk_pll_ref && pll_hw->ref_root_hw) {
		pll_ref_root_hw = to_pll_hw(pll_hw->ref_root_hw);

		spin_lock(&pll_ref_root_hw->lock);
		list_del_init(&pll_hw->node);

		spin_lock(&pll_hw->lock);
		pll_hw->ref_hw = NULL;
		pll_hw->ref_root_hw = NULL;
		spin_unlock(&pll_hw->lock);

		spin_unlock(&pll_ref_root_hw->lock);

		return;
	}

	CLK_PLL_DBG("%s %s\n", __clk_get_name(clk_pll), __clk_get_name(clk_pll_ref));

	clk_ref_root = clk_pll_ref;
	for (parent = clk_get_parent(clk_pll_ref); parent != NULL; parent = clk_get_parent(parent)) {
		clk_ref_root = parent;
	}

	pll_ref_root_hw = to_pll_hw(__clk_get_hw(clk_ref_root));

	spin_lock(&pll_ref_root_hw->lock);
	list_add_tail(&pll_hw->node, &pll_ref_root_hw->associated_pll_list);

	spin_lock(&pll_hw->lock);
	pll_hw->ref_hw = __clk_get_hw(clk_pll_ref);
	pll_hw->ref_root_hw = __clk_get_hw(clk_ref_root);
	spin_unlock(&pll_hw->lock);

	spin_unlock(&pll_ref_root_hw->lock);

	CLK_PLL_DBG("%s %s %s\n", clk_hw_get_name(&pll_hw->hw),
		    clk_hw_get_name(pll_hw->ref_hw),
		    clk_hw_get_name(pll_hw->ref_root_hw));
}
EXPORT_SYMBOL(pll_ref_set);

static int clk_pll_notifier_cb(struct notifier_block *nb, unsigned long event, void *data)
{
	int ret = 0;
	struct clk_pll_hw *pll_hw = to_pll_hw_nb(nb);
	struct clk_pll_hw *associated_pll_hw;

	if (event == POST_RATE_CHANGE) {
		list_for_each_entry(associated_pll_hw, &pll_hw->associated_pll_list, node) {
			/* update ref rate */
			associated_pll_hw->cur_ref_rate = clk_get_rate(associated_pll_hw->ref_hw->clk);
			associated_pll_hw->cur_rate = clk_get_rate(associated_pll_hw->hw.clk);

			CLK_PLL_DBG("%s cur_ref_rate:%u, cur_rate:%lu\n", __clk_get_name(associated_pll_hw->hw.clk),
				    associated_pll_hw->cur_ref_rate,
				    associated_pll_hw->cur_rate);
		}
	}

	return notifier_from_errno(ret);
}

static int clk_pll_set_rate(struct clk_hw *hw, unsigned long rate, unsigned long parent_rate)
{
	struct clk_pll_hw *pll_hw = to_pll_hw(hw);

	return pll_rate_set(pll_hw, rate);
}

static long clk_pll_round_rate(struct clk_hw *hw, unsigned long rate, unsigned long *parent_rate)
{
	struct clk_pll_hw *pll_hw = to_pll_hw(hw);

	if (rate < pll_hw->min_rate)
		rate = pll_hw->min_rate;
	else if (rate > pll_hw->max_rate)
		rate = pll_hw->max_rate;

	return rate;
}

unsigned long clk_pll_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct clk_pll_hw *pll_hw = to_pll_hw(hw);

	return pll_rate_get(pll_hw);
}

static const struct clk_ops ax_clk_pll_ops = {
    .set_rate = clk_pll_set_rate,
    .round_rate = clk_pll_round_rate,
    .recalc_rate = clk_pll_recalc_rate,
};

static struct axera_clock_data *clk_pll_init(struct device_node *np, int nr_clks)
{
	int ret;
	void __iomem *base;
	struct clk **clk_table;
	struct axera_clock_data *clk_data;

	base = of_iomap(np, 0);
	if (!base) {
		CLK_PLL_ERR("failed to map clock registers\n");
		goto err;
	}

	clk_data = kzalloc(sizeof(*clk_data), GFP_KERNEL);
	if (!clk_data)
		goto err;

	clk_data->base = base;
	clk_table = kcalloc(nr_clks, sizeof(*clk_table), GFP_KERNEL);
	if (!clk_table)
		goto err_data;

	clk_data->clk_data.clks = clk_table;
	clk_data->clk_data.clk_num = nr_clks;
	ret = of_clk_add_provider(np, of_clk_src_onecell_get, &clk_data->clk_data);
	if (ret)
		goto err_data;

	return clk_data;

err_data:
	kfree(clk_data);
err:
	return NULL;
}

static void clk_pll_register(struct clk_pll_cfg *cfgs, int nums, struct axera_clock_data *data)
{
	int i;
	u32 div;
	struct clk_pll_cfg *cfg;
	struct clk *clk;
	struct clk_init_data init = {0,};
	struct clk_pll_hw *pll_hw;

	pll_hw = kzalloc(sizeof(*pll_hw) * nums, GFP_KERNEL);
	if (!pll_hw)
		return;

	for (i = 0; i < nums; i++) {
		cfg = &cfgs[i];

		init.name = cfg->name;
		init.flags = cfg->flags;
 		init.parent_names = &cfg->parent_name;
		init.num_parents = (cfg->parent_name ? 1 : 0);
		init.ops = &ax_clk_pll_ops;

		pll_hw[i].id = cfg->id;
		pll_hw[i].ref_rate = cfg->ref_rate;
		pll_hw[i].cur_ref_rate = cfg->ref_rate;

		div = cfg->div;
		if ((POST_DIV_GET(div) != DIV_AUTO) && (POST_DIV_GET(div) > POST_DIV_MAX)) {
			div &= (~(_POST_DIV_MASK));
			div |= POST_DIV(POST_DIV_MAX);
		}

		if ((PRE_DIV_GET(div) != DIV_AUTO) && (PRE_DIV_GET(div) > PRE_DIV_MAX)) {
			div &= (~(_PRE_DIV_MASK));
			div |= PRE_DIV(PRE_DIV_MAX);
		}

		if (POST_DIV_GET(div) == DIV_AUTO) {
			pll_hw[i].min_rate = PLL_FVCO_MIN_RATE >> POST_DIV_MAX;
			pll_hw[i].max_rate = PLL_FVCO_MAX_RATE >> POST_DIV_MIN;
		} else {
			pll_hw[i].min_rate = PLL_FVCO_MIN_RATE >> POST_DIV_GET(div);
			pll_hw[i].max_rate = PLL_FVCO_MAX_RATE >> POST_DIV_GET(div);
		}

		pll_hw[i].div = div;

		pll_hw[i].hw.init = &init;
		pll_hw[i].base = data->base;
		pll_hw[i].regs.re_open_reg = data->base + cfg->re_open_reg_offs;
		pll_hw[i].regs.re_open_bit = cfg->re_open_bit;
		pll_hw[i].regs.on_cfg_reg = data->base + cfg->on_cfg_reg_offs;
		pll_hw[i].regs.cfg0_reg = data->base + cfg->cfg0_reg_offs;
		pll_hw[i].regs.cfg1_reg = data->base + cfg->cfg1_reg_offs;
		pll_hw[i].regs.lock_sts_reg = data->base + cfg->lock_sts_reg_offs;
		pll_hw[i].regs.rdy_sts_reg = data->base + cfg->rdy_sts_reg_offs;
		pll_hw[i].regs.rdy_bit = cfg->rdy_bit;
		pll_hw[i].clk_nb.notifier_call = clk_pll_notifier_cb;

		spin_lock_init(&pll_hw[i].lock);

		INIT_LIST_HEAD(&pll_hw[i].associated_pll_list);

		clk = clk_register(NULL, &pll_hw[i].hw);
		if (IS_ERR(clk)) {
			CLK_PLL_ERR("failed to register clock for %s\n", cfg->name);
			continue;
		}

		if (clk_notifier_register(clk, &pll_hw[i].clk_nb))
			CLK_PLL_ERR("failed to register clock notifier for %s\n", cfg->name);

		clk_hw_set_rate_range(&pll_hw[i].hw, pll_hw[i].min_rate, pll_hw[i].max_rate);

		data->clk_data.clks[i] = clk;
	}
}

void ax650x_clk_pll_init(struct device_node *np)
{
	struct axera_clock_data *clk_data_top;

	clk_data_top = clk_pll_init(np, ARRAY_SIZE(ax650_pll_cfgs));
	if (!clk_data_top)
	    return;

	clk_pll_register(ax650_pll_cfgs, ARRAY_SIZE(ax650_pll_cfgs), clk_data_top);
}
EXPORT_SYMBOL(ax650x_clk_pll_init);

