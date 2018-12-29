// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Asteroid : public Space_Object
{
public:
    void spawn_wave_asteroid();
    void spawn_split_asteroid(u8 asteroid_size, s8 vel_x, s8 vel_y, Position pos);
    static void offset_position(u8& pos_minor, s8 vel_major);
    void update(u8& asteroid_count, u8& asteroid_wave_spawn_time, Vector_Generator& vector_generator, RenderWindow& window);
    u8 get_size() const;
    u8 get_points() const;
    static bool blocking_spawn(const Asteroid asteroid[]);
private:
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
};
