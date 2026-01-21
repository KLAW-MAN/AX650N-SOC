/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGPAGENETWORK_H
#define SETTINGPAGENETWORK_H

#include <QWidget>

#include "SettingPageBase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingPageNetwork; }
QT_END_NAMESPACE

class SettingPageNetwork : public QWidget, public SettingPageBase
{
    Q_OBJECT

public:
    SettingPageNetwork(QWidget *parent = nullptr);
    ~SettingPageNetwork();

public:
    virtual int OnLoad();
    virtual int OnSave();

private:
    Ui::SettingPageNetwork *ui;

};
#endif // SETTINGPAGENETWORK_H
