/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
#include "ax_ftc_extern.h"

AX_S32 ISP_DRV_IO_WRITE32(AX_VOID *reg_handle, AX_U32 offset, AX_U32 data)
{
    VIN_CHECK_HANDLE(reg_handle);

    return AX_REG32_WRITE(reg_handle + offset - 0x4200, data);
}

AX_U32 ISP_DRV_IO_READ32(AX_VOID *reg_handle, AX_U32 offset)
{
    AX_U32 value;

    VIN_CHECK_HANDLE(reg_handle);
    value = AX_REG32_READ(reg_handle + offset - 0x4200);

    return value;
}
AX_U32 ISP_DRV_SEQ_READ32(AX_VOID *reg_handle, AX_U32 offset)
{
    AX_U32 value;

    VIN_CHECK_HANDLE(reg_handle);

    value = AX_REG32_READ(reg_handle + offset);

    return value;
}

AX_U32 ISP_DRV_IO_RELOAD_READ32(AX_VOID *reg_handle, AX_U32 offset)
{
    VIN_CHECK_HANDLE(reg_handle);

    return AX_REG32_READ(reg_handle + offset);
}

AX_S32 ISP_DRV_IO_RELOAD_WRITE32(AX_VOID *reg_handle, AX_U32 offset, AX_U32 data)
{
    VIN_CHECK_HANDLE(reg_handle);

    return AX_REG32_WRITE(reg_handle + offset, data);
}

