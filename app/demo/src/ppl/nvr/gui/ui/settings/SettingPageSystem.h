/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGPAGESYSTEM_H
#define SETTINGPAGESYSTEM_H

#include <QWidget>

#include "SettingPageBase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingPageSystem; }
QT_END_NAMESPACE

class SettingPageSystem : public QWidget, public SettingPageBase
{
    Q_OBJECT

public:
    SettingPageSystem(QWidget *parent = nullptr);
    ~SettingPageSystem();

public:
    virtual int OnLoad();
    virtual int OnSave();

signals:
    void signal_res_changed(int nSelResInd);

private:
    Ui::SettingPageSystem *ui;

};
#endif // SETTINGPAGESYSTEM_H
