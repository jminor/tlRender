// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <tlCore/StringFormat.h>

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
            namespace
            {
                const float itemWidth = 100.F;
                const float itemHeight = 100.F;
                const float itemMargin = 2.F;
                const float itemBorder = 2.F;
            }

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
                setZoom(1.F, _mouseInside ? _mousePos : viewportCenter());
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
                const QPalette palette = this->palette();
                QFontMetrics fm(font());
                const float fl = fm.lineSpacing();
                const float fa = fm.ascent();
                const float fd = fm.descent();
                painter.setFont(font());

                const int w = viewport()->width();
                const int h = viewport()->height();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();
                const math::BBox2f viewport(
                    hs / _zoom / itemWidth,
                    vs / _zoom / itemHeight,
                    w / _zoom / itemWidth,
                    h / _zoom / itemHeight);

                // Time marks.
                QVector<QRectF> timeRects;
                for (float x = floorf(viewport.min.x); x < ceilf(viewport.max.x); x += 1.F)
                {
                    timeRects.append(QRect(
                        ((x * itemWidth - hs / _zoom) * _zoom),
                        0.F,
                        1.F,
                        h));
                }
                painter.setPen(Qt::NoPen);
                painter.setBrush(palette.color(QPalette::ColorRole::Mid));
                painter.drawRects(timeRects);

                // Clips.
                QVector<QRectF> lightRects;
                QVector<QRectF> buttonRects;
                QVector<QRectF> nameRects;
                QVector<QString> nameLabels;
                QVector<QRectF> sourceRangeRects;
                QVector<QString> sourceRangeLabels;
                for (auto& clip : _clips)
                {
                    if (clip.bbox.intersects(viewport))
                    {
                        math::BBox2f bbox(
                            ((clip.bbox.min.x * itemWidth - hs / _zoom) * _zoom),
                            ((clip.bbox.min.y * itemHeight - vs / _zoom) * _zoom),
                            (clip.bbox.w() * itemWidth * _zoom),
                            (clip.bbox.h() * itemHeight * _zoom));

                        bbox = bbox.margin(-itemMargin);
                        lightRects.append(QRectF(bbox.min.x, bbox.min.y, bbox.w(), bbox.h()));

                        bbox = bbox.margin(-itemBorder);
                        buttonRects.append(QRectF(bbox.min.x, bbox.min.y, bbox.w(), bbox.h()));

                        bbox = bbox.margin(-itemMargin);
                        const math::BBox2f nameBBox(
                            bbox.min.x,
                            bbox.min.y,
                            bbox.w() * .75F,
                            std::min(fl, bbox.h()));
                        if (clip.nameLabelSize.width() <= nameBBox.w() &&
                            clip.nameLabelSize.height() <= nameBBox.h())
                        {
                            nameRects.append(QRectF(nameBBox.min.x, nameBBox.min.y, nameBBox.w(), nameBBox.h()));
                            nameLabels.append(clip.nameLabel);
                        }

                        const math::BBox2f sourceRangeBBox(
                            bbox.min.x + bbox.w() * .75F,
                            bbox.min.y,
                            bbox.w() * .25F,
                            bbox.h() / 2.F);
                        if (clip.sourceRangeLabelSize.width() <= sourceRangeBBox.w() &&
                            clip.sourceRangeLabelSize.height() <= sourceRangeBBox.h())
                        {
                            nameRects.append(QRectF(
                                sourceRangeBBox.max.x - clip.sourceRangeLabelSize.width(),
                                sourceRangeBBox.min.y,
                                clip.sourceRangeLabelSize.width(),
                                sourceRangeBBox.h()));
                            nameLabels.append(clip.sourceRangeLabel);
                        }
                    }
                }
                painter.setPen(Qt::NoPen);
                painter.setBrush(palette.color(QPalette::ColorRole::Light));
                painter.drawRects(lightRects);
                painter.setBrush(palette.color(QPalette::ColorRole::Button));
                painter.drawRects(buttonRects);
                painter.setPen(palette.color(QPalette::ColorRole::WindowText));
                painter.setBrush(Qt::NoBrush);
                for (size_t i = 0; i < nameRects.size(); ++i)
                {
                    painter.drawText(
                        nameRects[i].x(),
                        nameRects[i].y() + fa,
                        nameLabels[i]);
                }
                for (size_t i = 0; i < sourceRangeRects.size(); ++i)
                {
                    painter.drawText(
                        sourceRangeRects[i].x(),
                        sourceRangeRects[i].y() + fa,
                        sourceRangeLabels[i]);
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
                _clips.clear();

                QFontMetrics fm(font());
                float y = 0.F;
                for (const auto& child : _otioTimeline->tracks()->children())
                {
                    if (auto otioTrack = dynamic_cast<otio::Track*>(child.value))
                    {
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
                                    item.bbox = math::BBox2f(
                                        trimmedRangeScaled.start_time().value(),
                                        y,
                                        trimmedRangeScaled.duration().value(),
                                        1.F);
                                    item.nameLabel = QString::fromUtf8(otioItem->name().c_str());
                                    if (auto otioClip = dynamic_cast<otio::Clip*>(otioItem))
                                    {
                                        if (item.nameLabel.isEmpty())
                                        {
                                            if (auto mediaRef = otioClip->media_reference())
                                            {
                                                item.nameLabel = QString::fromUtf8(mediaRef->name().c_str());
                                            }
                                        }
                                        item.nameLabelSize = fm.size(Qt::TextSingleLine, item.nameLabel);
                                        const auto sourceRangeOpt = otioClip->source_range();
                                        if (sourceRangeOpt.has_value())
                                        {
                                            const std::string label = string::Format("{0}:{1}@{2}").
                                                arg(sourceRangeOpt->start_time().value()).
                                                arg(sourceRangeOpt->duration().value()).
                                                arg(sourceRangeOpt->duration().rate());
                                            item.sourceRangeLabel = QString::fromUtf8(label.c_str());
                                        }
                                        item.sourceRangeLabelSize = fm.size(Qt::TextSingleLine, item.sourceRangeLabel);
                                        _clips.push_back(item);
                                    }
                                }
                            }
                        }
                    }

                    y += 1.F;
                }
            }

            void TimelineWidget::_scrollBarsUpdate()
            {
                const QSize viewportSize = viewport()->size();
                horizontalScrollBar()->setPageStep(viewportSize.width());
                verticalScrollBar()->setPageStep(viewportSize.height());
                horizontalScrollBar()->setRange(0, _duration.value() * itemWidth * _zoom - viewportSize.width());
                verticalScrollBar()->setRange(0, _tracks * itemHeight * _zoom - viewportSize.height());
            }
        }
    }
}
