// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Ship.h"

#include <cstring>
#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

static void negative_vel_change(s8& vel_major, u8& vel_minor, u8 old_vel_minor);
static void positive_vel_change(s8& vel_major, u8& vel_minor, u8 old_vel_minor);
static void dampen_velocity_axis(s8& vel_major, u8& vel_minor);

void Ship::spawn()
{
    vel_x_major = 0;
    vel_y_major = 0;
    pos = {16, 12, 96, 96};
}

void Ship::crash(u8& player_lives, u8& ship_spawn_timer)
{
    player_lives--;
    status = EXPLOSION_START;
    ship_spawn_timer = 129;
}

void Ship::update(u8 fast_timer, u8 direction, Offset explosion_x[], Offset explosion_y[], bool thrusting, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        update_position();
        const bool draw_thrust = thrusting && fast_timer & 4;
        draw(direction, draw_thrust, vector_generator, window);
    }
    else
    {
        handle_explosion(explosion_x, explosion_y, vector_generator, window);
        if (fast_timer & 1)
        {
            status++;
            if (status == INDISCERNIBLE)
                spawn();
        }
    }
}

void Ship::add_thrust(u8 direction, u8& vel_x_minor, u8& vel_y_minor)
{
    // x
    const u8 old_vel_x_minor = vel_x_minor;
    vel_x_minor += lookup_cosine(direction) * 2;

    if (direction > 64 && direction < 192) // left
        negative_vel_change(vel_x_major, vel_x_minor, old_vel_x_minor);
    else // right
        positive_vel_change(vel_x_major, vel_x_minor, old_vel_x_minor);

    // y
    const u8 old_vel_y_minor = vel_y_minor;
    vel_y_minor += lookup_sine(direction) * 2;

    if (direction > 128) // down
        negative_vel_change(vel_y_major, vel_y_minor, old_vel_y_minor);
    else // up
        positive_vel_change(vel_y_major, vel_y_minor, old_vel_y_minor);
}

void negative_vel_change(s8& vel_major, u8& vel_minor, u8 old_vel_minor)
{
    if (underflowed_u8(vel_minor, old_vel_minor))
        vel_major == MIN_VEL ? vel_minor = 1 : vel_major--;
}

void positive_vel_change(s8& vel_major, u8& vel_minor, u8 old_vel_minor)
{
    if (overflowed_u8(vel_minor, old_vel_minor))
        vel_major == MAX_VEL ? vel_minor = 255 : vel_major++;
}

void Ship::dampen_velocity(u8& vel_x_minor, u8& vel_y_minor)
{
    dampen_velocity_axis(vel_x_major, vel_x_minor);
    dampen_velocity_axis(vel_y_major, vel_y_minor);
}

void dampen_velocity_axis(s8& vel_major, u8& vel_minor)
{
    if (vel_major < 0) // reduction of 2 to 128
    {
        const u8 old_vel_minor = vel_minor;
        vel_minor -= vel_major * 2; // subtract the negative vel_major to increase value
        if (overflowed_u8(vel_minor, old_vel_minor))
            vel_major++;
    }
    else if (vel_minor || vel_major) // reduction of 1 to 127
    {
        const u8 old_vel_minor = vel_minor;
        vel_minor -= vel_major * 2 + 1;
        if (underflowed_u8(vel_minor, old_vel_minor))
            vel_major--;
    }
}

void Ship::draw(u8 direction, bool draw_thrust, Vector_Generator& vector_generator, RenderWindow& window) const
{
    bool flip_x = false;
    bool flip_y = false;

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
    set_position_and_size(DIV_4, vector_generator, window);
    vector_generator.process(SHIP_TABLE, window, SHIP_OFFSET_TABLE[vector_offset], flip_x, flip_y);

    if (draw_thrust)
        vector_generator.process(SHIP_THRUST_TABLE, window, SHIP_THRUST_OFFSET_TABLE[vector_offset], flip_x, flip_y);
}

void Ship::handle_explosion(Offset explosion_x[], Offset explosion_y[], Vector_Generator& vector_generator, RenderWindow& window) const
{
    u16 vector_object[6 * 6 + 1];
    u8 final_index;
    bool done = false;
    for (u8 i = 0; i < 6 && !done; i++)
    {
        const u16 delta_x = update_explosion_offset(explosion_x[i], EXPLOSION_VELOCITY[i].x);
        const u16 delta_y = update_explosion_offset(explosion_y[i], EXPLOSION_VELOCITY[i].y);

        const u16 word_0 = VCTR_9 << 12 | delta_y;
        const u16 word_1 =                delta_x;

        const u16 explosion_part[] =
        {
            // move to the explosion
            word_0,
            word_1,

            // explosion
            SHIP_EXPLOSION[i],

            // back to start of explosion
            static_cast<u16>((SHIP_EXPLOSION[i] & 0xFF0F) ^ 0x0404),

            // back to ship position
            static_cast<u16>(word_0 ^ 0x0400),
            static_cast<u16>(word_1 ^ 0x0400)
        };
        std::memcpy(&vector_object[i * 6], explosion_part, sizeof(explosion_part));

        // make pieces disappear as the ship status gets higher
        final_index = (i + 1) * 6;
        if (status > 255 - (i + 1) * 16)
            done = true;
    }
    vector_object[final_index] = RTSL << 12;
    set_position_and_size(MUL_1, vector_generator, window);
    vector_generator.process(vector_object, window);
}

u16 Ship::update_explosion_offset(Offset& offset, s8 velocity) const
{
    if (status < 162)
        offset.major = velocity >> 4;

    update_position_axis(offset.major, offset.minor, velocity);

    s16 long_vector_delta = offset.major;
    if (velocity < 0)
        long_vector_delta -= 256;

    // return VCTR delta equivalent to explosion offset
    return (velocity < 0) << 10 | std::abs(long_vector_delta);
}
