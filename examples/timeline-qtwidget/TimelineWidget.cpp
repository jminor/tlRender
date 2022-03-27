// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <tlCore/Math.h>
#include <tlCore/Range.h>
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
                const math::FloatRange zoomRange(.1F, 10.F);

                const float itemWidth = 100.F;
                const float itemHeight = 100.F;
                const float itemSpacing = 0.F;
                const float itemMargin = 2.F;
                const float itemBorder = 2.F;
                const float textMargin = 4.F;

                const QColor itemDefaultColor = QColor(60, 60, 60);
                const QColor clipColor = QColor(90, 90, 150);
                const QColor timeBackgroundColor = QColor(0, 0, 0);
            }

            TimelineWidget::TimelineWidget(QWidget* parent) :
                QAbstractScrollArea(parent)
            {
                setMouseTracking(true);
                setFocusPolicy(Qt::StrongFocus);

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
                const float tmp = math::clamp(zoom, zoomRange.getMin(), zoomRange.getMax());
                if (tmp == _zoom)
                    return;

                const float th = _timeIndicatorHeight();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();

                math::Vector2i scroll;
                scroll.x = (hs + focus.x) * (tmp / _zoom) - focus.x;
                scroll.y = (vs + focus.y - th) * (tmp / _zoom) - (focus.y - th);

                _zoom = tmp;

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
                painter.setFont(font());

                const int w = viewport()->width();
                const int h = viewport()->height();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();
                const math::BBox2f viewportBBox(hs, vs, w, h);

                _tracksPaint(painter, viewportBBox);
                _itemsPaint(painter, viewportBBox);
                _transitionsPaint(painter, viewportBBox);
                _timeIndicatorPaint(painter, viewportBBox);
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

            float TimelineWidget::_timeIndicatorHeight() const
            {
                return QFontMetrics(font()).lineSpacing();
            }

            void TimelineWidget::_tracksPaint(QPainter& painter, const math::BBox2f& viewportBBox)
            {}

            void TimelineWidget::_itemsPaint(QPainter& painter, const math::BBox2f& viewportBBox)
            {
                const QPalette palette = this->palette();
                QFontMetrics fm(font());
                const float fl = fm.lineSpacing();
                const float fa = fm.ascent();

                const float th = _timeIndicatorHeight();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();

                for (auto& item : _items)
                {
                    const math::BBox2f bbox = math::BBox2f(
                        (item.range.start_time().value() * itemWidth) * _zoom,
                        th + (item.track * (itemHeight + itemSpacing)) * _zoom,
                        (item.range.duration().value() * itemWidth) * _zoom,
                        itemHeight * _zoom);
                    if (bbox.intersects(viewportBBox))
                    {
                        const math::BBox2f marginBBox = math::BBox2f(
                            math::Vector2f(bbox.min.x - hs, bbox.min.y - vs),
                            math::Vector2f(bbox.max.x - hs, bbox.max.y - vs)).
                            margin(-itemMargin);
                        if (marginBBox.isValid())
                        {
                            painter.fillRect(
                                marginBBox.min.x,
                                marginBBox.min.y,
                                marginBBox.w(),
                                marginBBox.h(),
                                item.color.lighter(120));
                        }

                        const math::BBox2f borderBBox = marginBBox.margin(-itemBorder);
                        if (borderBBox.isValid())
                        {
                            painter.fillRect(
                                borderBBox.min.x,
                                borderBBox.min.y,
                                borderBBox.w(),
                                borderBBox.h(),
                                item.color);
                        }

                        painter.setPen(palette.color(QPalette::ColorRole::WindowText));
                        painter.setBrush(Qt::NoBrush);

                        const math::BBox2f insideBBox = borderBBox.margin(-textMargin);
                        const math::BBox2f nameBBox(
                            insideBBox.min.x,
                            insideBBox.min.y,
                            insideBBox.w() * .75F,
                            std::min(fl, insideBBox.h()));
                        const auto& nameLabel = item.labels["Name"];
                        if (nameLabel.second.width() <= nameBBox.w() &&
                            nameLabel.second.height() <= nameBBox.h())
                        {
                            painter.drawText(
                                nameBBox.min.x,
                                nameBBox.min.y + fa,
                                nameLabel.first);
                        }

                        const math::BBox2f sourceRangeBBox(
                            insideBBox.min.x + insideBBox.w() * .75F,
                            insideBBox.min.y,
                            insideBBox.w() * .25F,
                            insideBBox.h() / 2.F);
                        const auto& sourceRangeLabel = item.labels["SourceRange"];
                        if (sourceRangeLabel.second.width() <= sourceRangeBBox.w() &&
                            sourceRangeLabel.second.height() <= sourceRangeBBox.h())
                        {
                            painter.drawText(
                                sourceRangeBBox.max.x - sourceRangeLabel.second.width(),
                                sourceRangeBBox.min.y + fa,
                                sourceRangeLabel.first);
                        }
                    }
                }
            }

            void TimelineWidget::_transitionsPaint(QPainter& painter, const math::BBox2f& viewportBBox)
            {}

            void TimelineWidget::_timeIndicatorPaint(QPainter& painter, const math::BBox2f& viewportBBox)
            {
                const QPalette palette = this->palette();
                QFontMetrics fm(font());
                const float fa = fm.ascent();

                const float th = _timeIndicatorHeight();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();
                const math::BBox2f viewportBBoxScaled(
                    math::Vector2f(viewportBBox.min.x / itemWidth / _zoom, viewportBBox.min.y / itemHeight / _zoom),
                    math::Vector2f(viewportBBox.max.x / itemWidth / _zoom, viewportBBox.max.y / itemHeight / _zoom));

                painter.fillRect(0, 0, viewport()->width(), th, timeBackgroundColor);

                for (float x = floorf(viewportBBoxScaled.min.x); x < ceilf(viewportBBoxScaled.max.x); x += 1.F)
                {
                    const math::BBox2f bbox(
                        x * itemWidth * _zoom - hs,
                        0.F,
                        itemWidth * _zoom,
                        th);

                    painter.setPen(Qt::NoPen);
                    painter.fillRect(
                        bbox.min.x,
                        bbox.min.y,
                        1.F,
                        bbox.h(),
                        palette.color(QPalette::ColorRole::WindowText));

                    const math::BBox2f textBBox(
                        bbox.min.x + 1.F + textMargin,
                        bbox.min.y,
                        bbox.w() - 1.F - textMargin,
                        bbox.h());
                    std::string label = string::Format("{0}").arg(x);
                    QString qlabel = QString::fromUtf8(label.c_str());
                    QSize qsize = fm.size(Qt::TextSingleLine, qlabel);
                    if (qsize.width() <= textBBox.w())
                    {
                        painter.setPen(palette.color(QPalette::ColorRole::WindowText));
                        painter.setBrush(Qt::NoBrush);
                        painter.drawText(
                            textBBox.min.x,
                            textBBox.min.y + fa,
                            qlabel);
                    }
                }
            }

            void TimelineWidget::_itemsUpdate()
            {
                _items.clear();

                QFontMetrics fm(font());
                int track = 0;
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
                                    item.range = trimmedRangeScaled;
                                    item.track = track;
                                    QColor color = itemDefaultColor;
                                    std::string name = otioItem->name();
                                    if (auto otioClip = dynamic_cast<otio::Clip*>(otioItem))
                                    {
                                        color = clipColor;
                                        if (name.empty())
                                        {
                                            if (auto mediaRef = otioClip->media_reference())
                                            {
                                                name = mediaRef->name().c_str();
                                            }
                                        }
                                    }
                                    item.color = color;
                                    std::string label = otioItem->schema_name();
                                    if (!name.empty())
                                    {
                                        label.append(string::Format(": {0}").arg(name));
                                    }
                                    item.labels["Name"].first = QString::fromUtf8(label.c_str());
                                    item.labels["Name"].second = fm.size(Qt::TextSingleLine, item.labels["Name"].first);
                                    const auto sourceRangeOpt = otioItem->source_range();
                                    if (sourceRangeOpt.has_value())
                                    {
                                        label = string::Format("{0},{1}@{2}").
                                            arg(sourceRangeOpt->start_time().value()).
                                            arg(sourceRangeOpt->duration().value()).
                                            arg(sourceRangeOpt->duration().rate());
                                        item.labels["SourceRange"].first = QString::fromUtf8(label.c_str());
                                    }
                                    item.labels["SourceRange"].second = fm.size(Qt::TextSingleLine, item.labels["SourceRange"].first);
                                    _items.push_back(item);
                                }
                            }
                        }

                        ++track;
                    }
                }
            }

            void TimelineWidget::_scrollBarsUpdate()
            {
                const QSize viewportSize = viewport()->size();
                const float th = _timeIndicatorHeight();

                horizontalScrollBar()->setPageStep(viewportSize.width());
                verticalScrollBar()->setPageStep(viewportSize.height());
                horizontalScrollBar()->setRange(
                    0,
                    _duration.value() * itemWidth * _zoom - viewportSize.width());
                verticalScrollBar()->setRange(
                    0,
                    th + (_tracks * itemHeight + (_tracks - 1) * itemSpacing) * _zoom - viewportSize.height());
            }
        }
    }
}
