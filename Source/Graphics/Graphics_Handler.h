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
    static bool draw_packed_character(const u8 character, Vector_Generator& vector_generator, sf::RenderWindow& window);
public:
    static void draw_character(const u8 character, Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void draw_lives(const u8 lives, Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void draw_digit(const u8 digit, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten = false);
    static void draw_score(const u16 number, const u8 rightmost_x, const u8 y, const u8 num_scale, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten = false);
    static void draw_score(const u8 player, const u16 score, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten = false);
    static void draw_text(const u8 text, const Language language, Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void draw_multiplayer_scores(Vector_Generator& vector_generator);
    static void draw_copyright(Vector_Generator& vector_generator, sf::RenderWindow& window);
    static void copy_from_vector_table(u16 output_vector[], const u16 vector_object[], const u8 vector_offset);
};
