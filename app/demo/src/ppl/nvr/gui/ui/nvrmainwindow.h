/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef NVRMAINWINDOW_H
#define NVRMAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QProcess>
#include <QTcpSocket>
#include <QFutureWatcher>
#include <QFutureInterface>
#include <QThread>
#include <QCloseEvent>
#include "preview/PreviewLeftToolbar.h"
#include "preview/PreviewTopToolbar.h"
#include "preview/PreviewMain.h"

#include "playback/PlaybackLeftToolbar.h"
#include "playback/PlaybackTopToolbar.h"
#include "playback/PlaybackMain.h"

#include "settings/SettingsLeftToolbar.h"
#include "settings/SettingsTopToolbar.h"
#include "settings/SettingsMain.h"

#include "round/RoundPatrolMain.h"

#include "TestSuiteConfigParser.h"
#include "test_suite/ts_main.hpp"
#include "test_suite/ts_preview.hpp"
#include "test_suite/ts_playback.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class NVRMainWindow; }
QT_END_NAMESPACE


class NVRMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    NVRMainWindow(unsigned int type, unsigned int w = 0, unsigned int h = 0, QWidget *parent = nullptr);
    ~NVRMainWindow();

    AX_VOID DrawFBMouse(QPoint& pos);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private:
    AX_VOID RegisterTSWidgets();
    AX_VOID TestSuiteThread(AX_VOID* pArg);
    CTestSuiteBase* FindTsInstance(const string& strModuleName);
    AX_BOOL SetTsModuleConfig(const string& strModuleName, const AX_NVR_TS_CONFIG_PTR pModuleConfig);

signals:
    void signal_result_feedback(const AX_NVR_ACTION_RES_T& enActionType);
    void signal_exit();

private:
    Ui::NVRMainWindow *ui;
    CAXThread m_threadTS;

public slots:
    void OnEnablePolling(bool bPolling);
    void OnVoIfSyncChanged(AX_S32 nSelVoIfSyncIndex);

private slots:
    void OnTimeOutPolling();
    // void OnFinished();
    // void OnUpdate(int value);
    void StartTestSuiteThread(AX_NVR_TS_CONFIG_PTR pTsCfg);
    void StopTestSuiteThread(AX_NVR_TS_CONFIG_PTR pTsCfg);
    void OnOperateWidget(const TS_OPERATION_INFO_T& tOprInfo);

public:
    // preivew
    PreviewLeftToolbar *m_pPreviewLeftToolbar = nullptr;
    PreviewTopToolbar *m_pPreviewTopToolbar = nullptr;
    PreviewMain *m_pPreviewMain = nullptr;

    // playback
    PlaybackLeftToolbar *m_pPlaybackLeftToolbar = nullptr;
    PlaybackTopToolbar *m_pPlaybackTopToolbar = nullptr;
    PlaybackMain *m_pPlaybackMain = nullptr;

    // settings
    SettingsLeftToolbar *m_pSettingsLeftToolbar = nullptr;
    SettingsTopToolbar *m_pSettingsTopToolbar = nullptr;
    SettingsMain *m_pSettingsMain = nullptr;

    // round
    RoundPatrolMain *m_pRoundPatrolMain = nullptr;
    AX_BOOL m_bRoundPatrolMain = AX_FALSE;

    // test suite for main window
    CTestMain *m_pTsMain = nullptr;
    CTestPreview *m_pTsPreview = nullptr;
    CTestPlayback *m_pTsPlayback = nullptr;

private:
    // QProcess m_process;
    QFutureWatcher<AX_NVR_ACTION_RES_T> m_watcherRPatrol;
    QTimer *m_pTimerPolling = nullptr;
    QTcpSocket *m_pTcpSocket = nullptr;

    AX_NVR_TS_CONFIG_T m_tTestSuiteConfig;

    AX_S32 m_hFbMouse{-1};
    AX_S32 m_nFbMouseIndex{-1};

    void __close(void);
};

class GlobalMouseListener : public QObject
{
    Q_OBJECT
public:
    explicit GlobalMouseListener(QObject *parent = nullptr)
        : QObject(parent)
    {
        qApp->installEventFilter(this);
    }

    AX_VOID SetResponser(NVRMainWindow *pMainWnd) {
        m_pMainWnd = pMainWnd;
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::MouseMove) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            QPoint globalPos = mouseEvent->globalPos();
            if (m_pMainWnd) {
                m_pMainWnd->DrawFBMouse(globalPos);
            }
        }
        return QObject::eventFilter(obj, event);
    }

private:
    NVRMainWindow* m_pMainWnd {nullptr};
};
#endif // NVRMAINWINDOW_H
