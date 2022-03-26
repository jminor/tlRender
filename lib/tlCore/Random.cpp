// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include <tlCore/Random.h>

#include <cstdlib>

namespace tl
{
    namespace math
    {
        int random(const IntRange& range)
        {
            return std::rand() / static_cast<float>(RAND_MAX) * (range.getMax() - range.getMin() + 1) + range.getMin();
        }

        float random(const FloatRange& range)
        {
            return std::rand() / static_cast<float>(RAND_MAX) * (range.getMax() - range.getMin()) + range.getMin();
        }
    }
}