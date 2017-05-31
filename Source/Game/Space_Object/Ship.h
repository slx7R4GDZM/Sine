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
    static void dampen_velocity_axis(s8& vel_major, u8& vel_minor);
    void draw(bool& draw_thrust, const u8 direction, Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    void spawn();
    void crash(u8& player_lives, u8& ship_spawn_timer);
    void update(const u8 fast_timer, const u8 direction, bool& draw_thrust, Vector_Generator& vector_generator, sf::RenderWindow& window);
    void add_thrust(const u8 direction, u8& vel_x_minor, u8& vel_y_minor);
    void dampen_velocity(u8& vel_x_minor, u8& vel_y_minor);
};
