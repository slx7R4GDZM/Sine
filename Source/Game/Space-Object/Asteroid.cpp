// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Asteroid.h"

#include "../../Graphics/Vector-Generator.h"
#include "../../Other/Constants.h"
#include "../../Other/Vectors.h"

static float set_asteroid_velocity(float old_velocity);

void Asteroid::spawn_wave_asteroid(float delta_time)
{
    status = (random_u8() & ASTEROID_TYPE) | LARGE_ASTEROID;
    vel_x = set_asteroid_velocity(0);
    vel_y = set_asteroid_velocity(0);

    if (random_u8() & 1)
    {
        pos.x = 0;
        pos.y = random(0, MAX_Y_POS);
    }
    else
    {
        pos.x = random(0, MAX_X_POS);
        pos.y = 0;
    }
    update_position(delta_time);
}

void Asteroid::spawn_split_asteroid(u8 asteroid_size, float vel_x, float vel_y, Position pos)
{
    status = (random_u8() & ASTEROID_TYPE) | asteroid_size >> 1;
    this->vel_x = set_asteroid_velocity(vel_x);
    this->vel_y = set_asteroid_velocity(vel_y);
    this->pos = pos;
}

float set_asteroid_velocity(float old_velocity)
{
    float velocity = old_velocity + random(-15.5f, 15.5f, true);
    if (velocity < 0)
        velocity = clamp(velocity, -31, -6);
    else
        velocity = clamp(velocity,   6, 31);

    return velocity;
}

void Asteroid::offset_position(float& position, float velocity)
{
    u8 pos_minor = (position - std::floor(position)) * 256;
    pos_minor ^= (static_cast<s8>(velocity) & 31) * 2;

    position = std::floor(position) + pos_minor / 256.0f;
}

void Asteroid::update(float delta_time, u8& asteroid_count, float& asteroid_wave_spawn_time, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (!status)
        return;

    if (status < TRUE_EXPLOSION_START)
    {
        update_position(delta_time);
        draw(vector_generator, window);
    }
    else
    {
        draw_explosion(vector_generator, window);
        advance_explosion(((twos_complement(status) >> 4) + 1) * delta_time);
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
            if (asteroid[i].pos.x >= 12 && asteroid[i].pos.x <= 20
             && asteroid[i].pos.y >=  8 && asteroid[i].pos.y <= 16)
                return true;
        }
    }
    return false;
}
