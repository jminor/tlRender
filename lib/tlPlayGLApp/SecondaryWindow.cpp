// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#include <tlPlayGLApp/SecondaryWindow.h>

#include <tlPlayGLApp/App.h>

#include <tlPlay/ColorModel.h>
#include <tlPlay/FilesModel.h>
#include <tlPlay/ViewportModel.h>

#include <tlTimelineUI/TimelineViewport.h>

namespace tl
{
    namespace play_gl
    {
        struct SecondaryWindow::Private
        {
            std::shared_ptr<timelineui::TimelineViewport> viewport;

            std::shared_ptr<observer::ListObserver<std::shared_ptr<timeline::Player> > > playersObserver;
            std::shared_ptr<observer::ValueObserver<timeline::BackgroundOptions> > backgroundOptionsObserver;
            std::shared_ptr<observer::ValueObserver<timeline::ColorConfigOptions> > colorConfigOptionsObserver;
            std::shared_ptr<observer::ValueObserver<timeline::LUTOptions> > lutOptionsObserver;
            std::shared_ptr<observer::ValueObserver<timeline::ImageOptions> > imageOptionsObserver;
            std::shared_ptr<observer::ValueObserver<timeline::DisplayOptions> > displayOptionsObserver;
            std::shared_ptr<observer::ValueObserver<timeline::CompareOptions> > compareOptionsObserver;
        };

        void SecondaryWindow::_init(
            const std::shared_ptr<App>& app,
            const std::shared_ptr<system::Context>& context)
        {
            Window::_init("tlplay-gl 2", context);
            TLRENDER_P();

            p.viewport = timelineui::TimelineViewport::create(context);
            p.viewport->setParent(shared_from_this());

            p.playersObserver = observer::ListObserver<std::shared_ptr<timeline::Player> >::create(
                app->observeActivePlayers(),
                [this](const std::vector<std::shared_ptr<timeline::Player> >& value)
                {
                    _p->viewport->setPlayers(value);
                });

            p.backgroundOptionsObserver = observer::ValueObserver<timeline::BackgroundOptions>::create(
                app->getViewportModel()->observeBackgroundOptions(),
                [this](const timeline::BackgroundOptions& value)
                {
                    _p->viewport->setBackgroundOptions(value);
                });

            p.colorConfigOptionsObserver = observer::ValueObserver<timeline::ColorConfigOptions>::create(
                app->getColorModel()->observeColorConfigOptions(),
                [this](const timeline::ColorConfigOptions& value)
                {
                    _p->viewport->setColorConfigOptions(value);
                });

            p.lutOptionsObserver = observer::ValueObserver<timeline::LUTOptions>::create(
                app->getColorModel()->observeLUTOptions(),
                [this](const timeline::LUTOptions& value)
                {
                    _p->viewport->setLUTOptions(value);
                });

            p.imageOptionsObserver = observer::ValueObserver<timeline::ImageOptions>::create(
                app->getColorModel()->observeImageOptions(),
                [this](const timeline::ImageOptions& value)
                {
                    _p->viewport->setImageOptions({ value });
                });

            p.displayOptionsObserver = observer::ValueObserver<timeline::DisplayOptions>::create(
                app->getColorModel()->observeDisplayOptions(),
                [this](const timeline::DisplayOptions& value)
                {
                    _p->viewport->setDisplayOptions({ value });
                });

            p.compareOptionsObserver = observer::ValueObserver<timeline::CompareOptions>::create(
                app->getFilesModel()->observeCompareOptions(),
                [this](const timeline::CompareOptions& value)
                {
                    _p->viewport->setCompareOptions(value);
                });
        }

        SecondaryWindow::SecondaryWindow() :
            _p(new Private)
        {}

        SecondaryWindow::~SecondaryWindow()
        {
            TLRENDER_P();
            _makeCurrent();
            auto i = std::find(_children.begin(), _children.end(), p.viewport);
            if (i != _children.end())
            {
                _children.erase(i);
            }
        }

        std::shared_ptr<SecondaryWindow> SecondaryWindow::create(
            const std::shared_ptr<App>& app,
            const std::shared_ptr<system::Context>& context)
        {
            auto out = std::shared_ptr<SecondaryWindow>(new SecondaryWindow);
            out->_init(app, context);
            return out;
        }
    }
}
