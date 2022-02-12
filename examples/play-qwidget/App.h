// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include "ColorModel.h"
#include "FilesModel.h"
#include "MainWindow.h"
#include "SettingsObject.h"

#include <tlApp/IApp.h>

#include <tlQt/TimeObject.h>
#include <tlQt/TimelinePlayer.h>

#include <QApplication>

namespace tl
{
    //! Application options.
    struct Options
    {
        imaging::ColorConfig colorConfig;
    };

    //! Application.
    class App : public QApplication, public app::IApp
    {
        Q_OBJECT

    public:
        App(int& argc, char** argv);
        ~App() override;

        //! Get the time object.
        qt::TimeObject* timeObject() const;

        //! Get the settings object.
        SettingsObject* settingsObject() const;

        //! Get the files model.
        const std::shared_ptr<FilesModel>& filesModel() const;

        //! Get the color model.
        const std::shared_ptr<ColorModel>& colorModel() const;

    public Q_SLOTS:
        //! Open a file.
        void open(const QString&, const QString& = QString());

        //! Open a file dialog.
        void openDialog();

        //! Open a file with audio dialog.
        void openWithAudioDialog();

    private Q_SLOTS:
        void _activeCallback(const std::vector<std::shared_ptr<FilesModelItem> >&);
        void _settingsCallback();

    private:
        void _settingsUpdate();

        std::string _input;
        Options _options;

        qt::TimeObject* _timeObject = nullptr;
        SettingsObject* _settingsObject = nullptr;
        std::shared_ptr<FilesModel> _filesModel;
        std::shared_ptr<observer::ListObserver<std::shared_ptr<FilesModelItem> > > _activeObserver;
        std::vector<std::shared_ptr<FilesModelItem> > _active;
        std::shared_ptr<observer::ListObserver<int> > _layersObserver;
        std::shared_ptr<ColorModel> _colorModel;

        std::vector<qt::TimelinePlayer*> _timelinePlayers;

        MainWindow* _mainWindow = nullptr;
    };
}
