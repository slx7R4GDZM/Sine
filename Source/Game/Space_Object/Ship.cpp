// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Ship.h"

#include "../../Other/Vectors.h"

const s8 MIN_VEL = -64;
const u8 MAX_VEL = 63;
//const u8 MAX_SHIP_VECTOR_LENGTH = 13;
//const u8 MAX_THRUST_VECTOR_LENGTH = 5;
const u8 MAX_SHIP_VECTOR_LENGTH = 15;
const u8 MAX_THRUST_VECTOR_LENGTH = 7;

Ship::Ship()
{
    vel_x_major = 0;
    vel_y_major = 0;
    // low velocity is not reset on ship create or respawn
    // high velocity is set to 0 on collision, not on respawn
    // need to check if area is clear of other spaceobjects
    //if (spawn_area_clear())
        pos = {16, 12, 96, 96};
}

u8 Ship::update(Vector_Generator& vector_generator, sf::RenderWindow& window, const u8 fast_timer, const u8 direction, bool& draw_thrust_graphic)
{
    if (status == ALIVE)
    {
        update_position();
        draw(draw_thrust_graphic, direction, vector_generator, window);
    }
    else if (status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(vector_generator, window);
        if (fast_timer % 2)
            status++;
    }
    return status;
}

void Ship::add_thrust(const u8 direction, s8& vel_x_major, u8& vel_x_minor, s8& vel_y_major, u8& vel_y_minor)
{
    // x
    u8 old_vel_minor = vel_x_minor;
    vel_x_minor += lookup_cosine(direction) * 2;

    if (direction > 64 && direction < 192) // left
        negative_vel_change(vel_x_major, vel_x_minor, old_vel_minor);
    else // right
        positive_vel_change(vel_x_major, vel_x_minor, old_vel_minor);

    // y
    old_vel_minor = vel_y_minor;
    vel_y_minor += lookup_sine(direction) * 2;

    if (direction > 128) // down
        negative_vel_change(vel_y_major, vel_y_minor, old_vel_minor);
    else // up
        positive_vel_change(vel_y_major, vel_y_minor, old_vel_minor);
}

void Ship::negative_vel_change(s8& vel_major, u8& vel_minor, const u8 old_vel_minor)
{
    if (underflowed_u8(vel_minor, old_vel_minor))
        vel_major == MIN_VEL ? vel_minor = 1 : vel_major--;
}

void Ship::positive_vel_change(s8& vel_major, u8& vel_minor, const u8 old_vel_minor)
{
    if (overflowed_u8(vel_minor, old_vel_minor))
        vel_major == MAX_VEL ? vel_minor = 255 : vel_major++;
}

void Ship::dampen_velocity(s8& vel_x_major, u8& vel_x_minor, s8& vel_y_major, u8& vel_y_minor)
{
    dampen_velocity(vel_x_major, vel_x_minor);
    dampen_velocity(vel_y_major, vel_y_minor);
}

void Ship::dampen_velocity(s8& vel_major, u8& vel_minor)
{
    if (vel_major < 0) // reduction of 2 to 128
    {
        u8 old_vel_minor = vel_minor;
        vel_minor -= vel_major * 2; // subtract the negative vel_major to increase value
        if (overflowed_u8(vel_minor, old_vel_minor))
            vel_major++;
    }
    else if (vel_minor || vel_major) // reduction of 1 to 127
    {
        u8 old_vel_minor = vel_minor;
        vel_minor -= vel_major * 2 + 1;
        if (underflowed_u8(vel_minor, old_vel_minor))
            vel_major--;
    }
}

void Ship::draw(bool& draw_thrust_graphic, const u8 direction, Vector_Generator& vector_generator, sf::RenderWindow& window) const
{
    u8 ship_vector_offset,
    ship_thrust_vector_offset;
    bool flip_x = false;
    bool flip_y = false;
    calculate_ship_vector_properties(direction, ship_vector_offset, ship_thrust_vector_offset, flip_x, flip_y);

    u16 ship_vector[MAX_SHIP_VECTOR_LENGTH];
    copy_from_vector_table(ship_vector, SHIP_TABLE, ship_vector_offset);
    vector_generator.flip(ship_vector, flip_x, flip_y);
    vector_generator.load_absolute(pos, -2);
    vector_generator.process(ship_vector, window);

    if (draw_thrust_graphic)
    {
        draw_thrust(ship_thrust_vector_offset, flip_x, flip_y, vector_generator, window);
        draw_thrust_graphic = false;
    }
}

void Ship::draw_thrust(const u8 ship_thrust_vector_offset, const bool flip_x, const bool flip_y, Vector_Generator& vector_generator, sf::RenderWindow& window)
{
    u16 ship_thrust_vector[MAX_THRUST_VECTOR_LENGTH];
    copy_from_vector_table(ship_thrust_vector, SHIP_THRUST_TABLE, ship_thrust_vector_offset);
    vector_generator.flip(ship_thrust_vector, flip_x, flip_y);
    vector_generator.process(ship_thrust_vector, window);
}

void Ship::calculate_ship_vector_properties(const u8 direction, u8& ship_vector_offset, u8& ship_thrust_vector_offset, bool& flip_x, bool& flip_y)
{
    u8 vector_offset;
    if (direction < 64)
        vector_offset = direction / 4;
    else if (direction < 128)
    {
        vector_offset = 31 - (direction - 1) / 4;
        flip_x = true;
    }
    else if (direction < 192)
    {
        vector_offset = (direction - 128) / 4;
        flip_x = true;
        flip_y = true;
    }
    else
    {
        vector_offset = 63 - (direction - 1) / 4;
        flip_y = true;
    }
    ship_vector_offset = SHIP_OFFSET_TABLE[vector_offset];
    ship_thrust_vector_offset = SHIP_THRUST_OFFSET_TABLE[vector_offset];
}
