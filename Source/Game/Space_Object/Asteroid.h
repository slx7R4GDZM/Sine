// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space_Object.h"
#include "../../Graphics/Vector_Generator.h"
#include "../../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Asteroid : public Space_Object
{
private:
    s8 get_starting_velocity() const;
    void draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    Asteroid();
    void spawn_wave_asteroid();
    void spawn_crash_asteroid(const u8 status, const Position pos);
    void update(Vector_Generator& vector_generator, sf::RenderWindow& window, u8& asteroid_count, u8& asteroid_wave_spawn_time);
    u8 get_size() const;
    u8 get_points() const;
};
