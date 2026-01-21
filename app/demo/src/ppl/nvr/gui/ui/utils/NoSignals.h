/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once
#include <QWidget>
#include <QPushButton>
#include <QString>

class NoSignals: public QObject {
    Q_OBJECT
public:
    static void setChecked(QPushButton* widget, bool isChecked) {
        widget->blockSignals(true);
        widget->setChecked(isChecked);
        widget->blockSignals(false);
    }
};
