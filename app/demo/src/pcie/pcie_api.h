/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef PCIE_API_H__
#define PCIE_API_H__

#include <string>

#include "ax_base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum _PCIE_CMD_TYPE_E {
    PCIE_CMD_HAND_SHAKE = 0,
    PCIE_CMD_SWITCH_AI_E,
    PCIE_CMD_AI_RESULT_E,
    PCIE_CMD_H264_DATA_E,
    PCIE_CMD_H265_DATA_E
} PCIE_CMD_TYPE_E;

typedef struct _PCIE_CMD_MSG_HEAD_T
{
    PCIE_CMD_TYPE_E nCmdType;
    AX_U32 nDataLen;
    AX_U32 nChannel;
    AX_U32 nSn;
    AX_U16 nCheckSum;
} PCIE_CMD_MSG_HEAD_T;

typedef struct _PCIE_CMD_MSG_T
{
    PCIE_CMD_MSG_HEAD_T stMsgHead;
    AX_U8 nMsgBody[1024*800];
} PCIE_CMD_MSG_T;

typedef enum _PCIE_ERROR_TYPE_E {
    PCIE_SUCCESS           = 0,
    PCIE_ERROR             = -1,
    PCIE_ERROR_TIMEOUT     = -2,
    PCIE_ERROR_CHECKSUM    = -3,
    PCIE_ERROR_REPEAT_DATA = -4,
} PCIE_ERROR_TYPE_E;

AX_S32 PCIe_Init(AX_BOOL bMaster, AX_U16 nTargetSlaveCnt, AX_U16 nChannelNum, AX_U32 nDmaBufferSize, AX_S16 nTraceData = 0, AX_S16 nRetryCount = 1);

/*
Parameter nTimeout:
Greater than 0: Timeout duration, unit: milliseconds;
-1: Block and wait, will wait indefinitely until an event occurs;
0: Do not block, return immediately.

Return values:
0 (Success),
-1 (Failure),
-2 (Timeout),
-3 (Peer data verification failed)
*/
AX_S32 PCIe_Send(PCIE_CMD_TYPE_E nCmdType, AX_S16 nDevice, AX_S16 nChannel,
                 AX_U8* pDataBuf, AX_U32 nSize, AX_S32 nTimeout = -1);

/*
Parameter nTimeout:
Greater than 0: Timeout duration, unit: milliseconds;
-1: Block and wait, will wait indefinitely until an event occurs;
0: Do not block, return immediately.

Return values:
0 (Success),
-1 (Failure),
-2 (Timeout),
-3 (Peer data verification failed)
*/
AX_S32 PCIe_Recv(PCIE_CMD_TYPE_E nCmdType, AX_S16 nDevice, AX_S16 nChannel,
                 AX_U8* pDataBuf, AX_U32 nSize, AX_S32 nTimeout = -1);

AX_S32 PCIe_DeInit();


#ifdef __cplusplus
}
#endif

#endif