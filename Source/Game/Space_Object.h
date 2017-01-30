// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Graphics/Vector_Generator.h"
#include "../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Space_Object
{
private:
    static void update_position_axis(u8& pos_major, u8& pos_minor, const s8 vel_major);
    static void wrap_position(u8& pos_major, const u8 max_pos_major);
    static bool hit(const u16 pos1, const u16 pos2, const u8 minimum_space);
protected:
    s8 vel_x_major;
    s8 vel_y_major;
    Position pos;
    u8 status;

    Space_Object();
    static s8 lookup_sine(const u8 direction);
    static s8 lookup_cosine(const u8 direction);
    void update_position();
    void draw_explosion(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    static u8 limit_position(const u8 pos_major, const u8 max_pos_major);
    static bool collide(const Space_Object o1, const Space_Object& o2, const u8 minimum_space);

    s8 get_vel_x() const;
    s8 get_vel_y() const;
    Position get_position() const;
    u8 get_status() const;

    void set_velocity(const s8 vel_x_major, const s8 vel_y_major);
    void set_position_major(const u8 pos_x_major, const u8 pos_y_major);
    void set_status(const u8 status);
};
