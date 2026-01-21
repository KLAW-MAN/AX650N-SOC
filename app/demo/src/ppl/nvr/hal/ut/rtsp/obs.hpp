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
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>
#include "istream.hpp"

class CRtspObserver : public IStreamObserver {
public:
    CRtspObserver(AX_U32 id, AX_U32 nDelay) : m_id(id), m_nDelay(nDelay){};

    AX_BOOL OnRecvStreamData(CONST STREAM_FRAME_T& stFrame) override {
        // printf("%lld: client %d recv %d bytes, nalu %d\n", us(), m_id, stFrame.frame.stVideo.nLen, stFrame.frame.stVideo.enNalu);

        if (m_nDelay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_nDelay));
        }

        return AX_TRUE;
    };

    AX_BOOL OnRecvStreamInfo(CONST STREAM_INFO_T& stInfo) override {
        // printf("client %d:\n", m_id);
        // printf("  url: %s\n", stInfo.strURL.c_str());
        printf("client %2d, video: payload %d, %d x %d, %dfps\n", m_id, stInfo.stVideo.enPayload, stInfo.stVideo.nWidth,
               stInfo.stVideo.nHeight, stInfo.stVideo.nFps);
        return AX_TRUE;
    };

    AX_VOID OnNotifyConnStatus(CONST AX_CHAR* pUrl, CONNECT_STATUS_E enStatus) override {
        /*
            switch (enStatus) {
                case DISCONNECT:
                    printf("client %d: disconnect\n", m_id);
                    break;
                case RECONNECT:
                    printf("client %d: reconnect\n", m_id);
                    break;
                case CONNECTED:
                    printf("client %d: connected\n", m_id);
                    break;
                default:
                    break;
            }
        */
    }

    AX_U64 us(AX_VOID) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
    }

private:
    AX_U32 m_id;
    AX_U32 m_nDelay;
};
