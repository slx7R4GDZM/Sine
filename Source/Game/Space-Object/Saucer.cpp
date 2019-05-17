// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Saucer.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

static u8 ship_saucer_distance(u8 ship_pos_hi, u8 ship_pos_lo, s8 saucer_vel, u8 saucer_pos_hi, u8 saucer_pos_lo);
static u8 shot_x_direction(u8 x_distance, u8 y_distance);
static u8 shot_y_direction(u8 x_distance, u8 y_distance);
static u8 calculate_shot_angle(u8 x_distance, u8 y_distance);
static u8 look_up_angle(u8 distance_1, u8 distance_2, bool step);

void Saucer::spawn(Score player_score, u8 saucer_spawn_time_start)
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

    if (random_byte() & 0x80)
    {
        vel_x_major = 16;
        pos.x_major = 0;
        pos.x_minor = 0;
    }
    else
    {
        vel_x_major = -16;
        pos.x_major = 31;
        pos.x_minor = 255;
    }

    const u8 y_random = random_byte();
    vel_y_major = 0;
    pos.y_major = y_random / 8;
    if (pos.y_major > 23)
        pos.y_major -= 8;

    pos.y_minor = (y_random & 7) * 32;
}

void Saucer::crash(u8& saucer_spawn_time, u8 saucer_spawn_time_start)
{
    status = EXPLOSION_START;
    saucer_spawn_time = saucer_spawn_time_start;
}

void Saucer::update(u8 fast_timer, u8& saucer_spawn_time, u8 saucer_spawn_time_start, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        if (fast_timer == 0 || fast_timer == 128)
            determine_vertical_velocity();

        const u8 old_pos_x_major = pos.x_major;
        update_position();
        attempt_remove(old_pos_x_major, saucer_spawn_time, saucer_spawn_time_start);

        if (status != INDISCERNIBLE)
            draw(vector_generator, window);
    }
    else
    {
        draw_explosion(vector_generator, window);
        status += (twos_complement(status) >> 4) + 1;
    }
}

void Saucer::determine_vertical_velocity()
{
    switch (random_byte() & 3)
    {
    case 0:
        vel_y_major = -16;
        break;
    case 3:
        vel_y_major = 16;
        break;
    default:
        vel_y_major = 0;
        break;
    }
}

void Saucer::attempt_remove(u8 old_pos_x_major, u8& saucer_spawn_time, u8 saucer_spawn_time_start)
{
    if ((vel_x_major < 0 && pos.x_major > old_pos_x_major)
     || (vel_x_major > 0 && pos.x_major < old_pos_x_major))
    {
        status = INDISCERNIBLE;
        saucer_spawn_time = saucer_spawn_time_start;
    }
}

u8 Saucer::targeted_shot(Position ship_pos) const
{
    const u8 x_distance = ship_saucer_distance(ship_pos.x_major, ship_pos.x_minor, vel_x_major, pos.x_major, pos.x_minor);
    const u8 y_distance = ship_saucer_distance(ship_pos.y_major, ship_pos.y_minor, vel_y_major, pos.y_major, pos.y_minor);
    return shot_x_direction(x_distance, y_distance);
}

u8 ship_saucer_distance(u8 ship_pos_hi, u8 ship_pos_lo, s8 saucer_vel, u8 saucer_pos_hi, u8 saucer_pos_lo)
{
    const u8 distance_minor = ship_pos_lo - saucer_pos_lo;
    const bool underflowed = ship_pos_lo < saucer_pos_lo;
    const u8 distance_major = ship_pos_hi - saucer_pos_hi - underflowed;

    const u8 distance = distance_major << 2 | distance_minor >> 6;
    const u8 velocity_offset = saucer_vel >> 1;

    return distance - velocity_offset;
}

u8 shot_x_direction(u8 x_distance, u8 y_distance)
{
    if (x_distance < 128)
        return shot_y_direction(x_distance, y_distance);

    const u8 direction = shot_y_direction(twos_complement(x_distance), y_distance);
    return twos_complement(direction ^ 0x80);
}

u8 shot_y_direction(u8 x_distance, u8 y_distance)
{
    if (y_distance < 128)
        return calculate_shot_angle(x_distance, y_distance);

    const u8 direction = calculate_shot_angle(x_distance, twos_complement(y_distance));
    return twos_complement(direction);
}

u8 calculate_shot_angle(u8 x_distance, u8 y_distance)
{
    if (x_distance == y_distance)
        return 0x20;

    if (x_distance > y_distance)
        return look_up_angle(y_distance, x_distance, false);

    const u8 direction = look_up_angle(x_distance, y_distance, true);
    return twos_complement(direction - 0x40);
}

u8 look_up_angle(u8 distance_1, u8 distance_2, bool step)
{
    u8 offset = 0;
    for (u8 i = 0; i < 4; i++)
    {
        offset = offset << 1 | step;
        distance_1 <<= 1;
        if (distance_1 < distance_2)
            step = false;
        else
        {
            distance_1 -= distance_2;
            step = true;
        }
    }
    offset = offset << 1 | step;
    offset &= 0xF;
    return SHOT_ANGLE_TABLE[offset];
}

s8 Saucer::shot_offset(bool accurate_shot)
{
    s8 offset = random_byte() & SHOT_OFFSET_AND[accurate_shot];
    if (offset < 0)
        offset |= SHOT_OFFSET_OR[accurate_shot];

    return offset + accurate_shot;
}

u8 Saucer::get_size(bool bonus) const
{
    if (status == LARGE_SAUCER)
        return LARGE_SAUCER_SIZE + (bonus ? BONUS_SIZE_2 : 0);

    if (status == SMALL_SAUCER && bonus)
        return BONUS_SIZE_3;

    return 0;
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
