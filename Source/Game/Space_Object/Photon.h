// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space_Object.h"
#include "../../Graphics/Vector_Generator.h"
#include "../../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Photon : public Space_Object
{
private:
    u8 photon_timer;

    static void solve_position(const s8 base_offset, u8& pos_major, u8& pos_minor);
    void draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    Photon();
    void initialize_photon(const u8 direction, const s8 vel_x, const s8 vel_y, const Position base_pos);
    void update(Vector_Generator& vector_generator, sf::RenderWindow& window);
};
