// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include "TimelineWidget.h"

#include <QAction>
#include <QMainWindow>

namespace tl
{
    namespace examples
    {
        namespace timeline_qtwidget
        {
            //! Main window.
            class MainWindow : public QMainWindow
            {
                Q_OBJECT

            public:
                MainWindow(
                    const std::string& input,
                    QWidget* parent = nullptr);

            protected:
                void dragEnterEvent(QDragEnterEvent*) override;
                void dragMoveEvent(QDragMoveEvent*) override;
                void dragLeaveEvent(QDragLeaveEvent*) override;
                void dropEvent(QDropEvent*) override;

            private:
                void _open(const std::string&);

                std::string _input;
                otio::SerializableObject::Retainer<otio::Timeline> _otioTimeline;

                QMap<QString, QAction*> _actions;
                TimelineWidget* _timelineWidget;
            };
        }
    }
}
