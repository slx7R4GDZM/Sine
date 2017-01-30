// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Asteroid.h"

#include "../../Other/Vectors.h"

void Asteroid::spawn_wave_asteroid()
{
    // need to check if area is clear of other spaceobjects
    status = random_byte() / 8 * 8 % 32 + 4;
    vel_x_major = get_starting_velocity();
    vel_y_major = get_starting_velocity();

    u8 random = random_byte();
    if (random % 2)
    {
        pos.x_major = 0;
        pos.y_major = random / 2 % 32;
        if (pos.y_major >= 24)
            pos.y_major -= 8;
    }
    else
    {
        pos.x_major = random / 2 % 32;
        pos.y_major = 0;
        pos.y_major = 0;
    }
    pos.x_minor = 0;
    pos.y_minor = 0;
}

void Asteroid::spawn_crash_asteroid(const u8 status, const Position pos)
{
    this->status = random_byte() / 8 * 8 % 32 + ((status & 0x07) >> 1);
    this->pos = pos;
    vel_x_major = get_starting_velocity();
    vel_y_major = get_starting_velocity();
}

s8 Asteroid::get_starting_velocity() const
{
    u8 random = random_byte();
    if (random < 128)
        random %= 16;
    else
        random = random % 16 + 16;
    switch (status & 0x07)
    {
        case 4:
            return LARGE_ASTEROID_SPAWN_VELOCITY[random];
            break;
        case 2:
            return MEDIUM_ASTEROID_SPAWN_VELOCITY[random];
            break;
        case 1:
            return SMALL_ASTEROID_SPAWN_VELOCITY[random];
            break;
    }
}

void Asteroid::update(Vector_Generator& vector_generator, sf::RenderWindow& window, u8& asteroid_count, u8& asteroid_wave_spawn_time)
{
    if (status && status <= 0x1C)
    {
        update_position();
        draw(vector_generator, window);
    }
    else if (status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(vector_generator, window);
        status += 16 - (status - 1) / 16;
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
    switch (status & 0x07)
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

u8 Asteroid::get_points() const
{
    switch (status & 0x07)
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

void Asteroid::draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const
{
    Scale scale;
    switch (status & 0x07)
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
    vector_generator.load_absolute(pos, scale);
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
    bool asteroid_in_spawn = false;
    for (u8 i = 0; i < MAX_ASTEROIDS && !asteroid_in_spawn; i++)
    {
        if (asteroid[i].status != INDISCERNIBLE)
        {
            if (asteroid[i].pos.x_major >= 12 && asteroid[i].pos.x_major <= 19
             && asteroid[i].pos.y_major >=  8 && asteroid[i].pos.y_major <= 15)
                asteroid_in_spawn = true;
        }
    }
    return asteroid_in_spawn;
}
