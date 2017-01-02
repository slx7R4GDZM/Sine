// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common_Things.h"
#include "../Settings/Settings_Handler.h"

class Input_Handler
{
private:
    bool button_held[TOTAL_BUTTONS];
    bool button_pushed[TOTAL_BUTTONS];
public:
    Input_Handler() = default;
    void update(const Settings_Handler& settings_handler);
    bool is_pressed(const Button button) const;
    bool on_press(const Button button) const;
};
