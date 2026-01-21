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

#include "ax_global_type.h"

#define RTP_VERSION		2
#define RTP_MAXPAYLOAD  1400

#ifdef __LATENCY_DEBUG__
#define __SEND_LATENCY_DEBUG_DATA__
#endif

#pragma pack(4)
typedef struct {
    AX_S32 elapsed[5];
    AX_U64 timestamp; /* RTP send timestamp */
} RtpLatency_T;

typedef struct {
	AX_U8  v_p_x_cc;
    AX_U8  payload;
	AX_U16 seq_num;
	AX_U32 timestamp;
	AX_U32 ssrc;
    AX_U64 u64SeqNum;

#ifdef __SEND_LATENCY_DEBUG_DATA__
    RtpLatency_T latency;
#endif

} RTP_HEADER;
#pragma pack()

#define AX_RTP_HEADER_LEN sizeof(RTP_HEADER)

typedef struct _AX_MEDIA_DATA_T {
    AX_U8   *mediaData;
    AX_S32   mediaDataLen;
    AX_U32   timestamp;
    AX_S32   type;
    AX_U64   u64SeqNum;
    AX_U64   u64UserData;
} AX_MEDIA_DATA_T;


