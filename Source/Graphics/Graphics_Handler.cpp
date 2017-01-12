// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Graphics_Handler.h"

#include "../Other/Text.h"
#include "../Other/Vectors.h"
#include <cmath>

void Graphics_Handler::draw_message(const u16 message[], const u8 iteration, Vector_Generator& vector_generator, sf::RenderWindow& window)
{
    bool done = false;
    for (u8 i = iteration; !done; i++)
    {
        for (u8 c = 2; !done && c != 255; c--)
        {
            u8 character = (message[i] >> (c * 5 + 1)) & 0x1F;
            if (character == 0) // null
                done = true;
            else if (character < 5) // space, 0, 1, 2
                draw_character(character - 1, vector_generator, window);
            else // letters
                draw_character(character + 6, vector_generator, window);
        }
        if (!done) // if no null chars then check the stop bit
            done = message[i] & 0x0001;
    }
}

void Graphics_Handler::draw_character(const u8 character, Vector_Generator& vector_generator, sf::RenderWindow& window)
{
    vector_generator.process(CHARACTER_TABLE, window, CHARACTER_OFFSET_TABLE[character]);
}

void Graphics_Handler::draw_digit(const u8 digit, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten)
{
    vector_generator.process(CHARACTER_TABLE, window, CHARACTER_OFFSET_TABLE[digit + 1], false, false, brighten);
}

void Graphics_Handler::draw_score(const u16 number, const u8 rightmost_x, const u8 y, const Scale num_scale, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten)
{
    string num_string = std::to_string(number);
    u8 digits = num_string.length();

    vector_generator.load_absolute(rightmost_x - (digits * 3 * std::pow(2, static_cast<u8>(num_scale))), y, num_scale);
    for (u8 i = 0; i < digits; i++)
        draw_digit(num_string[i] - '0', vector_generator, window, brighten);

    // add trailing 0
    draw_digit(0, vector_generator, window, brighten);
}

// used for drawing the player 1 and player 2 scores
void Graphics_Handler::draw_score(const u8 player, const u16 score, Vector_Generator& vector_generator, sf::RenderWindow& window, const bool brighten)
{
    if (player == 0)
        draw_score(score, 49, 37, MUL_2, vector_generator, window, brighten);
    else
        draw_score(score, 216, 37, MUL_2, vector_generator, window, brighten);
}

void Graphics_Handler::draw_text(const Text text, const Language language, Vector_Generator& vector_generator, sf::RenderWindow& window)
{
    switch (language)
    {
        case ENGLISH:
            draw_message(ENGLISH_TEXT_TABLE, ENGLISH_OFFSET_TABLE[text], vector_generator, window);
            break;
        case GERMAN:
            draw_message(GERMAN_TEXT_TABLE, GERMAN_OFFSET_TABLE[text], vector_generator, window);
            break;
        case FRENCH:
            draw_message(FRENCH_TEXT_TABLE, FRENCH_OFFSET_TABLE[text], vector_generator, window);
            break;
        case SPANISH:
            draw_message(SPANISH_TEXT_TABLE, SPANISH_OFFSET_TABLE[text], vector_generator, window);
            break;
    }
}
