/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGPAGEBASE_H
#define SETTINGPAGEBASE_H


class SettingPageBase
{
public:
    virtual int OnLoad() = 0;
    virtual int OnSave() = 0;
};
#endif // SETTINGPAGEBASE_H
