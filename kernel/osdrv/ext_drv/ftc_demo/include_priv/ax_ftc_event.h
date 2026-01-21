/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_AX_FTC_EVENT_H__
#define __DEF_AX_FTC_EVENT_H__

//#include "ax_base_type.h"

#define SW_RELOAD_0_SET 0x3C
#define SW_RELOAD_0_CLR 0x40

AX_S32 vin_ftc_node_event_forward(AX_VOID);
AX_S32 vin_ftc_node_event_backward(AX_VOID);
#endif //__DEF_NODE_FTC_EVENT_H__
