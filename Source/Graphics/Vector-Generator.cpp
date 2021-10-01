// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Vector-Generator.h"

#include <cmath>
#include <SFML/Graphics.hpp>
#include "../Settings/Settings-Handler.h"

const float DESIRED_LINE_WIDTH = 0.75f;
const float MINIMUM_LINE_WIDTH = 0.5f;
const u16 INTERNAL_RES = 1024;

Vector_Generator::Vector_Generator(const Settings_Handler& settings)
{
    settings.get_settings(crop_ratio, gamma_table);
    set_resolution_scale(settings);
}

void Vector_Generator::set_resolution_scale(const Settings_Handler& settings)
{
    this->resolution = settings.get_resolution();
    if (crop_ratio < 1.0f)
    {
        if (resolution.x > resolution.y * crop_ratio)
            crop_with_extra_space(resolution.x, x_offset, resolution.y, y_offset);
        else
        {
            res_scale = INTERNAL_RES / (resolution.x / crop_ratio);
            x_offset = (resolution.x - resolution.x / crop_ratio) * res_scale / 2;
            y_offset = (resolution.y - resolution.x / crop_ratio) * res_scale / 2;
        }
    }
    else
    {
        if (resolution.x < resolution.y * crop_ratio)
            crop_with_extra_space(resolution.y, y_offset, resolution.x, x_offset);
        else
        {
            res_scale = INTERNAL_RES / (resolution.y * crop_ratio);
            x_offset = (resolution.x - resolution.y * crop_ratio) * res_scale / 2;
            y_offset = (resolution.y - resolution.y * crop_ratio) * res_scale / 2;
        }
    }
    line_thickness = DESIRED_LINE_WIDTH / res_scale;
}

void Vector_Generator::crop_with_extra_space(u32 axis_to_crop, float& crop_offset, u32 scale_axis, float& scale_offset)
{
    res_scale = static_cast<float>(INTERNAL_RES) / scale_axis;
    crop_offset = static_cast<s32>(axis_to_crop - scale_axis) * res_scale / 2;
    scale_offset = 0;
}

void Vector_Generator::process(const u16 vector_object[], RenderWindow& window, u16 iteration, float rotation_radians, bool brighten)
{
    bool done = false;
    while (!done)
    {
        const Opcode opcode = static_cast<Opcode>(vector_object[iteration] >> 12);
        switch (opcode) // case ranges are not an official C++ standard
        {
        case VCTR_0 ... VCTR_9:
            draw_long_vector(opcode, vector_object, iteration, rotation_radians, window);
            break;
        case LABS:
            load_absolute(vector_object, iteration);
            break;
        case RTSL:
            done = true;
            break;
        case SVEC:
            draw_short_vector(vector_object, iteration, rotation_radians, brighten, window);
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

void Vector_Generator::draw_long_vector(Opcode opcode, const u16 vector_object[], u16& iteration, float rotation_radians, RenderWindow& window)
{
    float delta_y = vector_object[iteration]     & 0x03FF;
    float delta_x = vector_object[iteration + 1] & 0x03FF;

    delta_y = apply_global_scale(delta_y, opcode - 9);
    delta_x = apply_global_scale(delta_x, opcode - 9);

    if (vector_object[iteration++] & 0x0400)
        delta_y = -delta_y;
    if (vector_object[iteration]   & 0x0400)
        delta_x = -delta_x;

    const u8 brightness = vector_object[iteration] >> 12;

    draw_vector(delta_x, delta_y, brightness, rotation_radians, window);
}

void Vector_Generator::load_absolute(const u16 vector_object[], u16& iteration)
{
    current_y = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration++] & 0x0400)
        current_y = -current_y;

    current_x = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration]   & 0x0400)
        current_x = -current_x;

    global_scale = static_cast<Global_Scale>(vector_object[iteration] >> 12);
}

void Vector_Generator::draw_short_vector(const u16 vector_object[], u16 iteration, float rotation_radians, bool brighten, RenderWindow& window)
{
    float delta_x = (vector_object[iteration] & 0x0003) << 8;
    float delta_y =  vector_object[iteration] & 0x0300;

    const u8 local_scale = (vector_object[iteration] & 0x0008) >>  2
                         | (vector_object[iteration] & 0x0800) >> 11;

    delta_x = apply_global_scale(delta_x, local_scale - 7);
    delta_y = apply_global_scale(delta_y, local_scale - 7);

    if (vector_object[iteration] & 0x0004)
        delta_x = -delta_x;
    if (vector_object[iteration] & 0x0400)
        delta_y = -delta_y;

    u8 brightness = (vector_object[iteration] & 0x00F0) >> 4;
    if (brighten)
        brightness += 2;

    draw_vector(delta_x, delta_y, brightness, rotation_radians, window);
}

float Vector_Generator::apply_global_scale(float delta, float local_scale) const
{
    const float local_scaled_delta = delta * std::pow(2, local_scale);

    if (global_scale < DIV_256)
        return local_scaled_delta * std::pow(2, global_scale);

    return local_scaled_delta * std::pow(2, global_scale - 16);
}

// drawing stuff

void Vector_Generator::draw_vector(float raw_delta_x, float raw_delta_y, u8 brightness, float rotation_radians, RenderWindow& window)
{
    const float height = std::sin(rotation_radians);
    const float width = std::cos(rotation_radians);
    const float delta_x = raw_delta_x * width - raw_delta_y * height;
    const float delta_y = raw_delta_x * height + raw_delta_y * width;
    if (brightness)
    {
        const float adjusted_x = current_x + x_offset;
        const float adjusted_y = current_y + y_offset;

        const float scaled_x_start =                adjusted_x / res_scale;
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

void Vector_Generator::draw_wide_line_segment(float start_x, float start_y, float delta_x, float delta_y, sf::Color vector_color, RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        const float length = std::sqrt(std::pow(delta_x, 2) + std::pow(delta_y, 2))
                           / res_scale + line_thickness * 2;
        sf::RectangleShape line(sf::Vector2f(length, line_thickness * 2));
        const float rotation_degrees = std::atan2(-delta_y, delta_x) * 180 / PI;

        line.setOrigin(line_thickness, line_thickness);
        line.setRotation(rotation_degrees);
        line.setFillColor(vector_color);
        line.setPosition(start_x, start_y);

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

void Vector_Generator::draw_thin_line_segment(float start_x, float start_y, float delta_x, float delta_y, sf::Color vector_color, RenderWindow& window) const
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

// float vector object functions

void Vector_Generator::process(const float vector_object[], RenderWindow& window, u16 iteration, float rotation_radians, bool brighten)
{
    bool done = false;
    while (!done)
    {
        const float opcode = vector_object[iteration++];
        if (opcode <= VCTR_9)
            draw_long_vector(opcode, vector_object, iteration, rotation_radians, window);
        else if (opcode == LABS)
            load_absolute(vector_object, iteration);
        else if (opcode == RTSL)
            done = true;
        else if (opcode == SVEC)
            draw_short_vector(vector_object, iteration, rotation_radians, brighten, window);
        else if (opcode == EQIV)
        {
            // only implemented SVEC in EQIV because other opcodes weren't needed
            const u16 old_vector_object[] = {static_cast<u16>(vector_object[iteration])};
            draw_short_vector(old_vector_object, 0, rotation_radians, brighten, window);
            iteration++;
        }
        else
        {
            cerr << "Process: invalid opcode \"" << static_cast<u16>(opcode) << "\"\n";
            done = true;
            break;
        }
    }
}

void Vector_Generator::draw_long_vector(float opcode, const float vector_object[], u16& iteration, float rotation_radians, RenderWindow& window)
{
    const float delta_x = apply_global_scale(vector_object[iteration++], opcode - 9);
    const float delta_y = apply_global_scale(vector_object[iteration++], opcode - 9);
    const float brightness = vector_object[iteration++];

    draw_vector(delta_x, delta_y, brightness, rotation_radians, window);
}

void Vector_Generator::load_absolute(const float vector_object[], u16& iteration)
{
    current_x = vector_object[iteration++];
    current_y = vector_object[iteration++];
    global_scale = vector_object[iteration++];
}

void Vector_Generator::draw_short_vector(const float vector_object[], u16& iteration, float rotation_radians, bool brighten, RenderWindow& window)
{
    const float local_scale = vector_object[iteration + 2] + 1;
    const float delta_x = apply_global_scale(vector_object[iteration++], local_scale);
    const float delta_y = apply_global_scale(vector_object[iteration], local_scale);
    iteration += 2;
    float brightness = vector_object[iteration++];
    if (brighten)
        brightness += 2;

    draw_vector(delta_x, delta_y, brightness, rotation_radians, window);
}
