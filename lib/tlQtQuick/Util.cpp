// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include <tlQtQuick/Util.h>

#include <tlQtQuick/GLFramebufferObject.h>

#include <tlQt/Util.h>

using namespace tl::core;

namespace tl
{
    namespace qt
    {
        namespace quick
        {
            void init()
            {
                qt::init();

                qmlRegisterType<GLFramebufferObject>("tlQuick", 1, 0, "GLFramebufferObject");

                QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
            }

            namespace
            {
                std::weak_ptr<Context> _context;
            }

            void setContext(const std::shared_ptr<Context>& context)
            {
                _context = context;
            }

            const std::weak_ptr<Context>& context()
            {
                return _context;
            }
        }
    }
}
