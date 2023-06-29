// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2023 Darby Johnston
// All rights reserved.

#include <tlUI/Menu.h>

namespace tl
{
    namespace play_gl
    {
        class App;

        //! Render menu.
        class RenderMenu : public ui::Menu
        {
            TLRENDER_NON_COPYABLE(RenderMenu);

        protected:
            void _init(
                const std::shared_ptr<App>&,
                const std::shared_ptr<system::Context>&);

            RenderMenu();

        public:
            ~RenderMenu();

            static std::shared_ptr<RenderMenu> create(
                const std::shared_ptr<App>&,
                const std::shared_ptr<system::Context>&);

            void close() override;

        private:
            TLRENDER_PRIVATE();
        };
    }
}
