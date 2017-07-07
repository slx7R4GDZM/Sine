// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Vector_Generator.h"

#include <cmath>

const float PI = std::atan(1) * 4;
const float DESIRED_LINE_WIDTH = 0.75f;
const float MINIMUM_LINE_WIDTH = 0.5f;
const u16 INTERNAL_RES = 4096; // the internal resolution axes are 12-bit due to the
                               // two connected 6-bit Binary Rate Multipliers that
                               // supplied the clocks to the X and Y position counters

Vector_Generator::Vector_Generator(const Settings_Handler settings_handler)
{
    settings_handler.get_settings(simulate_DAC, crop_ratio, gamma_table);
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
    line_thickness = DESIRED_LINE_WIDTH / (res_scale / 4);
}

void Vector_Generator::process(const u16 vector_object[], sf::RenderWindow& window, u8 iteration, const bool flip_x, const bool flip_y, const bool brighten)
{
    bool done = false;
    while (!done)
    {
        Opcode opcode = static_cast<Opcode>(vector_object[iteration] >> 12);
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

void Vector_Generator::draw_long_vector(const Opcode opcode, const u16 vector_object[], u8& iteration, const bool flip_x, const bool flip_y, sf::RenderWindow& window)
{
    s16 delta_y = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration++] & 0x0400)
        delta_y = -delta_y;

    s16 delta_x = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration] & 0x0400)
        delta_x = -delta_x;

    delta_x = apply_global_scale((delta_x << 2) >> (9 - opcode));
    delta_y = apply_global_scale((delta_y << 2) >> (9 - opcode));

    const u8 brightness = vector_object[iteration] >> 12;

    draw_vector(delta_x, delta_y, opcode, brightness, flip_x, flip_y, window);
}

void Vector_Generator::load_absolute(const u16 vector_object[], u8& iteration)
{
    current_y = (vector_object[iteration++] & 0x03FF) << 2;
    current_x = (vector_object[iteration] & 0x03FF) << 2;
    global_scale = static_cast<Global_Scale>(vector_object[iteration] >> 12);
}

void Vector_Generator::draw_short_vector(const u16 vector_object[], u8& iteration, const bool flip_x, const bool flip_y, const bool brighten, sf::RenderWindow& window)
{
    s16 delta_y = vector_object[iteration] & 0x0300;
    if (vector_object[iteration] & 0x0400)
        delta_y = -delta_y;

    s16 delta_x = (vector_object[iteration] & 0x0003) << 8;
    if (vector_object[iteration] & 0x0004)
        delta_x = -delta_x;

    const u8 local_scale = ((vector_object[iteration] & 0x0008) >> 2)
                         + ((vector_object[iteration] & 0x0800) >> 11);

    delta_x = apply_global_scale((delta_x << 2) >> (7 - local_scale));
    delta_y = apply_global_scale((delta_y << 2) >> (7 - local_scale));

    u8 brightness = (vector_object[iteration] & 0x00F0) >> 4;
    if (brighten)
        brightness += 2;

    draw_vector(delta_x, delta_y, local_scale, brightness, flip_x, flip_y, window);
}

s16 Vector_Generator::apply_global_scale(const s16 delta) const
{
    if (global_scale <= MUL_128)
        return delta << global_scale;
    else
        return delta >> (16 - global_scale);
}

// drawing stuff

void Vector_Generator::draw_vector(const s16 raw_delta_x, const s16 raw_delta_y, const u8 local_scale, const u8 brightness, const bool flip_x, const bool flip_y, sf::RenderWindow& window)
{
    const s16 delta_x = (flip_x ? -raw_delta_x : raw_delta_x);
    const s16 delta_y = (flip_y ? -raw_delta_y : raw_delta_y);
    if (brightness)
    {
        float adjusted_x;
        float adjusted_y;
        if (simulate_DAC)
        {
            // mask off the lower 2 bits of the 12-bit internal resolution
            // thus simulating the 10-bit DAC resolution
            adjusted_x = static_cast<s16>(current_x & 0xFFFC) + x_offset;
            adjusted_y = static_cast<s16>(current_y & 0xFFFC) + y_offset;
        }
        else
        {
            adjusted_x = current_x + x_offset;
            adjusted_y = current_y + y_offset;
        }

        const float scaled_x_start = adjusted_x / res_scale;
        const float scaled_y_start = resolution.y - adjusted_y / res_scale;
        sf::Color vector_color = sf::Color(255, 255, 255, gamma_table[brightness]);
        if (line_thickness >= MINIMUM_LINE_WIDTH)
            draw_wide_line_segment(scaled_x_start, scaled_y_start, delta_x, delta_y, vector_color, window);
        else
            draw_thin_line_segment(scaled_x_start, scaled_y_start, adjusted_x, adjusted_y, delta_x, delta_y, vector_color, window);
    }
    current_x += delta_x;
    current_y += delta_y;
}

void Vector_Generator::draw_wide_line_segment(const float starting_x, const float starting_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        const float length = std::sqrt(std::pow(delta_x, 2) + std::pow(delta_y, 2)) / res_scale;
        sf::RectangleShape line(sf::Vector2f(length, 0.0f));
        line.setOutlineThickness(line_thickness);
        line.setRotation(std::atan2(-delta_y, delta_x) * 180 / PI);

        line.setPosition(starting_x, starting_y);
        line.setOutlineColor(vector_color);
        window.draw(line);
    }
    else
    {
        const float dot_radius = PI / res_scale;
        sf::CircleShape dot(dot_radius);
        dot.setOrigin(dot_radius, dot_radius);

        dot.setPosition(starting_x, starting_y);
        dot.setFillColor(vector_color);
        window.draw(dot);
    }
}

void Vector_Generator::draw_thin_line_segment(const float scaled_x_start, const float scaled_y_start, const float x_start, const float y_start, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        const float x_end = (x_start + delta_x) / res_scale;
        const float y_end = resolution.y - (y_start + delta_y) / res_scale;
        const sf::Vertex line[2] =
        {
            sf::Vertex(sf::Vector2f(scaled_x_start, scaled_y_start), vector_color),
            sf::Vertex(sf::Vector2f(x_end, y_end), vector_color)
        };
        window.draw(line, 2, sf::Lines);
    }
    else
    {
        const sf::Vertex point[1] = sf::Vertex(sf::Vector2f(scaled_x_start, scaled_y_start), vector_color);
        window.draw(point, 1, sf::Points);
    }
}
