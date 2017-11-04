// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common-Things.h"

class Vector_Generator;

class Space_Object
{
private:
    static void wrap_position(u8& pos_major, const u8 max_pos_major);
    static bool hit(const u16 pos1, const u16 pos2, const u8 minimum_space);
protected:
    u8 status;
    s8 vel_x_major;
    s8 vel_y_major;
    Position pos;

    Space_Object();
    static s8 lookup_sine(const u8 direction);
    static s8 lookup_cosine(const u8 direction);
    void update_position();
    static void update_position_axis(u8& pos_major, u8& pos_minor, const s8 vel_major);
    void draw_explosion(Vector_Generator& vector_generator, RenderWindow& window) const;
    void set_position_and_size(const Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window) const;
public:
    static u8 limit_position(const u8 pos_major, const u8 max_pos_major);
    bool collide(const Space_Object& object, const u8 minimum_space) const;

    u8 get_status() const;
    s8 get_vel_x() const;
    s8 get_vel_y() const;
    Position get_position() const;

    void set_status(const u8 status);
    void set_velocity(const s8 vel_x_major, const s8 vel_y_major);
    void set_position_major(const u8 pos_x_major, const u8 pos_y_major);
};
