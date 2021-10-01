// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Settings-Handler.h"

#include <cfloat>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include "../Other/Text.h"

static string get_next(const string& line, unsigned& position);
template <typename T>
static T clamp_string_value(const string& setting, const string& value, T min_v, T max_v);

Settings_Handler::Settings_Handler()
    : button_key_code{Kb::Space, Kb::E, Kb::Left, Kb::Down, Kb::Right, Kb::Num1, Kb::Num2, Kb::W, Kb::D, Kb::A, Kb::F11, Kb::Escape}
    , option_switch{ENGLISH, 1, 0, 0, 2}
    , starting_window_mode(WIN_NORMAL)
    , fallback_res(1024, 790)
    , fallback_win_pos(-1, -1)
    , inactive_mode(PAUSE)
    , crop_ratio(1024.0f / 790)
    , samples_MSAA(3)
    , gamma_correction(1.0f)
    , enable_v_sync(false)
    , frame_rate_limit(120)
    , frame_limiter_mode(BUSY_WAITING)
{
    std::ifstream in_file(SETTINGS_FILENAME);
    if (in_file.good())
        parse_file_settings(in_file);
    else
    {
        cerr << "Error reading file " << SETTINGS_FILENAME << ": " << std::strerror(errno) << '\n';
        std::ofstream out_file(SETTINGS_FILENAME);

        if (out_file.good())
        {
            clog << "Created file " << SETTINGS_FILENAME << '\n';
            out_file << DEFAULT_SETTINGS;
        }
        else
            cerr << "Error writing file " << SETTINGS_FILENAME << ": " << std::strerror(errno) << '\n';
    }
}

void Settings_Handler::parse_file_settings(std::ifstream& input)
{
    string line;
    while (std::getline(input, line))
    {
        unsigned line_pos = 0;
        const string setting = get_next(line, line_pos);
        if (setting.size() && setting[0] != '#')
        {
            const string value = get_next(line, line_pos);
            if (value.empty())
                continue;
            if (setting[0] == 'B')
                parse_buttons(setting, value);
            else
                parse_settings(setting, value);
        }
    }
}

string get_next(const string& line, unsigned& position)
{
    // move past the whitespace
    while (position < line.size() && !std::isgraph(line[position]))
        position++;

    // grab the usable text
    string output;
    int i;
    for (i = 0; position + i <= line.size(); i++)
    {
        if (!std::isgraph(line[position + i]))
        {
            output = line.substr(position, i);
            break;
        }
    }
    position += i + 1;
    return output;
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
    if (current_button == INVALID_BUTTON)
    {
        cerr << "Invalid button \"" << setting << "\"\n";
        return;
    }

    Kb::Key key = Kb::Unknown;
    for (u8 i = 0; i < Kb::KeyCount && key == Kb::Unknown; i++)
    {
        if (value == KEY_TABLE[i])
        {
            key = Kb::Key(i);
            button_key_code[current_button] = Kb::Key(i);
        }
    }
    if (key == Kb::Unknown)
    {
        cerr << "Invalid key \"" << value << "\" used for button " << setting << '\n';
        cerr << "Button " << setting
             << " defaulting to key " << KEY_TABLE[button_key_code[current_button]] << '\n';
    }
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
    else if (setting == "Starting-Window-Mode")
        starting_window_mode = clamp_string_value(setting, value, WIN_NORMAL, WIN_FULLSCREEN);
    else if (setting == "Starting-X-Resolution")
        fallback_res.x = clamp_string_value(setting, value, 1u, UINT32_MAX);
    else if (setting == "Starting-Y-Resolution")
        fallback_res.y = clamp_string_value(setting, value, 1u, UINT32_MAX);
    else if (setting == "Start-Window-Position-X")
        fallback_win_pos.x = clamp_string_value(setting, value, INT32_MIN, INT32_MAX);
    else if (setting == "Start-Window-Position-Y")
        fallback_win_pos.y = clamp_string_value(setting, value, INT32_MIN, INT32_MAX);
    else if (setting == "Inactive-Mode")
        inactive_mode = clamp_string_value(setting, value, PAUSE, RUN_WITH_INPUT);
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
    else if (setting == "Frame-Rate-Limit")
    {
        frame_rate_limit = std::stod(value);
        if (frame_rate_limit != -1)
            frame_rate_limit = clamp_string_value(setting, value, 2.0f, FLT_MAX);
    }
    else if (setting == "Frame-Limit-Mode")
        frame_limiter_mode = clamp_string_value(setting, value, SLEEPING, BUSY_WAITING);
    else
        cerr << "Invalid setting \"" << setting << "\"\n";
}

template <typename T>
T clamp_string_value(const string& setting, const string& value, T min_v, T max_v)
{
    const T var_v = static_cast<T>(std::stod(value));
    if (var_v >= min_v && var_v <= max_v)
        return var_v;

    if (var_v < min_v)
    {
        cerr << "Value for setting " << setting << " is too low, limiting it to " << min_v << '\n';
        return min_v;
    }

    cerr << "Value for setting " << setting << " is too high, limiting it to " << max_v << '\n';
    return max_v;
}

void Settings_Handler::create_startup_window(RenderWindow& win)
{
    if (!samples_MSAA)
        context_settings.antialiasingLevel = 0;
    else
        context_settings.antialiasingLevel = std::pow(2, samples_MSAA);

    if (starting_window_mode != WIN_FULLSCREEN)
        handle_window_creation(win);
    else
    {
        current_res = fallback_res;
        handle_fullscreen_creation(win);
    }
}

void Settings_Handler::handle_window_creation(RenderWindow& win)
{
    if (starting_window_mode != WIN_BORDERLESS)
        window_style = sf::Style::Default;
    else
        window_style = sf::Style::None;

    current_res = fallback_res;
    create_window(win);
}

void Settings_Handler::handle_fullscreen_creation(RenderWindow& win)
{
    window_style = sf::Style::Fullscreen;
    fallback_res = current_res;
    fallback_win_pos = win.getPosition();

    if (starting_window_mode != WIN_FULLSCREEN)
        current_res = {sf::VideoMode::getDesktopMode().width,
                       sf::VideoMode::getDesktopMode().height};
    else if (win.isOpen())
        // if the game started out in fullscreen then re-entering fullscreen
        // will use the config resolution rather than the desktop resolution
        current_res = static_cast<sf::Vector2u>(win.getDefaultView().getSize());

    create_window(win);
}

void Settings_Handler::create_window(RenderWindow& win)
{
    if (current_res.x == 0)
        current_res.x = 1;
    if (current_res.y == 0)
        current_res.y = 1;

    win.create(sf::VideoMode(current_res.x, current_res.y), "Sine", window_style, context_settings);

    // set current res to window size in case the window couldn't be created at the desired size
    current_res = win.getSize();

    if (window_style == sf::Style::Fullscreen)
        win.setMouseCursorVisible(false);
    else if (fallback_win_pos.x != -1 && fallback_win_pos.y != -1)
        win.setPosition(fallback_win_pos);
    else if (fallback_win_pos.x != -1)
        win.setPosition(sf::Vector2i(fallback_win_pos.x, win.getPosition().y));
    else if (fallback_win_pos.y != -1)
        win.setPosition(sf::Vector2i(win.getPosition().x, fallback_win_pos.y));

    win.setVerticalSyncEnabled(enable_v_sync);
}

void Settings_Handler::resize_window(sf::Vector2u new_res, RenderWindow& win)
{
    current_res = new_res;
    if (current_res.x == 0 || current_res.y == 0)
    {
        // recreate the window if the new resolution is invalid
        fallback_win_pos = win.getPosition();
        create_window(win);
    }
    else
    {
        // adjust the view if the window was resized with a valid resolution
        const sf::View new_view({0, 0, static_cast<float>(current_res.x), static_cast<float>(current_res.y)});
        win.setView(new_view);
    }
}

void Settings_Handler::toggle_fullscreen(RenderWindow& win)
{
    if (window_style == sf::Style::Fullscreen)
        handle_window_creation(win);
    else
        handle_fullscreen_creation(win);
}

void Settings_Handler::output_settings() const
{
    clog << "----------------------------------------";
    for (u8 i = 0; i < TOTAL_BUTTONS; i++)
        clog << '\n' << std::setw(19) << BUTTON_TABLE[i] << " = " << KEY_TABLE[button_key_code[i]];

    clog << "\n----------------------------------------";
    clog << "\nLanguage = " << static_cast<u16>(option_switch.language);
    clog << "\nStarting-Lives = " << static_cast<u16>(option_switch.starting_lives);
    clog << "\nCenter-Coin-Multiplier = " << static_cast<u16>(option_switch.center_coin_multiplier);
    clog << "\nRight-Coin-Multiplier = " << static_cast<u16>(option_switch.right_coin_multiplier);
    clog << "\nCoinage = " << static_cast<u16>(option_switch.coinage);

    clog << "\n----------------------------------------";
    clog << "\nStarting-Window-Mode = " << static_cast<u16>(starting_window_mode);
    clog << "\nStarting-X-Resolution = " << fallback_res.x;
    clog << "\nStarting-Y-Resolution = " << fallback_res.y;
    clog << "\nStart-Window-Position-X = " << fallback_win_pos.x;
    clog << "\nStart-Window-Position-Y = " << fallback_win_pos.y;
    clog << "\nInactive-Mode = " << static_cast<u16>(inactive_mode);

    clog << "\n----------------------------------------";
    clog << "\nCrop-Ratio = " << crop_ratio;
    clog << "\nMSAA-Quality = " << static_cast<u16>(samples_MSAA);
    clog << "\nGamma-Correction = " << gamma_correction;
    clog << "\nV-Sync-Enabled = " << enable_v_sync;
    clog << "\nFrame-Rate-Limit = " << frame_rate_limit;
    clog << "\nFrame-Limit-Mode = " << frame_limiter_mode;
    clog << "\n----------------------------------------\n";
}

Kb::Key Settings_Handler::get_button_key_code(Button button) const
{
    return button_key_code[button];
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

float Settings_Handler::get_frame_rate_limit() const
{
    return frame_rate_limit;
}

Frame_Limiter_Mode Settings_Handler::get_frame_limiter_mode() const
{
    return frame_limiter_mode;
}

void Settings_Handler::get_settings(float& crop_ratio, u8 gamma_table[]) const
{
    crop_ratio = this->crop_ratio;
    for (u8 i = 0; i < 16; i++)
        gamma_table[i] = std::round(std::pow(i * 17 / 255.0f, 1 / gamma_correction) * 255);
}
