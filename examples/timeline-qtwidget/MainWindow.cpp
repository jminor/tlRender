// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "MainWindow.h"

#include "App.h"

#include <tlCore/Random.h>
#include <tlCore/StringFormat.h>

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

                _actions["ZoomIn"] = new QAction(this);
                _actions["ZoomIn"]->setIcon(QIcon(":/Icons/ViewZoomIn.svg"));
                _actions["ZoomIn"]->setAutoRepeat(true);
                _actions["ZoomIn"]->setToolTip(tr("Zoom in"));

                _actions["ZoomOut"] = new QAction(this);
                _actions["ZoomOut"]->setIcon(QIcon(":/Icons/ViewZoomOut.svg"));
                _actions["ZoomOut"]->setAutoRepeat(true);
                _actions["ZoomOut"]->setToolTip(tr("Zoom in"));

                auto zoomSpinBox = new QDoubleSpinBox;
                zoomSpinBox->setRange(0.1, 10.0);
                zoomSpinBox->setSingleStep(0.1);
                zoomSpinBox->setToolTip(tr("Zoom"));

                auto toolBar = addToolBar("Tool Bar");
                toolBar->addAction(_actions["ZoomIn"]);
                toolBar->addAction(_actions["ZoomOut"]);
                toolBar->addWidget(zoomSpinBox);

                _timelineWidget = new TimelineWidget;
                _timelineWidget->setFrameShape(QFrame::NoFrame);
                auto scrollArea = new QScrollArea;
                scrollArea->setWidgetResizable(true);
                scrollArea->setWidget(_timelineWidget);
                setCentralWidget(scrollArea);

                zoomSpinBox->setValue(_timelineWidget->zoom());

                connect(
                    _actions["ZoomIn"],
                    SIGNAL(triggered()),
                    _timelineWidget,
                    SLOT(zoomIn()));

                connect(
                    _actions["ZoomOut"],
                    SIGNAL(triggered()),
                    _timelineWidget,
                    SLOT(zoomOut()));

                connect(
                    zoomSpinBox,
                    QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    [this](double value)
                    {
                        _timelineWidget->setZoom(value, _timelineWidget->viewportCenter());
                    });

                connect(
                    _timelineWidget,
                    &TimelineWidget::zoomChanged,
                    [zoomSpinBox](float value)
                    {
                        zoomSpinBox->setValue(value);
                    });
                
                if (!input.empty())
                {
                    _open(input);
                }

                resize(1280, 720);

                _timelineWidget->setFocus();

                if (1)
                {
                    const math::Vector2i count(50, 50);
                    _otioTimeline = new otio::Timeline;
                    otio::ErrorStatus errorStatus;
                    for (int i = 0; i < count.y; ++i)
                    {
                        auto otioTrack = new otio::Track;
                        otio::Item* otioItemPrev = nullptr;
                        for (int j = 0; j < count.x; ++j)
                        {
                            int index = math::random(math::IntRange(0, 1));
                            if (!otioItemPrev ||
                                (1 == index && dynamic_cast<otio::Gap*>(otioItemPrev)) ||
                                j == (count.x - 1))
                            {
                                index = 0;
                            }
                            
                            otio::Item* otioItem = nullptr;
                            switch (index)
                            {
                            case 0:
                            {
                                auto otioClip = new otio::Clip;
                                otioClip->set_name(string::Format("{0}").arg(math::random(math::IntRange(0, 1000))));
                                otioClip->set_source_range(otime::TimeRange(
                                    otime::RationalTime(0, 24),
                                    otime::RationalTime(math::random(math::IntRange(3, 6 * 24)), 24)));
                                otioItem = otioClip;
                                break;
                            }
                            case 1:
                            {
                                auto otioGap = new otio::Gap;
                                otioGap->set_source_range(otime::TimeRange(
                                    otime::RationalTime(0, 24),
                                    otime::RationalTime(math::random(math::IntRange(3, 12 * 24)), 24)));
                                otioItem = otioGap;
                                break;
                            }
                            default: break;
                            }

                            if (otioItem)
                            {
                                otioTrack->append_child(otioItem, &errorStatus);
                            }
                            otioItemPrev = otioItem;
                        }
                        _otioTimeline->tracks()->append_child(otioTrack, &errorStatus);
                    }
                    _timelineWidget->setTimeline(_otioTimeline);
                }
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
