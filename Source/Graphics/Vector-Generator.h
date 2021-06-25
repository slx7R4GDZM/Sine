// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common-Things.h"

class Settings_Handler;

class Vector_Generator
{
public:
    explicit Vector_Generator(const Settings_Handler& settings);

    void set_resolution_scale(const Settings_Handler& settings);
    void process(const u16 vector_object[], RenderWindow& window, u16 iteration = 0, float rotation_radians = 0, bool brighten = false);

    void process(const float vector_object[], RenderWindow& window, u16 iteration = 0, float rotation_radians = 0, bool brighten = false);
private:
    void crop_with_extra_space(u32 axis_to_crop, float& crop_offset, u32 scale_axis, float& scale_offset);

    void draw_long_vector(Opcode opcode, const u16 vector_object[], u16& iteration, float rotation_radians, RenderWindow& window);
    void load_absolute(const u16 vector_object[], u16& iteration);
    void draw_short_vector(const u16 vector_object[], u16 iteration, float rotation_radians, bool brighten, RenderWindow& window);

    float apply_global_scale(float delta, float local_scale) const;

    void draw_vector(float raw_delta_x, float raw_delta_y, u8 brightness, float rotation_radians, RenderWindow& window);
    void draw_wide_line_segment(float start_x, float start_y, float delta_x, float delta_y, sf::Color vector_color, RenderWindow& window) const;
    void draw_thin_line_segment(float start_x, float start_y, float delta_x, float delta_y, sf::Color vector_color, RenderWindow& window) const;

    void draw_long_vector(float opcode, const float vector_object[], u16& iteration, float rotation_radians, RenderWindow& window);
    void load_absolute(const float vector_object[], u16& iteration);
    void draw_short_vector(const float vector_object[], u16& iteration, float rotation_radians, bool brighten, RenderWindow& window);

    // vector generator variables
    float current_x;
    float current_y;
    float global_scale;

    // modern graphics variables
    float res_scale;
    float x_offset;
    float y_offset;
    float line_thickness;

    sf::Vector2u resolution;
    float crop_ratio;
    u8 gamma_table[16];
};
