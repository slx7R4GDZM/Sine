// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Common-Things.h"

#include <cstdlib>
#include <random>

bool operator>(Score score_1, Score score_2)
{
    for (u8 i = 1; i < 2; i--)
    {
        if (score_1.points[i] > score_2.points[i])
            return true;
        if (score_1.points[i] < score_2.points[i])
            return false;
    }

    // scores are the same
    return false;
}

bool operator<=(Score score_1, Score score_2)
{
    return !(score_1 > score_2);
}

bool overflowed_u8(u8 var_current, u8 var_previous)
{
    return var_current < var_previous;
}

// underflow is not the right term for this, better name would be negative_overflowed
bool underflowed_u8(u8 var_current, u8 var_previous)
{
    return var_current > var_previous;
}

float random(float minimum, float maximum, bool inclusive)
{
    if (inclusive)
        maximum = std::nextafter(maximum, maximum + 1);

    static std::uniform_real_distribution<float> distribution(minimum, maximum);
    static std::mt19937_64 generator(std::time(nullptr));

    return distribution(generator);
}

u8 random_u8()
{
    static std::uniform_int_distribution<u8> distribution(0, UINT8_MAX);
    static std::mt19937_64 generator(std::time(nullptr));

    return distribution(generator);
}

float clamp(float value, float min_value, float max_value)
{
    if (value < min_value)
        return min_value;
    if (value > max_value)
        return max_value;

    return value;
}

void add_to_number(u8 number[], u8 num_size, u8 to_add, bool bonus)
{
    bool add_to_next = add_BCD(number[0], to_add, bonus);
    for (u8 i = 1; i < num_size && add_to_next; i++)
        add_to_next = add_BCD(number[i], 0, add_to_next);
}

// this uses binary-coded decimal
bool add_BCD(u8& number, u8 to_add, bool overflow)
{
    u16 new_number = number + to_add + overflow;
    if ((number & 0xF) + (to_add & 0xF) + overflow > 9)
        new_number += 6;

    if (new_number > 0x99)
        new_number += 0x60;

    number = new_number;

    // overflow
    return new_number > 0x99;
}

u8 twos_complement(u8 number)
{
    return ~number + 1;
}