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

bool operator>(const Score& score_1, const Score& score_2)
{
    for (u8 i = 1; i != UINT8_MAX; i--)
    {
        if (score_1.points[i] > score_2.points[i])
            return true;
        else if (score_1.points[i] < score_2.points[i])
            return false;
    }

    // scores are the same
    return false;
}
