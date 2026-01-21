/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __SC850SL_H__
#define __SC850SL_H__

#include "ax_base_type.h"
#include "isp_sensor_internal.h"

#define SC850SL_SLAVE_ADDR          (0x30)
#define SC850SL_SENSOR_ID           (0x9d1e)
#define SC850SL_ADDR_BYTE           (2)
#define SC850SL_DATA_BYTE           (1)
#define SC850SL_SWAP_BYTES          (1)

extern SNS_STATE_OBJ *gSc850slCtx[AX_VIN_MAX_PIPE_NUM];

#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = gSc850slCtx[dev])
#define SENSOR_SET_CTX(dev, pstCtx) (gSc850slCtx[dev] = pstCtx)
#define SENSOR_RESET_CTX(dev) (gSc850slCtx[dev] = NULL)

AX_S32 sc850sl_get_bus_num(ISP_PIPE_ID nPipeId);


#endif  //end __SC850SL_H__
