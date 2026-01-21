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
#include "AXThread.hpp"
#include "TransferHelper.hpp"


class CAiSwitchSimulator {
public:
    CAiSwitchSimulator(AX_VOID) = default;
    ~CAiSwitchSimulator(AX_VOID) = default;

    AX_BOOL Init(std::string strPath);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    AX_VOID BindTransfer(CTransferHelper* pInstance);

protected:
    AX_BOOL LoadConfig(std::string& strConfigPath);

private:
    AX_VOID WorkThread(AX_VOID* pArg);

protected:
    CAXThread m_threadWork;
    CTransferHelper* m_pTransferHelper {nullptr};
    CDispatcher* m_pDispatcher {nullptr};

};