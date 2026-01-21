/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __LOG_2_MEM_H__
#define __LOG_2_MEM_H__

#include <stdint.h>

void log_2_mem_store(uint8_t *log_buff, rt_size_t log_len);
int log_2_mem_init(void);

#endif // __LOG_2_MEM_H__
