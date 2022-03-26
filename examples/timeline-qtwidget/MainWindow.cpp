// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "MainWindow.h"

#include "App.h"

#include <tlCore/Random.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>

#include <QDoubleSpinBox>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QScrollArea>
#include <QToolBar>

namespace tl
{
    namespace examples
    {
        namespace timeline_qtwidget
        {
            MainWindow::MainWindow(
                const std::string& input,
                QWidget* parent) :
                QMainWindow(parent)
            {
                setAcceptDrops(true);

                auto zoomSpinBox = new QDoubleSpinBox;
                zoomSpinBox->setRange(1.0, 1000.0);

                auto toolBar = addToolBar("Tool Bar");
                toolBar->addWidget(zoomSpinBox);

                _timelineWidget = new TimelineWidget;
                auto scrollArea = new QScrollArea;
                scrollArea->setWidgetResizable(true);
                scrollArea->setWidget(_timelineWidget);
                setCentralWidget(scrollArea);

                zoomSpinBox->setValue(_timelineWidget->zoom().x);

                connect(
                    zoomSpinBox,
                    QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    [this](double value)
                    {
                        _timelineWidget->setZoom(math::Vector2f(value, value));
                    });

                if (!input.empty())
                {
                    _open(input);
                }

                resize(1280, 720);

                _otioTimeline = new otio::Timeline;
                otio::ErrorStatus errorStatus;
                for (int i = 0; i < 1000; ++i)
                {
                    auto otioTrack = new otio::Track;
                    for (int j = 0; j < 100; ++j)
                    {
                        switch (math::random(math::IntRange(0, 1)))
                        {
                        case 0:
                        {
                            auto otioClip = new otio::Clip;
                            otioClip->set_source_range(otime::TimeRange(
                                otime::RationalTime(0.0, 24.0),
                                otime::RationalTime(math::random(math::FloatRange(3.0, 60.0 * 24.0)), 24.0)));
                            otioTrack->append_child(otioClip, &errorStatus);
                            break;
                        }
                        case 1:
                        {
                            auto otioGap = new otio::Gap;
                            otioGap->set_source_range(otime::TimeRange(
                                otime::RationalTime(0.0, 24.0),
                                otime::RationalTime(math::random(math::FloatRange(3.0, 60.0 * 24.0)), 24.0)));
                            otioTrack->append_child(otioGap, &errorStatus);
                            break;
                        }
                        default: break;
                        }
                    }
                    _otioTimeline->tracks()->append_child(otioTrack, &errorStatus);
                }
                _timelineWidget->setTimeline(_otioTimeline);
            }

            void MainWindow::dragEnterEvent(QDragEnterEvent* event)
            {
                const QMimeData* mimeData = event->mimeData();
                if (mimeData->hasUrls())
                {
                    event->acceptProposedAction();
                }
            }

            void MainWindow::dragMoveEvent(QDragMoveEvent* event)
            {
                const QMimeData* mimeData = event->mimeData();
                if (mimeData->hasUrls())
                {
                    event->acceptProposedAction();
                }
            }

            void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
            {
                event->accept();
            }

            void MainWindow::dropEvent(QDropEvent* event)
            {
                const QMimeData* mimeData = event->mimeData();
                if (mimeData->hasUrls())
                {
                    const auto urlList = mimeData->urls();
                    for (int i = 0; i < urlList.size(); ++i)
                    {
                        _open(urlList[i].toLocalFile().toUtf8().data());
                    }
                }
            }

            void MainWindow::_open(const std::string& fileName)
            {
                _otioTimeline = nullptr;
                try
                {
                    otio::ErrorStatus errorStatus;
                    _otioTimeline = dynamic_cast<otio::Timeline*>(otio::Timeline::from_json_file(fileName, &errorStatus));
                }
                catch (const std::exception& e)
                {
                    QMessageBox dialog;
                    dialog.setText(e.what());
                    dialog.exec();
                }
                _timelineWidget->setTimeline(_otioTimeline);
            }
        }
    }
}
