// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Constants.h"

class fstream;

class Settings_Handler
{
public:
    Settings_Handler();

    void create_startup_window(RenderWindow& win);
    void resize_window(sf::Vector2u new_res, RenderWindow& win);
    void toggle_fullscreen(RenderWindow& win);

    void output_settings() const;
    Kb::Key get_button_key_code(Button button) const;
    Option_Switch get_option_switch() const;
    sf::Vector2u get_resolution() const;
    Inactive_Mode get_inactive_mode() const;
    Frame_Limiter_Mode get_frame_limiter_mode() const;
    void get_settings(float& crop_ratio, u8 gamma_table[]) const;
private:
    void parse_file_settings(std::ifstream& input);
    void parse_buttons(const string& setting, const string& value);
    void parse_settings(const string& setting, const string& value);

    void handle_window_creation(RenderWindow& win);
    void handle_fullscreen_creation(RenderWindow& win);
    void create_window(RenderWindow& win);

    Kb::Key button_key_code[TOTAL_BUTTONS];
    Option_Switch option_switch;

    // some of these can probably be made to be temporary
    // and done with after the window is created
    Window_Mode starting_window_mode;
    u32 window_style;
    sf::Vector2u fallback_res;
    sf::Vector2u current_res;
    sf::Vector2i fallback_win_pos;
    Inactive_Mode inactive_mode;
    float crop_ratio;
    u8 samples_MSAA;
    sf::ContextSettings context_settings;
    float gamma_correction;
    bool enable_v_sync;
    Frame_Limiter_Mode frame_limiter_mode;
};
