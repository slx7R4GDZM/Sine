// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_COMMON_THINGS_HC
#define SINE_COMMON_THINGS_HC

#define HYPERSPACE        0
#define FIRE              1
#define LEFT_COIN         2
#define CENTER_COIN       3
#define RIGHT_COIN        4
#define ONE_PLAYER_START  5
#define TWO_PLAYER_START  6
#define THRUST            7
#define ROTATE_RIGHT      8
#define ROTATE_LEFT       9
#define UPDATE_WIN_SIZE  10
#define EXIT             11

#define HIGH_SCORES                             0
#define PLAYER_                                 1
#define YOUR_SCORE_IS_ONE_OF_THE_TEN_BEST       2
#define PEASE_ENTER_YOUR_INITIALS               3
#define PUSH_ROTATE_TO_SELECT_LETTER            4
#define PUSH_HYPERSPACE_WHEN_LETTER_IS_CORRECT  5
#define PUSH_START                              6
#define GAME_OVER                               7
#define ONE_COIN_2_PLAYS                        8
#define ONE_COIN_1_PLAY                         9
#define TWO_COINS_1_PLAY                       10

#define MUL_1    0
#define MUL_2    1
#define MUL_4    2
#define MUL_8    3
#define MUL_16   4
#define MUL_32   5
#define MUL_64   6
#define MUL_128  7
#define DIV_256  8
#define DIV_128  9
#define DIV_64  10
#define DIV_32  11
#define DIV_16  12
#define DIV_8   13
#define DIV_4   14
#define DIV_2   15

#define VCTR_0  0
#define VCTR_1  1
#define VCTR_2  2
#define VCTR_3  3
#define VCTR_4  4
#define VCTR_5  5
#define VCTR_6  6
#define VCTR_7  7
#define VCTR_8  8
#define VCTR_9  9
#define LABS   10
#define HALT   11
#define JSRL   12
#define RTSL   13
#define JMPL   14
#define SVEC   15

#define ENGLISH 0
#define GERMAN  1
#define FRENCH  2
#define SPANISH 3

#define WIN_NORMAL     0
#define WIN_BORDERLESS 1

#define SLEEPING     0
#define BUSY_WAITING 1

class Vector2I16
{
    I16 x;
    I16 y;
};

class Vector2U16
{
    U16 x;
    U16 y;
};

class Score
{
    U8 points[2];
};

class Offset
{
    U8 minor;
    U8 major;
};

class Velocity
{
    I8 x;
    I8 y;
};

Bool overflowed_U8(U8 var_current, U8 var_previous)
{
    return var_current < var_previous;
}

// underflow is not the right term for this, better name would be negative_overflowed
Bool underflowed_U8(U8 var_current, U8 var_previous)
{
    return var_current > var_previous;
}

U8 random_byte()
{
    return RandU16() & 0xFF;
}

I8 clamp_I8(I8 value, I8 min_value, I8 max_value)
{
    if (value < min_value)
        return min_value;
    else if (value > max_value)
        return max_value;
    else
        return value;
}

Bool score_more_than(Score score_1, Score score_2)
{
    U64 i;
    for (i = 1; i < 2; i--)
    {
        if (score_1.points[i] > score_2.points[i])
            return TRUE;
        else if (score_1.points[i] < score_2.points[i])
            return FALSE;
    }

    // scores are the same
    return FALSE;
}

#endif
