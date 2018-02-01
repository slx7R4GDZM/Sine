// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space-Object.h"

class Saucer : public Space_Object
{
private:
    void determine_vertical_velocity();
    void attempt_remove(const u8 old_pos_x_major, u8& saucer_spawn_and_shot_time, const u8 saucer_spawn_time_start);
    void draw(Vector_Generator& vector_generator, RenderWindow& window) const;
public:
    void spawn(const Score player_score, const u8 saucer_spawn_time_start);
    void crash(u8& saucer_spawn_and_shot_time, const u8 saucer_spawn_time_start);
    void update(const u8 fast_timer, u8& saucer_spawn_and_shot_time, const u8 saucer_spawn_time_start, Vector_Generator& vector_generator, RenderWindow& window);
    u8 get_size(const bool bonus) const;
    u8 get_points() const;
};
