// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Constants.h"

class Settings_Handler;

class Input_Handler
{
public:
    Input_Handler() = default;

    void update(const Settings_Handler& settings_handler);
    bool is_pressed(Button button) const;
    bool on_press(Button button) const;
    void release_all_buttons();
private:
    bool button_held[TOTAL_BUTTONS];
    bool button_pushed[TOTAL_BUTTONS];
};
