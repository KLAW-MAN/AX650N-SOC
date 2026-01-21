/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _DW_MIPI_DSI_H
#define _DW_MIPI_DSI_H

#include <mipi_dsi.h>

struct dw_mipi_dsi_dphy_timing {
	u16 data_hs2lp;
	u16 data_lp2hs;
	u16 clk_hs2lp;
	u16 clk_lp2hs;
};

struct dw_mipi_dsi {
	struct mipi_dsi_host dsi_host;
	struct mipi_dsi_device *device;
	void __iomem *base;
	unsigned int lane_mbps; /* per lane */
	u32 channel;
	unsigned int max_data_lanes;
	const struct mipi_dsi_phy_ops *phy_ops;
	struct dw_mipi_dsi_dphy_timing dphy_timing;
};

int dw_mipi_dsi_init(struct dw_mipi_dsi *dsi, struct mipi_dsi_device *device,
			  struct display_timing *timings, struct display_timing *adjusted_timings,
			  unsigned max_data_lanes, const struct mipi_dsi_phy_ops *phy_ops);
int dw_mipi_dsi_enable(struct dw_mipi_dsi *dsi);

#endif /* _DW_MIPI_DSI_H */