/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGSTOPTOOLBAR_H
#define SETTINGSTOPTOOLBAR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsTopToolbar; }
QT_END_NAMESPACE

class SettingsTopToolbar : public QWidget
{
    Q_OBJECT

public:
    SettingsTopToolbar(QWidget *parent = nullptr);
    ~SettingsTopToolbar();

private:
    Ui::SettingsTopToolbar *ui;
};
#endif // SETTINGSTOPTOOLBAR_H
