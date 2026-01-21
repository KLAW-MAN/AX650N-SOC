/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingsMain.h"
#include "ui_SettingsMain.h"
#include "global/UiGlobalDef.h"
#include "AppLogApi.h"

#define TAG "SETTING_MAIN"

SettingsMain::SettingsMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsMain)
    , m_pSettingPageSystem(new SettingPageSystem)
    , m_pSettingPageDevList(new SettingPageDevList)
    , m_pSettingPageStorage(new SettingPageStorage)
{
    ui->setupUi(this);
    this->setStyleSheet(CSS_WIDGET);
    ui->pushButtonSave->setStyleSheet(CSS_PUSHBUTTON);

    ui->verticalLayout_system->addWidget(m_pSettingPageSystem);
    ui->verticalLayout_dev_list->addWidget(m_pSettingPageDevList);
    ui->verticalLayout_storage->addWidget(m_pSettingPageStorage);

    connect(ui->pushButtonSave, &QPushButton::clicked, [&]() {
            if (m_nCurPageIndex < (int)m_vecPage.size()) {
                int ret = m_vecPage[m_nCurPageIndex]->OnSave();
                if (0 != ret) {
                    // TODO: error message
                }
            }
        });

    m_vecPage.clear();
    m_vecPage.push_back(m_pSettingPageSystem);
    m_vecPage.push_back(m_pSettingPageDevList);
    m_vecPage.push_back(m_pSettingPageStorage);

    connect(m_pSettingPageSystem, &SettingPageSystem::signal_res_changed, this, &SettingsMain::OnVoIfSyncChanged);
}

SettingsMain::~SettingsMain()
{
    delete ui;
    DEL_PTR(m_pSettingPageSystem);
    DEL_PTR(m_pSettingPageDevList);
    DEL_PTR(m_pSettingPageStorage);
}

void SettingsMain::OnChangePage(int page_index)
{
    m_nCurPageIndex = page_index;
    ui->stackedWidget->setCurrentIndex(m_nCurPageIndex);

    if (m_nCurPageIndex < (int)m_vecPage.size()) {
        int ret = m_vecPage[m_nCurPageIndex]->OnLoad();
        if (0 != ret) {
            // TODO: error message
        }
    }
}

void SettingsMain::OnVoIfSyncChanged(AX_S32 nSelVoIfSyncIndex) {
    LOG_MM_W(TAG, "Resolution changing triggered: %d", nSelVoIfSyncIndex);
    emit signal_res_changed(nSelVoIfSyncIndex);
}

void SettingsMain::showEvent(QShowEvent *evt)
{
    Q_UNUSED(evt)
    if (-1 == m_nCurPageIndex) {
        OnChangePage(0);
    }
}
