// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Space_Object.h"

#include "../Other/Vectors.h"

const u8 MAX_X_POS_MAJOR = 31;
const u8 MAX_Y_POS_MAJOR = 23;

s8 Space_Object::lookup_sine(const u8 direction)
{
    if (direction <= 64)
        return SINE_TABLE[direction];
    if (direction < 128)
        return SINE_TABLE[128 - direction];
    if (direction <= 192)
        return -SINE_TABLE[direction - 128];
    else
        return -SINE_TABLE[256 - direction];
}

s8 Space_Object::lookup_cosine(const u8 direction)
{
    return lookup_sine(direction + 64);
}

void Space_Object::update_position()
{
    update_position(pos.x_major, pos.x_minor, vel_x_major);
    update_position(pos.y_major, pos.y_minor, vel_y_major);
    wrap_position(pos.x_major, MAX_X_POS_MAJOR);
    wrap_position(pos.y_major, MAX_Y_POS_MAJOR);
}

void Space_Object::update_position(u8& pos_major, u8& pos_minor, const s8 vel_major)
{
    u8 old_pos_minor = pos_minor;
    pos_minor += vel_major;

    if (vel_major < 0) // down or left
    {
        if (underflowed_u8(pos_minor, old_pos_minor))
            pos_major--;
    }
    else if (vel_major > 0) // up or right
    {
        if (overflowed_u8(pos_minor, old_pos_minor))
            pos_major++;
    }
}

void Space_Object::wrap_position(u8& pos_major, const u8 max_pos_major)
{
    if (pos_major == 255) // underflowed
        pos_major = max_pos_major;
    else if (pos_major > max_pos_major)
        pos_major = 0;
}

u8 Space_Object::limit_position(const u8 pos_major, const u8 max_pos_major)
{
    if (pos_major < 3)
        return 3;
    else if (pos_major > max_pos_major)
        return max_pos_major;
    else
        return pos_major;
}

bool Space_Object::collide(const Space_Object o_1, const Space_Object& o_2, const u8 minimum_space)
{
    if (o_1.get_status() && o_1.get_status() < TRUE_EXPLOSION_START
     && o_2.get_status() && o_2.get_status() < TRUE_EXPLOSION_START)
    {
        Coordinate pos_1 = get_total_pos(o_1.get_position());
        Coordinate pos_2 = get_total_pos(o_2.get_position());
        if (hit(pos_1.position_x, pos_2.position_x, minimum_space)
         && hit(pos_1.position_y, pos_2.position_y, minimum_space))
            return true;
    }
    return false;
}

bool Space_Object::hit(const u16 pos_1, const u16 pos_2, const u8 minimum_space)
{
    if (pos_1 < pos_2)
    {
        if (pos_2 - pos_1 < minimum_space * 2)
            return true;
    }
    else
    {
        if (pos_1 - pos_2 < minimum_space * 2 + 1)
            return true;
    }
    return false;
}

void Space_Object::draw_explosion(Vector_Generator& vector_generator, sf::RenderWindow& window) const
{
    s8 scale = (status - 1) / 17 - 14;
    vector_generator.load_absolute(pos, scale);

    switch (status / 4 % 4)
    {
        case 0:
            vector_generator.process(EXPLOSION_4, window);
            break;
        case 1:
            vector_generator.process(EXPLOSION_3, window);
            break;
        case 2:
            vector_generator.process(EXPLOSION_2, window);
            break;
        case 3:
            vector_generator.process(EXPLOSION_1, window);
    }
}

void Space_Object::copy_from_vector_table(u16 output_vector[], const u16 vector_object[], const u8 vector_offset)
{
    u8 i = 0;
    for (u8 x = vector_offset; vector_object[x] != 0xD000; x++, i++)
        output_vector[i] = vector_object[x];

    output_vector[i] = 0xD000;
}

s8 Space_Object::get_vel_x() const
{
    return vel_x_major;
}

s8 Space_Object::get_vel_y() const
{
    return vel_y_major;
}

Position Space_Object::get_position() const
{
    return pos;
}

u8 Space_Object::get_status() const
{
    return status;
}

void Space_Object::set_velocity(const s8 vel_x_major, const s8 vel_y_major)
{
    this->vel_x_major = vel_x_major;
    this->vel_y_major = vel_y_major;
}

void Space_Object::set_position_major(const u8 pos_x_major, const u8 pos_y_major)
{
    pos.x_major = pos_x_major;
    pos.y_major = pos_y_major;
}

void Space_Object::set_status(const u8 status)
{
    this->status = status;
}
