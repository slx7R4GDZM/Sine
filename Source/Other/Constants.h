// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "Common-Things.h"

const nanoseconds TARGET_FRAME_TIME = 16666666ns;

const string SETTINGS_FILENAME = "Sine-Settings.cfg";

const string KEY_TABLE[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Escape", "LControl", "LShift", "LAlt", "LSystem", "RControl", "RShift", "RAlt", "RSystem", "Menu", "LBracket", "RBracket", "Semicolon", "Comma", "Period", "Quote", "Slash", "Backslash", "Tilde", "Equal", "Hyphen", "Space", "Enter", "Backspace", "Tab", "PageUp", "PageDown", "End", "Home", "Insert", "Delete", "Add", "Subtract", "Multiply", "Divide", "Left", "Right", "Up", "Down", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "Pause"};

const u8 TOTAL_BUTTONS = 12;
const string BUTTON_TABLE[] = {"B-Hyperspace", "B-Fire", "B-Left-Coin", "B-Center-Coin", "B-Right-Coin", "B-One-Player-Start", "B-Two-Player-Start", "B-Thrust", "B-Rotate-Right", "B-Rotate-Left", "B-Toggle-Fullscreen", "B-Exit"};

const u8 MAX_HS_COUNT = 10;
const u8 HS_NAME_LENGTH = 3;
const u8 MAX_PLAYERS = 2;

const Score NO_SCORE = {0, 0};

const u8 SINE_TABLE[] =
{
    0x00, 0x03, 0x06, 0x09, 0x0C, 0x10, 0x13, 0x16,
    0x19, 0x1C, 0x1F, 0x22, 0x25, 0x28, 0x2B, 0x2E,
    0x31, 0x33, 0x36, 0x39, 0x3C, 0x3F, 0x41, 0x44,
    0x47, 0x49, 0x4C, 0x4E, 0x51, 0x53, 0x55, 0x58,
    0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68,
    0x6A, 0x6B, 0x6D, 0x6F, 0x70, 0x71, 0x73, 0x74,
    0x75, 0x76, 0x78, 0x79, 0x7A, 0x7A, 0x7B, 0x7C,
    0x7D, 0x7D, 0x7E, 0x7E, 0x7E, 0x7F, 0x7F, 0x7F,
    0x7F
};

// spaceobjects
const u8 INDISCERNIBLE = 0;
const u8 TRUE_EXPLOSION_START = 128;
const u8 EXPLOSION_START = 160;
const u8 MAX_X_POS_MAJOR = 31;
const u8 MAX_Y_POS_MAJOR = 23;

// asteroid
const u8 MAX_ASTEROIDS = 27;
const u8 LARGE_ASTEROID_HITBOX = 132;
const u8 MEDIUM_ASTEROID_HITBOX = 72;
const u8 SMALL_ASTEROID_HITBOX = 42;
const u8 LARGE_ASTEROID_POINTS = 0x2;
const u8 MEDIUM_ASTEROID_POINTS = 0x5;
const u8 SMALL_ASTEROID_POINTS = 0x10;
const u8 LARGE_ASTEROID = 0x04;
const u8 MEDIUM_ASTEROID = 0x02;
const u8 SMALL_ASTEROID = 0x01;
const u8 ASTEROID_SIZE = 0x07;
const u8 ASTEROID_TYPE = 0x18;

// ship
const u8 ALIVE = 1;
const u8 SHIP_HITBOX = 28;
const s8 MIN_VEL = -64;
const u8 MAX_VEL = 63;
const struct
{
    s8 x;
    s8 y;
} EXPLOSION_VELOCITY[] =
{
    {-40,  30},
    { 50, -20},
    {  0, -60},
    { 60,  20},
    { 10,  70},
    {-40, -40}
};

// saucer
const u8 LARGE_SAUCER = 2;
const u8 SMALL_SAUCER = 1;
const u8 LARGE_SAUCER_HITBOX = 37; // should be 36 but the carry causes it to be 37
const u8 SMALL_SAUCER_HITBOX = 19; // same as above but with 18
const u8 LARGE_SAUCER_POINTS = 0x20;
const u8 SMALL_SAUCER_POINTS = 0x99;
const u8 MINIMUM_SAUCER_SPAWN_TIME = 32;
const u8 SHOT_ANGLE_TABLE[] =
{
    0x00, 0x02, 0x05, 0x07, 0x0A, 0x0C, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1A, 0x1C, 0x1D, 0x1F
};
const u8 SHOT_OFFSET_AND[] =
{
    0x8F, 0x87
};
const u8 SHOT_OFFSET_OR[] =
{
    0x70, 0x78
};

// photon
const u8 MAX_SAUCER_PHOTONS = 2;
const u8 MAX_SHIP_PHOTONS = 4;
