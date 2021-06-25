// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Space-Object.h"

#include "../Graphics/Vector-Generator.h"
#include "../Other/Constants.h"
#include "../Other/Vectors.h"

static void wrap_position(float& pos, float max_pos);

Space_Object::Space_Object()
{
    status = INDISCERNIBLE;
    explosion_progress = EXPLOSION_START;
}

float Space_Object::lookup_sine(float direction)
{
    return 127.0f / 128 * std::sin(direction);
}

float Space_Object::lookup_cosine(float direction)
{
    return 127.0f / 128 * std::cos(direction);
}

void Space_Object::update_position(float delta_time)
{
    update_position_axis(delta_time, pos.x, vel_x);
    update_position_axis(delta_time, pos.y, vel_y);
    wrap_position(pos.x, MAX_X_POS);
    wrap_position(pos.y, MAX_Y_POS);
}

void Space_Object::update_position_axis(float delta_time, float& position, float velocity)
{
    position += velocity / 256 * delta_time;
}

void wrap_position(float& pos, float max_pos)
{
    if (pos < 0)
        pos += max_pos;
    else if (pos >= max_pos)
        pos -= max_pos;
}

bool Space_Object::collide(Space_Object object, u8 radius) const
{
    if (       status == INDISCERNIBLE ||        status >= TRUE_EXPLOSION_START
     || object.status == INDISCERNIBLE || object.status >= TRUE_EXPLOSION_START)
        return false;

    const float x_distance = object.pos.x - pos.x;
    const float y_distance = object.pos.y - pos.y;

    const float combined_distance = std::sqrt(x_distance * x_distance + y_distance * y_distance);
    const float circular_hitbox = radius / 128.0f;

    return combined_distance <= circular_hitbox;
}

void Space_Object::draw_explosion(Vector_Generator& vector_generator, RenderWindow& window) const
{
    set_position_and_size(explosion_progress / 16, vector_generator, window);

    switch (status >> 2 & 3)
    {
    case 0:
        vector_generator.process(EXPLOSION_4, window);
        break;
    case 1:
        vector_generator.process(EXPLOSION_3, window);
        break;
    case 2:
        vector_generator.process(EXPLOSION_2, window);
        break;
    case 3:
        vector_generator.process(EXPLOSION_1, window);
        break;
    }
}

void Space_Object::set_position_and_size(float scale, Vector_Generator& vector_generator, RenderWindow& window) const
{
    // add 128 to y making the 4:3 Space_Object space centered inside the 1:1 DVG space
    const float vector_object[] =
    {
        LABS, pos.x * 32, pos.y * 32 + 128, scale,
        RTSL
    };
    vector_generator.process(vector_object, window);
}

void Space_Object::advance_explosion(float additonal_blast)
{
    explosion_progress += additonal_blast;
    if (explosion_progress < 256)
        status = explosion_progress;
    else
    {
        status = INDISCERNIBLE;
        explosion_progress = EXPLOSION_START;
    }
}
