// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>

#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>

namespace tl
{
    namespace examples
    {
        namespace timeline_qtwidget
        {
            TimelineWidget::TimelineWidget(QWidget* parent) :
                QAbstractScrollArea(parent)
            {
                setMouseTracking(true);

                _actions["ZoomIn"] = new QAction(this);
                _actions["ZoomIn"]->setShortcut(QKeySequence(Qt::Key_Equal));
                addAction(_actions["ZoomIn"]);

                _actions["ZoomOut"] = new QAction(this);
                _actions["ZoomOut"]->setShortcut(QKeySequence(Qt::Key_Minus));
                addAction(_actions["ZoomOut"]);

                connect(
                    _actions["ZoomIn"],
                    SIGNAL(triggered()),
                    SLOT(zoomIn()));

                connect(
                    _actions["ZoomOut"],
                    SIGNAL(triggered()),
                    SLOT(zoomOut()));
            }

            void TimelineWidget::setTimeline(const otio::SerializableObject::Retainer<otio::Timeline>& otioTimeline)
            {
                _otioTimeline = otioTimeline;

                otio::ErrorStatus errorStatus;
                otime::RationalTime duration;
                int tracks = 0;
                if (_otioTimeline)
                {
                    duration = _otioTimeline->duration(&errorStatus).rescaled_to(1.0);
                    tracks = _otioTimeline->tracks()->children().size();
                }
                _duration = duration;
                _tracks = tracks;

                _itemsUpdate();
                _scrollBarsUpdate();
                viewport()->update();
            }

            float TimelineWidget::zoom() const
            {
                return _zoom;
            }

            math::Vector2f TimelineWidget::viewportCenter() const
            {
                return math::Vector2f(viewport()->width() / 2.F, viewport()->height() / 2.F);
            }

            void TimelineWidget::setZoom(float zoom, const math::Vector2f& focus)
            {
                if (zoom == _zoom)
                    return;

                const QSize viewportSize = viewport()->size();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();

                math::Vector2i scroll;
                scroll.x = (hs + focus.x) * (zoom / _zoom) - focus.x;
                scroll.y = (vs + focus.y) * (zoom / _zoom) - focus.y;

                _zoom = zoom;

                _scrollBarsUpdate();
                horizontalScrollBar()->setValue(scroll.x);
                verticalScrollBar()->setValue(scroll.y);
                viewport()->update();

                Q_EMIT zoomChanged(_zoom);
            }

            void TimelineWidget::zoomReset()
            {
                setZoom(100.F, _mouseInside ? _mousePos : viewportCenter());
            }

            void TimelineWidget::zoomIn()
            {
                setZoom(_zoom * 2.F, _mouseInside ? _mousePos : viewportCenter());
            }

            void TimelineWidget::zoomOut()
            {
                setZoom(_zoom / 2.F, _mouseInside ? _mousePos : viewportCenter());
            }

            void TimelineWidget::resizeEvent(QResizeEvent*)
            {
                _scrollBarsUpdate();
            }

            void TimelineWidget::paintEvent(QPaintEvent*)
            {
                QPainter painter(viewport());

                const int w = viewport()->width();
                const int h = viewport()->height();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();
                const math::BBox2f viewport(
                    hs / _zoom,
                    vs / _zoom,
                    w / _zoom,
                    h / _zoom);

                painter.setPen(QPen(QColor(0, 0, 0), 1));
                painter.setBrush(QColor(255, 0, 0));
                for (const auto& row : _items)
                {
                    for (const auto& item : row)
                    {
                        if (ItemType::Clip == item.type && item.bbox.intersects(viewport))
                        {
                            const math::BBox2f bbox = math::BBox2f(
                                (item.bbox.min.x - hs / _zoom) * _zoom,
                                (item.bbox.min.y - vs / _zoom) * _zoom,
                                item.bbox.w() * _zoom,
                                item.bbox.h() * _zoom);
                            painter.drawRect(bbox.min.x, bbox.min.y, bbox.w(), bbox.h());
                        }
                    }
                }
            }

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
            void TimelineWidget::enterEvent(QEvent* event)
#else
            void TimelineWidget::enterEvent(QEnterEvent* event)
#endif
            {
                event->accept();
                _mouseInside = true;
                _mousePressed = false;
            }

            void TimelineWidget::leaveEvent(QEvent* event)
            {
                event->accept();
                _mouseInside = false;
                _mousePressed = false;
            }

            void TimelineWidget::mousePressEvent(QMouseEvent* event)
            {
                if (Qt::LeftButton == event->button() && event->modifiers() & Qt::ControlModifier)
                {
                    const float devicePixelRatio = window()->devicePixelRatio();
                    _mousePressed = true;
                    _mousePress.x = event->x();
                    _mousePress.y = event->y();
                    _scrollMousePress.x = horizontalScrollBar()->value();
                    _scrollMousePress.y = verticalScrollBar()->value();
                }
            }

            void TimelineWidget::mouseReleaseEvent(QMouseEvent* event)
            {
                _mousePressed = false;
            }

            void TimelineWidget::mouseMoveEvent(QMouseEvent* event)
            {
                _mousePos.x = event->x();
                _mousePos.y = event->y();
                if (_mousePressed)
                {
                    const math::Vector2f scroll(
                        _scrollMousePress.x - (_mousePos.x - _mousePress.x),
                        _scrollMousePress.y - (_mousePos.y - _mousePress.y));
                    horizontalScrollBar()->setValue(scroll.x);
                    verticalScrollBar()->setValue(scroll.y);
                }
            }

            void TimelineWidget::wheelEvent(QWheelEvent* event)
            {
                const float delta = event->angleDelta().y() / 8.F;
                setZoom(_zoom + delta, _mouseInside ? _mousePos : viewportCenter());
            }

            void TimelineWidget::_itemsUpdate()
            {
                _items.clear();

                const float itemHeight = 1.F;

                float y = 0.F;
                for (const auto& child : _otioTimeline->tracks()->children())
                {
                    if (auto otioTrack = dynamic_cast<otio::Track*>(child.value))
                    {
                        std::vector<Item> items;
                        for (const auto& trackChild : otioTrack->children())
                        {
                            if (auto otioItem = dynamic_cast<otio::Item*>(trackChild.value))
                            {
                                otio::ErrorStatus errorStatus;
                                const auto trimmedRangeOpt = otioItem->trimmed_range_in_parent(&errorStatus);
                                if (trimmedRangeOpt.has_value())
                                {
                                    const otime::TimeRange trimmedRange = trimmedRangeOpt.value();
                                    const otime::TimeRange trimmedRangeScaled(
                                        trimmedRange.start_time().rescaled_to(1.0),
                                        trimmedRange.duration().rescaled_to(1.0));

                                    Item item;
                                    item.p = otioItem;
                                    if (auto otioClip = dynamic_cast<otio::Clip*>(otioItem))
                                    {
                                        item.type = ItemType::Clip;
                                    }
                                    else if (auto otioGap = dynamic_cast<otio::Gap*>(otioItem))
                                    {
                                        item.type = ItemType::Gap;
                                    }
                                    item.bbox = math::BBox2f(
                                        trimmedRangeScaled.start_time().value(),
                                        y,
                                        trimmedRangeScaled.duration().value(),
                                        itemHeight);
                                    items.push_back(item);
                                }
                            }
                        }
                        _items.push_back(items);
                    }

                    y += itemHeight;
                }
            }

            void TimelineWidget::_scrollBarsUpdate()
            {
                const QSize viewportSize = viewport()->size();
                horizontalScrollBar()->setPageStep(viewportSize.width());
                verticalScrollBar()->setPageStep(viewportSize.height());
                horizontalScrollBar()->setRange(0, _duration.value() * _zoom - viewportSize.width());
                verticalScrollBar()->setRange(0, _tracks * _zoom - viewportSize.height());
            }
        }
    }
}
