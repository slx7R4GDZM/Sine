// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_GRAPHICS_HANDLER_HC
#define SINE_GRAPHICS_HANDLER_HC

#include "Graphics/Vector-Generator.HC"
#include "Other/Common-Things.HC"
#include "Other/Text.HC"
#include "Other/Vectors.HC"

U0 draw_character(U8 character, Vector_Generator* vector_generator, CDC* dc)
{
    process(vector_generator, CHARACTER_TABLE, dc, CHARACTER_OFFSET_TABLE[character]);
}

U0 draw_message(U16* message, U8 iteration, Vector_Generator* vector_generator, CDC* dc)
{
    Bool done = FALSE;
    U64 i;
    for (i = iteration; !done; i++)
    {
        U64 c;
        for (c = 2; !done && c < 3; c--)
        {
            U8 character = (message[i] >> (c * 5 + 1)) & 0x1F;
            if (character == 0) // null
                done = TRUE;
            else if (character < 5) // space, 0, 1, 2
                draw_character(character - 1, vector_generator, dc);
            else // letters
                draw_character(character + 6, vector_generator, dc);
        }
        if (!done) // if no null chars then check the stop bit
            done = message[i] & 0x0001;
    }
}

U0 draw_text(U8 text, U8 option_switch, Vector_Generator* vector_generator, CDC* dc)
{
    switch (option_switch >> 6)
    {
    case ENGLISH:
        draw_message(ENGLISH_TEXT_TABLE, ENGLISH_OFFSET_TABLE[text], vector_generator, dc);
        break;
    case GERMAN:
        draw_message(GERMAN_TEXT_TABLE, GERMAN_OFFSET_TABLE[text], vector_generator, dc);
        break;
    case FRENCH:
        draw_message(FRENCH_TEXT_TABLE, FRENCH_OFFSET_TABLE[text], vector_generator, dc);
        break;
    case SPANISH:
        draw_message(SPANISH_TEXT_TABLE, SPANISH_OFFSET_TABLE[text], vector_generator, dc);
        break;
    }
}

U0 set_position_and_size(U8 cur_x, U8 cur_y, U8 scale, Vector_Generator* vector_generator, CDC* dc)
{
    U16 vector_object[3];
    vector_object[0] = ( LABS << 12) | (cur_y << 2);
    vector_object[1] = (scale << 12) | (cur_x << 2);
    vector_object[2] = ( RTSL << 12);

    process(vector_generator, vector_object, dc);
}

U0 draw_digit(U8 digit, Vector_Generator* vector_generator, CDC* dc, Bool brighten = FALSE)
{
    process(vector_generator, CHARACTER_TABLE, dc, CHARACTER_OFFSET_TABLE[digit + 1], FALSE, FALSE, brighten);
}

U0 draw_chain_digit(U8 digit, Bool* drawn_digit, Bool last_digit, Bool brighten, Vector_Generator* vector_generator, CDC* dc)
{
    if (*drawn_digit || digit || last_digit)
    {
        draw_digit(digit, vector_generator, dc, brighten);
        *drawn_digit = TRUE;
    }
    else
        // space
        draw_character(0, vector_generator, dc);
}

U0 draw_number(U8* number, U8 num_size, Vector_Generator* vector_generator, CDC* dc, Bool add_zero = FALSE, Bool brighten = FALSE)
{
    Bool drawn_digit = FALSE;
    U64 i;
    for (i = num_size - 1; i < num_size; i--)
    {
        draw_chain_digit(number[i] >> 4, &drawn_digit, FALSE, brighten, vector_generator, dc);
        draw_chain_digit(number[i] & 0xF, &drawn_digit, i == 0, brighten, vector_generator, dc);
    }
    if (add_zero)
        draw_digit(0, vector_generator, dc, brighten);
}

U0 draw_player_score(U8 player, Score* score, Vector_Generator* vector_generator, CDC* dc, Bool brighten = FALSE)
{
    if (player == 0)
        set_position_and_size( 25, 219, MUL_2, vector_generator, dc);
    else
        set_position_and_size(192, 219, MUL_2, vector_generator, dc);

    draw_number(score[player].points, 2, vector_generator, dc, TRUE, brighten);
}

#endif
