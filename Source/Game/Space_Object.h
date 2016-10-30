// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Graphics/Vector_Generator.h"
#include "../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

const u8 SINE_TABLE[65]
{
    0x00, 0x03, 0x06, 0x09, 0x0C, 0x10, 0x13, 0x16,
    0x19, 0x1C, 0x1F, 0x22, 0x25, 0x28, 0x2B, 0x2E,
    0x31, 0x33, 0x36, 0x39, 0x3C, 0x3F, 0x41, 0x44,
    0x47, 0x49, 0x4C, 0x4E, 0x51, 0x53, 0x55, 0x58,
    0x5A, 0x5C, 0x5E, 0x60, 0x62, 0x64, 0x66, 0x68,
    0x6A, 0x6B, 0x6D, 0x6F, 0x70, 0x71, 0x73, 0x74,
    0x75, 0x76, 0x78, 0x79, 0x7A, 0x7A, 0x7B, 0x7C,
    0x7D, 0x7D, 0x7E, 0x7E, 0x7E, 0x7F, 0x7F, 0x7F,
    0x7F
};
const u8 TRUE_EXPLOSION_START = 128;
const u8 ALIVE = 1;

class Space_Object
{
private:
    static void update_position(u8& pos_major, u8& pos_minor, const s8 vel_major);
    static void wrap_position(u8& pos_major, const u8 max_pos_major);
protected:
    s8 vel_x_major;
    s8 vel_y_major;
    Position pos;
    u8 status;

    static s8 lookup_sine(const u8 direction);
    static s8 lookup_cosine(const u8 direction);
    void update_position();
    static bool hit(const u16 pos1, const u16 pos2, const u8 minimum_space);
    void draw_explosion(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
    static void copy_from_vector_table(u16 output_vector[], const u16 vector_object[], const u8 vector_offset);
public:
    Space_Object() = default;
    static u8 limit_position(const u8 pos_major, const u8 max_pos_major);
    static bool collide(const Space_Object o1, const Space_Object& o2, const u8 minimum_space);

    s8 get_vel_x() const;
    s8 get_vel_y() const;
    Position get_position() const;
    u8 get_status() const;

    void set_velocity(const s8 vel_x_major, const s8 vel_y_major);
    void set_position_major(const u8 pos_x_major, const u8 pos_y_major);
    void set_status(const u8 status);
};
