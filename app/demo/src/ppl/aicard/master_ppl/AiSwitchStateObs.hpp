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
#include "Dispatcher.hpp"


enum class STATE {
    STATE_DISABLE = 0,
    STATE_ENABLE = 1
};

class IStateObserver {
public:
    virtual ~IStateObserver(AX_VOID) = default;

public:
    virtual AX_VOID OnStateChanged(AX_U32 nState) = 0;
};

using IStateObserverUniquePtr = std::unique_ptr<IStateObserver>;
class CStateObserverMaker {
public:
    template <typename T, typename... Args>
    static IStateObserverUniquePtr CreateStateObserver(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    };

private:
    CStateObserverMaker(AX_VOID) noexcept = default;
    ~CStateObserverMaker(AX_VOID) = default;
};


/**
 * @brief
 *
 */
class CAiSwitchStateObs final : public IStateObserver {
public:
    CAiSwitchStateObs(CDispatcher* pSink) noexcept : m_pSink(pSink){};
    virtual ~CAiSwitchStateObs(AX_VOID) = default;

    AX_VOID OnStateChanged(AX_U32 nState) override {
        if (nullptr == m_pSink) {
            return;
        }

        if (STATE::STATE_DISABLE == (STATE)nState) {
            m_pSink->EnableSwitching(AX_FALSE);
        } else if (STATE::STATE_ENABLE == (STATE)nState) {
            m_pSink->EnableSwitching(AX_TRUE);
        } else {
            LOG_MM_W("STATE_OBS", "Unrecognized state %d", nState);
            return;
        }
    }

private:
    CDispatcher* m_pSink;
};