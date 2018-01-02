// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common-Things.h"

class Vector_Generator;

void draw_text(const Text text, const Language language, Vector_Generator& vector_generator, RenderWindow& window);
void draw_message(const u16 message[], const u8 iteration, Vector_Generator& vector_generator, RenderWindow& window);
void draw_character(const u8 character, Vector_Generator& vector_generator, RenderWindow& window);
void draw_player_score(const u8 player, const Score score[], Vector_Generator& vector_generator, RenderWindow& window, const bool brighten = false);
void draw_number(const u8 number[], const u8 num_size, Vector_Generator& vector_generator, RenderWindow& window, const bool add_zero = false, const bool brighten = false);
void draw_digit(const u8 digit, bool& drawn_digit, const bool last_digit, const bool brighten, Vector_Generator& vector_generator, RenderWindow& window);
void draw_digit(const u8 digit, Vector_Generator& vector_generator, RenderWindow& window, const bool brighten = false);
void set_position_and_size(const u8 cur_x, const u8 cur_y, const Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window);
