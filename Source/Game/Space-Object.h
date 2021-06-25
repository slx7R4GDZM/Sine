// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common-Things.h"

class Vector_Generator;

struct Position
{
    float x;
    float y;
};

class Space_Object
{
public:
    friend class Game;
    friend class Photon;

    bool collide(Space_Object object, u8 radius) const;
protected:
    Space_Object();

    static float lookup_sine(float direction);
    static float lookup_cosine(float direction);
    void update_position(float delta_time);
    static void update_position_axis(float delta_time, float& position, float velocity);
    void draw_explosion(Vector_Generator& vector_generator, RenderWindow& window) const;
    void set_position_and_size(float scale, Vector_Generator& vector_generator, RenderWindow& window) const;
    void advance_explosion(float additonal_blast);

    u8 status;
    float vel_x;
    float vel_y;
    Position pos;
private:
    float explosion_progress;
};
