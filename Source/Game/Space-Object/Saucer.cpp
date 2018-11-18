// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Saucer.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

void Saucer::spawn(const Score player_score, const u8 saucer_spawn_time_start)
{
    if (saucer_spawn_time_start >= 128)
        status = LARGE_SAUCER;
    else
    {
        if (player_score.points[1] >= 0x30)
            status = SMALL_SAUCER;
        else
        {
            if (random_byte() <= saucer_spawn_time_start / 2)
                status = LARGE_SAUCER;
            else
                status = SMALL_SAUCER;
        }
    }

    const u8 x_random = random_byte();
    if (x_random < 128)
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

    const u8 y_random = random_byte();
    vel_y_major = 0;
    pos.y_major = y_random / 8;
    if (pos.y_major > 23)
        pos.y_major -= 8;

    pos.y_minor = y_random * 32 % 256;
}

void Saucer::crash(u8& saucer_spawn_time, const u8 saucer_spawn_time_start)
{
    status = EXPLOSION_START;
    saucer_spawn_time = saucer_spawn_time_start;
}

void Saucer::update(const u8 fast_timer, u8& saucer_spawn_time, const u8 saucer_spawn_time_start, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (status == LARGE_SAUCER || status == SMALL_SAUCER)
    {
        if (fast_timer == 0 || fast_timer == 128)
            determine_vertical_velocity();

        const u8 old_pos_x_major = pos.x_major;
        update_position();
        attempt_remove(old_pos_x_major, saucer_spawn_time, saucer_spawn_time_start);

        if (status != INDISCERNIBLE)
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

void Saucer::attempt_remove(const u8 old_pos_x_major, u8& saucer_spawn_time, const u8 saucer_spawn_time_start)
{
    if ((vel_x_major < 0 && pos.x_major > old_pos_x_major)
     || (vel_x_major > 0 && pos.x_major < old_pos_x_major))
    {
        status = INDISCERNIBLE;
        saucer_spawn_time = saucer_spawn_time_start;
    }
}

u8 Saucer::get_size(const bool bonus) const
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

void Saucer::draw(Vector_Generator& vector_generator, RenderWindow& window) const
{
    if (status == LARGE_SAUCER)
        set_position_and_size(DIV_2, vector_generator, window);
    else
        set_position_and_size(DIV_4, vector_generator, window);

    vector_generator.process(SAUCER, window);
}
