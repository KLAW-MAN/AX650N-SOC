/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
#ifndef __AX650X_LPDDR4X_H__
#define __AX650X_LPDDR4X_H__

#define DDR_NUM               2
#define DDR_2D_TRAINING       1
#define DDR_CLK_2133          2133
#define DDR_CLK_3200          3200
#define DDR_CLK_3733          3733
#define DDR_CLK_4266          4266
#define DDR_CFG_2133          0
#define DDR_CFG_3200          0
#define DDR_CFG_3733          0
#define DDR_CFG_4266          1
#define DDR_CFG_DFS           1
#define PHY_DEBUG_ENABLE      0
void ddr_init(void);
#endif
