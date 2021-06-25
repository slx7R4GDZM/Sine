// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Ship : public Space_Object
{
public:
    void spawn();
    void crash(u8& player_lives, float& ship_spawn_timer);
    void update(float delta_time, u8 fast_timer, float direction, float explosion_x[], float explosion_y[], bool thrusting, Vector_Generator& vector_generator, RenderWindow& window);
    void add_thrust(float delta_time, float direction);
    void dampen_velocity(float delta_time);
private:
    void draw(float direction, bool draw_thrust, Vector_Generator& vector_generator, RenderWindow& window) const;
    void handle_explosion(float delta_time, float explosion_x[], float explosion_y[], Vector_Generator& vector_generator, RenderWindow& window) const;
    void update_explosion_offset(float delta_time, float& offset, float velocity) const;
};
