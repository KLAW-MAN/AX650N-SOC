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

#include "OS04a10.h"

/* for DCG-VS:DCG */
class COS04a10Dcg : public COS04a10 {
public:
    COS04a10Dcg(SENSOR_CONFIG_T tSensorConfig);
    virtual ~COS04a10Dcg(AX_VOID);

public:
    virtual AX_BOOL Open() override;
    virtual AX_BOOL Close() override;

protected:
    virtual AX_VOID InitSnsLibraryInfo(AX_VOID) override;
    virtual AX_VOID InitDevAttr() override;

    AX_BOOL OpenAll();
    AX_BOOL CloseAll();

};
