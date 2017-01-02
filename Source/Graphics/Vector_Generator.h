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
    Scale global_scale;

    // modern graphics variables
    float res_scale;
    float x_offset;
    float y_offset;
    float line_thickness;

    u32 x_resolution;
    u32 y_resolution;
    bool simulate_DAC;
    bool crop_image;
    u8 gamma_table[16];

    void draw_long_vector(const u8 opcode, const u16 vector_object[], u8& iteration, const bool flip_x, const bool flip_y, sf::RenderWindow& window);
    void load_absolute(const u16 vector_object[], u8& iteration, sf::RenderWindow& window);
    void draw_short_vector(const u16 vector_object[], u8& iteration, const bool flip_x, const bool flip_y, const bool brighten, sf::RenderWindow& window);

    void draw_vector(const s16 raw_delta_x, const s16 raw_delta_y, const u8 local_scale, const u8 brightness, const bool flip_x, const bool flip_y, sf::RenderWindow& window);
    void draw_wide_line_segment(const float starting_x, const float starting_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const;
    void draw_thin_line_segment(const float starting_x, const float starting_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const;
public:
    Vector_Generator(const Settings_Handler settings_handler);
    void process(const u16 vector_object[], sf::RenderWindow& window, u8 iteration = 0, const bool flip_x = false, const bool flip_y = false, const bool brighten = false);
    void load_absolute(const Position& pos, const Scale scale);
    void load_absolute(const u8 cur_x, const u8 cur_y, const Scale scale);
};
