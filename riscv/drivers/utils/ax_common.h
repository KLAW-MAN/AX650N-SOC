/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_COMMON_H__
#define __AX_COMMON_H__

#include <stdint.h>
#include "drv_timer64.h"
#include "ax_log.h"

#define AX_BIT(n)   (1 << n)

typedef struct {
    uint32_t reg_val;
    uint32_t reg_addr;
} reg_info_t;

#define ax_udelay t64_udelay
#define ax_mdelay t64_mdelay

#define ax_readl(addr)           (*(volatile unsigned int *)(addr))
#define ax_writel(value, addr)   (*(volatile unsigned int *)(addr) = (value))
#define ax_readw(addr)           (*(volatile unsigned short *)(addr))
#define ax_writew(value, addr)   (*(volatile unsigned short *)(addr) = (value))

#endif //__AX_COMMON_H__
