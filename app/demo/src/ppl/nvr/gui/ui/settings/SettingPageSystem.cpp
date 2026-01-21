/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingPageSystem.h"
#include "ui_SettingPageSystem.h"
#include "AXNVRFrameworkDefine.h"
#include "NVRConfigParser.h"
#include "global/UiGlobalDef.h"
#include "AppLogApi.h"

#define TAG "SETTING_SYS"


SettingPageSystem::SettingPageSystem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingPageSystem)
{
    ui->setupUi(this);
    this->setStyleSheet(CSS_WIDGET);
    ui->tabWidget->setStyleSheet(CSS_TABWIDGET);

}

SettingPageSystem::~SettingPageSystem()
{
    delete ui;
}

int SettingPageSystem::OnLoad()
{
    AX_NVR_DISPVO_CONFIG_T tPrimaryCfg = CNVRConfigParser::GetInstance()->GetPrimaryDispConfig();
    if (22/* 3840x2160P30 */ == tPrimaryCfg.nHDMI || 24/* 3840x2160P60 */ == tPrimaryCfg.nHDMI) {
        ui->comboBoxResolution->setItemText(0, "3840*2160");
    }

    return 0;
}

int SettingPageSystem::OnSave()
{
    AX_S32 nCurrSel = ui->comboBoxResolution->currentIndex();
    emit signal_res_changed(nCurrSel);
    return 0;
}
