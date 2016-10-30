// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space_Object.h"
#include "../../Graphics/Vector_Generator.h"
#include "../../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Saucer : public Space_Object
{
private:
    u8 y_counter;

    void determine_vertical_velocity();
    void draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    Saucer();
    u8 update(Vector_Generator& vector_generator, sf::RenderWindow& window);
    u8 get_size(bool bonus) const;
    u8 get_points() const;
};
