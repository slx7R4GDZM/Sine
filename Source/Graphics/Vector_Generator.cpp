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
    : x_offset(0)
    , y_offset(0)
{
    settings_handler.get_settings(x_resolution, y_resolution, simulate_DAC, crop_image, gamma_table);
    if (crop_image)
    {
        if (x_resolution < y_resolution)
        {
            res_scale = static_cast<float>(INTERNAL_RES) / y_resolution;
            x_offset = (y_resolution - x_resolution) * res_scale / -2;
        }
        else
        {
            res_scale = static_cast<float>(INTERNAL_RES) / x_resolution;
            y_offset = (x_resolution - y_resolution) * res_scale / -2;
        }
    }
    else
    {
        if (x_resolution < y_resolution)
        {
            res_scale = static_cast<float>(INTERNAL_RES) / x_resolution;
            y_offset = (y_resolution - x_resolution) * res_scale / 2;
        }
        else
        {
            res_scale = static_cast<float>(INTERNAL_RES) / y_resolution;
            x_offset = (x_resolution - y_resolution) * res_scale / 2;
        }
    }
    line_thickness = DESIRED_LINE_WIDTH / (res_scale / 4);
}

void Vector_Generator::flip(u16 vector_object[], const bool flip_x, const bool flip_y)
{
    bool done = false;
    for (u8 i = 0; !done; i++)
    {
        u8 opcode = vector_object[i] >> 12;
        switch (opcode) // case ranges are not an official C++ standard
        {
            case 0 ... 9: // DLVEC, VCTR, VEC
                if (flip_y)
                    vector_object[i] ^= 1 << 10;
                i++;
                if (flip_x)
                    vector_object[i] ^= 1 << 10;
                break;
            case 13: // DRTSL, RTSL, RTS
                done = true;
                break;
            case 15: // DSVEC, SVEC
                if (flip_x)
                    vector_object[i] ^= 1 << 2;
                if (flip_y)
                    vector_object[i] ^= 1 << 10;
                break;
            default:
                cerr << "Flip: invalid opcode \"" << static_cast<u16>(opcode) << "\"\n";
                done = true;
                break;
        }
    }
}

void Vector_Generator::brighten(u16 vector_object[])
{
    bool done = false;
    for (u8 i = 0; !done; i++)
    {
        u8 new_brightness;
        u8 opcode = vector_object[i] >> 12;
        switch (opcode)
        {
            case 13: // DRTSL, RTSL, RTS
                done = true;
                break;
            case 15: // DSVEC, SVEC
                new_brightness = ((vector_object[i] & 0x00F0) >> 4) + 2;
                vector_object[i] = (vector_object[i] & 0xFF0F) | new_brightness << 4;
                break;
            default:
                cerr << "Brighten: invalid opcode \"" << static_cast<u16>(opcode) << "\"\n";
                done = true;
                break;
        }
    }
}

void Vector_Generator::process(const u16 vector_object[], sf::RenderWindow& window, u8 iteration)
{
    bool done = false;
    while (!done)
    {
        u8 opcode = vector_object[iteration] >> 12;
        switch (opcode) // case ranges are not an official C++ standard
        {
            case 0 ... 9: // DLVEC, VCTR, VEC
                draw_long_vector(opcode, vector_object, iteration, window);
                break;
            case 10: // DLABS, LABS, CUR
                load_absolute(vector_object, iteration, window);
            case 13: // DRTSL, RTSL, RTS
                done = true;
                break;
            case 15: // DSVEC, SVEC
                draw_short_vector(vector_object, iteration, window);
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

// 0-9
void Vector_Generator::draw_long_vector(const u8 opcode, const u16 vector_object[], u8& iteration, sf::RenderWindow& window)
{
    s16 delta_y = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration++] & 0x0400)
        delta_y = -delta_y;

    s16 delta_x = vector_object[iteration] & 0x03FF;
    if (vector_object[iteration] & 0x0400)
        delta_x = -delta_x;

    u8 local_scale = (global_scale + opcode) & 0x0F;

    delta_x = (delta_x << 2) >> (9 - local_scale);
    delta_y = (delta_y << 2) >> (9 - local_scale);

    u8 brightness = vector_object[iteration] >> 12;
    draw_vector(delta_x, delta_y, brightness, window);
}

// 10
// used for space objects
void Vector_Generator::load_absolute(const Position& pos, const s8 scale)
{
    Coordinate current_pos = get_total_pos(pos);
    // divided by 2 to convert the 8192x6144 of the Space_Object
    // coordinate system to the 4096x4096 res used by the DVG
    current_x =                       current_pos.position_x / 2;
    // subtract from y res to reverse current_y
    // subtract 512 to make the 4:3 Space_Object space be centered in the 1:1 DVG space
    current_y = INTERNAL_RES - 512 - (current_pos.position_y / 2);
    global_scale = scale;
}

// used for letters, numbers, etc
void Vector_Generator::load_absolute(const u8 cur_x, const u8 cur_y, const s8 scale)
{
    current_x = cur_x << 4;
    current_y = cur_y << 4;
    global_scale = scale;
}

// should be required to do test pattern?
void Vector_Generator::load_absolute(const u16 vector_object[], u8& iteration, sf::RenderWindow& window)
{
    current_y = (vector_object[iteration++] & 0x03FF) << 2;
    current_x = (vector_object[iteration] & 0x03FF) << 2;
    global_scale = vector_object[iteration] >> 12;
}

// 15
void Vector_Generator::draw_short_vector(const u16 vector_object[], u8& iteration, sf::RenderWindow& window)
{
    s16 delta_y = vector_object[iteration] & 0x0300;
    if (vector_object[iteration] & 0x0400)
        delta_y = -delta_y;

    s16 delta_x = (vector_object[iteration] & 0x0003) << 8;
    if (vector_object[iteration] & 0x0004)
        delta_x = -delta_x;

    u8 local_scale = ((vector_object[iteration] >> 2) & 0x0002) + ((vector_object[iteration] >> 11) & 0x0001);
    local_scale = (global_scale + local_scale) & 0x0F;

    delta_x = (delta_x << 2) >> (7 - local_scale);
    delta_y = (delta_y << 2) >> (7 - local_scale);

    u8 brightness = (vector_object[iteration] >> 4) & 0x000F;
    draw_vector(delta_x, delta_y, brightness, window);
}

// drawing stuff

void Vector_Generator::draw_vector(const s16 delta_x, const s16 delta_y, const u8 brightness, sf::RenderWindow& window)
{
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

        sf::Color vector_color = sf::Color(255, 255, 255, gamma_table[brightness]);
        if (line_thickness < MINIMUM_LINE_WIDTH)
            draw_thin_line_segment(adjusted_x, adjusted_y, delta_x, delta_y, vector_color, window);
        else
            draw_wide_line_segment(adjusted_x, adjusted_y, delta_x, delta_y, vector_color, window);
    }

    current_x += delta_x;
    current_y -= delta_y;
}

void Vector_Generator::draw_wide_line_segment(const float starting_x, const float starting_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        float length = std::sqrt(std::pow(delta_x, 2) + std::pow(delta_y, 2)) / res_scale;
        sf::RectangleShape line(sf::Vector2f(length, 0.0f));
        line.setOutlineThickness(line_thickness);
        line.setRotation(std::atan2(-delta_y, delta_x) * 180 / PI);

        line.setPosition(starting_x / res_scale, starting_y / res_scale);
        line.setOutlineColor(vector_color);
        window.draw(line);
    }
    else
    {
        float dot_radius = PI / res_scale;
        sf::CircleShape dot(dot_radius);
        dot.setOrigin(dot_radius, dot_radius);

        dot.setPosition(starting_x / res_scale, starting_y / res_scale);
        dot.setFillColor(vector_color);
        window.draw(dot);
    }
}

void Vector_Generator::draw_thin_line_segment(const float starting_x, const float starting_y, const s16 delta_x, const s16 delta_y, const sf::Color vector_color, sf::RenderWindow& window) const
{
    if (delta_x || delta_y)
    {
        sf::Vertex line[2] =
        {
            sf::Vertex(sf::Vector2f(starting_x / res_scale, starting_y / res_scale), vector_color),
            sf::Vertex(sf::Vector2f((starting_x + delta_x) / res_scale, (starting_y - delta_y) / res_scale), vector_color)
        };
        window.draw(line, 2, sf::Lines);
    }
    else
    {
        sf::Vertex point[1] = sf::Vertex(sf::Vector2f(starting_x / res_scale, starting_y / res_scale), vector_color);
        window.draw(point, 1, sf::Points);
    }
}
