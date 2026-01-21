/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_DW_HDMI_H__
#define __AX_DW_HDMI_H__

int ax_dw_hdmi_init(u32 devid, struct display_timing *timing, struct display_timing *adjusted_timing, void *data);
int ax_dw_hdmi_read_edid(u32 devid, u8 *buf, int buf_size);
int ax_dw_hdmi_enable(u32 devid, const struct display_timing *edid);

#endif

