/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingPageNetwork.h"
#include "ui_SettingPageNetwork.h"

SettingPageNetwork::SettingPageNetwork(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingPageNetwork)
{
    ui->setupUi(this);
}

SettingPageNetwork::~SettingPageNetwork()
{
    delete ui;
}

int SettingPageNetwork::OnLoad()
{
    return 0;
}

int SettingPageNetwork::OnSave()
{
    return 0;
}
