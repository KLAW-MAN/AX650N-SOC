/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <drm/bridge/dw_hdmi.h>

#ifndef __AX_HDMI_H__
#define __AX_HDMI_H__

#define AX_HDMI_TX_PHY_CFG_MAX  32
#define AX_HDMI_TX_PHY_RES_MAX	16

struct phy_reg {
	u16 offs;
	u16 data;
};

struct ax_dw_hdmi_phy_cfg {
	unsigned long pixelclock;

	struct phy_reg phy_regs[AX_HDMI_TX_PHY_RES_MAX];
};

struct ax_dw_hdmi {
	u32 id;

	struct drm_encoder encoder;

	struct dw_hdmi_plat_data plat_data;

	struct ax_dw_hdmi_phy_cfg phy_cfgs[AX_HDMI_TX_PHY_CFG_MAX];

	struct dw_hdmi *hdmi;

	struct reset_control *rst_ctrl;
	struct reset_control *prst_ctrl;

	struct clk *video_clk;
	struct clk *apb_clk;
	struct clk *i32k_clk;
	struct clk *mm_isfr_clk;
};

#define to_ax_hdmi(x)	container_of(x, struct ax_dw_hdmi, x)
#define HDMI_DEV_MAX	2

int ax_hdmi_add_default_modes(struct drm_connector *connector);
ssize_t ax_hdmi_phy_reg_store(void *priv_data, const char *buf, size_t count);
ssize_t ax_hdmi_phy_reg_show(void *priv_data, char *buf);

#endif /* __AX_HDMI_H__ */
