// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Graphics-Handler.h"

#include "Vector-Generator.h"
#include "../Other/Text.h"
#include "../Other/Vectors.h"

void draw_text(const Text text, const Language language, Vector_Generator& vector_generator, RenderWindow& window)
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

void draw_message(const u16 message[], const u8 iteration, Vector_Generator& vector_generator, RenderWindow& window)
{
    bool done = false;
    for (u8 i = iteration; !done; i++)
    {
        for (u8 c = 2; !done && c != UINT8_MAX; c--)
        {
            const u8 character = (message[i] >> (c * 5 + 1)) & 0x1F;
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

void draw_character(const u8 character, Vector_Generator& vector_generator, RenderWindow& window)
{
    vector_generator.process(CHARACTER_TABLE, window, CHARACTER_OFFSET_TABLE[character]);
}

void draw_player_score(const u8 player, const Score score[], Vector_Generator& vector_generator, RenderWindow& window, const bool brighten)
{
    const u8 x = (player == 0 ? 25 : 192);
    set_position_and_size(x, 219, MUL_2, vector_generator, window);
    draw_number(score[player].points, 2, vector_generator, window, true, brighten);
}

void draw_number(const u8 number[], const u8 num_size, Vector_Generator& vector_generator, RenderWindow& window, const bool add_zero, const bool brighten)
{
    bool drawn_digit = false;
    for (u8 i = num_size - 1; i != UINT8_MAX; i--)
    {
        draw_digit(number[i] >> 4, drawn_digit, false, brighten, vector_generator, window);
        draw_digit(number[i] & 0xF, drawn_digit, i == 0, brighten, vector_generator, window);
    }
    if (add_zero)
        draw_digit(0, vector_generator, window, brighten);
}

void draw_digit(const u8 digit, bool& drawn_digit, const bool last_digit, const bool brighten, Vector_Generator& vector_generator, RenderWindow& window)
{
    if (drawn_digit || digit || last_digit)
    {
        draw_digit(digit, vector_generator, window, brighten);
        drawn_digit = true;
    }
    else
        // space
        draw_character(0, vector_generator, window);
}

void draw_digit(const u8 digit, Vector_Generator& vector_generator, RenderWindow& window, const bool brighten)
{
    vector_generator.process(CHARACTER_TABLE, window, CHARACTER_OFFSET_TABLE[digit + 1], false, false, brighten);
}

void set_position_and_size(const u8 cur_x, const u8 cur_y, const Global_Scale scale, Vector_Generator& vector_generator, RenderWindow& window)
{
    const u16 vector_object[] =
    {
        static_cast<u16>(( LABS << 12) | (cur_y << 2)),
        static_cast<u16>((scale << 12) | (cur_x << 2)),
                         ( RTSL << 12)
    };
    vector_generator.process(vector_object, window);
}
