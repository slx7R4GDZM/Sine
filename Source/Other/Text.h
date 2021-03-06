// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "Common-Things.h"

const u8 ENGLISH_OFFSET_TABLE[] = {0x00, 0x04, 0x07, 0x12, 0x1B, 0x25, 0x32, 0x36, 0x39, 0x3E, 0x43};
const u8  GERMAN_OFFSET_TABLE[] = {0x00, 0x05, 0x08, 0x15, 0x21, 0x2D, 0x3A, 0x41, 0x44, 0x4A, 0x50};
const u8  FRENCH_OFFSET_TABLE[] = {0x00, 0x05, 0x07, 0x13, 0x1B, 0x26, 0x34, 0x3A, 0x3F, 0x45, 0x4B};
const u8 SPANISH_OFFSET_TABLE[] = {0x00, 0x03, 0x06, 0x13, 0x1D, 0x2A, 0x30, 0x34, 0x39, 0x3F, 0x44};

const u16 ENGLISH_TEXT_TABLE[] =
{
    0x6356, 0x606E, 0x3CEC, 0x4DC0,
    0xA40A, 0xEA6C, 0x0800,
    0xECF2, 0xB06E, 0x3CEC, 0x485A, 0xB866, 0x9242, 0x9A82, 0xC312, 0x0E12, 0x904C, 0x4DF1,
    0xA412, 0x2DD2, 0x0A64, 0xC26C, 0x0F66, 0xCD82, 0x6C9A, 0xC34A, 0x85C0,
    0xA66E, 0x606C, 0x9E0A, 0xC242, 0xC4C2, 0xBA60, 0x49F0, 0x0C12, 0xC612, 0xB000,
    0xA66E, 0x6058, 0xED12, 0xB5E8, 0x29D2, 0x0ED8, 0x4C82, 0x8270, 0xC26C, 0x0B6E, 0x09E6, 0xB592, 0x3E00,
    0xA66E, 0x606E, 0xC16C, 0xC000,
    0x5962, 0x4866, 0xD26D,
    0x184E, 0x9B64, 0x0902, 0xA40A, 0xEDC0,
    0x184E, 0x9B64, 0x08C2, 0xA40A, 0xE800,
    0x204E, 0x9B64, 0xB846, 0x0D20, 0x2F40
};

const u16 GERMAN_TEXT_TABLE[] =
{
    0x64D2, 0x3B2E, 0xC26C, 0x5A4C, 0x936F,
    0xBD1A, 0x4C12, 0xB040,
    0x6B2C, 0x0A6C, 0x5A4C, 0x936E, 0x0B6E, 0xC052, 0x6C92, 0xB850, 0x4D82, 0xF258, 0x904C, 0x4DF0, 0x4C80,
    0x3370, 0xC242, 0x5A4C, 0x4C82, 0xBB52, 0x0B58, 0xB242, 0x6C9A, 0xC34A, 0x8264, 0x0A5A, 0x9000,
    0xF66C, 0x09B2, 0x3B2E, 0xC14C, 0x4CB6, 0x2B20, 0x0DA6, 0xC170, 0x4850, 0xB652, 0x3BD2, 0x9000,
    0xDA64, 0x904C, 0xC9D8, 0xBE0A, 0x3242, 0x9BC2, 0x6768, 0x4DAE, 0xA14E, 0x4850, 0xB652, 0x3BD2, 0x9000,
    0xBE0A, 0xB61E, 0x94D2, 0xA292, 0x0A2C, 0xCA4E, 0x7A65,
    0xBD1A, 0x4C12, 0x9213,
    0x1862, 0xCA64, 0xF242, 0x206E, 0xA352, 0x8240,
    0x1862, 0xCA64, 0xF242, 0x186E, 0xA352, 0x8000,
    0x2062, 0xCA64, 0xF264, 0x08C2, 0xBD1A, 0x4C00
};

const u16 FRENCH_TEXT_TABLE[] =
{
    0x8A5A, 0x8412, 0xCD82, 0xB9E6, 0xB240,
    0x74F2, 0x4D83,
    0xD4F0, 0xB242, 0xB9E6, 0xB242, 0x4DF0, 0x0E64, 0x0A12, 0xB846, 0x1062, 0x4B60, 0x8272, 0xB5C0,
    0xBEA8, 0x0A64, 0xC592, 0xF074, 0x9DC2, 0x6C9A, 0xC34A, 0x826F,
    0xA4F2, 0xBDD2, 0xF06C, 0x9E0A, 0xC242, 0xA4F2, 0xB074, 0x9DC2, 0x6C9A, 0xC34A, 0x826F,
    0xA4F2, 0xBDD2, 0xF058, 0xED12, 0xB5E8, 0x29D2, 0x0D72, 0x2C90, 0x0C12, 0xC62C, 0x484E, 0x9DAC, 0x49F0, 0x4800,
    0x2D28, 0xCF52, 0xB06E, 0xCD82, 0xBE0A, 0xB600,
    0x5364, 0x0A12, 0x0D0A, 0xB61A, 0x4800,
    0x1868, 0x6A4E, 0x4848, 0x0BA6, 0xCA72, 0xB5C0,
    0x1868, 0x6A4E, 0x4846, 0x0BA6, 0xCA72, 0xB000,
    0x2068, 0x6A4E, 0x4DC2, 0x185C, 0x9E52, 0xCD80
};

const u16 SPANISH_TEXT_TABLE[] =
{
    0xB24E, 0x9D90, 0xB800,
    0x7656, 0x2A26, 0xB040,
    0xBE42, 0xA664, 0xC15C, 0x4852, 0xBE0A, 0x0A64, 0xC592, 0x0C26, 0xB850, 0x6A7C, 0x0C52, 0x74EC, 0x4DC0,
    0xA4EC, 0x0A8A, 0xD4EC, 0x0A64, 0xC592, 0x0DF2, 0xB85A, 0x934E, 0x6960, 0x4DC0,
    0x9D2C, 0x6C4A, 0x0DA6, 0xC170, 0x4868, 0x2D8A, 0x0DD2, 0x824E, 0x3B66, 0x916C, 0x0C0A, 0x0C12, 0xC58B,
    0x9D2C, 0x6C4A, 0x0B3A, 0xA26C, 0xBD0A, 0x3A40,
    0xA660, 0xB96C, 0x0DF0, 0x2DB1,
    0x7652, 0x5CC2, 0xC26C, 0x8B64, 0x2A27,
    0x1854, 0x69D8, 0x2848, 0x0BB2, 0x4AE6, 0xB800,
    0x1854, 0x69D8, 0x2846, 0x0BB2, 0x4AE7,
    0x2054, 0x69D8, 0x2DC2, 0x185C, 0xCA56, 0x9800
};

const string DEFAULT_SETTINGS =
R"(# Lines starting with # are ignored by the program

#==============================================================================
# List of Available Keys
#==============================================================================

# Escape F1 F2 F3 F4 F5 F6 F7 F8 F9 F10 F11 F12 F13 F14 F15 Pause
# Tilde Num1 Num2 Num3 Num4 Num5 Num6 Num7 Num8 Num9 Num0 Hyphen Equal
# Tab Q W E R T Y U I O P LBracket RBracket Backslash                 Backspace
#      A S D F G H J K L Semicolon Quote Enter
# LShift Z X C V B N M Comma Period Slash RShift
# LControl LSystem LAlt Space RAlt RSystem Menu RControl
# Insert Home PageUp  Divide Multiply Subtract
# Delete End PageDown Numpad7 Numpad8 Numpad9 Add
#                     Numpad4 Numpad5 Numpad6
#         Up          Numpad1 Numpad2 Numpad3
#   Left Down Right   Numpad0

#==============================================================================
# Button Settings
#==============================================================================

 B-One-Player-Start Num1
 B-Two-Player-Start Num2
      B-Rotate-Left A
     B-Rotate-Right D
           B-Thrust W
             B-Fire E
       B-Hyperspace Space
        B-Left-Coin Left
      B-Center-Coin Down
       B-Right-Coin Right
B-Toggle-Fullscreen F11
             B-Exit Escape

#==============================================================================
# 8-Toggle Switch
#==============================================================================

## Language the game should use
# 0 = English (default)
# 1 = German
# 2 = French
# 3 = Spanish
Language 0

## Amount of starting lives
# 0 = 4-ship game
# 1 = 3-ship game (default)
Starting-Lives 1

## Center coin mechanism multiplier (left mech of a 2-mech door)
# 0 = Center coin mech x 1 (default)
# 1 = Center coin mech x 2
Center-Coin-Multiplier 0

## Right coin mechanism multiplier
# 0 = Right coin mech x 1 (default)
# 1 = Right coin mech x 4
# 2 = Right coin mech x 5
# 3 = Right coin mech x 6
Right-Coin-Multiplier 0

## Amount of coins required for playing
# 0 = Free play
# 1 = 1 coin for 2 plays
# 2 = 1 coin for 1 play (default)
# 3 = 2 coins for 1 play
Coinage 2

#==============================================================================
# Window Settings
#==============================================================================

## Which mode to start the window with
# 0 = Normal window (default)
# 1 = Borderless window
# 2 = Exclusive fullscreen
Starting-Window-Mode 0

## Starting window resolution
Starting-X-Resolution 1024
Starting-Y-Resolution 790

## Starting position for the game window
# -1 = Don't set the starting position (default)
Start-Window-Position-X -1
Start-Window-Position-Y -1

## What to do when the window isn't focused
# 0 = Pause the game (default)
# 1 = Run in background without input
# 2 = Run in background with input
Inactive-Mode 0

#==============================================================================
# Graphics Settings
#==============================================================================

## The x:y ratio to crop the image to
# < 1 = Crops the image starting with the left and right sides
# 1.0 = Scales the image to the lower resolution axis
# > 1 = Crops the image starting with the top and bottom (default)
Crop-Ratio 1.2962025

## MSAA toggle and quality setting
# 0 = Off
# 1 = 2x
# 2 = 4x
# 3 = 8x (default)
MSAA-Quality 3

## Gamma correction
Gamma-Correction 1.0

## Vertical synchronization
# 0 = Disabled (default)
# 1 = Enabled
V-Sync-Enabled 0

## Whether to use busy waiting or sleeping to limit FPS
# 0 = Use sleeping (default)
# 1 = Use busy waiting; this has high CPU usage, but it's consistent
Frame-Limit-Mode 0
)";
