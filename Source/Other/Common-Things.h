// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>

using namespace std::chrono;
using namespace std::chrono_literals;

using u8 = std::uint_fast8_t;
using u16 = std::uint_fast16_t;
using u32 = std::uint_fast32_t;
using u64 = std::uint_fast64_t;

using s8 = std::int_fast8_t;
using s16 = std::int_fast16_t;
using s32 = std::int_fast32_t;
using s64 = std::int_fast64_t;

using std::cerr;
using std::clog;
using std::string;

using sf::RenderWindow;
using Kb = sf::Keyboard;

enum Button
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
    TOGGLE_FULLSCREEN,
    EXIT
};

enum Text
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

enum Global_Scale
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

enum Opcode
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

enum Language
{
    ENGLISH,
    GERMAN,
    FRENCH,
    SPANISH
};

enum Window_Mode
{
    WIN_NORMAL,
    WIN_BORDERLESS,
    WIN_FULLSCREEN
};

enum Inactive_Mode
{
    PAUSE,
    RUN_WITHOUT_INPUT,
    RUN_WITH_INPUT
};

enum Frame_Limiter_Mode
{
    SLEEPING,
    BUSY_WAITING
};

struct Score
{
    u8 points[2];
};

struct Option_Switch
{
    Language language         : 2;
    u8 starting_lives         : 1;
    u8 center_coin_multiplier : 1;
    u8 right_coin_multiplier  : 2;
    u8 coinage                : 2;
};

struct Offset
{
    u8 minor;
    u8 major;
};

bool operator> (Score score_1, Score score_2);
bool operator<=(Score score_1, Score score_2);

bool overflowed_u8(u8 var_current, u8 var_previous);
bool underflowed_u8(u8 var_current, u8 var_previous);
u8 sbc(u8 num_1, u8 num_2, bool& carry);
u8 ror(u8 num, bool carry);
u8 random_byte();
s8 clamp_s8(s8 value, s8 min_value, s8 max_value);
void add_to_number(u8 number[], u8 num_size, u8 to_add, bool bonus);
bool add_BCD(u8& number, u8 to_add, bool overflow);
u8 twos_complement(u8 number);
