/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_RISCV_DRV__
#define __AX_RISCV_DRV__

#include "ax_global_type.h"

#define COMM_SYS_GLB        0x4200000   //aon_glb
#define COMM_SYS_DUMMY_SW11 (COMM_SYS_GLB + 0x1fc)

typedef enum {
    AX_RISCV_SUCCESS =              0,
    AX_RISCV_OPEN_FAIL =            0x80040001,
    AX_RISCV_MAP_FAIL =             0x80040002,
    AX_RISCV_PARA_FAIL =            0x80040003,
    AX_RISCV_READ_FAIL =            0x80040004,
    AX_RISCV_WRITE_FAIL =           0x80040005,
    AX_RISCV_SEND_CMD_FAIL =        0x80040006,
    AX_RISCV_QUERY_TIMEOUT_FAIL =   0x80040007,
    AX_RISCV_POWERON_FAIL =         0x80040008,
    AX_RISCV_BUSY =                 0x80040009,
    AX_RISCV_MAILBOX_TIMEOUT_FAIL = 0x8004000a,
    AX_RISCV_MAILBOX_RW_FAIL =      0x8004000b,
    AX_RISCV_NORESOURCE =           0x8004000c,
    AX_RISCV_RISCV_FB_ERROR =       0x8004000d,
    AX_RISCV_STATUS_ERROR =         0x8004000e,
    AX_RISCV_ERR_BUTT
} AX_RISCV_ERR_E;

typedef struct {
    AX_U8 id;
    AX_U8 data[31];
} mbox_msg_t; // message length is 32 bytes

AX_S32 ax_riscv_setup(AX_VOID);
AX_S32 ax_riscv_shutup(AX_VOID);
AX_S32 ax_riscv_send_message(mbox_msg_t *msg);
AX_S32 ax_riscv_recv_message(mbox_msg_t *msg);

#endif // __AX_RISCV_DRV__
