// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Common-Things.h"

#include <cstdlib>

bool overflowed_u8(const u8 var_current, const u8 var_previous)
{
    return var_current < var_previous;
}

// underflow is not the right term for this, better name would be negative_overflowed
bool underflowed_u8(const u8 var_current, const u8 var_previous)
{
    return var_current > var_previous;
}

u8 random_byte()
{
    return std::rand() % 256;
}

s8 clamp_s8(const s8 value, const s8 min_value, const s8 max_value)
{
    if (value < min_value)
        return min_value;
    else if (value > max_value)
        return max_value;
    else
        return value;
}

Coordinate get_total_pos(const Position& pos)
{
    const Coordinate total_pos =
    {
        static_cast<u16>(pos.x_major * 256 + pos.x_minor),
        static_cast<u16>(pos.y_major * 256 + pos.y_minor)
    };
    return total_pos;
}
