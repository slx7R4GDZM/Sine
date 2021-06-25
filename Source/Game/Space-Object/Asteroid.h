// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Asteroid : public Space_Object
{
public:
    void spawn_wave_asteroid();
    void spawn_split_asteroid(u8 asteroid_size, float vel_x, float vel_y, Position pos);
    static void offset_position(float& position, float velocity);
    void update(float delta_time, u8& asteroid_count, float& asteroid_wave_spawn_time, Vector_Generator& vector_generator, RenderWindow& window);
    u8 get_size() const;
    u8 get_points() const;
    static bool blocking_spawn(const Asteroid asteroid[]);
private:
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
};
