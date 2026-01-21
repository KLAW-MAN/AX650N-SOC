/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGSLEFTTOOLBAR_H
#define SETTINGSLEFTTOOLBAR_H

#include <QWidget>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QModelIndex>


QT_BEGIN_NAMESPACE
namespace Ui { class SettingsLeftToolbar; }
QT_END_NAMESPACE

class SettingsLeftToolbar : public QWidget
{
    Q_OBJECT

public:
    SettingsLeftToolbar(QWidget *parent = nullptr);
    ~SettingsLeftToolbar();

    // void InitPageList();

private:
    Ui::SettingsLeftToolbar *ui;
    // QStandardItemModel *m_ItemModel;


// private slots:
//     void OnPageIndexChange(int index);

signals:
    void signal_change_page(int page_index);
};
#endif // SETTINGSLEFTTOOLBAR_H
