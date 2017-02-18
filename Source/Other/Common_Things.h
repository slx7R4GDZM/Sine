// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

using kb = sf::Keyboard;

using namespace std::chrono;
using namespace std::chrono_literals;

using u8 = uint_fast8_t;
using u16 = uint_fast16_t;
using u32 = uint_fast32_t;
using u64 = uint_fast64_t;

using s8 = int_fast8_t;
using s16 = int_fast16_t;
using s32 = int_fast32_t;
using s64 = int_fast64_t;

using std::cerr;
using std::clog;
using std::string;

enum Button : u8
{
    HYPERSPACE,
    FIRE,
    LEFT_COIN,
    CENTER_COIN,
    RIGHT_COIN,
    ONE_PLAYER_START,
    TWO_PLAYER_START,
    THRUST,
    ROTATE_RIGHT,
    ROTATE_LEFT,
    EXIT
};

enum Text : u8
{
    HIGH_SCORES,
    PLAYER_,
    YOUR_SCORE_IS_ONE_OF_THE_TEN_BEST,
    PEASE_ENTER_YOUR_INITIALS,
    PUSH_ROTATE_TO_SELECT_LETTER,
    PUSH_HYPERSPACE_WHEN_LETTER_IS_CORRECT,
    PUSH_START,
    GAME_OVER,
    ONE_COIN_2_PLAYS,
    ONE_COIN_1_PLAY,
    TWO_COINS_1_PLAY
};

enum Scale : u8
{
	MUL_1,
	MUL_2,
	MUL_4,
	MUL_8,
	MUL_16,
	MUL_32,
	MUL_64,
	MUL_128,
	DIV_256,
	DIV_128,
	DIV_64,
	DIV_32,
	DIV_16,
	DIV_8,
	DIV_4,
	DIV_2
};

enum Opcode : u8
{
	VCTR_0,
	VCTR_1,
	VCTR_2,
	VCTR_3,
	VCTR_4,
	VCTR_5,
	VCTR_6,
	VCTR_7,
	VCTR_8,
	VCTR_9,
	LABS,
	HALT,
	JSRL,
	RTSL,
	JMPL,
	SVEC
};

enum Language : u8
{
    ENGLISH,
    GERMAN,
    FRENCH,
    SPANISH
};

enum Window_Mode : u8
{
    WIN_NORMAL,
    WIN_BORDERLESS,
    WIN_FULLSCREEN
};

enum Inactive_Mode : u8
{
    PAUSE,
    RUN_WITHOUT_INPUT,
    RUN_WITH_INPUT
};

enum Frame_Limiter_Mode : u8
{
    SLEEPING,
    BUSY_WAITING
};

/*struct Velocity {
    s8 x_major;
    s8 y_major;
};*/

struct Position
{
    u8 x_major;
    u8 y_major;
    u8 x_minor;
    u8 y_minor;
};

struct Coordinate
{
    u16 position_x;
    u16 position_y;
};

struct Option_Switch
{
    Language language         : 2;
    u8 starting_lives         : 1;
    u8 center_coin_multiplier : 1;
    u8 right_coin_multiplier  : 2;
    u8 coinage                : 2;
};

static bool overflowed_u8(const u8 var_current, const u8 var_previous)
{
    return var_current < var_previous ? 1 : 0;
}

// underflow is not the right term for this, better name would be negative_overflowed
static bool underflowed_u8(const u8 var_current, const u8 var_previous)
{
    return var_current > var_previous ? 1 : 0;
}

static u8 random_byte()
{
    return rand() % 256;
}

static s8 clamp_s8(const s8 value, const s8 min_value, const s8 max_value)
{
    if (value < min_value)
        return min_value;
    else if (value > max_value)
        return max_value;
    else
        return value;
}

static Coordinate get_total_pos(const Position& pos)
{
    Coordinate totalPos;
    totalPos.position_x = pos.x_major * 256 + pos.x_minor;
    totalPos.position_y = pos.y_major * 256 + pos.y_minor;
    return totalPos;
}
