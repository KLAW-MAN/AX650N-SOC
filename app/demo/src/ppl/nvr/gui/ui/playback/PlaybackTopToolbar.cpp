/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "PlaybackTopToolbar.h"
#include "ui_PlaybackTopToolbar.h"

PlaybackTopToolbar::PlaybackTopToolbar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlaybackTopToolbar)
{
    ui->setupUi(this);
    connect(ui->pushButton_main_sub1, &QPushButton::clicked, this, &PlaybackTopToolbar::OnChangeMainSub1);
}

PlaybackTopToolbar::~PlaybackTopToolbar()
{
    delete ui;
}

void PlaybackTopToolbar::OnChangeMainSub1() {
    emit signal_change_mainsub1();
}