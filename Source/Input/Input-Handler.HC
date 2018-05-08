// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_INPUT_HANDLER_HC
#define SINE_INPUT_HANDLER_HC

#include "Other/Constants.HC"
#include "Settings/Settings-Handler.HC"

class Input_Handler
{
    Bool button_held[TOTAL_BUTTONS];
    Bool button_pushed[TOTAL_BUTTONS];
};

U0 update(Input_Handler* input_handler, Settings_Handler* settings_handler)
{
    U64 i;
    for (i = 0; i < TOTAL_BUTTONS; i++)
    {
        Bool was_button_pushed = input_handler->button_held[i];
        if (Bt(kbd.down_bitmap, settings_handler->button_scan_code[i]))
            input_handler->button_held[i] = TRUE;
        else
            input_handler->button_held[i] = FALSE;

        input_handler->button_pushed[i] = !was_button_pushed && input_handler->button_held[i];
    }
}

Bool is_pressed(Input_Handler* input_handler, U8 button)
{
    return input_handler->button_held[button];
}

Bool on_press(Input_Handler* input_handler, U8 button)
{
    return input_handler->button_pushed[button];
}

U0 release_all_buttons(Input_Handler* input_handler)
{
    U64 i;
    for (i = 0; i < TOTAL_BUTTONS; i++)
    {
        input_handler->button_held[i] = FALSE;
        input_handler->button_pushed[i] = FALSE;
    }
}

#endif
