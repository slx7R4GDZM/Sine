// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Ship.h"

#include <cmath>
#include <cstring>
#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

static void limit_negative_velocity(float& velocity);
static void limit_positive_velocity(float& velocity);
static void dampen_velocity_axis(float delta_time, float& velocity);

void Ship::spawn()
{
    vel_x = 0;
    vel_y = 0;
    pos.x = 16 + 96.0f / 256;
    pos.y = 12 + 96.0f / 256;
}

void Ship::crash(u8& player_lives, float& ship_spawn_timer)
{
    player_lives--;
    status = EXPLOSION_START;
    ship_spawn_timer = 129;
}

void Ship::update(float delta_time, u8 fast_timer, float direction, float explosion_x[], float explosion_y[], bool thrusting, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        update_position(delta_time);
        const bool draw_thrust = thrusting && fast_timer & 4;
        draw(direction, draw_thrust, vector_generator, window);
    }
    else
    {
        handle_explosion(delta_time, explosion_x, explosion_y, vector_generator, window);
        advance_explosion(delta_time / 2);
        if (status == INDISCERNIBLE)
            spawn();
    }
}

void Ship::add_thrust(float delta_time, float direction)
{
    // x
    vel_x += lookup_cosine(direction) / 2 * delta_time;

    if (std::cos(direction) < 0)
        limit_negative_velocity(vel_x);
    else
        limit_positive_velocity(vel_x);

    // y
    vel_y += lookup_sine(direction) / 2 * delta_time;

    if (std::sin(direction) < 0)
        limit_negative_velocity(vel_y);
    else
        limit_positive_velocity(vel_y);
}

void limit_negative_velocity(float& velocity)
{
    if (velocity < MIN_VEL)
        velocity = MIN_VEL;
}

void limit_positive_velocity(float& velocity)
{
    if (velocity > MAX_VEL)
        velocity = MAX_VEL;
}

void Ship::dampen_velocity(float delta_time)
{
    dampen_velocity_axis(delta_time, vel_x);
    dampen_velocity_axis(delta_time, vel_y);
}

void dampen_velocity_axis(float delta_time, float& velocity)
{
    if (velocity < 0)
    {
        velocity -= velocity / 256 * delta_time;
        if (velocity > 0)
            velocity = 0;
    }
    else if (velocity > 0)
    {
        velocity -= velocity / 256 * delta_time;
        if (velocity < 0)
            velocity = 0;
    }
}

void Ship::draw(float direction, bool draw_thrust, Vector_Generator& vector_generator, RenderWindow& window) const
{
    set_position_and_size(DIV_4, vector_generator, window);
    vector_generator.process(SHIP, window, 0, direction);

    if (draw_thrust)
        vector_generator.process(SHIP_THRUST, window, 0, direction);
}

void Ship::handle_explosion(float delta_time, float explosion_x[], float explosion_y[], Vector_Generator& vector_generator, RenderWindow& window) const
{
    float vector_object[6 * 12 + 1];
    u8 final_index;
    bool done = false;
    for (u8 i = 0; i < 6 && !done; i++)
    {
        update_explosion_offset(delta_time, explosion_x[i], EXPLOSION_VELOCITY[i].x);
        update_explosion_offset(delta_time, explosion_y[i], EXPLOSION_VELOCITY[i].y);

        const float explosion_part[] =
        {
            // move to the explosion
            VCTR_9, explosion_x[i], explosion_y[i], 0,

            // explosion
            EQIV, static_cast<float>(SHIP_EXPLOSION[i]),

            // back to start of explosion
            EQIV, static_cast<float>((SHIP_EXPLOSION[i] & 0xFF0F) ^ 0x0404),

            // back to ship position
            VCTR_9, -explosion_x[i], -explosion_y[i], 0
        };
        std::memcpy(&vector_object[i * 12], explosion_part, sizeof(explosion_part));

        // make pieces disappear as the ship status gets higher
        final_index = (i + 1) * 12;
        if (status > 255 - (i + 1) * 16)
            done = true;
    }
    vector_object[final_index] = RTSL;
    set_position_and_size(MUL_1, vector_generator, window);
    vector_generator.process(vector_object, window);
}

void Ship::update_explosion_offset(float delta_time, float& offset, float velocity) const
{
    if (status <= EXPLOSION_START)
        offset = velocity / 16;

    update_position_axis(delta_time, offset, velocity);
}
