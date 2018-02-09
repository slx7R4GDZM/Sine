// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#ifndef SINE_ASTEROID_HC
#define SINE_ASTEROID_HC

#include "Game/Space-Object.HC"
#include "Graphics/Vector-Generator.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"

U0 set_starting_velocity(U8 status, I8* velocity)
{
    U8 random = random_byte();
    if (random < 128)
        random %= 16;
    else
        random = random % 16 + 16;

    switch (status & 0x07)
    {
    case 4:
        *velocity = LARGE_ASTEROID_SPAWN_VELOCITY[random];
        break;
    case 2:
        *velocity = MEDIUM_ASTEROID_SPAWN_VELOCITY[random];
        break;
    case 1:
        *velocity = SMALL_ASTEROID_SPAWN_VELOCITY[random];
        break;
    }
}

U0 spawn_wave_asteroid(Space_Object* asteroid)
{
    asteroid->status = random_byte() / 8 * 8 % 32 + 4;
    set_starting_velocity(asteroid->status, &asteroid->vel_x_major);
    set_starting_velocity(asteroid->status, &asteroid->vel_y_major);

    U8 random = random_byte();
    if (random % 2)
    {
        asteroid->pos.x_major = 0;
        asteroid->pos.y_major = random / 2 % 32;
        if (asteroid->pos.y_major >= 24)
            asteroid->pos.y_major -= 8;
    }
    else
    {
        asteroid->pos.x_major = random / 2 % 32;
        asteroid->pos.y_major = 0;
        asteroid->pos.y_major = 0;
    }
    asteroid->pos.x_minor = 0;
    asteroid->pos.y_minor = 0;
}

U0 spawn_crash_asteroid(Space_Object* asteroid, U8 status, Position* pos)
{
    asteroid->status = random_byte() / 8 * 8 % 32 + ((status & 0x07) >> 1);
    MemCpy(&asteroid->pos, pos, sizeof(Position));
    set_starting_velocity(asteroid->status, &asteroid->vel_x_major);
    set_starting_velocity(asteroid->status, &asteroid->vel_y_major);
}

U0 draw_asteroid(Space_Object* asteroid, Vector_Generator* vector_generator, CDC* dc)
{
    U8 scale;
    switch (asteroid->status & 0x07)
    {
    case 4:
        scale = MUL_1;
        break;
    case 2:
        scale = DIV_2;
        break;
    case 1:
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
    if (asteroid->status && asteroid->status <= 0x1C)
    {
        update_position(asteroid);
        draw_asteroid(asteroid, vector_generator, dc);
    }
    else if (asteroid->status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(asteroid, vector_generator, dc);
        asteroid->status += 16 - (asteroid->status - 1) / 16;
        if (asteroid->status == INDISCERNIBLE)
        {
            (*asteroid_count)--;
            if (*asteroid_count == 0)
                *asteroid_wave_spawn_time = 127;
        }
    }
}

U8 get_asteroid_size(Space_Object* asteroid)
{
    switch (asteroid->status & 0x07)
    {
    case 4:
        return LARGE_ASTEROID_SIZE;
        break;
    case 2:
        return MEDIUM_ASTEROID_SIZE;
        break;
    case 1:
        return SMALL_ASTEROID_SIZE;
        break;
    }
}

U8 get_asteroid_points(Space_Object* asteroid)
{
    switch (asteroid->status & 0x07)
    {
    case 4:
        return LARGE_ASTEROID_POINTS;
        break;
    case 2:
        return MEDIUM_ASTEROID_POINTS;
        break;
    case 1:
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
