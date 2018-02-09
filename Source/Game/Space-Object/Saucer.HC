// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#ifndef SINE_SAUCER_HC
#define SINE_SAUCER_HC

#include "Game/Space-Object.HC"
#include "Graphics/Vector-Generator.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"

U0 spawn_saucer(Space_Object* saucer, Score player_score, U8 saucer_spawn_time_start)
{
    if (saucer_spawn_time_start >= 128)
        saucer->status = LARGE_SAUCER;
    else
    {
        if (player_score.points[1] >= 0x30)
            saucer->status = SMALL_SAUCER;
        else
        {
            if (random_byte() <= saucer_spawn_time_start / 2)
                saucer->status = LARGE_SAUCER;
            else
                saucer->status = SMALL_SAUCER;
        }
    }

    U8 x_random = random_byte();
    if (x_random < 128)
    {
        saucer->vel_x_major = -16;
        saucer->pos.x_major = 31;
        saucer->pos.x_minor = 255;
    }
    else
    {
        saucer->vel_x_major = 16;
        saucer->pos.x_major = 0;
        saucer->pos.x_minor = 0;
    }

    U8 y_random = random_byte();
    saucer->vel_y_major = 0;
    saucer->pos.y_major = y_random / 8;
    if (saucer->pos.y_major > 23)
        saucer->pos.y_major -= 8;

    saucer->pos.y_minor = y_random * 32 % 256;
}

U0 crash(Space_Object* saucer, U8* saucer_spawn_and_shot_time, U8 saucer_spawn_time_start)
{
    saucer->status = EXPLOSION_START;
    *saucer_spawn_and_shot_time = saucer_spawn_time_start;
}

U0 draw_saucer(Space_Object* saucer, Vector_Generator* vector_generator, CDC* dc)
{
    if (saucer->status == LARGE_SAUCER)
        set_position_and_size(saucer, DIV_2, vector_generator, dc);
    else
        set_position_and_size(saucer, DIV_4, vector_generator, dc);

    process(vector_generator, SAUCER, dc);
}

U0 attempt_remove(Space_Object* saucer, U8 old_pos_x_major, U8* saucer_spawn_and_shot_time, U8 saucer_spawn_time_start)
{
    if ((saucer->vel_x_major < 0 && saucer->pos.x_major > old_pos_x_major)
     || (saucer->vel_x_major > 0 && saucer->pos.x_major < old_pos_x_major))
    {
        saucer->status = INDISCERNIBLE;
        *saucer_spawn_and_shot_time = saucer_spawn_time_start;
    }
}

U0 determine_vertical_velocity(Space_Object* saucer)
{
    switch (random_byte() % 4)
    {
    case 0:
        saucer->vel_y_major = 16;
        break;
    case 1:
        saucer->vel_y_major = -16;
        break;
    default:
        saucer->vel_y_major = 0;
        break;
    }
}

U0 update_saucer(Space_Object* saucer, U8 fast_timer, U8* saucer_spawn_and_shot_time, U8 saucer_spawn_time_start, Vector_Generator* vector_generator, CDC* dc)
{
    if (saucer->status == LARGE_SAUCER || saucer->status == SMALL_SAUCER)
    {
        if (fast_timer == 0 || fast_timer == 128)
            determine_vertical_velocity(saucer);

        U8 old_pos_x_major = saucer->pos.x_major;
        update_position(saucer);

        attempt_remove(saucer, old_pos_x_major, saucer_spawn_and_shot_time, saucer_spawn_time_start);

        if (saucer->status != INDISCERNIBLE)
            draw_saucer(saucer, vector_generator, dc);
    }
    else if (saucer->status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(saucer, vector_generator, dc);
        saucer->status += 16 - (saucer->status - 1) / 16;
    }
}

U8 get_saucer_size(Space_Object* saucer, Bool bonus)
{
    if (saucer->status == SMALL_SAUCER && bonus)
        return BONUS_SIZE_3;
    else if (saucer->status == LARGE_SAUCER)
    {
        if (bonus)
            return LARGE_SAUCER_SIZE + BONUS_SIZE_2;
        else
            return LARGE_SAUCER_SIZE;
    }
    else
        return 0;
}

U8 get_saucer_points(Space_Object* saucer)
{
    if (saucer->status == LARGE_SAUCER)
        return LARGE_SAUCER_POINTS;
    else
        return SMALL_SAUCER_POINTS;
}

#endif
