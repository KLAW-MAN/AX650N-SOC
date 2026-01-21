/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "PreviewLeftToolbar.h"
#include "ui_PreviewLeftToolbar.h"
#include "AXNVRFramework.h"
#include "AXNVRFrameworkDefine.h"

PreviewLeftToolbar::PreviewLeftToolbar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PreviewLeftToolbar)
{
    ui->setupUi(this);
    m_ItemModel = new QStandardItemModel(this);
}

PreviewLeftToolbar::~PreviewLeftToolbar()
{
    delete ui;
    delete m_ItemModel;
}

void PreviewLeftToolbar::showEvent(QShowEvent *event) {

    m_ItemModel->clear();
    CAXNVRPreviewCtrl *pPreview = CAXNVRFramework::GetInstance()->PreviewCtrl();
    if (pPreview) {
        vector<AX_NVR_DEV_INFO_T> vecDeviceInfo = pPreview->GetDevices();
        for (auto &&info : vecDeviceInfo) {
            QStandardItem *item = new QStandardItem(QString("%1:%2").arg(info.nChannelId + 1).arg(QString::fromUtf8((char*)info.szAlias)));
            m_ItemModel->appendRow(item);
        }
        ui->listView->setModel(m_ItemModel);
    }
    return QWidget::showEvent(event);
}
