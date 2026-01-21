/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGPAGESTORAGE_H
#define SETTINGPAGESTORAGE_H

#include <QWidget>

#include "SettingPageBase.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SettingPageStorage; }
QT_END_NAMESPACE

class SettingPageStorage : public QWidget, public SettingPageBase
{
    Q_OBJECT

public:
    SettingPageStorage(QWidget *parent = nullptr);
    ~SettingPageStorage();

public:
    virtual int OnLoad();
    virtual int OnSave();

private:
    Ui::SettingPageStorage *ui;

};
#endif // SETTINGPAGESTORAGE_H
