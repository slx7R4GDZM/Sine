// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "../Other/Common-Things.h"

class Vector_Generator;

void draw_text(Text text, Language language, Vector_Generator& vector_generator, RenderWindow& window);
void draw_character(u8 character, Vector_Generator& vector_generator, RenderWindow& window);
void draw_player_score(u8 player, const Score score[], Vector_Generator& vector_generator, RenderWindow& window, bool brighten = false);
void draw_number(const u8 number[], u8 num_size, Vector_Generator& vector_generator, RenderWindow& window, bool add_zero = false, bool brighten = false);
void draw_digit(u8 digit, Vector_Generator& vector_generator, RenderWindow& window, bool brighten = false);
void set_position_and_size(u8 cur_x, u8 cur_y, Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window);
