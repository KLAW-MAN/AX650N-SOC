/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SettingsLeftToolbar.h"
#include "ui_SettingsLeftToolbar.h"
#include "global/UiGlobalDef.h"

SettingsLeftToolbar::SettingsLeftToolbar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsLeftToolbar)
{
    ui->setupUi(this);
    ui->pushButtonSystem->setStyleSheet(CSS_PUSHBUTTON);
    ui->pushButtonDevice->setStyleSheet(CSS_PUSHBUTTON);
    ui->pushButtonRecord->setStyleSheet(CSS_PUSHBUTTON);

    connect(ui->pushButtonSystem, &QPushButton::clicked, [=](){ emit signal_change_page((int)CONFIG_TYPE::SYSTEM); });
    connect(ui->pushButtonDevice, &QPushButton::clicked, [=](){ emit signal_change_page((int)CONFIG_TYPE::DEVICE); });
    connect(ui->pushButtonRecord, &QPushButton::clicked, [=](){ emit signal_change_page((int)CONFIG_TYPE::RECORD); });
}

SettingsLeftToolbar::~SettingsLeftToolbar()
{
    delete ui;
    // delete m_ItemModel;
}

// void SettingsLeftToolbar::InitPageList()
// {
//     m_ItemModel = new QStandardItemModel(this);
//     QStringList strList;
//     strList.append("System Configure");
//     strList.append("Network Configure");
//     strList.append("Channel Management");
//     strList.append("Record Management");

//     int nCount = strList.size();
//     for(int i = 0; i < nCount; i++)
//     {
//         QString string = static_cast<QString>(strList.at(i));
//         QStandardItem *item = new QStandardItem(string);
//         m_ItemModel->appendRow(item);
//     }

//     ui->listView->setModel(m_ItemModel);
//     connect(ui->listView, SIGNAL(clicked(QModelIndex)), this, SLOT(OnPageIndexChange(QModelIndex)));
// }

// void SettingsLeftToolbar::OnPageIndexChange(int index)
// {
//     emit signal_change_page(index);
// }
