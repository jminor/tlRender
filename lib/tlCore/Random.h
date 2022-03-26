// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#pragma once

#include <tlCore/Range.h>

namespace tl
{
    namespace math
    {
        //! Get a random number.
        int random(const IntRange&);

        //! Get a random number.
        float random(const FloatRange&);
    }
}
