// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Saucer.h"

#include "../../Other/Vectors.h"

Saucer::Saucer()
{
    status = INDISCERNIBLE;
}

void Saucer::initialize_saucer(const u16 player_score, const u8 saucer_spawn_time_start)
{
    if (saucer_spawn_time_start >= 128)
        status = LARGE_SAUCER;
    else
    {
        if (player_score >= 3000)
            status = SMALL_SAUCER;
        else
        {
            if (random_byte() <= saucer_spawn_time_start / 2)
                status = LARGE_SAUCER;
            else
                status = SMALL_SAUCER;
        }
    }

    u8 random = random_byte();
    if (random < 128)
    {
        vel_x_major = -16;
        pos.x_major = 31;
        pos.x_minor = 255;
    }
    else
    {
        vel_x_major = 16;
        pos.x_major = 0;
        pos.x_minor = 0;
    }
    vel_y_major = 0;
    random = random_byte();
    pos.y_major = random / 8;
    if (pos.y_major > 23)
        pos.y_major -= 8;

    pos.y_minor = random * 32 % 256;
}

void Saucer::update(Vector_Generator& vector_generator, sf::RenderWindow& window, const u8 fast_timer, u8& saucer_spawn_and_shot_time, u8& saucer_spawn_time_start)
{
    if (status == LARGE_SAUCER || status == SMALL_SAUCER)
    {
        if (fast_timer == 0 || fast_timer == 128)
            determine_vertical_velocity();

        if (update_position()) // if saucer tries to wrap
        {
            status = INDISCERNIBLE;
            u8 new_saucer_spawn_time_start = saucer_spawn_time_start - 6;
            if (new_saucer_spawn_time_start >= MINIMUM_SAUCER_SPAWN_TIME)
                saucer_spawn_time_start = new_saucer_spawn_time_start;
            saucer_spawn_and_shot_time = saucer_spawn_time_start;
        }
        else
            draw(vector_generator, window);
    }
    else if (status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(vector_generator, window);
        status += 16 - (status - 1) / 16;
    }
}

void Saucer::determine_vertical_velocity()
{
    switch (random_byte() % 4)
    {
        case 0:
            vel_y_major = 16;
            break;
        case 1:
            vel_y_major = -16;
            break;
        default:
            vel_y_major = 0;
            break;
    }
}

bool Saucer::update_position()
{
    update_axis(pos.x_major, pos.x_minor, vel_x_major);
    update_axis(pos.y_major, pos.y_minor, vel_y_major);

    bool wrapped_x = false;
    if (wrap_position(pos.x_major, MAX_X_POS_MAJOR))
        wrapped_x = true;
    wrap_position(pos.y_major, MAX_Y_POS_MAJOR);

    return wrapped_x;
}

bool Saucer::wrap_position(u8& pos_major, const u8 max_pos_major)
{
    if (pos_major == 255) // underflowed
    {
        pos_major = max_pos_major;
        return true;
    }
    else if (pos_major > max_pos_major)
    {
        pos_major = 0;
        return true;
    }
    else
        return false;
}

u8 Saucer::get_size(bool bonus) const
{
    if (status == SMALL_SAUCER && bonus)
        return BONUS_SIZE_3;
    else if (status == LARGE_SAUCER)
        return LARGE_SAUCER_SIZE + (bonus ? BONUS_SIZE_2 : 0);
    else
        return 0;
}

u8 Saucer::get_points() const
{
    if (status == LARGE_SAUCER)
        return LARGE_SAUCER_POINTS;
    else
        return SMALL_SAUCER_POINTS;
}

void Saucer::draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const
{
    if (status == LARGE_SAUCER)
        vector_generator.load_absolute(pos, 15);
    else
        vector_generator.load_absolute(pos, 14);
    vector_generator.process(SAUCER, window);
}
