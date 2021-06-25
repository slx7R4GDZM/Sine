// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common-Things.h"

class Vector_Generator;

struct Position
{
    u8 x_major;
    u8 y_major;
    u8 x_minor;
    u8 y_minor;
};

class Space_Object
{
public:
    friend class Game;
    friend class Photon;

    static u8 limit_position(u8 pos_major, u8 max_pos_major);
    bool collide(Space_Object object, u8 radius) const;
protected:
    Space_Object();

    static s8 lookup_sine(u8 direction);
    static s8 lookup_cosine(u8 direction);
    void update_position();
    static void update_position_axis(u8& pos_major, u8& pos_minor, s8 vel_major);
    void draw_explosion(Vector_Generator& vector_generator, RenderWindow& window) const;
    void set_position_and_size(Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window) const;

    u8 status;
    s8 vel_x_major;
    s8 vel_y_major;
    Position pos;
};
