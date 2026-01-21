/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef PLAYBACKTOPTOOLBAR_H
#define PLAYBACKTOPTOOLBAR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class PlaybackTopToolbar; }
QT_END_NAMESPACE

class PlaybackTopToolbar : public QWidget
{
    Q_OBJECT

public:
    PlaybackTopToolbar(QWidget *parent = nullptr);
    ~PlaybackTopToolbar();

private slots:
    void OnChangeMainSub1();

private:
    Ui::PlaybackTopToolbar *ui;

signals:
    void signal_change_mainsub1(void);
};
#endif // PLAYBACKTOPTOOLBAR_H
