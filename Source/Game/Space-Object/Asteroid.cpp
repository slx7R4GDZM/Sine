// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Asteroid.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

static s8 set_asteroid_velocity(s8 old_velocity);

void Asteroid::spawn_wave_asteroid()
{
    status = (random_byte() & ASTEROID_TYPE) | LARGE_ASTEROID;
    vel_x_major = set_asteroid_velocity(0);
    vel_y_major = set_asteroid_velocity(0);

    const u8 random = random_byte();
    if (random & 1)
    {
        pos.x_major = 0;
        pos.y_major = random / 2 & 31;
        if (pos.y_major >= 24)
            pos.y_major -= 8;
    }
    else
    {
        pos.x_major = random / 2 & 31;
        pos.y_major = 0;
    }
    pos.x_minor = 0;
    pos.y_minor = 0;
}

void Asteroid::spawn_split_asteroid(u8 asteroid_size, s8 vel_x, s8 vel_y, Position pos)
{
    status = (random_byte() & ASTEROID_TYPE) | asteroid_size >> 1;
    vel_x_major = set_asteroid_velocity(vel_x);
    vel_y_major = set_asteroid_velocity(vel_y);
    this->pos = pos;
}

s8 set_asteroid_velocity(s8 old_velocity)
{
    // random offset from -16 to 15
    s8 offset = random_byte() & 0x8F;
    if (offset < 0)
        offset |= 0xF0;

    s8 velocity = old_velocity + offset;
    if (velocity < 0)
        velocity = clamp_s8(velocity, -31, -6);
    else
        velocity = clamp_s8(velocity,   6, 31);

    return velocity;
}

void Asteroid::offset_position(u8& pos_minor, s8 vel_major)
{
    pos_minor ^= (vel_major & 31) * 2;
}

void Asteroid::update(u8& asteroid_count, u8& asteroid_wave_spawn_time, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        update_position();
        draw(vector_generator, window);
    }
    else
    {
        draw_explosion(vector_generator, window);
        status += (twos_complement(status) >> 4) + 1;
        if (status == INDISCERNIBLE)
        {
            asteroid_count--;
            if (asteroid_count == 0)
                asteroid_wave_spawn_time = 127;
        }
    }
}

u8 Asteroid::get_size() const
{
    switch (status & ASTEROID_SIZE)
    {
    case LARGE_ASTEROID:
        return LARGE_ASTEROID_HITBOX;
        break;
    case MEDIUM_ASTEROID:
        return MEDIUM_ASTEROID_HITBOX;
        break;
    case SMALL_ASTEROID:
        return SMALL_ASTEROID_HITBOX;
        break;
    }
}

u8 Asteroid::get_points() const
{
    switch (status & ASTEROID_SIZE)
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

void Asteroid::draw(Vector_Generator& vector_generator, RenderWindow& window) const
{
    Global_Scale scale;
    switch (status & ASTEROID_SIZE)
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

    set_position_and_size(scale, vector_generator, window);
    switch (status >> 3)
    {
    case 0:
        vector_generator.process(ASTEROID_1, window);
        break;
    case 1:
        vector_generator.process(ASTEROID_2, window);
        break;
    case 2:
        vector_generator.process(ASTEROID_3, window);
        break;
    case 3:
        vector_generator.process(ASTEROID_4, window);
        break;
    }
}

bool Asteroid::blocking_spawn(const Asteroid asteroid[])
{
    for (u8 i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroid[i].status != INDISCERNIBLE)
        {
            if (asteroid[i].pos.x_major >= 12 && asteroid[i].pos.x_major <= 19
             && asteroid[i].pos.y_major >=  8 && asteroid[i].pos.y_major <= 15)
                return true;
        }
    }
    return false;
}
