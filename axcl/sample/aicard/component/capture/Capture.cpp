/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "Capture.hpp"
#include "AppLog.hpp"
#include "ElapsedTimer.hpp"

#include "axcl_sys.h"
#include "axcl_venc.h"
#include "axcl_rt_context.h"
#include "axcl_rt_memory.h"

#define QP_LEVEL       (90)
#define MAX_PIC_PATH   (260)

#define CAPTURE        "Capture"
#define TAG            "CapThread"
#define SNAP_ROOT_PATH "/opt/data/axcl_demo/snapshot"

using namespace std;

namespace {
using AXLockGuard = std::unique_lock<std::mutex>;
constexpr AX_U8 kReceiveWaitTimeoutSeconds = 2;
constexpr AX_S8 JPEG_ENCODE_ONCE_NAME[] = "JENC_ONCE";

static AX_S32 MallocJpegOutBuffer(AX_JPEG_ENCODE_ONCE_PARAMS_T* pStJpegEncodeOnceParam, AX_U32 frameSize) {
    AX_U64 phyBuff = 0;
    AX_VOID* virBuff = NULL;
    AX_S32 s32Ret = AX_SUCCESS;

    s32Ret = AXCL_SYS_MemAlloc(&phyBuff, &virBuff, frameSize, 0, (AX_S8 *)JPEG_ENCODE_ONCE_NAME);
    if (s32Ret) {
        LOG_MM_E(CAPTURE, "alloc mem err, size(%d).\n", frameSize);
        return -1;
    }

    pStJpegEncodeOnceParam->u32Len = frameSize;
    pStJpegEncodeOnceParam->ulPhyAddr = phyBuff;
    pStJpegEncodeOnceParam->pu8Addr = (AX_U8 *)virBuff;

    return AX_SUCCESS;
}

}  // namespace


AX_BOOL CCapture::SendFrame(AX_U32 nGrp, CAXFrame* axFrame) {
    if (!m_bCapture[nGrp] || m_nCaptureGrp[nGrp] != nGrp) {
        return AX_TRUE;
    }

    {
        AXLockGuard lck(m_mutexStat[nGrp]);
        m_bCapture[nGrp] = AX_FALSE;
        m_pAXFrame[nGrp] = axFrame;
    }

    AXLockGuard lck(m_mutexCapture[nGrp]);
    m_cvCapture[nGrp].notify_one();

    return AX_TRUE;
}

AX_VOID CCapture::CapturePictureThread(AX_VOID* pArg) {
    axclrtContext context;
    if (axclError ret = axclrtCreateContext(&context, m_nDevID); AXCL_SUCC != ret) {
        return;
    }

    std::tuple<AX_U32, AX_U32>* pParams = (std::tuple<AX_U32, AX_U32>*)pArg;
    AX_U32 nGrp = std::get<0>(*pParams);
    AX_U32 nCookie = std::get<1>(*pParams);
    CAXThread* pThread = m_vecThreadCapture[nGrp];

    LOG_MM_C(TAG, "[%d][%d] +++", nGrp, nCookie);

    while(pThread->IsRunning()) {
        {
            AXLockGuard lck(m_mutexStat[nGrp]);
            m_bCapture[nGrp] = AX_TRUE;
            m_nCaptureGrp[nGrp] = nGrp;
            m_pAXFrame[nGrp] = nullptr;
        }

        if (!pThread->IsRunning()) {
            break;
        }

        AXLockGuard lck(m_mutexCapture[nGrp]);
        m_cvCapture[nGrp].wait(lck);

        if (nullptr == m_pAXFrame[nGrp]) {
            continue;
        }

        AX_JPEG_ENCODE_ONCE_PARAMS_T stJpegEncodeOnceParam;
        memset(&stJpegEncodeOnceParam, 0, sizeof(stJpegEncodeOnceParam));
        const AX_VIDEO_FRAME_INFO_T& stVFrame = m_pAXFrame[nGrp]->stFrame.stVFrame;
        for (AX_U8 i = 0; i < 3; ++i) {
            stJpegEncodeOnceParam.u64PhyAddr[i] = stVFrame.stVFrame.u64PhyAddr[i];
            stJpegEncodeOnceParam.u32PicStride[i] = stVFrame.stVFrame.u32PicStride[i];
            stJpegEncodeOnceParam.u32HeaderSize[i] = stVFrame.stVFrame.u32HeaderSize[i];
        }

        stJpegEncodeOnceParam.stJpegParam.u32Qfactor = QP_LEVEL;
        stJpegEncodeOnceParam.u32Width = stVFrame.stVFrame.u32Width;
        stJpegEncodeOnceParam.u32Height = stVFrame.stVFrame.u32Height;
        stJpegEncodeOnceParam.enImgFormat = stVFrame.stVFrame.enImgFormat;
        stJpegEncodeOnceParam.stCompressInfo = stVFrame.stVFrame.stCompressInfo;

        AX_S32 s32Ret = MallocJpegOutBuffer(&stJpegEncodeOnceParam, stVFrame.stVFrame.u32FrameSize);
        if (AX_SUCCESS == s32Ret) {
            s32Ret = AXCL_VENC_JpegEncodeOneFrame(&stJpegEncodeOnceParam);
            if (AX_SUCCESS == s32Ret) {
                AX_VOID *host_mem = nullptr;
                axclrtMallocHost(&host_mem, stJpegEncodeOnceParam.u32Len);
                axclrtMemcpy(host_mem, (void*)stJpegEncodeOnceParam.ulPhyAddr, stJpegEncodeOnceParam.u32Len, AXCL_MEMCPY_DEVICE_TO_HOST);
                SaveSnapshotPic(m_pAXFrame[nGrp], nGrp, (AX_U8*)host_mem, stJpegEncodeOnceParam.u32Len);
                axclrtFreeHost(host_mem);
            } else {
                LOG_MM_E(TAG, "[%d] AXCL_VENC_JpegEncodeOneFrame failed[0x%x]", nGrp, s32Ret);
            }
            AXCL_SYS_MemFree(stJpegEncodeOnceParam.ulPhyAddr, stJpegEncodeOnceParam.pu8Addr);
        }
        ResetCaptureStatus(nGrp);
    }

    axclrtDestroyContext(context);
    LOG_MM_C(TAG, "[%d][%d] ---", nGrp, nCookie);
}

AX_BOOL CCapture::Init(AX_S32 nDevID, const CAPTURE_ATTR_T& stAttr) {
    LOG_MM_C(CAPTURE, "+++");

    m_nDevID = nDevID;
    m_nMaxDevVideoCount = stAttr.nGrpCount;
    m_nSkipFrm = stAttr.nSkipFrm;

    InitData();

    for (AX_U8 i = 0; i < stAttr.nGrpCount; i++) {
        m_vecThreadCapture.push_back(new CAXThread());
        m_mapCaptureThreadParams[i] = make_tuple(i, i);
    }

    MakeSnapshotDir();

    LOG_MM_C(CAPTURE, "---");
    return AX_TRUE;
}

AX_BOOL CCapture::DeInit(AX_VOID) {
    LOG_MM_C(CAPTURE, "+++");

    InitData();

    LOG_MM_C(CAPTURE, "---");
    return AX_TRUE;
}

AX_VOID CCapture::ResetCaptureStatus(AX_U32 nIndex) {
    AXLockGuard lck(m_mutexStat[nIndex]);
    m_bCapture[nIndex] = AX_FALSE;
    m_nCaptureGrp[nIndex] = 0;
    if (m_pAXFrame[nIndex]) {
        m_pAXFrame[nIndex]->FreeMem();
        m_pAXFrame[nIndex] = nullptr;
    }
}

AX_VOID CCapture::InitData(AX_VOID) {
    for (auto& m : m_vecThreadCapture) {
        if (m->IsRunning()) {
            LOG_MM_E(CAPTURE, "Capture thread is still running");
        }
        delete(m);
        m = nullptr;
    }
    m_vecThreadCapture.clear();
    m_mapCaptureThreadParams.clear();

    for (AX_S32 i = 0;i < MAX_CAPTURE_GRP;i++) {
        ResetCaptureStatus(i);
    }
}

AX_BOOL CCapture::Start(AX_VOID) {
    LOG_MM_C(TAG, "+++");

    for (AX_U8 i = 0; i < m_nMaxDevVideoCount; i++) {
        if (!m_vecThreadCapture[i]->Start([this](AX_VOID* pArg) -> AX_VOID { CapturePictureThread(pArg); }, &m_mapCaptureThreadParams[i], "DevCapPic")) {
            LOG_MM_E(TAG, "[%d] Create capture thread fail.", i);
            return AX_FALSE;
        }
    }

    LOG_MM_C(TAG, "---");
    return AX_TRUE;
}

AX_BOOL CCapture::Stop(AX_VOID) {
    LOG_MM_C(TAG, "+++");

    for (auto& m : m_vecThreadCapture) {
        m->Stop();
    }

    for (AX_U32 i = 0;i < m_vecThreadCapture.size();i++) {
        SendFrame(i, nullptr);
        m_vecThreadCapture[i]->Join();
    }

    LOG_MM_C(TAG, "---");
    return AX_TRUE;
}

AX_VOID CCapture::MakeSnapshotDir(AX_VOID) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    memset(m_szTime, 0x0, sizeof(m_szTime));
    sprintf(m_szTime, "%04d-%02d-%02d_%02d:%02d:%02d",
            timeinfo->tm_year + 1900,
            timeinfo->tm_mon + 1,
            timeinfo->tm_mday,
            timeinfo->tm_hour,
            timeinfo->tm_min,
            timeinfo->tm_sec);
    m_szTime[strlen(m_szTime)] = '\0';

    AX_CHAR szPath[MAX_PIC_PATH];
    sprintf(szPath, "%s/%s", SNAP_ROOT_PATH, m_szTime);

    DIR *dir;
    if ((dir = opendir(szPath)) == nullptr) {
        mkdir(szPath, S_IRWXU);
    }
    else {
        closedir(dir);
    }
}

AX_VOID CCapture::SaveSnapshotPic(CAXFrame* axFrame, AX_U32 nGrp, AX_U8 *pu8Addr, AX_U32 u32Len) {
    AX_CHAR szPath[MAX_PIC_PATH];
    sprintf(szPath, "%s/%s/grp_%d_%dx%d_seq_%lld.jpg",
            SNAP_ROOT_PATH,
            m_szTime,
            nGrp,
            axFrame->stFrame.stVFrame.stVFrame.u32Width,
            axFrame->stFrame.stVFrame.stVFrame.u32Height,
            axFrame->stFrame.stVFrame.stVFrame.u64SeqNum);
    std::string filePath = szPath;

    FILE *fp = fopen(filePath.c_str(), "w");
    if (!fp) {
        LOG_MM_E(TAG, "open snapshot file %s fail\n", filePath.c_str());
    }
    else {
        size_t ws = fwrite(pu8Addr, 1, u32Len, fp);
        // LOG_MM_C(TAG, "[%d] SaveSnapshotPic err[%d]", nGrp, errno);

        // AX_S32 ret1 = ferror(fp);
        // AX_S32 ret2 = feof(fp);
        // LOG_MM_C(TAG, "[%d] SaveSnapshotPic fwrite[%d], ferror[%d], feof[%d]", nGrp, ws, ret1, ret2);

        fflush(fp);
        fclose(fp);
        fp = nullptr;

        LOG_MM_C(TAG, "[%d] SaveSnapshotPic succ, len: %d, write: %d, path: %s", nGrp, u32Len, ws, filePath.c_str());
    }
}
