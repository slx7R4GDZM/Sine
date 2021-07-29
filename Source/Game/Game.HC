// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.TXT file for details.

#ifndef SINE_GAME_HC
#define SINE_GAME_HC

#include "Game/Space-Object.HC"
#include "Game/Space-Object/Asteroid.HC"
#include "Game/Space-Object/Ship.HC"
#include "Game/Space-Object/Saucer.HC"
#include "Game/Space-Object/Photon.HC"
#include "Graphics/Graphics-Handler.HC"
#include "Graphics/Vector-Generator.HC"
#include "Input/Input-Handler.HC"
#include "Other/Constants.HC"
#include "Other/Vectors.HC"
#include "Settings/Settings-Handler.HC"

Settings_Handler settings;
CDC* dc;
Input_Handler input;

U8 current_player;
U8 last_game_player_count;
U8 player_count;
Score high_score_table[MAX_HS_COUNT];
U8 name_entry_letter_pos;
U8 player_HS_place[MAX_PLAYERS];
U8 names_HS[MAX_HS_COUNT * HS_NAME_LENGTH];
Score player_score[MAX_PLAYERS];
U8 starting_lives;
U8 player_lives[MAX_PLAYERS];
U8 hyperspace_flag;
U8 player_text_timer;
U8 fast_timer;
U8 slow_timer;
U8 ship_direction;
U8 saucer_direction;
U8 ship_vel_x_minor;
U8 ship_vel_y_minor;
// sound stuff will go here
U8 credits;
U8 option_switch;
U8 pre_credit_coins;
Offset ship_explosion_x[6];
Offset ship_explosion_y[6];
class Player
{
    Space_Object asteroid[MAX_ASTEROIDS];
    Space_Object ship;
    Space_Object saucer;
    Space_Object saucer_photon[MAX_SAUCER_PHOTONS];
    Space_Object ship_photon[MAX_SHIP_PHOTONS];
    U8 asteroids_per_wave;
    U8 asteroid_count;
    U8 saucer_spawn_and_shot_time;
    U8 saucer_spawn_time_start;
    U8 block_saucer_spawn_time;
    U8 ship_spawn_timer;
    U8 asteroid_wave_spawn_time;
    U8 asteroids_to_slow_saucer_spawn;
} player[MAX_PLAYERS];

U0 handle_saucer_stuff(Player* player)
{
    if (fast_timer & 3)
        return;

    player->saucer_spawn_and_shot_time--;
    if (player->saucer.status == INDISCERNIBLE)
    {
        if (player->block_saucer_spawn_time)
            player->block_saucer_spawn_time--;

        if (player->saucer_spawn_and_shot_time == 0)
        {
            player->saucer_spawn_and_shot_time = 18;
            if (player->asteroid_count == 0)
                return;

            if (player->block_saucer_spawn_time
             && player->asteroid_count >= player->asteroids_to_slow_saucer_spawn)
                return;

            U8 new_saucer_spawn_time_start = player->saucer_spawn_time_start - 6;
            if (new_saucer_spawn_time_start >= MINIMUM_SAUCER_SPAWN_TIME)
                player->saucer_spawn_time_start = new_saucer_spawn_time_start;

            spawn_saucer(&player->saucer, player_score[current_player], player->saucer_spawn_time_start);
        }
    }
    else if (player->saucer_spawn_and_shot_time == 0)
    {
        if (player->saucer.status == LARGE_SAUCER)
            saucer_direction = random_byte();
        else
        {
            saucer_direction = targeted_shot(player->saucer, player->ship.pos);
            Bool accurate_shot = player_score[current_player].points[1] >= 0x35;
            saucer_direction += shot_offset(accurate_shot);
        }
        fire_photon(player->saucer_photon, MAX_SAUCER_PHOTONS, saucer_direction, player->saucer);
        player->saucer_spawn_and_shot_time = 10;
    }
}

U0 handle_ship_stuff(Player* player)
{
    // on_press should be handled slightly differently, also need a limit on the photon shooting speed?
    if (on_press(&input, FIRE) && !player_text_timer && !player->ship_spawn_timer)
        fire_photon(player->ship_photon, MAX_SHIP_PHOTONS, ship_direction, player->ship);
    if (is_pressed(&input, HYPERSPACE) && !player_text_timer && !player->ship_spawn_timer)
    {
        player->ship.status = INDISCERNIBLE;
        player->ship.vel_x_major = 0;
        player->ship.vel_y_major = 0;
        player->ship_spawn_timer = 48;

        player->ship.pos.x_major = limit_position(random_byte() & 31, 28);
        hyperspace_flag = 1;
        U8 pos_y_major = random_byte() & 31;
        if (pos_y_major >= 24)
        {
            pos_y_major = (pos_y_major & 7) * 2 + 4;
            if (pos_y_major >= player->asteroid_count)
                hyperspace_flag = 128;
        }
        player->ship.pos.y_major = limit_position(pos_y_major, 20);
    }
    if (player->ship_spawn_timer > 0 && player->ship.status < TRUE_EXPLOSION_START)
    {
        if (player->ship_spawn_timer > 1)
            player->ship_spawn_timer--;
        else if (player->ship_spawn_timer == 1)
        {
            // ship trying to spawn
            if (hyperspace_flag == 0)
            {
                if (!blocking_spawn(player->asteroid))
                {
                    player->ship.status = ALIVE;
                    player->ship_spawn_timer = 0;
                }
            }
            // hyperspace re-entry
            else if (hyperspace_flag < 128)
            {
                player->ship.status = ALIVE;
                player->ship_spawn_timer = 0;
                hyperspace_flag = 0;
            }
            // hyperspace failure
            else
            {
                crash_ship(&player->ship, &player_lives[current_player], &player->ship_spawn_timer);
                hyperspace_flag = 0;
            }
        }
    }
    if (is_pressed(&input, ROTATE_LEFT))
    {
        if (!player->ship_spawn_timer)
            ship_direction += 3;
    }
    else if (is_pressed(&input, ROTATE_RIGHT))
    {
        if (!player->ship_spawn_timer)
            ship_direction -= 3;
    }
    if ((fast_timer & 1) == 0 && !player->ship_spawn_timer)
    {
        if (is_pressed(&input, THRUST))
            add_thrust(&player->ship, ship_direction, &ship_vel_x_minor, &ship_vel_y_minor);
        else
            dampen_velocity(&player->ship, &ship_vel_x_minor, &ship_vel_y_minor);
    }
    if (player->ship.status && player->ship.status < TRUE_EXPLOSION_START)
        handle_saucer_stuff(player);
}

Bool spawn_asteroid(Player* player, U8 crashed_ast, U8 ast_size, U8* new_ast)
{
    U64 i;
    for (i = 0; i < MAX_ASTEROIDS && player->asteroid_count < MAX_ASTEROIDS; i++)
    {
        if (player->asteroid[i].status == INDISCERNIBLE)
        {
            *new_ast = i;
            spawn_split_asteroid(&player->asteroid[i],
                                 ast_size,
                                 player->asteroid[crashed_ast].vel_x_major,
                                 player->asteroid[crashed_ast].vel_y_major,
                                 player->asteroid[crashed_ast].pos);
            player->asteroid_count++;
            return TRUE;
        }
    }
    return FALSE;
}

U0 crash_asteroid(Player* player, U8 crashed_ast)
{
    U8 ast_size = player->asteroid[crashed_ast].status & ASTEROID_SIZE;
    player->asteroid[crashed_ast].status = EXPLOSION_START;
    player->block_saucer_spawn_time = 80;
    if (ast_size == SMALL_ASTEROID)
        return;

    U8 new_ast;
    if (!spawn_asteroid(player, crashed_ast, ast_size, &new_ast))
        return;

    offset_asteroid_position(player->asteroid[new_ast].pos.x_minor,
                             player->asteroid[new_ast].vel_x_major);

    if (!spawn_asteroid(player, crashed_ast, ast_size, &new_ast))
        return;

    offset_asteroid_position(player->asteroid[new_ast].pos.y_minor,
                             player->asteroid[new_ast].vel_y_major);
}

U0 attempt_asteroid_wave_spawn(Player* player)
{
    if (player->asteroid_wave_spawn_time)
        player->asteroid_wave_spawn_time--;

    if (player->asteroid_count == 0 && player->saucer.status == INDISCERNIBLE && !player->asteroid_wave_spawn_time)
    {
        if (player->asteroids_per_wave < 10)
            player->asteroids_per_wave += 2;
        else if (player->asteroids_per_wave == 10)
            player->asteroids_per_wave++;

        player->saucer_spawn_and_shot_time = 127;
        if (player->asteroids_to_slow_saucer_spawn < 10)
            player->asteroids_to_slow_saucer_spawn++;

        U64 i;
        for (i = 0; i < player->asteroids_per_wave; i++)
        {
            spawn_wave_asteroid(&player->asteroid[i]);
            player->asteroid_count++;
        }
    }
}

U0 clear_space_objects(Player* player)
{
    U64 i;
    for (i = 0; i < MAX_ASTEROIDS; i++)
        player->asteroid[i].status = INDISCERNIBLE;

    player->ship.status = INDISCERNIBLE;
    player->saucer.status = INDISCERNIBLE;

    for (i = 0; i < MAX_SAUCER_PHOTONS; i++)
        player->saucer_photon[i].status = INDISCERNIBLE;

    for (i = 0; i < MAX_SHIP_PHOTONS; i++)
        player->ship_photon[i].status = INDISCERNIBLE;
}

U0 add_points(U8 points, Bool bonus)
{
    U8 old_ten_thousandth = player_score[current_player].points[1] & 0xF0;
    add_to_number(player_score[current_player].points, 2, points, bonus);

    U8 new_ten_thousandth = player_score[current_player].points[1] & 0xF0;
    if (new_ten_thousandth != old_ten_thousandth)
        player_lives[current_player]++;
}

U0 handle_collision(Player* player)
{
    // ship
    U64 i;
    for (i = 0; player->ship.status == ALIVE && i < MAX_ASTEROIDS; i++)
    {
        if (collide(player->ship, player->asteroid[i], SHIP_HITBOX + get_asteroid_size(player->asteroid[i])))
        {
            add_points(get_asteroid_points(player->asteroid[i]), FALSE);
            crash_asteroid(player, i);
            crash_ship(&player->ship, &player_lives[current_player], &player->ship_spawn_timer);
        }
    }

    // saucer
    for (i = 0; (player->saucer.status == LARGE_SAUCER || player->saucer.status == SMALL_SAUCER) && i < MAX_ASTEROIDS; i++)
    {
        if (collide(player->saucer, player->asteroid[i], get_saucer_size(player->saucer, FALSE) + get_asteroid_size(player->asteroid[i])))
        {
            crash_asteroid(player, i);
            crash_saucer(&player->saucer, &player->saucer_spawn_and_shot_time, player->saucer_spawn_time_start);
        }
    }
    if (collide(player->saucer, player->ship, get_saucer_size(player->saucer, FALSE) + SMALL_ASTEROID_HITBOX))
    {
        add_points(get_saucer_points(player->saucer), player->saucer.status == SMALL_SAUCER);
        crash_ship(&player->ship, &player_lives[current_player], &player->ship_spawn_timer);
        crash_saucer(&player->saucer, &player->saucer_spawn_and_shot_time, player->saucer_spawn_time_start);
    }

    // saucer photon
    Bool crashed;
    U64 x;
    for (i = 0; i < MAX_SAUCER_PHOTONS; i++)
    {
        for (x = 0, crashed = FALSE; x < MAX_ASTEROIDS && !crashed; x++)
        {
            if (collide(player->saucer_photon[i], player->asteroid[x], get_asteroid_size(player->asteroid[x])))
            {
                crash_asteroid(player, x);
                player->saucer_photon[i].status = INDISCERNIBLE;
                crashed = TRUE;
            }
        }
        if (collide(player->saucer_photon[i], player->ship, SMALL_ASTEROID_HITBOX))
        {
            crash_ship(&player->ship, &player_lives[current_player], &player->ship_spawn_timer);
            player->saucer_photon[i].status = INDISCERNIBLE;
        }
    }

    // ship photon
    for (i = 0; i < MAX_SHIP_PHOTONS; i++)
    {
        for (x = 0, crashed = FALSE; x < MAX_ASTEROIDS && !crashed; x++)
        {
            if (collide(player->ship_photon[i], player->asteroid[x], get_asteroid_size(player->asteroid[x])))
            {
                add_points(get_asteroid_points(player->asteroid[x]), FALSE);
                crash_asteroid(player, x);
                player->ship_photon[i].status = INDISCERNIBLE;
                crashed = TRUE;
            }
        }
        if (collide(player->ship_photon[i], player->ship, SMALL_ASTEROID_HITBOX))
        {
            crash_ship(&player->ship, &player_lives[current_player], &player->ship_spawn_timer);
            player->ship_photon[i].status = INDISCERNIBLE;
        }
        if (collide(player->ship_photon[i], player->saucer, get_saucer_size(player->saucer, TRUE)))
        {
            add_points(get_saucer_points(player->saucer), player->saucer.status == SMALL_SAUCER);
            crash_saucer(&player->saucer, &player->saucer_spawn_and_shot_time, player->saucer_spawn_time_start);
            player->ship_photon[i].status = INDISCERNIBLE;
        }
    }
}

U0 update_space_objects(Player* player, Vector_Generator* vector_generator)
{
    U64 i;
    for (i = 0; i < MAX_ASTEROIDS; i++)
        update_asteroid(&player->asteroid[i], &player->asteroid_count, &player->asteroid_wave_spawn_time, vector_generator, dc);

    update_ship(&player->ship, fast_timer, ship_direction, ship_explosion_x, ship_explosion_y, is_pressed(&input, THRUST), vector_generator, dc);
    update_saucer(&player->saucer, fast_timer, &player->saucer_spawn_and_shot_time, player->saucer_spawn_time_start, vector_generator, dc);

    for (i = 0; i < MAX_SAUCER_PHOTONS; i++)
        update_photon(&player->saucer_photon[i], fast_timer, vector_generator, dc);

    for (i = 0; i < MAX_SHIP_PHOTONS; i++)
        update_photon(&player->ship_photon[i], fast_timer, vector_generator, dc);

    handle_collision(player);
}

U0 handle_HS_entry_input()
{
    if (on_press(&input, HYPERSPACE))
    {
        name_entry_letter_pos++;
        if (name_entry_letter_pos == HS_NAME_LENGTH)
        {
            name_entry_letter_pos = 0;
            player_HS_place[current_player] = 255;
            if (current_player > 0)
                current_player--;

            slow_timer = 240;
        }
        else
        {
            names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 11;
            slow_timer = 244;
        }
    }
    if ((fast_timer & 7) == 0)
    {
        if (is_pressed(&input, ROTATE_LEFT))
        {
            if (names_HS[player_HS_place[current_player] + name_entry_letter_pos] == 0)
                names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 11;
            else if (names_HS[player_HS_place[current_player] + name_entry_letter_pos] == 36)
                names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 0;
            else
                names_HS[player_HS_place[current_player] + name_entry_letter_pos]++;
        }
        else if (is_pressed(&input, ROTATE_RIGHT))
        {
            if (names_HS[player_HS_place[current_player] + name_entry_letter_pos] == 11)
                names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 0;
            else if (names_HS[player_HS_place[current_player] + name_entry_letter_pos] == 0)
                names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 36;
            else
                names_HS[player_HS_place[current_player] + name_entry_letter_pos]--;
        }
    }
}

U0 draw_HS_entry_screen(Vector_Generator* vector_generator)
{
    // draw high score entry control help
    if (last_game_player_count > 1)
    {
        set_position_and_size(100, 182, MUL_2, vector_generator, dc);
        draw_text(PLAYER_, option_switch, vector_generator, dc);
        if (fast_timer & 16)
            draw_digit(current_player + 1, vector_generator, dc);
    }
    set_position_and_size(12, 170, MUL_2, vector_generator, dc);
    draw_text(YOUR_SCORE_IS_ONE_OF_THE_TEN_BEST,      option_switch, vector_generator, dc);
    set_position_and_size(12, 162, MUL_2, vector_generator, dc);
    draw_text(PEASE_ENTER_YOUR_INITIALS,              option_switch, vector_generator, dc);
    set_position_and_size(12, 154, MUL_2, vector_generator, dc);
    draw_text(PUSH_ROTATE_TO_SELECT_LETTER,           option_switch, vector_generator, dc);
    set_position_and_size(12, 146, MUL_2, vector_generator, dc);
    draw_text(PUSH_HYPERSPACE_WHEN_LETTER_IS_CORRECT, option_switch, vector_generator, dc);

    // draw name entry
    set_position_and_size(100, 57, MUL_4, vector_generator, dc);
    U64 c;
    for (c = 0; c < HS_NAME_LENGTH; c++)
    {
        if (names_HS[player_HS_place[current_player] + c] == 0)
            process(vector_generator, UNDERSCORE, dc);
        else
            draw_character(names_HS[player_HS_place[current_player] + c], vector_generator, dc);
    }
}

U0 handle_HS_entry(Vector_Generator* vector_generator)
{
    // player took too long to enter name, time out back to attract mode
    if (slow_timer == 0)
    {
        current_player = 0;
        name_entry_letter_pos = 0;
        player_HS_place[0] = 255;
        player_HS_place[1] = 255;
        slow_timer = 240;
    }
    else
    {
        draw_HS_entry_screen(vector_generator);
        handle_HS_entry_input();
    }
}

U0 insert_any_new_high_scores()
{
    // move the old scores and initals down and insert the new high scores
    U64 p;
    for (p = 0; p < MAX_PLAYERS; p++)
    {
        if (player_HS_place[p] != 255)
        {
            U64 s;
            for (s = 0; s < MAX_HS_COUNT; s++)
            {
                if (score_greater(player_score[p], high_score_table[s]))
                {
                    // move the high score list down by one from where the new score is added
                    U64 i;
                    for (i = MAX_HS_COUNT - 1; i > s; i--)
                    {
                        MemCpy(&high_score_table[i], &high_score_table[i - 1], sizeof(Score));

                        // also move high score initials list down one
                        U64 c;
                        for (c = 0; c < HS_NAME_LENGTH; c++)
                            names_HS[i * HS_NAME_LENGTH + c] = names_HS[(i - 1) * HS_NAME_LENGTH + c];
                    }
                    // insert new score
                    MemCpy(&high_score_table[s], &player_score[p], sizeof(Score));

                    // set initials to "A  "
                    names_HS[s * HS_NAME_LENGTH] = 11;
                    names_HS[s * HS_NAME_LENGTH + 1] = 0;
                    names_HS[s * HS_NAME_LENGTH + 2] = 0;

                    break;
                }
            }
        }
    }
}

U0 end_game()
{
    current_player = player_count - 1;
    last_game_player_count = player_count;
    player_count = 0;
    name_entry_letter_pos = 0;

    // for each player compare their score to the hs table and get their hs placement
    U64 p;
    for (p = 0; p < MAX_PLAYERS; p++)
    {
        U64 s;
        for (s = 0; s < MAX_HS_COUNT; s++)
        {
            if (score_greater(player_score[p], high_score_table[s]))
            {
                player_HS_place[p] = s * HS_NAME_LENGTH;
                s = MAX_HS_COUNT; // cheeky loop escape
            }
        }
    }

    // if a new high score is set
    if (player_HS_place[0] != 255 || player_HS_place[1] != 255)
    {
        slow_timer = 240;

        // if p1 gets a hs in mp and p2 doesn't then just skip p2 entering their name
        if (player_HS_place[current_player] == 255)
            current_player--;

        // deal with both players getting high scores
        if (player_HS_place[0] != 255 && player_HS_place[1] != 255)
        {
            if (score_greater(player_score[0], player_score[1]))
                player_HS_place[1] += HS_NAME_LENGTH;
            else
                player_HS_place[0] += HS_NAME_LENGTH;
        }
        insert_any_new_high_scores();
    }
    else
        slow_timer = 255;
}

U0 update_player(Vector_Generator* vector_generator)
{
    if (player_text_timer)
    {
        set_position_and_size(100, 182, MUL_2, vector_generator, dc);
        draw_text(PLAYER_, option_switch, vector_generator, dc);
        draw_digit(current_player + 1, vector_generator, dc);
        player_text_timer--;
    }
    else if (!player_lives[current_player] && !any(player[current_player].ship_photon, MAX_SHIP_PHOTONS))
    {
        set_position_and_size(100, 157, MUL_2, vector_generator, dc);
        draw_text(GAME_OVER, option_switch, vector_generator, dc);
        if (player_count == 2)
        {
            set_position_and_size(100, 182, MUL_2, vector_generator, dc);
            draw_text(PLAYER_, option_switch, vector_generator, dc);
            draw_digit(current_player + 1, vector_generator, dc);
        }
    }
    if (!player_text_timer)
        handle_ship_stuff(&player[current_player]);

    attempt_asteroid_wave_spawn(&player[current_player]);
    update_space_objects(&player[current_player], vector_generator);

    // if the ship is destroyed, determine if there's enough lives to keep playing
    if (!player_text_timer && player[current_player].ship.status == INDISCERNIBLE && player[current_player].ship_spawn_timer >= 128)
    {
        if (player_count == 1)
        {
            if (player_lives[0])
            {
                player[0].saucer.status = INDISCERNIBLE;
                player[0].saucer_spawn_and_shot_time = player[0].saucer_spawn_time_start;
                player[0].ship_spawn_timer = 16;
            }
            else
                end_game();
        }
        else
        {
            if (player_lives[!current_player])
            {
                current_player = !current_player;
                player_text_timer = 128;
                player[current_player].saucer.status = INDISCERNIBLE;
                player[current_player].saucer_spawn_and_shot_time = player[current_player].saucer_spawn_time_start;
                player[current_player].ship_spawn_timer = 16;
            }
            else if (player_lives[current_player])
            {
                player_text_timer = 128;
                player[current_player].saucer.status = INDISCERNIBLE;
                player[current_player].saucer_spawn_and_shot_time = player[current_player].saucer_spawn_time_start;
                player[current_player].ship_spawn_timer = 16;
            }
            else
                end_game();
        }
    }
    set_position_and_size(40, 213, DIV_4, vector_generator, dc);
    U64 i;
    for (i = 0; i < player_lives[0] && i < 55; i++)
        process(vector_generator, LIVES_REMAINING_SHIP, dc);
}

U0 attract_mode(Vector_Generator* vector_generator)
{
    if (option_switch & 0x03)
    {
        set_position_and_size(80, 57, MUL_2, vector_generator, dc);
        if ((option_switch & 0x03) == 1)
            draw_text(ONE_COIN_2_PLAYS, option_switch, vector_generator, dc);
        else if ((option_switch & 0x03) == 2)
            draw_text(ONE_COIN_1_PLAY, option_switch, vector_generator, dc);
        else
            draw_text(TWO_COINS_1_PLAY, option_switch, vector_generator, dc);
    }

    // flash "push start" after enough credits
    if (credits >= 1 && (fast_timer & 32) == 0)
    {
        set_position_and_size(100, 198, MUL_2, vector_generator, dc);
        draw_text(PUSH_START, option_switch, vector_generator, dc);
    }

    if (score_greater(high_score_table[0], NO_SCORE) && (slow_timer & 4) == 0)
    {
        set_position_and_size(100, 182, MUL_2, vector_generator, dc);
        draw_text(HIGH_SCORES, option_switch, vector_generator, dc);

        // draw highscore table
        U64 i;
        U8 score_position[1];
        MemSetU8(score_position, 0, sizeof(U8));
        for (i = 0; i < MAX_HS_COUNT && score_greater(high_score_table[i], NO_SCORE); i++)
        {
            // draw score position
            add_to_number(score_position, 1, 1, FALSE);
            set_position_and_size(95, 167 - i * 8, MUL_2, vector_generator, dc);
            draw_number(score_position, 1, vector_generator, dc);
            process(vector_generator, DOT, dc);
            draw_character(0, vector_generator, dc);

            // draw score
            draw_number(high_score_table[i].points, 2, vector_generator, dc, TRUE);
            draw_character(0, vector_generator, dc);

            // draw score initials
            U64 c;
            for (c = 0; c < HS_NAME_LENGTH; c++)
                draw_character(names_HS[i * HS_NAME_LENGTH + c], vector_generator, dc);
        }
    }
    else
    {
        // show autodemo
        handle_saucer_stuff(&player[0]);
        attempt_asteroid_wave_spawn(&player[0]);
        update_space_objects(&player[0], vector_generator);
    }
}

U0 handle_game_start()
{
    current_player = 0;
    // names_HS[MAX_HS_COUNT * HS_NAME_LENGTH - 1] = 0; // bug that sets the final initial of the 10th high score to space
    player_text_timer = 128;
    U64 i;
    for (i = 0; i < player_count; i++)
    {
        player_HS_place[i] = 255;
        MemCpy(&player_score[i], &NO_SCORE, sizeof(Score));
        player_lives[i] = starting_lives;
        clear_space_objects(&player[i]);
        spawn_ship(&player[i].ship);
        player[i].asteroids_per_wave = 2;
        player[i].asteroid_count = 0;
        player[i].saucer_spawn_and_shot_time = 146;
        player[i].saucer_spawn_time_start = 146;
        player[i].ship_spawn_timer = 1;
        player[i].asteroid_wave_spawn_time = 127;
        player[i].asteroids_to_slow_saucer_spawn = 5;
    }
}

U0 attempt_game_start()
{
    if (on_press(&input, ONE_PLAYER_START))
    {
        if (credits >= 1)
        {
            credits--;
            player_count = 1;
            handle_game_start();
        }
    }
    else if (on_press(&input, TWO_PLAYER_START))
    {
        if (credits >= 2)
        {
            credits -= 2;
            player_count = 2;
            handle_game_start();
        }
    }
}

U0 add_credit()
{
    switch (option_switch & 0x03)
    {
    case 1:
        credits += 2;
        break;
    case 2:
    case 3:
        credits += 1;
        break;
    }
}

U0 coin_input_to_credit()
{
    // input to pre-credits
    if (on_press(&input, LEFT_COIN))
        pre_credit_coins++;

    if (on_press(&input, CENTER_COIN))
    {
        if ((option_switch & 0x10) == 0)
            pre_credit_coins++;
        else
            pre_credit_coins += 2;
    }
    if (on_press(&input, RIGHT_COIN))
    {
        switch (option_switch & 0x0C)
        {
        case 0:
            pre_credit_coins++;
            break;
        case 4:
            pre_credit_coins += 4;
            break;
        case 8:
            pre_credit_coins += 5;
            break;
        case 12:
            pre_credit_coins += 6;
            break;
        }
    }

    // pre-credits to credits
    if ((option_switch & 0x03) == 0)
        pre_credit_coins = 0;
    else if ((option_switch & 0x03) == 3)
    {
        if (pre_credit_coins >= 2)
        {
            pre_credit_coins -= 2;
            add_credit();
        }
    }
    else if (pre_credit_coins >= 1)
    {
        pre_credit_coins--;
        add_credit();
    }
}

U0 draw_copyright(Vector_Generator* vector_generator)
{
    process(vector_generator, COPYRIGHT_SYMBOL, dc);

    // 2016_
    draw_character(3, vector_generator, dc);
    draw_character(1, vector_generator, dc);
    draw_character(2, vector_generator, dc);
    draw_character(7, vector_generator, dc);
    draw_character(0, vector_generator, dc);

    // slx7R4GDZM
    draw_character(29, vector_generator, dc);
    draw_character(22, vector_generator, dc);
    draw_character(34, vector_generator, dc);
    draw_character( 8, vector_generator, dc);
    draw_character(28, vector_generator, dc);
    draw_character( 5, vector_generator, dc);
    draw_character(17, vector_generator, dc);
    draw_character(14, vector_generator, dc);
    draw_character(36, vector_generator, dc);
    draw_character(23, vector_generator, dc);
}

U0 draw_multiplayer_scores(Vector_Generator* vector_generator)
{
    // while waiting to spawn make the score of the current player flash
    // and otherwise brighten the score of the current player
    if (!hyperspace_flag && player[current_player].ship.status == INDISCERNIBLE)
    {
        if (current_player == 0)
        {
            if (fast_timer & 16)
                draw_player_score(0, player_score, vector_generator, dc, TRUE);

            draw_player_score(1, player_score, vector_generator, dc);
        }
        else
        {
            if (fast_timer & 16)
                draw_player_score(1, player_score, vector_generator, dc, TRUE);

            draw_player_score(0, player_score, vector_generator, dc);
        }
    }
    else
    {
        if (current_player == 0)
        {
            draw_player_score(0, player_score, vector_generator, dc, TRUE);
            draw_player_score(1, player_score, vector_generator, dc);
        }
        else
        {
            draw_player_score(0, player_score, vector_generator, dc);
            draw_player_score(1, player_score, vector_generator, dc, TRUE);
        }
    }
}

U0 limit_FPS(Settings_Handler* settings, I64 start_time)
{
    if (settings->frame_limiter_mode == SLEEPING)
    {
        if (cnts.jiffies - start_time < TARGET_FRAME_TIME_JIFFY)
            SleepUntil(start_time + TARGET_FRAME_TIME_JIFFY);
    }
    else
    {
        while (SysTimerRead() - start_time < TARGET_FRAME_TIME_SYSTEM)
            Yield();
    }
}

U0 initialize_memory()
{
    dc = DCAlias();
    current_player = 0;
    last_game_player_count = 0;
    player_count = 0;
    MemSetU8(high_score_table, 0, MAX_HS_COUNT * sizeof(Score));
    name_entry_letter_pos = 0;
    MemSetU8(player_HS_place, 176, MAX_PLAYERS);
    MemSetU8(names_HS, 0, MAX_HS_COUNT * HS_NAME_LENGTH);
    MemSetU8(player_score, 0, MAX_PLAYERS * sizeof(Score));
    MemSetU8(player_lives, 0, MAX_PLAYERS);
    hyperspace_flag = 0;
    player_text_timer = 0;
    fast_timer = 0;
    slow_timer = 0;
    ship_direction = 0;
    ship_vel_x_minor = 0;
    ship_vel_y_minor = 0;
    credits = 0;
    option_switch = settings.option_switch;
    if ((option_switch & 0x20) == 0)
        starting_lives = 4;
    else
        starting_lives = 3;

    pre_credit_coins = 0;
    MemSetU8(ship_explosion_x, 0, sizeof(ship_explosion_x));
    MemSetU8(ship_explosion_y, 0, sizeof(ship_explosion_x));

    U64 i;
    for (i = 0; i < MAX_PLAYERS; i++)
    {
        U64 x;
        for (x = 0; x < MAX_ASTEROIDS; x++)
            player[i].asteroid[x].status = 0;

        player[i].ship.status = 0;
        player[i].saucer.status = 0;

        for (x = 0; x < MAX_SAUCER_PHOTONS; x++)
            player[i].saucer_photon[x].status = 0;

        for (x = 0; x < MAX_SHIP_PHOTONS; x++)
            player[i].ship_photon[x].status = 0;
    }
    player[0].ship.pos.x_major = 0;
    player[0].ship.pos.y_major = 0;
    player[0].ship.pos.x_minor = 0;
    player[0].ship.pos.y_minor = 0;
    player[0].asteroids_per_wave = 2;
    player[0].asteroid_count = 0;
    player[0].saucer_spawn_and_shot_time = 0;
    player[0].saucer_spawn_time_start = 0;
    player[0].block_saucer_spawn_time = 0;
    player[1].block_saucer_spawn_time = 0;
    player[0].asteroid_wave_spawn_time = 0;
    player[0].asteroids_to_slow_saucer_spawn = 0;
}

U0 init_game(CDoc* std_out)
{
    init_settings_handler(&settings, std_out);
    set_up_startup_window(&settings);
    output_settings(&settings, std_out);

    Vector_Generator vector_generator;
    init_vector_generator(&vector_generator, &settings);

    initialize_memory();

    // main loop
    Bool running = TRUE;
    while (running)
    {
        I64 start_time;
        if (settings.frame_limiter_mode == SLEEPING)
            start_time = cnts.jiffies;
        else
            start_time = SysTimerRead();

        DCFill();
        update_input(&input, &settings);
        if (on_press(&input, UPDATE_WIN_SIZE))
            set_resolution_scale(&vector_generator);
        else if (on_press(&input, EXIT))
            running = FALSE;

        // do game stuff
        coin_input_to_credit();
        if (player_count == 0)
        {
            draw_player_score(0, player_score, &vector_generator, dc);
            draw_player_score(1, player_score, &vector_generator, dc);

            if ((option_switch & 0x03) == 0)
                credits = 2;

            if (player_HS_place[0] < 128 || player_HS_place[1] < 128)
                handle_HS_entry(&vector_generator);
            else
                attract_mode(&vector_generator);

            attempt_game_start();
        }
        else if (player_count == 1)
        {
            update_player(&vector_generator);
            draw_player_score(0, player_score, &vector_generator, dc);
        }
        else // two-player mode
        {
            update_player(&vector_generator);
            draw_multiplayer_scores(&vector_generator);

            // draw player 2 lives
            set_position_and_size(207, 213, DIV_4, &vector_generator, dc);
            U64 i;
            for (i = 0; i < player_lives[1] && i < 13; i++)
                process(&vector_generator, LIVES_REMAINING_SHIP, dc);
        }
        set_position_and_size(120, 219, MUL_1, &vector_generator, dc);
        draw_number(high_score_table[0].points, 2, &vector_generator, dc, TRUE);
        draw_copyright(&vector_generator);

        if (fast_timer == 255)
            slow_timer++;

        fast_timer++;

        limit_FPS(&settings, start_time);
    }
}

#endif
