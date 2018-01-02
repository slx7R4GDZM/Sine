// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Asteroid : public Space_Object
{
private:
    s8 get_starting_velocity() const;
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
public:
    void spawn_wave_asteroid();
    void spawn_crash_asteroid(const u8 status, const Position pos);
    void update(u8& asteroid_count, u8& asteroid_wave_spawn_time, Vector_Generator& vector_generator, RenderWindow& window);
    u8 get_size() const;
    u8 get_points() const;
    static bool blocking_spawn(const Asteroid asteroid[]);
};
