// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common_Things.h"
#include "../Settings/Settings_Handler.h"
#include <SFML/Graphics.hpp>

class Vector_Generator
{
private:
    // vector generator variables
    s16 current_x;
    s16 current_y;
    Global_Scale global_scale;

    // modern graphics variables
    float res_scale;
    float x_offset;
    float y_offset;
    float line_thickness;

    sf::Vector2u resolution;
    bool simulate_DAC;
    float crop_ratio;
    u8 gamma_table[16];

    void draw_long_vector(const Opcode opcode, const u16 vector_object[], u8& iteration, const bool flip_x, const bool flip_y, sf::RenderWindow& window);
    void load_absolute(const u16 vector_object[], u8& iteration);
    void draw_short_vector(const u16 vector_object[], u8& iteration, const bool flip_x, const bool flip_y, const bool brighten, sf::RenderWindow& window);

    u16 apply_global_scale(const u16 delta) const;

    void draw_vector(const s16 raw_delta_x, const s16 raw_delta_y, const u8 brightness, const bool flip_x, const bool flip_y, sf::RenderWindow& window);
    void draw_wide_line_segment(const float starting_x, const float starting_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const;
    void draw_thin_line_segment(const float scaled_x_start, const float scaled_y_start, const float x_start, const float y_start, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const;
public:
    Vector_Generator(const Settings_Handler settings_handler);
    void set_resolution_scale(const sf::Vector2u resolution);
    void process(const u16 vector_object[], sf::RenderWindow& window, u8 iteration = 0, const bool flip_x = false, const bool flip_y = false, const bool brighten = false);
};
