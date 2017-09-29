// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Vector-Generator.h"

#include <cmath>
#include <SFML/Graphics.hpp>
#include "../Settings/Settings-Handler.h"

const float PI = std::atan(1) * 4;
const float DESIRED_LINE_WIDTH = 0.75f;
const float MINIMUM_LINE_WIDTH = 0.5f;
const u16 INTERNAL_RES = 1024;

Vector_Generator::Vector_Generator(const Settings_Handler settings_handler)
{
    settings_handler.get_settings(crop_ratio, gamma_table);
    set_resolution_scale(settings_handler.get_resolution());
}

void Vector_Generator::set_resolution_scale(const sf::Vector2u resolution)
{
    this->resolution = resolution;
    if (resolution.x <= resolution.y && crop_ratio >= 1.0f)
    {
        res_scale = static_cast<float>(INTERNAL_RES) / resolution.x;
        x_offset = 0;
        y_offset = (resolution.y - resolution.x) * res_scale / 2;
    }
    else if (resolution.x >= resolution.y && crop_ratio <= 1.0f)
    {
        res_scale = static_cast<float>(INTERNAL_RES) / resolution.y;
        x_offset = (resolution.x - resolution.y) * res_scale / 2;
        y_offset = 0;
    }
    else
    {
        res_scale = INTERNAL_RES / (resolution.y * crop_ratio);
        x_offset = (resolution.x - resolution.y * crop_ratio) * res_scale / 2;
        y_offset = (resolution.y - resolution.y * crop_ratio) * res_scale / 2;
    }
    line_thickness = DESIRED_LINE_WIDTH / res_scale;
}

void Vector_Generator::process(const u16 vector_object[], RenderWindow& window, u16 iteration, const bool flip_x, const bool flip_y, const bool brighten)
{
    bool done = false;
    while (!done)
    {
        const Opcode opcode = static_cast<Opcode>(vector_object[iteration] >> 12);
        switch (opcode) // case ranges are not an official C++ standard
        {
        case VCTR_0 ... VCTR_9:
            draw_long_vector(opcode, vector_object, iteration, flip_x, flip_y, window);
            break;
        case LABS:
            load_absolute(vector_object, iteration);
            break;
        case RTSL:
            done = true;
            break;
        case SVEC:
            draw_short_vector(vector_object, iteration, flip_x, flip_y, brighten, window);
            break;
        default:
            cerr << "Process: invalid opcode \"" << static_cast<u16>(opcode) << "\"\n";
            done = true;
            break;
        }
        iteration++;
    }
}

// opcodes

void Vector_Generator::draw_long_vector(const Opcode opcode, const u16 vector_object[], u16& iteration, const bool flip_x, const bool flip_y, RenderWindow& window)
{
    s16 delta_y = vector_object[iteration] & 0x03FF;
    s16 delta_x = vector_object[iteration + 1] & 0x03FF;

    delta_y = apply_global_scale(delta_y >> (9 - opcode));
    delta_x = apply_global_scale(delta_x >> (9 - opcode));

    if (vector_object[iteration++] & 0x0400)
        delta_y = -delta_y;
    if (vector_object[iteration] & 0x0400)
        delta_x = -delta_x;

    const u8 brightness = vector_object[iteration] >> 12;

    draw_vector(delta_x, delta_y, brightness, flip_x, flip_y, window);
}

void Vector_Generator::load_absolute(const u16 vector_object[], u16& iteration)
{
    current_y = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration++] & 0x0400)
        current_y = -current_y;

    current_x = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration] & 0x0400)
        current_x = -current_x;

    global_scale = static_cast<Global_Scale>(vector_object[iteration] >> 12);
}

void Vector_Generator::draw_short_vector(const u16 vector_object[], u16& iteration, const bool flip_x, const bool flip_y, const bool brighten, RenderWindow& window)
{
    s16 delta_x = (vector_object[iteration] & 0x0003) << 8;
    s16 delta_y = vector_object[iteration] & 0x0300;

    const u8 local_scale = ((vector_object[iteration] & 0x0008) >> 2)
                         | ((vector_object[iteration] & 0x0800) >> 11);

    delta_x = apply_global_scale(delta_x >> (7 - local_scale));
    delta_y = apply_global_scale(delta_y >> (7 - local_scale));

    if (vector_object[iteration] & 0x0004)
        delta_x = -delta_x;
    if (vector_object[iteration] & 0x0400)
        delta_y = -delta_y;

    u8 brightness = (vector_object[iteration] & 0x00F0) >> 4;
    if (brighten)
        brightness += 2;

    draw_vector(delta_x, delta_y, brightness, flip_x, flip_y, window);
}

s16 Vector_Generator::apply_global_scale(const s16 delta) const
{
    if (global_scale <= MUL_128)
        return delta << global_scale;
    else
        return delta >> (16 - global_scale);
}

// drawing stuff

void Vector_Generator::draw_vector(const s16 raw_delta_x, const s16 raw_delta_y, const u8 brightness, const bool flip_x, const bool flip_y, RenderWindow& window)
{
    const s16 delta_x = (flip_x ? -raw_delta_x : raw_delta_x);
    const s16 delta_y = (flip_y ? -raw_delta_y : raw_delta_y);
    if (brightness)
    {
        const float adjusted_x = current_x + x_offset;
        const float adjusted_y = current_y + y_offset;

        const float scaled_x_start = adjusted_x / res_scale;
        const float scaled_y_start = resolution.y - adjusted_y / res_scale;
        const sf::Color vector_color = sf::Color(255, 255, 255, gamma_table[brightness]);
        if (line_thickness >= MINIMUM_LINE_WIDTH)
            draw_wide_line_segment(scaled_x_start, scaled_y_start, delta_x, delta_y, vector_color, window);
        else
            draw_thin_line_segment(scaled_x_start, scaled_y_start, delta_x, delta_y, vector_color, window);
    }
    current_x += delta_x;
    current_y += delta_y;
}

void Vector_Generator::draw_wide_line_segment(const float start_x, const float start_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        const float length = std::sqrt(std::pow(delta_x, 2) + std::pow(delta_y, 2)) / res_scale
                           + line_thickness * 2;
        sf::RectangleShape line(sf::Vector2f(length, 0.0f));
        const float rotation_radians = std::atan2(-delta_y, delta_x);
        const float rotation_degrees = rotation_radians * 180 / PI;

        line.setRotation(rotation_degrees);
        line.setOutlineThickness(line_thickness);
        line.setOutlineColor(vector_color);

        const float line_x_offset = std::cos(rotation_radians) * line_thickness;
        const float line_y_offset = std::sin(rotation_radians) * line_thickness;

        line.setPosition(start_x - line_x_offset, start_y - line_y_offset);
        window.draw(line);
    }
    else
    {
        const float dot_radius = PI / res_scale / 4;
        sf::CircleShape dot(dot_radius);
        dot.setOrigin(dot_radius, dot_radius);

        dot.setPosition(start_x, start_y);
        dot.setFillColor(vector_color);
        window.draw(dot);
    }
}

void Vector_Generator::draw_thin_line_segment(const float start_x, const float start_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        const float end_x = start_x + delta_x / res_scale;
        const float end_y = start_y - delta_y / res_scale;
        const sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(start_x, start_y), vector_color),
            sf::Vertex(sf::Vector2f(end_x, end_y), vector_color)
        };
        window.draw(line, 2, sf::Lines);
    }
    else
    {
        const sf::Vertex point[] =
        {
            sf::Vertex(sf::Vector2f(start_x, start_y), vector_color)
        };
        window.draw(point, 1, sf::Points);
    }
}
