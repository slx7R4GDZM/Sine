// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Space-Object.h"

#include "../Graphics/Vector-Generator.h"
#include "../Other/Constants.h"
#include "../Other/Vectors.h"

Space_Object::Space_Object()
{
    status = INDISCERNIBLE;
}

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
    update_position_axis(pos.x_major, pos.x_minor, vel_x_major);
    update_position_axis(pos.y_major, pos.y_minor, vel_y_major);
    wrap_position(pos.x_major, MAX_X_POS_MAJOR);
    wrap_position(pos.y_major, MAX_Y_POS_MAJOR);
}

void Space_Object::update_position_axis(u8& pos_major, u8& pos_minor, const s8 vel_major)
{
    const u8 old_pos_minor = pos_minor;
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

bool Space_Object::collide(const Space_Object& object, const u8 minimum_space) const
{
    if (status && status < TRUE_EXPLOSION_START
     && object.status && object.status < TRUE_EXPLOSION_START)
    {
        Coordinate pos_1 = get_total_pos(pos);
        Coordinate pos_2 = get_total_pos(object.pos);
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

void Space_Object::draw_explosion(Vector_Generator& vector_generator, RenderWindow& window) const
{
    const Global_Scale scale = static_cast<Global_Scale>((status + 33) / 17);
    set_position_and_size(scale, vector_generator, window);

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
        break;
    }
}

void Space_Object::set_position_and_size(const Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window) const
{
    // add 128 to y making the 4:3 Space_Object space centered inside the 1:1 DVG space
    const u16 vector_object[] =
    {
        static_cast<u16>(((LABS << 12) | (pos.y_major << 5) | (pos.y_minor >> 3)) + 128),
        static_cast<u16>((scale << 12) | (pos.x_major << 5) | (pos.x_minor >> 3)),
                          (RTSL << 12)
    };
    vector_generator.process(vector_object, window);
}

u8 Space_Object::get_status() const
{
    return status;
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

void Space_Object::set_status(const u8 status)
{
    this->status = status;
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
