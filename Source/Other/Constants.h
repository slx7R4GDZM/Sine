// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "Common_Things.h"
#include "Constants.h"

const nanoseconds MAX_FRAME_TIME = 16666667ns;

const u8 TOTAL_BUTTONS = 12;
const u8 TOTAL_KEYS = 101;
const string BUTTON_TABLE[TOTAL_BUTTONS] = {"B_Hyperspace", "B_Fire", "B_Left-Coin", "B_Center-Coin", "B_Right-Coin", "B_One-Player-Start", "B_Two-Player-Start", "B_Thrust", "B_Rotate-Right", "B_Rotate-Left", "B_Toggle-Fullscreen", "B_Exit"};
const string KEY_TABLE[TOTAL_KEYS] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Escape", "LControl", "LShift", "LAlt", "LSystem", "RControl", "RShift", "RAlt", "RSystem", "Menu", "LBracket", "RBracket", "SemiColon", "Comma", "Period", "Quote", "Slash", "BackSlash", "Tilde", "Equal", "Dash", "Space", "Return", "BackSpace", "Tab", "PageUp", "PageDown", "End", "Home", "Insert", "Delete", "Add", "Subtract", "Multiply", "Divide", "Left", "Right", "Up", "Down", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "Pause"};

const u16 MAX_SCORE = 10000;
const u8 MAX_HS_COUNT = 10;
const u8 HS_NAME_LENGTH = 3;
const u8 MAX_PLAYERS = 2;

const u8 SINE_TABLE[65]
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
const u8 TRUE_EXPLOSION_START = 128;
const u8 EXPLOSION_START = 160;
const u8 BONUS_SIZE_1 = 71;
const u8 BONUS_SIZE_2 = 50;
const u8 BONUS_SIZE_3 = 62;
const u8 MAX_X_POS_MAJOR = 31;
const u8 MAX_Y_POS_MAJOR = 23;

// asteroid
const u8 MAX_ASTEROIDS = 27;
const u8 LARGE_ASTEROID_SIZE = 90;
const u8 MEDIUM_ASTEROID_SIZE = 30;
const u8 SMALL_ASTEROID_SIZE = 0;
const u8 LARGE_ASTEROID_POINTS = 2;
const u8 MEDIUM_ASTEROID_POINTS = 5;
const u8 SMALL_ASTEROID_POINTS = 10;
const s8 LARGE_ASTEROID_SPAWN_VELOCITY[] = {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, -0x10, -0x0F, -0x0E, -0x0D, -0x0C, -0x0B, -0x0A, -0x09, -0x08, -0x07, -0x06, -0x06, -0x06, -0x06, -0x06, -0x06};
const s8 MEDIUM_ASTEROID_SPAWN_VELOCITY[] = {0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, -0x1F, -0x1E, -0x1C, -0x1A, -0x18, -0x16, -0x14, -0x12, -0x10, -0x0E, -0x0C, -0x0B, -0x0A, -0x09, -0x08, -0x07};
const s8 SMALL_ASTEROID_SPAWN_VELOCITY[] = {0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x15, 0x18, 0x1B, 0x1E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, -0x1F, -0x1F, -0x1F, -0x1F, -0x1F, -0x1F, -0x1E, -0x1B, -0x18, -0x15, -0x12, -0x10, -0x0E, -0x0C, -0x0A, -0x08};

// saucer
const u8 LARGE_SAUCER = 2;
const u8 SMALL_SAUCER = 1;
const u8 LARGE_SAUCER_SIZE = 30;
const u8 LARGE_SAUCER_POINTS = 20;
const u8 SMALL_SAUCER_POINTS = 100;
const u8 MINIMUM_SAUCER_SPAWN_TIME = 32;

// ship
const u8 INDISCERNIBLE = 0;
const u8 ALIVE = 1;
const s8 MIN_VEL = -64;
const u8 MAX_VEL = 63;

// photon
const u8 MAX_SAUCER_PHOTONS = 2;
const u8 MAX_SHIP_PHOTONS = 4;
const u8 PHOTON_SIZE = 43;
