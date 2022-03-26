// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/Time.h>
#include <tlCore/BBox.h>

#include <opentimelineio/timeline.h>

#include <QAbstractScrollArea>
#include <QAction>

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

                float zoom() const;
                math::Vector2f viewportCenter() const;

            public Q_SLOTS:
                void setZoom(float, const math::Vector2f& focus);
                void zoomReset();
                void zoomIn();
                void zoomOut();

            Q_SIGNALS:
                void zoomChanged(float);

            protected:
                void resizeEvent(QResizeEvent*) override;
                void paintEvent(QPaintEvent*) override;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
                void enterEvent(QEvent*) override;
#else
                void enterEvent(QEnterEvent*) override;
#endif
                void leaveEvent(QEvent*) override;
                void mousePressEvent(QMouseEvent*) override;
                void mouseReleaseEvent(QMouseEvent*) override;
                void mouseMoveEvent(QMouseEvent*) override;
                void wheelEvent(QWheelEvent*) override;

            private:
                void _itemsUpdate();
                void _scrollBarsUpdate();

                otio::SerializableObject::Retainer<otio::Timeline> _otioTimeline;
                otime::RationalTime _duration;
                int _tracks = 0;
                float _zoom = 100.F;

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
                    math::BBox2f bbox;
                };
                std::vector<std::vector<Item> > _items;

                QMap<QString, QAction*> _actions;

                bool _mouseInside = false;
                bool _mousePressed = false;
                math::Vector2f _mousePos;
                math::Vector2f _mousePress;
                math::Vector2f _scrollMousePress;
            };
        }
    }
}
