// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#include <tlUI/PushButton.h>

#include <tlUI/DrawUtil.h>

namespace tl
{
    namespace ui
    {
        struct PushButton::Private
        {
            struct Size
            {
                imaging::FontInfo fontInfo;
                imaging::FontMetrics fontMetrics;
                math::Vector2i textSize;
                int margin = 0;
                int margin2 = 0;
                int border = 0;
            };
            Size size;
        };

        void PushButton::_init(
            const std::shared_ptr<system::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            IButton::_init("tl::ui::PushButton", context, parent);
        }

        PushButton::PushButton() :
            _p(new Private)
        {}

        PushButton::~PushButton()
        {}

        std::shared_ptr<PushButton> PushButton::create(
            const std::shared_ptr<system::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<PushButton>(new PushButton);
            out->_init(context, parent);
            return out;
        }

        void PushButton::sizeEvent(const SizeEvent& event)
        {
            IButton::sizeEvent(event);
            TLRENDER_P();

            p.size.margin = event.style->getSizeRole(SizeRole::MarginSmall) * event.contentScale;
            p.size.margin2 = event.style->getSizeRole(SizeRole::MarginInside) * event.contentScale;
            p.size.border = event.style->getSizeRole(SizeRole::Border) * event.contentScale;

            _sizeHint.x = 0;
            _sizeHint.y = 0;
            if (!_text.empty())
            {
                p.size.fontInfo = _fontInfo;
                p.size.fontInfo.size *= event.contentScale;
                p.size.fontMetrics = event.fontSystem->getMetrics(p.size.fontInfo);
                p.size.textSize = event.fontSystem->measure(_text, p.size.fontInfo);

                _sizeHint.x = event.fontSystem->measure(_text, p.size.fontInfo).x + p.size.margin2 * 2;
                _sizeHint.y = p.size.fontMetrics.lineHeight;
            }
            if (_iconImage)
            {
                _sizeHint.x += _iconImage->getWidth();
                _sizeHint.y = std::max(
                    _sizeHint.y,
                    static_cast<int>(_iconImage->getHeight()));
            }
            _sizeHint.x += p.size.margin * 2 * 2;
            _sizeHint.y += p.size.margin2 * 2;
        }

        void PushButton::drawEvent(const DrawEvent& event)
        {
            IButton::drawEvent(event);
            TLRENDER_P();

            math::BBox2i g = _geometry;

            event.render->drawMesh(
                border(g, p.size.border, p.size.margin / 2),
                event.style->getColorRole(ColorRole::Border));

            const auto mesh = rect(g.margin(-p.size.border), p.size.margin / 2);
            const ColorRole colorRole = _checked->get() ?
                ColorRole::Checked :
                _buttonRole;
            if (colorRole != ColorRole::None)
            {
                event.render->drawMesh(
                    mesh,
                    event.style->getColorRole(colorRole));
            }

            if (_pressed && _geometry.contains(_cursorPos))
            {
                event.render->drawMesh(
                    mesh,
                    event.style->getColorRole(ColorRole::Pressed));
            }
            else if (_inside)
            {
                event.render->drawMesh(
                    mesh,
                    event.style->getColorRole(ColorRole::Hover));
            }

            int x = g.x() + p.size.margin * 2;
            if (_iconImage)
            {
                const auto iconSize = _iconImage->getSize();
                event.render->drawImage(
                  _iconImage,
                  math::BBox2i(x, g.y() + p.size.margin2, iconSize.w, iconSize.h));
                x += iconSize.w;
            }
            
            if (!_text.empty())
            {
                math::Vector2i pos(
                    x + (g.max.x - p.size.margin * 2 - x) / 2 - p.size.textSize.x / 2,
                    g.y() + g.h() / 2 - p.size.textSize.y / 2 + p.size.fontMetrics.ascender);
                event.render->drawText(
                    event.fontSystem->getGlyphs(_text, p.size.fontInfo),
                    pos,
                    event.style->getColorRole(ColorRole::Text));
            }
        }
    }
}
