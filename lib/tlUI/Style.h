// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/Color.h>
#include <tlCore/Context.h>
#include <tlCore/FontSystem.h>

namespace tl
{
    namespace ui
    {
        //! Size roles.
        enum class SizeRole
        {
            None,
            Margin,
            MarginSmall,
            MarginLarge,
            MarginInside,
            Spacing,
            SpacingSmall,
            SpacingLarge,
            SpacingTool,
            Border,
            ScrollArea,
            Handle,
            HandleSmall,

            Count,
            First = None
        };
        TLRENDER_ENUM(SizeRole);
        TLRENDER_ENUM_SERIALIZE(SizeRole);

        //! Color roles.
        enum class ColorRole
        {
            None,
            Window,
            Base,
            Button,
            Text,
            Border,
            Hover,
            Pressed,
            Checked,
            KeyFocus,
            Red,
            Green,
            Blue,
            Cyan,
            Magenta,
            Yellow,

            Count,
            First = None
        };
        TLRENDER_ENUM(ColorRole);
        TLRENDER_ENUM_SERIALIZE(ColorRole);

        //! Font roles.
        enum class FontRole
        {
            None,
            Label,
            Mono,
            Title,

            Count,
            First = None
        };
        TLRENDER_ENUM(FontRole);
        TLRENDER_ENUM_SERIALIZE(FontRole);

        //! Style.
        class Style : public std::enable_shared_from_this<Style>
        {
            TLRENDER_NON_COPYABLE(Style);

        protected:
            void _init(
                const std::shared_ptr<system::Context>&);

            Style();

        public:
            ~Style();

            //! Create a new style.
            static std::shared_ptr<Style> create(
                const std::shared_ptr<system::Context>&);

            //! Get a size role.
            int getSizeRole(SizeRole, float scale) const;

            //! Get a color role.
            imaging::Color4f getColorRole(ColorRole) const;

            //! Get a font role.
            imaging::FontInfo getFontRole(FontRole, float scale) const;

        private:
            TLRENDER_PRIVATE();
        };
    }
}
