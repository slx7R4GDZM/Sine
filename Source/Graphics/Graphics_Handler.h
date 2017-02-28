// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "Vector_Generator.h"
#include "../Other/Common_Things.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Graphics_Handler
{
private:
    static void draw_message(const u16 message[], const u8 iteration, Vector_Generator& vector_generator, sf::RenderWindow& window);
public:
    static void draw_character(const u8 character, Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void draw_digit(const u8 digit, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten = false);
    static void draw_number(const u16 number, const u8 rightmost_x, const u8 y, const Scale num_scale, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool add_zero = false, const bool brighten = false);
    static void draw_score(const u8 player, const u16 score, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten = false);
    static void draw_text(const Text text, const Language language, Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void draw_multiplayer_scores(Vector_Generator& vector_generator);
    static void draw_copyright(Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void set_position_and_size(const u8 cur_x, const u8 cur_y, const Scale scale, Vector_Generator& vector_generator, sf::RenderWindow& window);
};
