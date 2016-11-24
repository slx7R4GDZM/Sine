// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Settings_Handler.h"

#include "../Other/Text.h"
#include <cmath>
#include <cstring>
#include <iomanip>

const string BUTTON_TABLE[TOTAL_BUTTONS] = {"B_Hyperspace", "B_Fire", "B_Left-Coin", "B_Center-Coin", "B_Right-Coin", "B_One-Player-Start", "B_Two-Player-Start", "B_Thrust", "B_Rotate-Right", "B_Rotate-Left", "B_Exit"};
const u8 TOTAL_KEYS = 101;
const string KEY_TABLE[TOTAL_KEYS] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Num0", "Num1", "Num2", "Num3", "Num4", "Num5", "Num6", "Num7", "Num8", "Num9", "Escape", "LControl", "LShift", "LAlt", "LSystem", "RControl", "RShift", "RAlt", "RSystem", "Menu", "LBracket", "RBracket", "SemiColon", "Comma", "Period", "Quote", "Slash", "BackSlash", "Tilde", "Equal", "Dash", "Space", "Return", "BackSpace", "Tab", "PageUp", "PageDown", "End", "Home", "Insert", "Delete", "Add", "Subtract", "Multiply", "Divide", "Left", "Right", "Up", "Down", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "Pause"};

Settings_Handler::Settings_Handler()
    : button{sf::Keyboard::Space, sf::Keyboard::E, sf::Keyboard::Left, sf::Keyboard::Down, sf::Keyboard::Right, sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::W, sf::Keyboard::D, sf::Keyboard::A, sf::Keyboard::Escape}
    , option_switch{ENGLISH, 1, 0, 0, 2}
    , window_mode(WIN_NORMAL)
    , x_resolution(1024)
    , y_resolution(790)
    , start_win_pos_x(-1)
    , start_win_pos_y(-1)
    , inactive_mode(PAUSE)
    , simulate_DAC(true)
    , crop_image(true)
    , samples_MSAA(3)
    , gamma_correction(1.0)
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
        if (line[0] != '#' && !line.empty())
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
    const u8 INVALID_BUTTON = UINT8_MAX;
    u8 current_button = INVALID_BUTTON;
    for (u8 i = 0; i < TOTAL_BUTTONS && current_button == INVALID_BUTTON; i++)
    {
        if (setting == BUTTON_TABLE[i])
            current_button = i;
    }

    // determine if the key choice exists
    if (current_button != INVALID_BUTTON)
    {
        sf::Keyboard::Key key = sf::Keyboard::Unknown;
        for (u8 i = 0; i < TOTAL_KEYS && key == sf::Keyboard::Unknown; i++)
        {
            if (value == KEY_TABLE[i])
            {
                key = sf::Keyboard::Key(i);
                button[current_button] = sf::Keyboard::Key(i);
            }
        }
        if (key == sf::Keyboard::Unknown)
        {
            cerr << "Invalid key \"" << value << "\" used for button " << setting << '\n';
            cerr << "Button " << setting
                 << " defaulting to key " << KEY_TABLE[button[current_button]] << "\n\n";
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
        x_resolution = clamp_string_value(setting, value, 0u, UINT32_MAX);
    else if (setting == "Y-Resolution")
        y_resolution = clamp_string_value(setting, value, 0u, UINT32_MAX);
    else if (setting == "Start-Window-Position-X")
        start_win_pos_x = clamp_string_value(setting, value, INT32_MIN, INT32_MAX);
    else if (setting == "Start-Window-Position-Y")
        start_win_pos_y = clamp_string_value(setting, value, INT32_MIN, INT32_MAX);
    else if (setting == "Inactive-Mode")
        inactive_mode = clamp_string_value(setting, value, PAUSE, RUN_WITH_INPUT);
    else if (setting == "Simulate-DAC")
        simulate_DAC = clamp_string_value(setting, value, false, true);
    else if (setting == "Crop-Image")
        crop_image = clamp_string_value(setting, value, false, true);
    else if (setting == "MSAA-Quality")
        // 8x MSAA is the best you can get on Nvidia, not sure about AMD or Intel
        // you can choose 16x in SFML but it's clearly not true MSAA
        // might be possible to get true 16x+ with SLI but no idea
        samples_MSAA = clamp_string_value(setting, value, 0, 3);
    else if (setting == "Gamma-Correction")
        gamma_correction = clamp_string_value(setting, value, 0.0f, 2048.0f);
    else if (setting == "V-Sync-Enabled")
        enable_v_sync = clamp_string_value(setting, value, 0, 1);
    else if (setting == "Frame-Limit-Mode")
        frame_limiter_mode = clamp_string_value(setting, value, SLEEPING, BUSY_WAITING);
    else
        cerr << "Invalid setting \"" << setting << "\"\n\n";
}

template <typename T>
T Settings_Handler::clamp_string_value(const string& setting, const string& value, const T min_v, const T max_v)
{
    T var_v = static_cast<T>(std::stod(value));
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

void Settings_Handler::apply_window_settings(sf::RenderWindow& win)
{
    u8 window_style;
    switch (window_mode)
    {
        case WIN_NORMAL:
            window_style = sf::Style::Titlebar | sf::Style::Close;
            break;
        case WIN_BORDERLESS:
            window_style = sf::Style::None;
            break;
        case WIN_FULLSCREEN:
            window_style = sf::Style::Fullscreen;
            break;
    }

    sf::ContextSettings context_settings;
    if (!samples_MSAA)
        context_settings.antialiasingLevel = 0;
    else
        context_settings.antialiasingLevel = std::pow(2, samples_MSAA);

    win.create(sf::VideoMode(x_resolution, y_resolution), "Sine", window_style, context_settings);

    if (window_mode == WIN_FULLSCREEN)
        win.setMouseCursorVisible(false);
    else
    {
        if (start_win_pos_x != -1 && start_win_pos_y != -1)
            win.setPosition(sf::Vector2i(start_win_pos_x, start_win_pos_y));
        else if (start_win_pos_x != -1)
            win.setPosition(sf::Vector2i(start_win_pos_x, win.getPosition().y));
        else if (start_win_pos_y != -1)
            win.setPosition(sf::Vector2i(win.getPosition().x, start_win_pos_y));
    }

    win.setVerticalSyncEnabled(enable_v_sync);
}

void Settings_Handler::output_settings() const
{
    clog << '\n';
    for (u8 i = 0; i < TOTAL_BUTTONS; i++)
        clog << std::setw(18) << BUTTON_TABLE[i] << " = " << KEY_TABLE[button[i]] << '\n';

    clog << "=======================================";
    clog << "\nLanguage = " << static_cast<u16>(option_switch.language);
    clog << "\nStarting-Lives = " << static_cast<u16>(option_switch.starting_lives);
    clog << "\nCenter-Coin-Multiplier = " << static_cast<u16>(option_switch.center_coin_multiplier);
    clog << "\nRight-Coin-Multiplier = " << static_cast<u16>(option_switch.right_coin_multiplier);
    clog << "\nCoinage = " << static_cast<u16>(option_switch.coinage);

    clog << "\n========================================";
    clog << "\nWindow-Mode = " << static_cast<u16>(window_mode);
    clog << "\nX-Resolution = " << x_resolution;
    clog << "\nY-Resolution = " << y_resolution;
    clog << "\nStart-Window-Position-X = " << start_win_pos_x;
    clog << "\nStart-Window-Position-Y = " << start_win_pos_y;
    clog << "\nInactive-Mode = " << static_cast<u16>(inactive_mode);

    clog << "\n========================================";
    clog << "\nSimulate-DAC = " << simulate_DAC;
    clog << "\nCrop-Image = " << crop_image;
    clog << "\nMSAA-Quality = " << static_cast<u16>(samples_MSAA);
    clog << "\nGamma-Correction = " << gamma_correction;
    clog << "\nV-Sync-Enabled = " << enable_v_sync;
    clog << "\nFrame-Limit-Mode = " << frame_limiter_mode << '\n';
}

sf::Keyboard::Key Settings_Handler::get_button_key(const u8 button_key) const
{
    return button[button_key];
}

Option_Switch Settings_Handler::get_option_switch() const
{
    return option_switch;
}

u8 Settings_Handler::get_inactive_mode() const
{
    return inactive_mode;
}

Frame_Limiter_Mode Settings_Handler::get_frame_limiter_mode() const
{
    return frame_limiter_mode;
}

void Settings_Handler::get_settings(u32& x_resolution, u32& y_resolution, bool& simulate_DAC, bool& crop_image, u8 gamma_table[]) const
{
    x_resolution = this->x_resolution;
    y_resolution = this->y_resolution;
    simulate_DAC = this->simulate_DAC;
    crop_image = this->crop_image;
    for (u8 i = 0; i < 16; i++)
        gamma_table[i] = 255 * std::pow(static_cast<double>(i) * 17 / 255, 1 / gamma_correction) + 0.5;
}
