// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlQWidget/FilmstripWidget.h>

#include <tlCore/Timeline.h>

#include <QMainWindow>

namespace tl
{
    //! Main window.
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(
            const std::string&                    input,
            const std::shared_ptr<core::Context>& context,
            QWidget*                              parent  = nullptr);

    protected:
        void dragEnterEvent(QDragEnterEvent*) override;
        void dragMoveEvent(QDragMoveEvent*) override;
        void dragLeaveEvent(QDragLeaveEvent*) override;
        void dropEvent(QDropEvent*) override;

    private:
        void _open(const std::string&);

        std::shared_ptr<core::Context> _context;
        std::string _input;
        std::shared_ptr<timeline::Timeline> _timeline;
        qwidget::FilmstripWidget* _filmstripWidget;
    };
}
