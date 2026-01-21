/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __DEF_AX_FTC_PROCESS_H__
#define __DEF_AX_FTC_PROCESS_H__

//#include "ax_base_type.h"
#include "ax_global_type.h"
AX_S32 vin_ftc_node_set_data(AX_U32 cmd, AX_VOID *pdata);
AX_S32 vin_ftc_node_get_data(AX_U32 cmd, AX_VOID *pdata);

AX_S32 __vin_dev_ftc_node_trigger_event_find(vin_dev_ftc_event_info_t *ftc_event_info);
#endif //__DEF_NODE_FTC_PROCESS_H__
