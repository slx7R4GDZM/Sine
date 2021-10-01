// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Saucer : public Space_Object
{
public:
    void spawn(float delta_time, Score player_score, u8 saucer_spawn_time_start);
    void crash(float& saucer_spawn_time, u8 saucer_spawn_time_start);
    void update(float delta_time, u8 fast_timer, float& saucer_spawn_time, u8 saucer_spawn_time_start, Vector_Generator& vector_generator, RenderWindow& window);
    float targeted_shot(Position ship_pos) const;
    static float shot_offset(bool accurate_shot);
    u8 get_size(bool bonus) const;
    u8 get_points() const;
private:
    void determine_vertical_velocity(float& delta_time);
    void attempt_remove(float old_pos_x, float& saucer_spawn_time, u8 saucer_spawn_time_start);
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;

    float vertical_delay;
};
