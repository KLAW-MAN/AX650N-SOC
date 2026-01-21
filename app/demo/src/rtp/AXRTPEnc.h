/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#include <netinet/in.h>
#include <pthread.h>
#include "AXRTPDefine.h"

typedef struct _AX_MEDIA_QUEUE_NODE_T
{
    AX_MEDIA_DATA_T stMediaData;
    struct _AX_MEDIA_QUEUE_NODE_T *next;
}AX_MEDIA_QUEUE_NODE_T;

typedef struct _AX_MEDIA_QUEUE_T
{
    AX_MEDIA_QUEUE_NODE_T *front;
    AX_MEDIA_QUEUE_NODE_T *rear;
}AX_MEDIA_QUEUE_T;

typedef void (*RTP_PREP_CALLBACK)(const AX_MEDIA_DATA_T &mediaData, AX_U8 *rtpBuf, AX_U32 bufLen);
typedef void (*RTP_SENT_CALLBACK)(const AX_MEDIA_DATA_T &mediaData);

typedef struct _AX_RTP_ENC_T {
    AX_CHAR ip[24];
    AX_U32 sockPort;

    AX_S32 nSockfd;
    struct sockaddr_in deskaddr;
    AX_S32 nSeqNum;

    AX_MEDIA_QUEUE_T stMediaQueue;
    pthread_mutex_t mutexMediaQueue;
    pthread_cond_t  condMediaQueue;
    AX_S32 nCondShareVariable;
    pthread_t mediaDataPid;
    AX_S32 nThreadStart;

    RTP_PREP_CALLBACK cbPrep;
    RTP_SENT_CALLBACK cbSent;

} AX_RTP_ENC_T;

AX_S32 AX_RTPENC_Init(AX_RTP_ENC_T *pRtpEnc, const AX_CHAR *ipAddr, AX_U32 ipPort, RTP_PREP_CALLBACK cbPrep, RTP_SENT_CALLBACK cbSent);
AX_S32 AX_RTPENC_DeInit(AX_RTP_ENC_T *pRtpEnc);
AX_S32 AX_RTPENC_Proc(AX_RTP_ENC_T *pRtpEnc, const AX_U8 *inBuf, AX_S32 inLen, AX_S32 nType, AX_U64 u64PTS, AX_U64 u64SeqNum, AX_U64 nUserData);