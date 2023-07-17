// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#include <tlPlayGLApp/Tools.h>

#include <tlCore/Error.h>
#include <tlCore/String.h>

#include <array>
#include <iostream>

namespace tl
{
    namespace play_gl
    {
        TLRENDER_ENUM_IMPL(
            Tool,
            "Files",
            "Compare",
            "Color",
            "Info",
            "Audio",
            "Devices",
            "Settings",
            "Messages",
            "SystemLog");
        TLRENDER_ENUM_SERIALIZE_IMPL(Tool);

        std::string getText(Tool value)
        {
            const std::array<std::string, static_cast<size_t>(Tool::Count)> data =
            {
                "Files",
                "Compare",
                "Color",
                "Information",
                "Audio",
                "Devices",
                "Settings",
                "Messages",
                "System Log"
            };
            return data[static_cast<size_t>(value)];
        }

        std::string getIcon(Tool value)
        {
            const std::array<std::string, static_cast<size_t>(Tool::Count)> data =
            {
                "Files",
                "Compare",
                "Color",
                "Info",
                "Audio",
                "Devices",
                "Settings",
                "Messages",
                ""
            };
            return data[static_cast<size_t>(value)];
        }

        ui::Key getShortcut(Tool value)
        {
            const std::array<ui::Key, static_cast<size_t>(Tool::Count)> data =
            {
                ui::Key::F1,
                ui::Key::F2,
                ui::Key::F3,
                ui::Key::F4,
                ui::Key::F5,
                ui::Key::F6,
                ui::Key::F9,
                ui::Key::F10,
                ui::Key::F11
            };
            return data[static_cast<size_t>(value)];
        }

        struct ToolsModel::Private
        {
            std::shared_ptr<observer::Map<Tool, bool> > toolsVisible;
        };

        void ToolsModel::_init()
        {
            TLRENDER_P();
            p.toolsVisible = observer::Map<Tool, bool>::create();
        }

        ToolsModel::ToolsModel() :
            _p(new Private)
        {}

        ToolsModel::~ToolsModel()
        {}

        std::shared_ptr<ToolsModel> ToolsModel::create()
        {
            auto out = std::shared_ptr<ToolsModel>(new ToolsModel);
            out->_init();
            return out;
        }

        const std::map<Tool, bool>& ToolsModel::getToolsVisible() const
        {
            return _p->toolsVisible->get();
        }

        std::shared_ptr<observer::IMap<Tool, bool> > ToolsModel::observeToolsVisible() const
        {
            return _p->toolsVisible;
        }

        void ToolsModel::setToolVisible(Tool tool, bool value)
        {
            TLRENDER_P();
            auto visible = p.toolsVisible->get();
            visible[tool] = value;
            p.toolsVisible->setIfChanged(visible);
        }
    }
}
