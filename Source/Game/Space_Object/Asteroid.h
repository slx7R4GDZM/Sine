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
    static u8 get_starting_velocity();
    void draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    Asteroid();
    Asteroid(const u8 status, const Position pos);
    u8 update(Vector_Generator& vector_generator, sf::RenderWindow& window);
    u8 get_size() const;
    u8 get_points() const;
};
