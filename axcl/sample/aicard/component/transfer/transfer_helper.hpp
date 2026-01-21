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

#include <vector>
#include <list>
#include <map>

#include "ax_vdec_type.h"

#include "AXThread.hpp"
#include "AXRingBuffer.h"
#include "IStreamHandler.hpp"

typedef struct _TRANSFER_SEND_DATA {
    CAXRingBuffer* pRingBuffer {nullptr};
    AX_U8 nChannel;
    AX_U8 nFPS;
    _TRANSFER_SEND_DATA() {
        memset(this, 0, sizeof(_TRANSFER_SEND_DATA));
    }
    ~_TRANSFER_SEND_DATA() {
        if (pRingBuffer) {
            delete pRingBuffer;
            pRingBuffer = nullptr;
        }
    }
} TRANSFER_SEND_DATA_T;


class CTransferHelper : public IStreamObserver  {
public:
    CTransferHelper(AX_VOID) = default;
    virtual ~CTransferHelper(AX_VOID) = default;

    AX_BOOL Init(AX_S32 nDevID, AX_U32 nMaxHstVideoCount, AX_U32 nMaxDevVideoCount);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    /* static, only can register or unregister before Start */
    AX_BOOL RegObserver(AX_VDEC_GRP vdGrp, IStreamObserver *pObs);
    AX_BOOL UnRegObserver(AX_VDEC_GRP vdGrp, IStreamObserver *pObs);
    AX_BOOL UnRegAllObservers(AX_VOID);

    /* stream data callback */
    AX_BOOL OnRecvVideoData(AX_S32 nChannel, const AX_U8 *pData, AX_U32 nLen, AX_U64 nPTS) override;
    AX_BOOL OnRecvAudioData(AX_S32 nChannel, const AX_U8 *pData, AX_U32 nLen, AX_U64 nPTS) override;

protected:
    AX_VOID SendStreamThread(AX_VOID* pArg);
    AX_VOID ClearBuf(AX_VOID);

protected:
    AX_S32 m_nDevID{0};
    AX_U32 m_nMaxHstVideoCount{0};
    AX_U32 m_nMaxDevVideoCount{0};

    std::vector<TRANSFER_SEND_DATA_T> m_vecSendStream;
    std::vector<CAXThread*> m_vecThreadSendStream;
    std::map<AX_U32, std::tuple<AX_U32, AX_U32>> m_mapSendThreadParams;

    std::mutex m_mtxObs;
    std::map<AX_VDEC_GRP, std::list<IStreamObserver *>> m_mapObs;
};
