/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingPageRoundPatrol.h"
#include "ui_SettingPageRoundPatrol.h"

SettingPageRoundPatrol::SettingPageRoundPatrol(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingPageRoundPatrol)
{
    ui->setupUi(this);
}

SettingPageRoundPatrol::~SettingPageRoundPatrol()
{
    delete ui;
}
