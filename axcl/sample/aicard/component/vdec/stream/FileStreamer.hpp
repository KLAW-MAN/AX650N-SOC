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
#include "AXThread.hpp"
#include "IStreamHandler.hpp"
#include "nalu_lock_fifo.hpp"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavcodec/bsf.h" /* ffmpeg 7.1 */
}

class CFileStreamer : public CStreamBaseHander {
public:
    CFileStreamer(AX_VOID) = default;
    virtual ~CFileStreamer(AX_VOID) = default;

    AX_BOOL Init(const STREAMER_ATTR_T &stAttr) override;
    AX_BOOL DeInit(AX_VOID) override;

    AX_BOOL Start(AX_VOID) override;
    AX_BOOL Stop(AX_VOID) override;

protected:
    AX_VOID DemuxThread(AX_VOID *pArg);
    AX_VOID DispatchThread(AX_VOID* pArg);

private:
    AX_S32 m_nDevID{0};
    AX_S32 m_nVideoIndex{-1};
    AX_BOOL m_bLoop{AX_FALSE};
    AX_BOOL m_bSyncObs{AX_FALSE};
    AVFormatContext *m_pAvFmtCtx{nullptr};
    AVBSFContext *m_pAvBSFCtx{nullptr};
    AVPacket *m_pAvPkt{nullptr};
    CAXThread m_DemuxThread;
    AX_U32 m_nMaxSendNaluIntervalMilliseconds{0};
    AX_U32 m_nForceFps{0};
    AX_U32 m_nFrmDelay{0};
    CAXThread m_DispatchThread;
    /* dispatch fifo */
    nalu_lock_fifo *m_fifo{nullptr};
    AX_BOOL m_bFrameRateControl{AX_FALSE};
};
