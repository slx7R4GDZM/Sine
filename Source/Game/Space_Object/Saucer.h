// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Space_Object.h"
#include "../../Graphics/Vector_Generator.h"
#include "../../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Saucer : public Space_Object
{
private:
    void determine_vertical_velocity();
    void attempt_remove(const s8 wrap_vel, u8& saucer_spawn_and_shot_time, const u8 saucer_spawn_time_start);
    void draw(Vector_Generator& vector_generator, sf::RenderWindow& window) const;
public:
    void spawn(const u16 player_score, const u8 saucer_spawn_time_start);
    void crash(u8& saucer_spawn_and_shot_time, const u8 saucer_spawn_time_start);
    void update(Vector_Generator& vector_generator, sf::RenderWindow& window, const u8 fast_timer, u8& saucer_spawn_and_shot_time, const u8 saucer_spawn_time_start);
    u8 get_size(bool bonus) const;
    u8 get_points() const;
};
