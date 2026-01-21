/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef PREVIEWLEFTTOOLBAR_H
#define PREVIEWLEFTTOOLBAR_H

#include <QWidget>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class PreviewLeftToolbar; }
QT_END_NAMESPACE

class PreviewLeftToolbar : public QWidget
{
    Q_OBJECT

public:
    PreviewLeftToolbar(QWidget *parent = nullptr);
    ~PreviewLeftToolbar();

protected:
    virtual void showEvent(QShowEvent *event) override;


private:
    Ui::PreviewLeftToolbar *ui;
    QStandardItemModel *m_ItemModel;
};
#endif // PREVIEWLEFTTOOLBAR_H
