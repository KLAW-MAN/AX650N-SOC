/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC500AI_H__
#define __SC500AI_H__

#include "ax_base_type.h"
#include "ax_sensor_struct.h"
#include "ax_module_version.h"

#define SC500AI_SLAVE_ADDR       (0x30) /**< i2c slave address of the sc500ai camera sensor */
#define SC500AI_SENSOR_ID        (0xce1f)
#define SC500AI_ADDR_BYTE        (2)
#define SC500AI_DATA_BYTE        (1)
#define SC500AI_SWAP_BYTES       (1)

extern SNS_STATE_OBJ *gSc500aiCtx[AX_VIN_MAX_PIPE_NUM];

#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = gSc500aiCtx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (gSc500aiCtx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (gSc500aiCtx[dev] = AX_NULL)

AX_S32 sc500ai_get_bus_num(ISP_PIPE_ID nPipeId);

#endif  //end __SC500AI_H__
