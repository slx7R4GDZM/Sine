// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common_Things.h"
#include "../Other/Constants.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <fstream>

class Settings_Handler
{
private:
    kb::Key button_key[TOTAL_BUTTONS];
    Option_Switch option_switch;

    // these can probably be made to be temporary
    // and done with after the window is created
    Window_Mode window_mode;
    u32 x_resolution;
    u32 y_resolution;
    s32 start_win_pos_x;
    s32 start_win_pos_y;
    Inactive_Mode inactive_mode;
    bool simulate_DAC;
    float crop_ratio;
    u8 samples_MSAA;
    float gamma_correction;
    bool enable_v_sync;
    Frame_Limiter_Mode frame_limiter_mode;

    void parse_file_settings(std::ifstream& input);
    void parse_buttons(const string& setting, const string& value);
    void parse_settings(const string& setting, const string& value);
    template <typename T>
    static T clamp_string_value(const string& setting, const string& value, const T min_v, const T max_v);
public:
    Settings_Handler();
    void apply_window_settings(sf::RenderWindow& win) const;
    void output_settings() const;
    kb::Key get_button_key(const Button button_key) const;
    Option_Switch get_option_switch() const;
    Inactive_Mode get_inactive_mode() const;
    Frame_Limiter_Mode get_frame_limiter_mode() const;
    void get_settings(u32& x_resolution, u32& y_resolution, bool& simulate_DAC, float& crop_ratio, u8 gamma_table[]) const;
};
