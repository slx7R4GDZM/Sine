// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#ifndef SINE_SETTINGS_HANDLER_HC
#define SINE_SETTINGS_HANDLER_HC

#include "Other/Constants.HC"
#include "Other/Text.HC"

class Settings_Handler
{
    U8 button_scan_code[TOTAL_BUTTONS];
    U8 option_switch;

    // some of these can probably be made to be temporary
    // and done with after the window is created
    U8 starting_window_mode;
    U32 window_style;
    Vector2U16 fallback_res;
    Vector2U16 current_res;
    Vector2I16 fallback_win_pos;
    U8 inactive_mode;
    F64 crop_ratio;
    F64 gamma_correction;
    Bool frame_limiter_mode;
};

U0 init_settings_handler(Settings_Handler* settings_handler)
{
    MemCpy(&settings_handler->button_scan_code, &DEFAULT_BUTTON_SCAN_CODES, TOTAL_BUTTONS);
    settings_handler->option_switch = (ENGLISH << 6)
                                    | (1 << 5)
                                    | (0 << 4)
                                    | (0 << 2)
                                    | (2 << 0);
    settings_handler->starting_window_mode = WIN_NORMAL;
    settings_handler->fallback_res.x = 640;
    settings_handler->fallback_res.y = 480;
    // copy fallback_res into current_res until settings_handler is implemented properly
    MemCpy(&settings_handler->current_res, &settings_handler->fallback_res, sizeof(Vector2U16));
    settings_handler->fallback_win_pos.x = -1;
    settings_handler->fallback_win_pos.y = -1;
    settings_handler->inactive_mode = PAUSE;
    settings_handler->crop_ratio = 1024.0 / 790;
    settings_handler->gamma_correction = 1;
    settings_handler->frame_limiter_mode = SLEEPING;
}

U0 output_settings(Settings_Handler* settings_handler)
{
    Print("----------------------------------------");
    //

    Print("\n----------------------------------------");
    Print("\nLanguage = %u", settings_handler->option_switch >> 6);
    Print("\nStarting-Lives = %u", (settings_handler->option_switch >> 5) & 0x01);
    Print("\nCenter-Coin-Multiplier = %u", (settings_handler->option_switch >> 4) & 0x01);
    Print("\nRight-Coin-Multiplier = %u", (settings_handler->option_switch >> 2) & 0x03);
    Print("\nCoinage = %u", settings_handler->option_switch & 0x03);

    Print("\n----------------------------------------");
    Print("\nStarting-Window-Mode = %u", settings_handler->starting_window_mode);
    Print("\nStarting-X-Resolution = %u", settings_handler->fallback_res.x);
    Print("\nStarting-Y-Resolution = %u", settings_handler->fallback_res.y);
    Print("\nStart-Window-Position-X = %d", settings_handler->fallback_win_pos.x);
    Print("\nStart-Window-Position-Y = %d", settings_handler->fallback_win_pos.y);
    Print("\nInactive-Mode = %u", settings_handler->inactive_mode);

    Print("\n----------------------------------------");
    Print("\nCrop-Ratio = %n", settings_handler->crop_ratio);
    Print("\nGamma-Correction = %n", settings_handler->gamma_correction);
    Print("\nFrame-Limit-Mode = %u", settings_handler->frame_limiter_mode);
    Print("\n----------------------------------------\n");
}

#endif
