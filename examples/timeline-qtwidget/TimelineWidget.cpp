// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "TimelineWidget.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>

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
            {}

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

            const math::Vector2f& TimelineWidget::zoom() const
            {
                return _zoom;
            }

            void TimelineWidget::setZoom(const math::Vector2f& value)
            {
                if (value == _zoom)
                    return;
                _zoom = value;
                _scrollBarsUpdate();
                viewport()->update();
            }

            void TimelineWidget::resizeEvent(QResizeEvent*)
            {
                _scrollBarsUpdate();
            }

            void TimelineWidget::paintEvent(QPaintEvent*)
            {
                QPainter painter(viewport());

                const float itemHeight = 1.F;
                const int w = viewport()->width();
                const int hs = horizontalScrollBar()->value();
                const int vs = verticalScrollBar()->value();

                const otime::TimeRange viewRange(
                    otime::RationalTime(hs / _zoom.x, 1.0),
                    otime::RationalTime(w / _zoom.x, 1.0));

                painter.setPen(QPen(QColor(0, 0, 0), 1));
                painter.setBrush(QColor(255, 0, 0));
                float y = 0.F;
                for (const auto& row : _items)
                {
                    for (const auto& item : row)
                    {
                        if (ItemType::Clip == item.type && viewRange.intersects(item.trimmedRangeScaled))
                        {
                            const math::BBox2f bbox(
                                (item.trimmedRangeScaled.start_time().value() - hs / _zoom.x) * _zoom.x,
                                (y - vs / _zoom.y) * _zoom.y,
                                item.trimmedRangeScaled.duration().value() * _zoom.x,
                                itemHeight * _zoom.y);

                            painter.drawRect(bbox.min.x, bbox.min.y, bbox.w(), bbox.h());
                        }
                    }

                    y += itemHeight;
                }
            }

            void TimelineWidget::_itemsUpdate()
            {
                _items.clear();

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
                                    item.trimmedRangeScaled = trimmedRangeScaled;
                                    items.push_back(item);
                                }
                            }
                        }
                        _items.push_back(items);
                    }
                }
            }

            void TimelineWidget::_scrollBarsUpdate()
            {
                const auto viewportSize = viewport()->size();
                horizontalScrollBar()->setPageStep(viewportSize.width());
                verticalScrollBar()->setPageStep(viewportSize.height());
                horizontalScrollBar()->setRange(0, _duration.value() * _zoom.x - viewportSize.width());
                verticalScrollBar()->setRange(0, _tracks * _zoom.y - viewportSize.height());
            }
        }
    }
}
