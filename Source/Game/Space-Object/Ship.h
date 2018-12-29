// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Ship : public Space_Object
{
public:
    void spawn();
    void crash(u8& player_lives, u8& ship_spawn_timer);
    void update(u8 fast_timer, u8 direction, Offset explosion_x[], Offset explosion_y[], bool thrusting, Vector_Generator& vector_generator, RenderWindow& window);
    void add_thrust(u8 direction, u8& vel_x_minor, u8& vel_y_minor);
    void dampen_velocity(u8& vel_x_minor, u8& vel_y_minor);
private:
    void draw(u8 direction, bool draw_thrust, Vector_Generator& vector_generator, RenderWindow& window) const;
    void handle_explosion(Offset explosion_x[], Offset explosion_y[], Vector_Generator& vector_generator, RenderWindow& window) const;
    u16 update_explosion_offset(Offset& offset, s8 velocity) const;
};
