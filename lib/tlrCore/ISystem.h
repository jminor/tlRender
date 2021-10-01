// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021 Darby Johnston
// All rights reserved.

#pragma once

#include <tlrCore/LogSystem.h>

namespace tlr
{
    namespace core
    {
        class LogSystem;

        //! Base class for systems.
        class ISystem : public ICoreSystem
        {
        protected:
            void _init(
                const std::string& name,
                const std::shared_ptr<Context>&);
            ISystem();

        public:
            ~ISystem() override;

        protected:
            void _log(const std::string&, LogType = LogType::Message);

        private:
            std::weak_ptr<LogSystem> _logSystem;
        };
    }
}
