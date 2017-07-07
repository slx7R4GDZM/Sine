// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Settings_Handler.h"

#include "../Other/Text.h"
#include <cfloat>
#include <cmath>
#include <cstring>
#include <iomanip>

Settings_Handler::Settings_Handler()
    : button_key{kb::Space, kb::E, kb::Left, kb::Down, kb::Right, kb::Num1, kb::Num2, kb::W, kb::D, kb::A, kb::F11, kb::Escape}
    , option_switch{ENGLISH, 1, 0, 0, 2}
    , window_mode(WIN_NORMAL)
    , fallback_res(1024, 790)
    , fallback_win_pos(-1, -1)
    , inactive_mode(PAUSE)
    , simulate_DAC(true)
    , crop_ratio(1.2962962f)
    , samples_MSAA(3)
    , gamma_correction(1.0f)
    , enable_v_sync(false)
    , frame_limiter_mode(SLEEPING)
{
    std::ifstream in_file("Sine_Settings.cfg");
    if (in_file.good())
        parse_file_settings(in_file);
    else
    {
        cerr << "Error reading file Sine_Settings.cfg: " << strerror(errno) << "\n\n";
        std::ofstream out_file("Sine_Settings.cfg");

        if (out_file.good())
            out_file << DEFAULT_SETTINGS;
        else
            cerr << "Error writing file Sine_Settings.cfg: " << strerror(errno) << "\n\n";
    }
}

void Settings_Handler::parse_file_settings(std::ifstream& input)
{
    string line;
    string setting;
    string value;
    while (getline(input, line))
    {
        if (!line.empty() && line[0] != '#')
        {
            while (line[0] == ' ')
                line.erase(0, 1);

            for (u8 i = 0, option = 0; i < line.length(); i++)
            {
                if (line[i] == ' ') // setting
                {
                    setting = line.substr(0, i);
                    option = 1;
                }
                else if (option == 1) // value
                {
                    value = line.substr(i, line.length() - i);
                    i = line.length();
                }
            }
            if (setting[0] == 'B')
                parse_buttons(setting, value);
            else
                parse_settings(setting, value);
        }
    }
}

void Settings_Handler::parse_buttons(const string& setting, const string& value)
{
    // determine if the button choice exists
    const Button INVALID_BUTTON = static_cast<Button>(UINT8_MAX);
    Button current_button = INVALID_BUTTON;
    for (u8 i = 0; i < TOTAL_BUTTONS && current_button == INVALID_BUTTON; i++)
    {
        if (setting == BUTTON_TABLE[i])
            current_button = static_cast<Button>(i);
    }

    // determine if the key choice exists
    if (current_button != INVALID_BUTTON)
    {
        kb::Key key = kb::Unknown;
        for (u8 i = 0; i < TOTAL_KEYS && key == kb::Unknown; i++)
        {
            if (value == KEY_TABLE[i])
            {
                key = kb::Key(i);
                button_key[current_button] = kb::Key(i);
            }
        }
        if (key == kb::Unknown)
        {
            cerr << "Invalid key \"" << value << "\" used for button " << setting << '\n';
            cerr << "Button " << setting
                 << " defaulting to key " << KEY_TABLE[button_key[current_button]] << "\n\n";
        }
    }
    else
        cerr << "\nInvalid button \"" << setting << "\"\n\n";
}

void Settings_Handler::parse_settings(const string& setting, const string& value)
{
    if (setting == "Language")
        option_switch.language = clamp_string_value(setting, value, ENGLISH, SPANISH);
    else if (setting == "Starting-Lives")
        option_switch.starting_lives = clamp_string_value(setting, value, 0, 1);
    else if (setting == "Center-Coin-Multiplier")
        option_switch.center_coin_multiplier = clamp_string_value(setting, value, 0, 1);
    else if (setting == "Right-Coin-Multiplier")
        option_switch.right_coin_multiplier = clamp_string_value(setting, value, 0, 3);
    else if (setting == "Coinage")
        option_switch.coinage = clamp_string_value(setting, value, 0, 3);
    else if (setting == "Window-Mode")
        window_mode = clamp_string_value(setting, value, WIN_NORMAL, WIN_FULLSCREEN);
    else if (setting == "X-Resolution")
        fallback_res.x = clamp_string_value(setting, value, 1u, UINT32_MAX);
    else if (setting == "Y-Resolution")
        fallback_res.y = clamp_string_value(setting, value, 1u, UINT32_MAX);
    else if (setting == "Start-Window-Position-X")
        fallback_win_pos.x = clamp_string_value(setting, value, INT32_MIN, INT32_MAX);
    else if (setting == "Start-Window-Position-Y")
        fallback_win_pos.y = clamp_string_value(setting, value, INT32_MIN, INT32_MAX);
    else if (setting == "Inactive-Mode")
        inactive_mode = clamp_string_value(setting, value, PAUSE, RUN_WITH_INPUT);
    else if (setting == "Simulate-DAC")
        simulate_DAC = clamp_string_value(setting, value, false, true);
    else if (setting == "Crop-Ratio")
        crop_ratio = clamp_string_value(setting, value, FLT_MIN, FLT_MAX);
    else if (setting == "MSAA-Quality")
        // 8x MSAA is the best you can get on Nvidia, not sure about AMD or Intel
        // you can choose 16x in SFML but it's clearly not true MSAA
        // might be possible to get true 16x+ with SLI but no idea
        samples_MSAA = clamp_string_value(setting, value, 0, 3);
    else if (setting == "Gamma-Correction")
        gamma_correction = clamp_string_value(setting, value, FLT_MIN, FLT_MAX);
    else if (setting == "V-Sync-Enabled")
        enable_v_sync = clamp_string_value(setting, value, false, true);
    else if (setting == "Frame-Limit-Mode")
        frame_limiter_mode = clamp_string_value(setting, value, SLEEPING, BUSY_WAITING);
    else
        cerr << "Invalid setting \"" << setting << "\"\n\n";
}

template <typename T>
T Settings_Handler::clamp_string_value(const string& setting, const string& value, const T min_v, const T max_v)
{
    const T var_v = static_cast<T>(std::stod(value));
    if (var_v >= min_v && var_v <= max_v)
        return var_v;
    else if (var_v < min_v)
    {
        cerr << "Value for setting " << setting << " is too low, limiting it to " << min_v << "\n\n";
        return min_v;
    }
    else
    {
        cerr << "Value for setting " << setting << " is too high, limiting it to " << max_v << "\n\n";
        return max_v;
    }
}

void Settings_Handler::create_window(sf::RenderWindow& win)
{
    if (!samples_MSAA)
        context_settings.antialiasingLevel = 0;
    else
        context_settings.antialiasingLevel = std::pow(2, samples_MSAA);

    create_window(false, false, fallback_res, win);
}

void Settings_Handler::create_window(const bool toggle_fullscreen, const bool reuse_pos, const sf::Vector2u new_res, sf::RenderWindow& win)
{
    if (toggle_fullscreen)
    {
        if (window_style != sf::Style::Fullscreen)
        {
            fallback_res = current_res;
            current_res = new_res;
            fallback_win_pos = win.getPosition();
            window_style = sf::Style::Fullscreen;
        }
        else
        {
            current_res = fallback_res;
            switch (window_mode)
            {
            case WIN_NORMAL:
            case WIN_FULLSCREEN:
                window_style = sf::Style::Default;
                break;
            case WIN_BORDERLESS:
                window_style = sf::Style::None;
                break;
            }
        }
    }
    else
    {
        // if creating the window for the first time or resizing it
        current_res = new_res;
        if (reuse_pos)
            fallback_win_pos = win.getPosition();
        switch (window_mode)
        {
        case WIN_NORMAL:
            window_style = sf::Style::Default;
            break;
        case WIN_BORDERLESS:
            window_style = sf::Style::None;
            break;
        case WIN_FULLSCREEN:
            if (reuse_pos)
                window_style = sf::Style::Default;
            else
                window_style = sf::Style::Fullscreen;
            break;
        }
    }

    win.create(sf::VideoMode(current_res.x, current_res.y), "Sine", window_style, context_settings);

    // set current res to window size in case the window couldn't be created at the desired size
    current_res = win.getSize();

    if (window_style == sf::Style::Fullscreen)
        win.setMouseCursorVisible(false);
    else if (reuse_pos)
        win.setPosition(fallback_win_pos);
    else
    {
        if (fallback_win_pos.x != -1 && fallback_win_pos.y != -1)
            win.setPosition(sf::Vector2i(fallback_win_pos.x, fallback_win_pos.y));
        else if (fallback_win_pos.x != -1)
            win.setPosition(sf::Vector2i(fallback_win_pos.x, win.getPosition().y));
        else if (fallback_win_pos.y != -1)
            win.setPosition(sf::Vector2i(win.getPosition().x, fallback_win_pos.y));
    }
    win.setVerticalSyncEnabled(enable_v_sync);
}

void Settings_Handler::output_settings() const
{
    clog << '\n';
    for (u8 i = 0; i < TOTAL_BUTTONS; i++)
        clog << std::setw(19) << BUTTON_TABLE[i] << " = " << KEY_TABLE[button_key[i]] << '\n';

    clog << "----------------------------------------";
    clog << "\nLanguage = " << static_cast<u16>(option_switch.language);
    clog << "\nStarting-Lives = " << static_cast<u16>(option_switch.starting_lives);
    clog << "\nCenter-Coin-Multiplier = " << static_cast<u16>(option_switch.center_coin_multiplier);
    clog << "\nRight-Coin-Multiplier = " << static_cast<u16>(option_switch.right_coin_multiplier);
    clog << "\nCoinage = " << static_cast<u16>(option_switch.coinage);

    clog << "\n----------------------------------------";
    clog << "\nWindow-Mode = " << static_cast<u16>(window_mode);
    clog << "\nX-Resolution = " << fallback_res.x;
    clog << "\nY-Resolution = " << fallback_res.y;
    clog << "\nStart-Window-Position-X = " << fallback_win_pos.x;
    clog << "\nStart-Window-Position-Y = " << fallback_win_pos.y;
    clog << "\nInactive-Mode = " << static_cast<u16>(inactive_mode);

    clog << "\n----------------------------------------";
    clog << "\nSimulate-DAC = " << simulate_DAC;
    clog << "\nCrop-Ratio = " << crop_ratio;
    clog << "\nMSAA-Quality = " << static_cast<u16>(samples_MSAA);
    clog << "\nGamma-Correction = " << gamma_correction;
    clog << "\nV-Sync-Enabled = " << enable_v_sync;
    clog << "\nFrame-Limit-Mode = " << frame_limiter_mode << '\n';
}

kb::Key Settings_Handler::get_button_key(const Button button) const
{
    return button_key[button];
}

Option_Switch Settings_Handler::get_option_switch() const
{
    return option_switch;
}

sf::Vector2u Settings_Handler::get_resolution() const
{
    return current_res;
}

Inactive_Mode Settings_Handler::get_inactive_mode() const
{
    return inactive_mode;
}

Frame_Limiter_Mode Settings_Handler::get_frame_limiter_mode() const
{
    return frame_limiter_mode;
}

void Settings_Handler::get_settings(bool& simulate_DAC, float& crop_ratio, u8 gamma_table[]) const
{
    simulate_DAC = this->simulate_DAC;
    crop_ratio = this->crop_ratio;
    for (u8 i = 0; i < 16; i++)
        gamma_table[i] = 255 * std::pow(static_cast<float>(i) * 17 / 255, 1 / gamma_correction) + 0.5f;
}
