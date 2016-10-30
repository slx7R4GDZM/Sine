// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Input_Handler.h"

#include <SFML/Window/Keyboard.hpp>

void Input_Handler::update(const Settings_Handler& settings_handler)
{
    for (u8 i = 0; i < TOTAL_BUTTONS; i++)
    {
        bool was_button_pushed = button_held[i];
        button_held[i] = sf::Keyboard::isKeyPressed(settings_handler.get_button_key(i)) ? 1 : 0;
        button_pushed[i] = !was_button_pushed && button_held[i] ? 1 : 0;
    }
}

bool Input_Handler::is_pressed(const u8 button) const
{
    return button_held[button];
}

bool Input_Handler::on_press(const u8 button) const
{
    return button_pushed[button];
}
