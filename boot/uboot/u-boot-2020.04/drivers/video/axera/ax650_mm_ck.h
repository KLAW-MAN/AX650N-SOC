/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX650_MM_CK_H__
#define __AX650_MM_CK_H__

void ax650_dpu_reset(u32 id);
void ax650_dsi_reset(u32 id);
void ax650_hdmi_reset(u32 id);
int ax650_pixel_clk_set(u32 id, u32 clk);

#endif
