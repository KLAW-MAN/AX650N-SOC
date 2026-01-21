/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef SETTINGSMAIN_H
#define SETTINGSMAIN_H

#include <QWidget>
#include <QShowEvent>

#include "SettingPageSystem.h"
#include "SettingPageNetwork.h"
#include "SettingPageDevList.h"
#include "SettingPageStorage.h"


QT_BEGIN_NAMESPACE
namespace Ui { class SettingsMain; }
QT_END_NAMESPACE

class SettingsMain : public QWidget
{
    Q_OBJECT

public:
    SettingsMain(QWidget *parent = nullptr);
    ~SettingsMain();

    virtual void showEvent(QShowEvent *) override;

public slots:
    void OnChangePage(int page_index);
    void OnVoIfSyncChanged(AX_S32 nSelVoIfSyncIndex);

signals:
    void signal_res_changed(int nSelResInd);

private:
    Ui::SettingsMain *ui;

    int m_nCurPageIndex{-1};
    QVector<SettingPageBase*> m_vecPage;

    SettingPageSystem  *m_pSettingPageSystem;
    SettingPageDevList *m_pSettingPageDevList;
    SettingPageStorage *m_pSettingPageStorage;
};
#endif // SETTINGSMAIN_H
