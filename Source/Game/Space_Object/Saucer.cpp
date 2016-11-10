// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Saucer.h"

#include "../../Other/Vectors.h"

const u8 SMALL_SAUCER = 1;
const u8 LARGE_SAUCER = 2;
const u8 LARGE_SAUCER_SIZE = 30;
const u8 BONUS_SIZE_2 = 50;
const u8 BONUS_SIZE_3 = 62;
const u8 LARGE_SAUCER_POINTS = 20;
const u8 SMALL_SAUCER_POINTS = 100;

Saucer::Saucer()
{
    status = LARGE_SAUCER; // or SMALL_SAUCER based on saucer_spawn_time_start?

    u8 random = random_byte();
    if (random < 128)
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
    random = random_byte();
    pos.y_major = random / 8;
    if (pos.y_major > 23)
        pos.y_major -= 8;

    pos.y_minor = random * 32 % 256;

    y_counter = 0; // hacky incorrect variable?
}

u8 Saucer::update(Vector_Generator& vector_generator, sf::RenderWindow& window)
{
    if (status == LARGE_SAUCER)
    {
        if (!y_counter)
            determine_vertical_velocity();
        y_counter++;
        if (y_counter == 128)
            y_counter = 0;

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

void Saucer::determine_vertical_velocity()
{
    switch (rand_s8(0, 3))
    {
        case 0: // down
            vel_y_major = -16;
            break;
        case 1: // up
            vel_y_major = 16;
            break;
        default: // stable y
            vel_y_major = 0;
            break;
    }
}

u8 Saucer::get_size(bool bonus) const
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

void Saucer::draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const
{
    vector_generator.load_absolute(pos, -1);
    vector_generator.process(SAUCER, window);
}

/*
x vel can be 16 or -16
y vel can be 16, -16, or 0 (goes nowhere, up, or down 128 frames at a time)
X is constant direction from spawn
Only goes across screen once before despawning (at least when status is 2)
Can go past the y on the screen
*/
