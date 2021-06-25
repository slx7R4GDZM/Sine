// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Space-Object.h"

#include "../Graphics/Vector-Generator.h"
#include "../Other/Constants.h"
#include "../Other/Vectors.h"

static void wrap_position(u8& pos_major, u8 max_pos_major);
static bool miss(u8 pos_major_1, u8 pos_minor_1, u8 pos_major_2, u8 pos_minor_2, u8& distance_LSB);

Space_Object::Space_Object()
{
    status = INDISCERNIBLE;
}

s8 Space_Object::lookup_sine(u8 direction)
{
    if (direction <= 64)
        return SINE_TABLE[direction];
    if (direction < 128)
        return SINE_TABLE[128 - direction];
    if (direction <= 192)
        return -SINE_TABLE[direction - 128];

    return -SINE_TABLE[256 - direction];
}

s8 Space_Object::lookup_cosine(u8 direction)
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

void Space_Object::update_position_axis(u8& pos_major, u8& pos_minor, s8 vel_major)
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

void wrap_position(u8& pos_major, u8 max_pos_major)
{
    if (pos_major == 255) // underflowed
        pos_major = max_pos_major;
    else if (pos_major > max_pos_major)
        pos_major = 0;
}

u8 Space_Object::limit_position(u8 pos_major, u8 max_pos_major)
{
    if (pos_major < 3)
        return 3;

    if (pos_major > max_pos_major)
        return max_pos_major;

    return pos_major;
}

bool Space_Object::collide(Space_Object object, u8 radius) const
{
    if (       status == INDISCERNIBLE ||        status >= TRUE_EXPLOSION_START
     || object.status == INDISCERNIBLE || object.status >= TRUE_EXPLOSION_START)
        return false;

    u8 x_distance;
    u8 y_distance;
    if (miss(pos.x_major, pos.x_minor, object.pos.x_major, object.pos.x_minor, x_distance)
     || miss(pos.y_major, pos.y_minor, object.pos.y_major, object.pos.y_minor, y_distance))
        return false;

    if (x_distance > radius || y_distance > radius)
        return false;

    const int combined_distance = x_distance + y_distance;
    const int circular_hitbox = radius + (radius >> 1); // radius * 1.5, approximation of sqrt 2

    return combined_distance < circular_hitbox;
}

bool miss(u8 pos_major_1, u8 pos_minor_1, u8 pos_major_2, u8 pos_minor_2, u8& distance_LSB)
{
    bool carry = 1;
    distance_LSB = sbc(pos_minor_2, pos_minor_1, carry);

    u8 distance_MSB = sbc(pos_major_2, pos_major_1, carry);
    carry = distance_MSB & 1;
    distance_MSB &= 0xFE;

    distance_LSB = ror(distance_LSB, carry);

    if (distance_MSB == 0)
        return false;

    if (distance_MSB != 0xFE)
        return true;

    distance_LSB = ~distance_LSB;
    return false;
}

void Space_Object::draw_explosion(Vector_Generator& vector_generator, RenderWindow& window) const
{
    const Global_Scale scale = static_cast<Global_Scale>(((status & 0xF0) + 0x10) >> 4);
    set_position_and_size(scale, vector_generator, window);

    switch (status >> 2 & 3)
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

void Space_Object::set_position_and_size(Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window) const
{
    // add 128 to y making the 4:3 Space_Object space centered inside the 1:1 DVG space
    const u16 vector_object[] =
    {
        static_cast<u16>((LABS << 12 | pos.y_major << 5 | pos.y_minor >> 3) + 128),
        static_cast<u16>(scale << 12 | pos.x_major << 5 | pos.x_minor >> 3),
                          RTSL << 12
    };
    vector_generator.process(vector_object, window);
}
