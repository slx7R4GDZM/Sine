// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Input_Handler.h"

void Input_Handler::update(const Settings_Handler& settings_handler)
{
    for (u8 i = 0; i < TOTAL_BUTTONS; i++)
    {
        bool was_button_pushed = button_held[i];
        if (kb::isKeyPressed(settings_handler.get_button_key(static_cast<Button>(i))))
            button_held[i] = true;
        else
            button_held[i] = false;
        button_pushed[i] = !was_button_pushed && button_held[i] ? 1 : 0;
    }
}

bool Input_Handler::is_pressed(const Button button) const
{
    return button_held[button];
}

bool Input_Handler::on_press(const Button button) const
{
    return button_pushed[button];
}
