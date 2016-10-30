// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <fstream>

const u8 TOTAL_BUTTONS = 11;

class Settings_Handler
{
private:
    enum Window_Mode : u8
    {
        WIN_NORMAL,
        WIN_BORDERLESS,
        WIN_FULLSCREEN
    };

    sf::Keyboard::Key button[TOTAL_BUTTONS];
    Option_Switch option_switch;

    // these can probably be made to be temporary
    // and done with after the window is created
    Window_Mode window_mode;
    u32 x_resolution;
    u32 y_resolution;
    s32 start_win_pos_x;
    s32 start_win_pos_y;
    Inactive_Mode inactive_mode;
    bool crop_image;
    u8 samples_MSAA;
    float gamma_correction;
    bool enable_v_sync;
    Frame_Limiter_Mode frame_limiter_mode;

    template <typename T>
    static T clamp_string_value(const string& value, const T min_v, const T max_v);
    void parse_file_settings(std::ifstream& input);
    void parse_buttons(const string& setting, const string& value);
    void parse_settings(const string& setting, const string& value);
public:
    const string BUTTON_TABLE[TOTAL_BUTTONS] = {"B_Hyperspace", "B_Fire", "B_Left-Coin", "B_Center-Coin", "B_Right-Coin", "B_One-Player-Start", "B_Two-Player-Start", "B_Thrust", "B_Rotate-Right", "B_Rotate-Left", "B_Exit"};

    Settings_Handler();
    void apply_window_settings(sf::RenderWindow& win);
    void output_settings() const;
    sf::Keyboard::Key get_button_key(const u8 button_key) const;
    Option_Switch get_option_switch() const;
    u8 get_inactive_mode() const;
    Frame_Limiter_Mode get_frame_limiter_mode() const;
    void get_settings(u32& x_resolution, u32& y_resolution, bool& crop_image, u8 gamma_table[]) const;
};
