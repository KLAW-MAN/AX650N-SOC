/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXCL_RT_CONTROL_H__
#define __AXCL_RT_CONTROL_H__

#include "axcl_rt_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Changesub vendor id and sub device id of current PCIe device
 * @note Only available for EP device with NOR flash as storage
 * @param sub_vendor_id EP sub vendor id
 * @param sub_device_id EP sub device id
 * @return axclError
 */
AXCL_EXPORT axclError axclrtControlChangePCIeSubId(uint32_t sub_vendor_id, uint32_t sub_device_id);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_RT_CONTROL_H__ */
