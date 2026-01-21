/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef PLAYBACKCTRL_H
#define PLAYBACKCTRL_H

#include <QWidget>
#include <QFutureWatcher>
#include <QFutureInterface>
#include <QThread>
#include <QSignalMapper>

#include "global/UiGlobalDef.h"
#include "PlaybackLeftToolbar.h"
#include "PlaybackTimeLine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PlaybackCtrl; }
QT_END_NAMESPACE

class PlaybackCtrl : public QWidget
{
    Q_OBJECT

public:
    PlaybackCtrl(QWidget *parent = nullptr);
    ~PlaybackCtrl(void);



private:
    Ui::PlaybackCtrl *ui;

private slots:
    void OnCtrlBtnClicked(void);

signals:
    void signal_playback_action(PLAYBACK_ACTION_TYPE enActionType);

public:
    void EnableSingleLabelCtrl(bool bEnable);
    void EnableRepeatBtn(bool bEnable);
    AX_BOOL IsRepeatPlayback();
};
#endif // PLAYBACKCTRL_H