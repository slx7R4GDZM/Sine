// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_ASTEROID_HC
#define SINE_ASTEROID_HC

#include "Game/Space-Object.HC"
#include "Graphics/Vector-Generator.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"

I8 set_asteroid_velocity(I8 old_velocity)
{
    // random offset from -16 to 15
    I8 offset = to_I8(random_byte() & 0x8F);
    if (offset < 0)
        offset |= 0xF0;

    I8 velocity = old_velocity + offset;
    if (velocity < 0)
        velocity = clamp_I8(velocity, -31, -6);
    else
        velocity = clamp_I8(velocity,   6, 31);

    return velocity;
}

U0 offset_position(U8* pos_minor, I8 vel_major)
{
    *pos_minor ^= (vel_major & 31) * 2;
}

U0 spawn_wave_asteroid(Space_Object* asteroid)
{
    asteroid->status = (random_byte() & ASTEROID_TYPE) | LARGE_ASTEROID;
    asteroid->vel_x_major = set_asteroid_velocity(0);
    asteroid->vel_y_major = set_asteroid_velocity(0);

    U8 random = random_byte();
    if (random & 1)
    {
        asteroid->pos.x_major = 0;
        asteroid->pos.y_major = random / 2 & 31;
        if (asteroid->pos.y_major >= 24)
            asteroid->pos.y_major -= 8;
    }
    else
    {
        asteroid->pos.x_major = random / 2 & 31;
        asteroid->pos.y_major = 0;
    }
    asteroid->pos.x_minor = 0;
    asteroid->pos.y_minor = 0;
}

U0 spawn_split_asteroid(Space_Object* asteroid, U8 asteroid_size, I8 vel_x, I8 vel_y, Position pos)
{
    asteroid->status = (random_byte() & ASTEROID_TYPE) | asteroid_size >> 1;
    asteroid->vel_x_major = set_asteroid_velocity(vel_x);
    asteroid->vel_y_major = set_asteroid_velocity(vel_y);
    MemCpy(&asteroid->pos, &pos, sizeof(Position));
}

U0 draw_asteroid(Space_Object* asteroid, Vector_Generator* vector_generator, CDC* dc)
{
    U8 scale;
    switch (asteroid->status & ASTEROID_SIZE)
    {
    case LARGE_ASTEROID:
        scale = MUL_1;
        break;
    case MEDIUM_ASTEROID:
        scale = DIV_2;
        break;
    case SMALL_ASTEROID:
        scale = DIV_4;
        break;
    }

    set_position_and_size(asteroid, scale, vector_generator, dc);
    switch (asteroid->status >> 3)
    {
    case 0:
        process(vector_generator, ASTEROID_1, dc);
        break;
    case 1:
        process(vector_generator, ASTEROID_2, dc);
        break;
    case 2:
        process(vector_generator, ASTEROID_3, dc);
        break;
    case 3:
        process(vector_generator, ASTEROID_4, dc);
        break;
    }
}

U0 update_asteroid(Space_Object* asteroid, U8* asteroid_count, U8* asteroid_wave_spawn_time, Vector_Generator* vector_generator, CDC* dc)
{
    if (!asteroid->status)
        return;

    if (asteroid->status < TRUE_EXPLOSION_START)
    {
        update_position(asteroid);
        draw_asteroid(asteroid, vector_generator, dc);
    }
    else
    {
        draw_explosion(asteroid, vector_generator, dc);
        asteroid->status += (twos_complement(asteroid->status) >> 4) + 1;
        if (asteroid->status == INDISCERNIBLE)
        {
            (*asteroid_count)--;
            if (*asteroid_count == 0)
                *asteroid_wave_spawn_time = 127;
        }
    }
}

U8 get_asteroid_size(Space_Object asteroid)
{
    switch (asteroid.status & ASTEROID_SIZE)
    {
    case LARGE_ASTEROID:
        return LARGE_ASTEROID_SIZE;
        break;
    case MEDIUM_ASTEROID:
        return MEDIUM_ASTEROID_SIZE;
        break;
    case SMALL_ASTEROID:
        return SMALL_ASTEROID_SIZE;
        break;
    }
}

U8 get_asteroid_points(Space_Object asteroid)
{
    switch (asteroid.status & ASTEROID_SIZE)
    {
    case LARGE_ASTEROID:
        return LARGE_ASTEROID_POINTS;
        break;
    case MEDIUM_ASTEROID:
        return MEDIUM_ASTEROID_POINTS;
        break;
    case SMALL_ASTEROID:
        return SMALL_ASTEROID_POINTS;
        break;
    }
}

Bool blocking_spawn(Space_Object* asteroid)
{
    U64 i;
    for (i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroid[i].status != INDISCERNIBLE)
        {
            if (asteroid[i].pos.x_major >= 12 && asteroid[i].pos.x_major <= 19
             && asteroid[i].pos.y_major >=  8 && asteroid[i].pos.y_major <= 15)
                return TRUE;
        }
    }
    return FALSE;
}

#endif
