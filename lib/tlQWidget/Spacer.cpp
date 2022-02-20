// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include <tlQWidget/Spacer.h>

namespace tl
{
    namespace qwidget
    {
        Spacer::Spacer(Qt::Orientation orientation, QWidget* parent) :
            QFrame(parent),
            _orientation(orientation)
        {
            _widgetUpdate();
        }
        
        Spacer::~Spacer()
        {}

        void Spacer::setOrientation(Qt::Orientation value)
        {
            if (value == _orientation)
                return;
            _orientation = value;
            _widgetUpdate();
        }

        void Spacer::_widgetUpdate()
        {
            switch (_orientation)
            {
            case Qt::Horizontal:
                setMinimumWidth(10);
                setMinimumHeight(1);
                break;
            case Qt::Vertical:
                setMinimumWidth(1);
                setMinimumHeight(10);
                break;
            default: break;
            }
        }
    }
}
