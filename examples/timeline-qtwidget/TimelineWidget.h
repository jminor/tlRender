// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/Time.h>
#include <tlCore/BBox.h>

#include <opentimelineio/timeline.h>

#include <QAbstractScrollArea >

namespace tl
{
    namespace examples
    {
        namespace timeline_qtwidget
        {
            //! Timeline widget.
            class TimelineWidget : public QAbstractScrollArea
            {
                Q_OBJECT

            public:
                TimelineWidget(QWidget* parent = nullptr);

                void setTimeline(const otio::SerializableObject::Retainer<otio::Timeline>&);

                const math::Vector2f& zoom() const;

            public Q_SLOTS:
                void setZoom(const math::Vector2f&);

            protected:
                void resizeEvent(QResizeEvent*) override;
                void paintEvent(QPaintEvent*) override;

            private:
                void _itemsUpdate();
                void _scrollBarsUpdate();

                otio::SerializableObject::Retainer<otio::Timeline> _otioTimeline;
                otime::RationalTime _duration;
                int _tracks = 0;
                math::Vector2f _zoom = math::Vector2f(100.F, 100.F);

                enum class ItemType
                {
                    None,
                    Clip,
                    Gap
                };
                struct Item
                {
                    otio::Item* p = nullptr;
                    ItemType type = ItemType::None;
                    otime::TimeRange trimmedRangeScaled;
                };
                std::vector<std::vector<Item> > _items;
            };
        }
    }
}
