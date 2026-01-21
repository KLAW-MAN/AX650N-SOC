/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "AXNVRPlaybackCtrl.h"
#include "AppLogApi.h"

#define TAG "PLAYBACK"

AX_VOID CAXNVRPlaybakCtrl::Init(const AX_NVR_FILE_MGR_ATTR_T &stAttr) {
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);

    if (m_bInit) {
        return;
    }

    m_bInit = AX_TRUE;
    m_stAttr = stAttr;
    for (AX_U32 nDevID = 0; nDevID < m_stAttr.nMaxCount; ++nDevID) {
        AX_NVR_FILE_T stDevice;
        stDevice.pPlaybackChn = this->createPlaybackChannel(nDevID);
        stDevice.nChannelId = nDevID;
        m_mapDevice.emplace(nDevID, stDevice);
    }

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
}

AX_VOID CAXNVRPlaybakCtrl::DeInit() {
    LOG_MM_I(TAG, "+++");
    std::lock_guard<std::mutex> lock(mutex_);

    if (!m_bInit) return;

    m_bInit = AX_FALSE;

    /* If resolution changed in SETTING page, DISP is reset and operations on DISP should be ignored. */
    AX_BOOL bDispValid = m_stAttr.pPrimary->GetAttr().bIfSyncReset ? AX_FALSE : AX_TRUE;

    for (auto& it : m_mapDevice) {
        // const auto& nDevID = it.first;
        auto& stDevice = it.second;
        if (stDevice.pPlaybackChn) {
            stDevice.pPlaybackChn->StopDisp(bDispValid);
            stDevice.pPlaybackChn->StopFile();
            delete stDevice.pPlaybackChn;
            stDevice.pPlaybackChn = nullptr;
        }
    }

    LOG_MM_I(TAG, "---");
}

AX_VOID CAXNVRPlaybakCtrl::SetAttr(const AX_NVR_FILE_MGR_ATTR_T &stAttr) {
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);
    m_stAttr = stAttr;
    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
}


std::vector<AX_NVR_DEV_ID> CAXNVRPlaybakCtrl::GetDeviceFiles(AX_VOID) {
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);

    std::vector<AX_NVR_DEV_ID> vecDevID;
    LOG_M_D(TAG, "GetAllDeviceInfo path=%s", __func__, __LINE__, m_stAttr.strPath.c_str());
    std::vector<AXDS_DEVICE_INFO_T> vecDeviceInfo = CDataStreamPlay::GetAllDeviceInfo(m_stAttr.strPath);
    for (auto &it: vecDeviceInfo) {
        if (!it.bValid) continue;

        auto device = m_mapDevice.find(it.nDeviceID);
        if (device == m_mapDevice.end()) {
            LOG_M_E(TAG, "[%s][%d] Playback Channel %d not found", __func__, __LINE__, it.nDeviceID);
            continue;
        }

        auto& stDevice = device->second;
        stDevice.stDeviceInfo = it;
        if (stDevice.stDeviceInfo.bValid) {
            LOG_M_D(TAG, "valid dev=%d", __func__, __LINE__, stDevice.stDeviceInfo.nDeviceID);
            vecDevID.emplace_back(stDevice.stDeviceInfo.nDeviceID);
        }
    }
    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return vecDevID;
}

std::vector<AXDS_VIDEO_INFO_T> CAXNVRPlaybakCtrl::GetDeviceFileInfo(AX_NVR_DEV_ID nDeviceID, const string &strDate) {
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);

    std::vector<AXDS_VIDEO_INFO_T> vecDevicePeriod;

    CVideoInfoMap mapTimePeriodInfo = CDataStreamPlay::GetVideoInfo(m_stAttr.strPath, strDate);
    auto dev = mapTimePeriodInfo.find(nDeviceID);
    auto device = m_mapDevice.find(nDeviceID);
    if (dev == mapTimePeriodInfo.end() || device == m_mapDevice.end()) {
        LOG_M_E(TAG, "[%s][%d] Playback Channel %d not found", __func__, __LINE__, nDeviceID);
        return vecDevicePeriod;
    }
    auto& stDevice = device->second;
    if (stDevice.enPlayChnIdx == AX_NVR_CHN_IDX_TYPE::MAIN && dev->second.size() >= 1) {
        return dev->second[0];
    }
    else if (stDevice.enPlayChnIdx == AX_NVR_CHN_IDX_TYPE::SUB1 && dev->second.size() >= 2) {
        return dev->second[1];
    }
    else {
        LOG_M_E(TAG, "[%s][%d] not found device and stream");
    }

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return vecDevicePeriod;
}

AX_BOOL CAXNVRPlaybakCtrl::Start(const ax_nvr_channel_vector &vecChn, AX_U32 nDate, AX_U32 nTime, AX_U32 nTick, AX_BOOL bReverse, AX_BOOL bRepeat) {
    LOG_MM_I(TAG, "+++ ");
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_FALSE;

    do {
        if (m_stAttr.pPrimary == nullptr) {
            LOG_M_E(TAG, "[%s][%d]pPrimary null pointer", __func__, __LINE__);
            break;
        }

        if (vecChn.size() == 0) {
            LOG_M_E(TAG, "[%s][%d]vecChn.size invalid", __func__, __LINE__);
            break;
        }

        const CVO *pVo = m_stAttr.pPrimary->GetVo();
        if (pVo == nullptr) {
            LOG_M_E(TAG, "[%s][%d]pVo null pointer", __func__, __LINE__);
            break;
        }

        VO_ATTR_T attr = pVo->GetAttr();
        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDevID = get<1>(chn);
            if (nDevID == -1) continue;

            VO_CHN nVoChn = get<0>(chn) % attr.stChnInfo.nCount;
            auto it = m_mapDevice.find(nDevID);
            if (it == m_mapDevice.end()) {
                LOG_M_E(TAG, "[%s][%d]Playback Channel not found", __func__, __LINE__);
                continue;
            }

            const auto& stDevice = it->second;
            if (!stDevice.pPlaybackChn) {
                LOG_M_E(TAG, "[%s][%d]Playback Channel invalid", __func__, __LINE__);
                continue;
            }

            if (nDate == 0) { // In case directly change direction between forward and backward, starts with the vo displayed time
                std::tuple<AX_U32, AX_U32, AX_U32> displayedTime = stDevice.pPlaybackChn->GetCurrDisplayedTime();
                nDate = std::get<0>(displayedTime);
                nTime = std::get<1>(displayedTime);
                nTick = std::get<2>(displayedTime);

                LOG_MM_C(TAG, "Change direction at time %d %d %d of displayed.", nDate, nTime, nTick);
            }

            if (!stDevice.pPlaybackChn->StartFile(nDate, nTime, nTick, bReverse, bRepeat)) {
                LOG_M_E(TAG, "[%s][%d]StartFile failed", __func__, __LINE__);
                bbRet = AX_FALSE;
                continue;
            }

            if (!stDevice.pPlaybackChn->StartDisp(attr.voLayer, nVoChn, attr.stChnInfo.arrChns[nVoChn].stRect)) {
                LOG_M_E(TAG, "[%s][%d]StartDisp failed", __func__, __LINE__);
                bbRet = AX_FALSE;
                continue;
            }
        }

        m_stAttr.bReverse = bReverse;
        m_stAttr.bRepeat = bRepeat;

        bRet = bbRet;
    } while(0);

    LOG_MM_I(TAG, "---");
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::StopAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_TRUE;
    do {
        if (m_stAttr.pPrimary == nullptr) {
            break;
        }

        /* If resolution changed in SETTING page, DISP is reset and operations on DISP should be ignored. */
        AX_BOOL bDispValid = m_stAttr.pPrimary->GetAttr().bIfSyncReset ? AX_FALSE : AX_TRUE;
        for (const auto& it : m_mapDevice) {
            const auto& stDevice = it.second;
            if (stDevice.pPlaybackChn == nullptr) continue;

            stDevice.pPlaybackChn->StopDisp(bDispValid);
            stDevice.pPlaybackChn->StopFile();
        }

    } while(0);

    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::PreStopVo() {
    AX_BOOL bRet = AX_TRUE;
    do {
        for (const auto& it : m_mapDevice) {
            const auto& stDevice = it.second;
            if (stDevice.pPlaybackChn == nullptr) continue;

            stDevice.pPlaybackChn->PreStopVo();
        }

    } while(0);

    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::ContinueVo() {
    AX_BOOL bRet = AX_TRUE;
    do {
        for (const auto& it : m_mapDevice) {
            const auto& stDevice = it.second;
            if (stDevice.pPlaybackChn == nullptr) continue;

            stDevice.pPlaybackChn->ContinueVo();
        }

    } while(0);

    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::Stop(const ax_nvr_channel_vector &vecChn) {
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_FALSE;
    do {
        if (m_stAttr.pPrimary == nullptr) {
            break;
        }

        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDevID = get<1>(chn);
            if (nDevID == -1) continue;

            auto it = m_mapDevice.find(nDevID);
            if (it == m_mapDevice.end()) {
                break;
            }

            const auto& stDevice = it->second;
            if (stDevice.pPlaybackChn == nullptr) continue;

            stDevice.pPlaybackChn->StopDisp();
            stDevice.pPlaybackChn->StopFile();
        }
        bRet = bbRet;
    } while(0);
    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::Update(const ax_nvr_channel_vector &vecChn) {
    AX_BOOL bRet = AX_FALSE;
    LOG_MM_I(TAG, "+++");
    std::lock_guard<std::mutex> lock(mutex_);

    do {
        if (m_stAttr.pPrimary == nullptr) {
            break;
        }

        const CVO *pVo = m_stAttr.pPrimary->GetVo();
        if (pVo == nullptr) {
            break;
        }
        VO_ATTR_T attr = pVo->GetAttr();

        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDevID = get<1>(chn);
            if (nDevID == -1) {
                continue;
            }

            VO_CHN nVoChn = get<0>(chn) % attr.stChnInfo.nCount;
            auto it = m_mapDevice.find(nDevID);
            if (it == m_mapDevice.end()) {
                bbRet = AX_FALSE;
                break;
            }

            const auto& stDevice = it->second;
            if (stDevice.pPlaybackChn == nullptr) continue;
            if (!stDevice.pPlaybackChn->UpdateRect(attr.stChnInfo.arrChns[nVoChn].stRect)) {
                bbRet = AX_FALSE;
                break;
            }
        }

        bRet = bbRet;

    } while(0);

    LOG_MM_I(TAG, "---");
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::PauseResume(const ax_nvr_channel_vector &vecChn, AX_BOOL bForceResume) {
    LOG_M_I(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_FALSE;
    do {
        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDeviceID = get<1>(chn);
            auto it = m_mapDevice.find(nDeviceID);
            if (it == m_mapDevice.end()) {
                bRet = AX_TRUE; /* Not playbacked channel would be treated normal as well */
                break;
            }

            const auto& stDevice = it->second;
            if (stDevice.pPlaybackChn == nullptr) continue;
            if (!stDevice.pPlaybackChn->PauseResume(bForceResume)) {
                bbRet = AX_FALSE;
                break;
            }
            bRet = bbRet;
        }
    } while(0);
    LOG_M_I(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::RestorePauseState(const ax_nvr_channel_vector &vecChn) {
    LOG_M_I(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_FALSE;
    do {
        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDeviceID = get<1>(chn);
            auto it = m_mapDevice.find(nDeviceID);
            if (it == m_mapDevice.end()) {
                bRet = AX_TRUE; /* Not playbacked channel would be treated normal as well */
                break;
            }

            const auto& stDevice = it->second;

            if (stDevice.pPlaybackChn == nullptr) continue;
            if (!stDevice.pPlaybackChn->RestorePauseState()) {
                bbRet = AX_FALSE;
                break;
            }
            bRet = bbRet;
        }
    } while(0);
    LOG_M_I(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::SetSpeed(const ax_nvr_channel_vector &vecChn, AX_F32 nSpeedfactor, AX_BOOL bNeedReset /*= AX_FALSE*/) {
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_FALSE;

    do {
        const CVO *pVo = m_stAttr.pPrimary->GetVo();
        if (pVo == nullptr) {
            LOG_M_E(TAG, "[%s][%d]pVo null pointer", __func__, __LINE__);
            break;
        }

        VO_ATTR_T attr = pVo->GetAttr();

        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDeviceID = get<1>(chn);
            auto it = m_mapDevice.find(nDeviceID);
            if (it == m_mapDevice.end()) {
                bbRet = AX_TRUE; /* Not playbacked channel would be treated normal as well */
                break;
            }

            const auto& nDevID = it->first;
            const auto& stDevice = it->second;

            if (stDevice.pPlaybackChn == nullptr) continue;

            if (bNeedReset) { /* Change speed should do reset process to clear cache for vdec/ivps/vo, which could make changing effective immediately */
                std::tuple<AX_U32, AX_U32, AX_U32> displayedTime = stDevice.pPlaybackChn->GetCurrDisplayedTime();
                AX_U32 nDate = std::get<0>(displayedTime);
                AX_U32 nTime = std::get<1>(displayedTime);
                AX_U32 nTick = std::get<2>(displayedTime);
                VO_CHN nVoChn = get<0>(chn) % attr.stChnInfo.nCount;

                stDevice.pPlaybackChn->StopDisp();
                stDevice.pPlaybackChn->StopFile();

                LOG_MM_C(TAG, "Change speed at time %d %d %d of displayed.", nDate, nTime, nTick);

                if (!stDevice.pPlaybackChn->StartFile(nDate, nTime, nTick, -1, -1)) {
                    LOG_M_E(TAG, "[%s][%d]StartFile failed", __func__, __LINE__);
                    bbRet = AX_FALSE;
                    continue;
                }

                if (!stDevice.pPlaybackChn->StartDisp(attr.voLayer, nVoChn, attr.stChnInfo.arrChns[nVoChn].stRect)) {
                    LOG_M_E(TAG, "[%s][%d]StartDisp failed", __func__, __LINE__);
                    bbRet = AX_FALSE;
                    continue;
                }
            }

            if (!stDevice.pPlaybackChn->UpdateFps(nSpeedfactor)) {
                bbRet = AX_FALSE;
                break;
            }
        }

        m_stAttr.nSpeedfactor = nSpeedfactor;
        bRet = bbRet;
    } while(0);

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::Step(const ax_nvr_channel_vector &vecChn, AX_BOOL bReverse) {
    AX_BOOL bRet = AX_FALSE;
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);

    do {
        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDeviceID = get<1>(chn);
            auto it = m_mapDevice.find(nDeviceID);
            if (it == m_mapDevice.end()) break;

            const auto& nDevID = it->first;
            const auto& stDevice = it->second;

            if (stDevice.pPlaybackChn == nullptr) continue;

            if (!stDevice.pPlaybackChn->Step(bReverse)) {
                bbRet = AX_FALSE;
                break;
            }
        }
        bRet = bbRet;
    } while(0);

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::SwitchMainSub1(const ax_nvr_channel_vector &vecChn, AX_U32 nDefaultDate /*= 0*/, AX_U32 nDefaultTime /*= 0*/) {

    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);
    AX_BOOL bRet = AX_FALSE;

    do {
        if (m_stAttr.pPrimary == nullptr) {
            LOG_M_E(TAG, "[%s][%d]pPrimary null pointer", __func__, __LINE__);
            break;
        }

        if (vecChn.size() == 0) {
            LOG_M_E(TAG, "[%s][%d]vecChn.size invalid", __func__, __LINE__);
            break;
        }

        CVO *pVo = const_cast<CVO *>(m_stAttr.pPrimary->GetVo());
        if (pVo == nullptr) {
            LOG_M_E(TAG, "[%s][%d]pVo null pointer", __func__, __LINE__);
            break;
        }
        VO_ATTR_T attr = pVo->GetAttr();
        AX_BOOL bbRet = AX_TRUE;
        for (auto &chn: vecChn) {
            AX_NVR_DEV_ID nDevID = get<1>(chn);
            if (nDevID == -1) continue;

            VO_CHN nVoChn = get<0>(chn) % attr.stChnInfo.nCount;
            auto it = m_mapDevice.find(nDevID);
            if (it == m_mapDevice.end()) {
                LOG_M_E(TAG, "[%s][%d]Playback Channel not found", __func__, __LINE__);
                break;
            }

            const auto& stDevice = it->second;
            if (!stDevice.pPlaybackChn) {
                LOG_M_E(TAG, "[%s][%d]Playback Channel invalid", __func__, __LINE__);
                break;
            }

            std::tuple<AX_U32, AX_U32, AX_U32> dateTimeTick = stDevice.pPlaybackChn->GetCurrDisplayedTime();
            AX_U32 nDate = std::get<0>(dateTimeTick);
            AX_U32 nTime = std::get<1>(dateTimeTick);
            AX_U32 nTick = std::get<2>(dateTimeTick);
            if (0 == nDate && 0 == nTime) {
                nDate = nDefaultDate;
                nTime = nDefaultTime;
                LOG_MM_W(TAG, "[%s][%d] Get current playback datetime failed, use timeline value as default.", nDevID, nVoChn);
            }

            if (!stDevice.pPlaybackChn->TrySwitchMainSub1(nDate, nTime)) {
                bbRet = AX_FALSE;
                break;
            }

            stDevice.pPlaybackChn->StopDisp();
            stDevice.pPlaybackChn->StopFile();

            if (!stDevice.pPlaybackChn->SwitchMainSub1(nDate, nTime)) {
                bbRet = AX_FALSE;
                break;
            }

            // Keep playback direction and repeat configuration
            if (!stDevice.pPlaybackChn->StartFile(nDate, nTime, nTick, m_stAttr.bReverse, m_stAttr.bRepeat)) {
                LOG_M_E(TAG, "[%s][%d]Start File failed", __func__, __LINE__);
                bbRet = AX_FALSE;
                break;
            }

            if (!stDevice.pPlaybackChn->StartDisp(attr.voLayer, nVoChn, attr.stChnInfo.arrChns[nVoChn].stRect)) {
                LOG_M_E(TAG, "[%s][%d]Start Disp failed", __func__, __LINE__);
                bbRet = AX_FALSE;
                break;
            }

            // Keep playback speed
            if (!stDevice.pPlaybackChn->UpdateFps(m_stAttr.nSpeedfactor)) {
                bbRet = AX_FALSE;
                break;
            }
        }

        bRet = bbRet;
    } while(0);


    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::ZoomAndMove(AX_NVR_DEV_ID nDeviceID, const AX_NVR_RECT_T &stCropRect, AX_BOOL bCrop) {
    AX_BOOL bRet = AX_FALSE;
    LOG_M_D(TAG, "[%s][%d] ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);

    do {
        if (m_stAttr.pPrimary == nullptr) {
            break;
        }

        auto it = m_mapDevice.find(nDeviceID);
        if (it == m_mapDevice.end()) {
            break;
        }

        const CVO *pVo = m_stAttr.pPrimary->GetVo();
        if (pVo == nullptr) break;
        VO_ATTR_T attr = pVo->GetAttr();
        int nCount = attr.stChnInfo.nCount;

        const auto& stDevice = it->second;
        AX_VO_RECT_T stRect;
        AX_IVPS_RECT_T stCropRectTmp;
        stCropRectTmp.nX = stCropRect.x;
        stCropRectTmp.nY = stCropRect.y;
        stCropRectTmp.nW = stCropRect.w;
        stCropRectTmp.nH = stCropRect.h;

        int nVoChn = stDevice.nChannelId % nCount;
        if (bCrop) {
            if (stCropRectTmp.nW * 32 < attr.stChnInfo.arrChns[nVoChn].stRect.u32Width
                || stCropRectTmp.nH * 32 < attr.stChnInfo.arrChns[nVoChn].stRect.u32Height) {
                break;
            }
        }

        if (!stDevice.pPlaybackChn->Crop(stRect, stCropRectTmp, bCrop)) {
            break;
        }

        bRet = AX_TRUE;
    } while(0);

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

AX_BOOL CAXNVRPlaybakCtrl::GetResolution(AX_NVR_DEV_ID nDeviceID, AX_U32 &nWidth, AX_U32 &nHeight) {
    AX_BOOL bRet = AX_FALSE;
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);
    std::lock_guard<std::mutex> lock(mutex_);

    do {
        if (m_stAttr.pPrimary == nullptr) {
            break;
        }

        auto it = m_mapDevice.find(nDeviceID);
        if (it == m_mapDevice.end()) {
            break;
        }

        const auto& stDevice = it->second;
        const AX_NVR_CHN_RES_T chnres = stDevice.pPlaybackChn->GetResolution();
        nWidth = chnres.w;
        nHeight = chnres.h;
        bRet = AX_TRUE;
    } while(0);

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}

CAXNVRChannel *CAXNVRPlaybakCtrl::createPlaybackChannel(AX_NVR_DEV_ID nDevID) {
    AX_NVR_CHN_ATTR_T stAttr;
    stAttr.nDevID = nDevID;
    // stAttr.enStreamSrcType = AX_NVR_CHN_SRC_TYPE::FFMPEG;
    stAttr.enStreamSrcType = AX_NVR_CHN_SRC_TYPE::RECORD;
    stAttr.enView = AX_NVR_CHN_VIEW_TYPE::PLAYBACK;
    stAttr.pPlayback = m_stAttr.pPlayback;
    stAttr.nFrmBufNum = 4;
    stAttr.nBackupInDepth = m_stAttr.pPrimary->GetAttr().u32PlaybakFifoDepth + 1;

    return CAXNVRChannel::CreateInstance(stAttr);
}
