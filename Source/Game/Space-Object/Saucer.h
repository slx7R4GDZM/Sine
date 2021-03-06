// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Saucer : public Space_Object
{
public:
    void spawn(Score player_score, u8 saucer_spawn_time_start);
    void crash(u8& saucer_spawn_time, u8 saucer_spawn_time_start);
    void update(u8 fast_timer, u8& saucer_spawn_time, u8 saucer_spawn_time_start, Vector_Generator& vector_generator, RenderWindow& window);
    u8 targeted_shot(Position ship_pos) const;
    static s8 shot_offset(bool accurate_shot);
    u8 get_size(bool bonus) const;
    u8 get_points() const;
private:
    void determine_vertical_velocity();
    void attempt_remove(u8 old_pos_x_major, u8& saucer_spawn_time, u8 saucer_spawn_time_start);
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
};
