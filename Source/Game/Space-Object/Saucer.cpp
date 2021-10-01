// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Saucer.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

static float ship_saucer_distance(float ship_pos, float saucer_vel, float saucer_pos);

void Saucer::spawn(float delta_time, Score player_score, u8 saucer_spawn_time_start)
{
    if (saucer_spawn_time_start >= 128)
        status = LARGE_SAUCER;
    else
    {
        if (player_score.points[1] >= 0x30)
            status = SMALL_SAUCER;
        else
        {
            if (random_u8() <= saucer_spawn_time_start / 2)
                status = LARGE_SAUCER;
            else
                status = SMALL_SAUCER;
        }
    }

    if (random_u8() & 0x80)
    {
        vel_x = 16;
        pos.x = 0;
    }
    else
    {
        vel_x = -16;
        pos.x = std::nextafter(MAX_X_POS, 0);
    }

    vel_y = 0;
    pos.y = random(0, MAX_Y_POS);

    vertical_delay = random(0, 128, true);
    determine_vertical_velocity(delta_time);
    update_position(delta_time);
}

void Saucer::crash(float& saucer_spawn_time, u8 saucer_spawn_time_start)
{
    status = EXPLOSION_START;
    saucer_spawn_time = saucer_spawn_time_start;
}

void Saucer::update(float delta_time, u8 fast_timer, float& saucer_spawn_time, u8 saucer_spawn_time_start, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        determine_vertical_velocity(delta_time);

        const float old_pos_x = pos.x;
        update_position(delta_time);
        attempt_remove(old_pos_x, saucer_spawn_time, saucer_spawn_time_start);

        if (status != INDISCERNIBLE)
            draw(vector_generator, window);
    }
    else
    {
        draw_explosion(vector_generator, window);
        advance_explosion(((twos_complement(status) >> 4) + 1) * delta_time);
    }
}

void Saucer::determine_vertical_velocity(float& delta_time)
{
    if (vertical_delay <= delta_time)
    {
        update_position(vertical_delay);

        const float initial_delta_time = delta_time;
        delta_time -= vertical_delay;
        vertical_delay -= initial_delta_time;

        switch (random_u8() & 3)
        {
        case 0:
            vel_y = -16;
            break;
        case 3:
            vel_y = 16;
            break;
        default:
            vel_y = 0;
            break;
        }
        vertical_delay += 128;
    }
}

void Saucer::attempt_remove(float old_pos_x, float& saucer_spawn_time, u8 saucer_spawn_time_start)
{
    if ((vel_x < 0 && pos.x > old_pos_x)
     || (vel_x > 0 && pos.x < old_pos_x))
    {
        status = INDISCERNIBLE;
        saucer_spawn_time = saucer_spawn_time_start;
    }
}

float Saucer::targeted_shot(Position ship_pos) const
{
    const float x_distance = ship_saucer_distance(ship_pos.x, vel_x, pos.x);
    const float y_distance = ship_saucer_distance(ship_pos.y, vel_y, pos.y);
    return std::atan2(y_distance, x_distance);
}

float ship_saucer_distance(float ship_pos, float saucer_vel, float saucer_pos)
{
    const float distance = (ship_pos - saucer_pos) * 4;
    const float velocity_offset = saucer_vel / 2;

    return distance - velocity_offset;
}

float Saucer::shot_offset(bool accurate_shot)
{
    if (accurate_shot)
        return random(-8, 8, true);

    return random(-16, 16, true);
}

u8 Saucer::get_size(bool bonus) const
{
    if (status == LARGE_SAUCER)
        return bonus ? MEDIUM_ASTEROID_HITBOX : LARGE_SAUCER_HITBOX;

    return bonus ? SMALL_ASTEROID_HITBOX : SMALL_SAUCER_HITBOX;
}

u8 Saucer::get_points() const
{
    if (status == LARGE_SAUCER)
        return LARGE_SAUCER_POINTS;

    return SMALL_SAUCER_POINTS;
}

void Saucer::draw(Vector_Generator& vector_generator, RenderWindow& window) const
{
    if (status == LARGE_SAUCER)
        set_position_and_size(DIV_2, vector_generator, window);
    else
        set_position_and_size(DIV_4, vector_generator, window);

    vector_generator.process(SAUCER, window);
}
