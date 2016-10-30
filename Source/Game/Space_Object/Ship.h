// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space_Object.h"
#include "../../Graphics/Vector_Generator.h"
#include "../../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Ship : public Space_Object
{
private:
    static void negative_vel_change(s8& vel_major, u8& vel_minor, const u8 old_vel_minor);
    static void positive_vel_change(s8& vel_major, u8& vel_minor, const u8 old_vel_minor);
    static void dampen_velocity(s8& vel_major, u8& vel_minor);
    void draw(bool& draw_thrust_graphic, const u8 direction, Vector_Generator& vector_generator, sf::RenderWindow& window) const;
    static void draw_thrust(const u8 ship_thrust_vector_offset, const bool flip_x, const bool flip_y, Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void calculate_ship_vector_properties(const u8 direction, u8& ship_vector_offset, u8& ship_thrust_vector_offset, bool& flip_x, bool& flip_y);
public:
    Ship();
    u8 update(Vector_Generator& vector_generator, sf::RenderWindow& window, const u8 fast_timer, const u8 direction, bool& draw_thrust_graphic);
    static void add_thrust(const u8 direction, s8& vel_x_major, u8& vel_x_minor, s8& vel_y_major, u8& vel_y_minor);
    static void dampen_velocity(s8& vel_x_major, u8& vel_x_minor, s8& vel_y_major, u8& vel_y_minor);
};
