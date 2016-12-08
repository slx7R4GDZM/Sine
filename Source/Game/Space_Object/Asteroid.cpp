// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Asteroid.h"

#include "../../Other/Vectors.h"

const u8 LARGE_ASTEROID_SIZE = 90;
const u8 MEDIUM_ASTEROID_SIZE = 30;
const u8 SMALL_ASTEROID_SIZE = 0;
const u8 LARGE_ASTEROID_POINTS = 2;
const u8 MEDIUM_ASTEROID_POINTS = 5;
const u8 SMALL_ASTEROID_POINTS = 10;

// asteroids from wave spawns
Asteroid::Asteroid()
{
    // need to check if area is clear of other spaceobjects
    //status = random_byte() / 8 * 8 % 32 + 4; // correct
    status = rand_s8(20, 23); // incorrect
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

// asteroids from explosions
Asteroid::Asteroid(const u8 status, const Position pos)
{
    if (status < 20)
        this->status = rand_s8(12, 15);
    else
        this->status = rand_s8(16, 19);

    this->pos = pos;

    vel_x_major = rand_s8(0, 1) ? rand_s8(-16, -6) : rand_s8(6, 15);
    vel_y_major = rand_s8(0, 1) ? rand_s8(-16, -6) : rand_s8(6, 15);
}

u8 Asteroid::get_starting_velocity()
{
    u8 random = random_byte();
    u8 vel_major = random % 16;
    if (random < 128)
    {
        if (vel_major < 6)
            vel_major = 6;
    }
    else
    {
        vel_major = -16 + vel_major;
        if (vel_major > -6)
            vel_major = -6;
    }
    return vel_major;
}

u8 Asteroid::update(Vector_Generator& vector_generator, sf::RenderWindow& window)
{
    if (status && status < 24)
    {
        update_position();
        draw(vector_generator, window);
    }
    else if (status >= TRUE_EXPLOSION_START)
    {
        draw_explosion(vector_generator, window);
        status += 16 - (status - 1) / 16;
    }
    return status;
}

u8 Asteroid::get_size() const
{
    if (status < 16)
        return SMALL_ASTEROID_SIZE;
    else if (status < 20)
        return MEDIUM_ASTEROID_SIZE;
    else
        return LARGE_ASTEROID_SIZE;
}

u8 Asteroid::get_points() const
{
    if (status < 16)
        return SMALL_ASTEROID_POINTS;
    else if (status < 20)
        return MEDIUM_ASTEROID_POINTS;
    else
        return LARGE_ASTEROID_POINTS;
}

void Asteroid::draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const
{
    u8 scale;
    if (status < 20)
        scale = status / 4 + 11;
    else
        scale = 0;
    vector_generator.load_absolute(pos, scale);

    switch (status % 4)
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
