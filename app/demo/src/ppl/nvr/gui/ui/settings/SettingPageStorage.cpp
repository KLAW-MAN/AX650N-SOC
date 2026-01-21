/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingPageStorage.h"
#include "ui_SettingPageStorage.h"

SettingPageStorage::SettingPageStorage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingPageStorage)
{
    ui->setupUi(this);
}

SettingPageStorage::~SettingPageStorage()
{
    delete ui;
}

int SettingPageStorage::OnLoad()
{
    return 0;
}

int SettingPageStorage::OnSave()
{
    return 0;
}
