// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

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

    if (random_byte() & 0x80)
    {
        saucer->vel_x_major = 16;
        saucer->pos.x_major = 0;
        saucer->pos.x_minor = 0;
    }
    else
    {
        saucer->vel_x_major = -16;
        saucer->pos.x_major = 31;
        saucer->pos.x_minor = 255;
    }

    U8 y_random = random_byte();
    saucer->vel_y_major = 0;
    saucer->pos.y_major = y_random / 8;
    if (saucer->pos.y_major > 23)
        saucer->pos.y_major -= 8;

    saucer->pos.y_minor = (y_random & 7) * 32;
}

U0 crash_saucer(Space_Object* saucer, U8* saucer_spawn_time, U8 saucer_spawn_time_start)
{
    saucer->status = EXPLOSION_START;
    *saucer_spawn_time = saucer_spawn_time_start;
}

U0 draw_saucer(Space_Object* saucer, Vector_Generator* vector_generator, CDC* dc)
{
    if (saucer->status == LARGE_SAUCER)
        set_position_and_size(saucer, DIV_2, vector_generator, dc);
    else
        set_position_and_size(saucer, DIV_4, vector_generator, dc);

    process(vector_generator, SAUCER, dc);
}

U0 attempt_remove(Space_Object* saucer, U8 old_pos_x_major, U8* saucer_spawn_time, U8 saucer_spawn_time_start)
{
    if ((saucer->vel_x_major < 0 && saucer->pos.x_major > old_pos_x_major)
     || (saucer->vel_x_major > 0 && saucer->pos.x_major < old_pos_x_major))
    {
        saucer->status = INDISCERNIBLE;
        *saucer_spawn_time = saucer_spawn_time_start;
    }
}

U0 determine_vertical_velocity(Space_Object* saucer)
{
    switch (random_byte() & 3)
    {
    case 0:
        saucer->vel_y_major = -16;
        break;
    case 3:
        saucer->vel_y_major = 16;
        break;
    default:
        saucer->vel_y_major = 0;
        break;
    }
}

U0 update_saucer(Space_Object* saucer, U8 fast_timer, U8* saucer_spawn_time, U8 saucer_spawn_time_start, Vector_Generator* vector_generator, CDC* dc)
{
    if (!saucer->status)
        return;

    if (saucer->status < TRUE_EXPLOSION_START)
    {
        if (fast_timer == 0 || fast_timer == 128)
            determine_vertical_velocity(saucer);

        U8 old_pos_x_major = saucer->pos.x_major;
        update_position(saucer);

        attempt_remove(saucer, old_pos_x_major, saucer_spawn_time, saucer_spawn_time_start);

        if (saucer->status != INDISCERNIBLE)
            draw_saucer(saucer, vector_generator, dc);
    }
    else
    {
        draw_explosion(saucer, vector_generator, dc);
        saucer->status += (twos_complement(saucer->status) >> 4) + 1;
    }
}

U8 look_up_angle(U8 distance_1, U8 distance_2, Bool step)
{
    U8 offset = 0;
    U64 i;
    for (i = 0; i < 4; i++)
    {
        offset = offset << 1 | step;
        distance_1 <<= 1;
        if (distance_1 < distance_2)
            step = FALSE;
        else
        {
            distance_1 -= distance_2;
            step = TRUE;
        }
    }
    offset = offset << 1 | step;
    offset &= 0xF;
    return SHOT_ANGLE_TABLE[offset];
}

U8 calculate_shot_angle(U8 x_distance, U8 y_distance)
{
    if (x_distance == y_distance)
        return 0x20;

    if (x_distance > y_distance)
        return look_up_angle(y_distance, x_distance, FALSE);

    U8 direction = look_up_angle(x_distance, y_distance, TRUE);
    return twos_complement(direction - 0x40);
}

U8 shot_y_direction(U8 x_distance, U8 y_distance)
{
    if (y_distance < 128)
        return calculate_shot_angle(x_distance, y_distance);

    U8 direction = calculate_shot_angle(x_distance, twos_complement(y_distance));
    return twos_complement(direction);
}

U8 shot_x_direction(U8 x_distance, U8 y_distance)
{
    if (x_distance < 128)
        return shot_y_direction(x_distance, y_distance);

    U8 direction = shot_y_direction(twos_complement(x_distance), y_distance);
    return twos_complement(direction ^ 0x80);
}

U8 ship_saucer_distance(U8 ship_pos_hi, U8 ship_pos_lo, I8 saucer_vel, U8 saucer_pos_hi, U8 saucer_pos_lo)
{
    U8 distance_minor = to_U8(ship_pos_lo - saucer_pos_lo);
    Bool underflowed = ship_pos_lo < saucer_pos_lo;
    U8 distance_major = ship_pos_hi - saucer_pos_hi - underflowed;

    U8 distance = distance_major << 2 | distance_minor >> 6;
    U8 velocity_offset = saucer_vel >> 1;

    return distance - velocity_offset;
}

U8 targeted_shot(Space_Object saucer, Position ship_pos)
{
    U8 x_distance = ship_saucer_distance(ship_pos.x_major, ship_pos.x_minor,
                                         saucer.vel_x_major, saucer.pos.x_major, saucer.pos.x_minor);
    U8 y_distance = ship_saucer_distance(ship_pos.y_major, ship_pos.y_minor,
                                         saucer.vel_y_major, saucer.pos.y_major, saucer.pos.y_minor);
    return shot_x_direction(x_distance, y_distance);
}

U8 shot_offset(Bool accurate_shot)
{
    U8 offset = random_byte() & SHOT_OFFSET_AND[accurate_shot];
    if (offset >= 128)
        offset |= SHOT_OFFSET_OR[accurate_shot];

    return offset + accurate_shot;
}

U8 get_saucer_size(Space_Object saucer, Bool bonus)
{
    if (saucer.status == LARGE_SAUCER)
    {
        if (bonus)
            return MEDIUM_ASTEROID_HITBOX;

        return LARGE_SAUCER_HITBOX;
    }
    if (bonus)
        return SMALL_ASTEROID_HITBOX;

    return SMALL_SAUCER_HITBOX;
}

U8 get_saucer_points(Space_Object saucer)
{
    if (saucer.status == LARGE_SAUCER)
        return LARGE_SAUCER_POINTS;

    return SMALL_SAUCER_POINTS;
}

#endif
