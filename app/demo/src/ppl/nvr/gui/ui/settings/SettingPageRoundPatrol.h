/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGPAGEROUNDPATROL_H
#define SETTINGPAGEROUNDPATROL_H

#include <QWidget>

#include "SettingPageBase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingPageRoundPatrol; }
QT_END_NAMESPACE

class SettingPageRoundPatrol : public QWidget, public SettingPageBase
{
    Q_OBJECT

public:
    SettingPageRoundPatrol(QWidget *parent = nullptr);
    ~SettingPageRoundPatrol();

public:
    virtual int OnLoad() override {return 0;};
    virtual int OnSave() override {return 0;};

private:
    Ui::SettingPageRoundPatrol *ui;

};
#endif // SETTINGPAGEROUNDPATROL_H
