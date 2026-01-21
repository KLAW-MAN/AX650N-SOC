/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingsTopToolbar.h"
#include "ui_SettingsTopToolbar.h"

SettingsTopToolbar::SettingsTopToolbar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsTopToolbar)
{
    ui->setupUi(this);
}

SettingsTopToolbar::~SettingsTopToolbar()
{
    delete ui;
}

