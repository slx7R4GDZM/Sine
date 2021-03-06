// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Game.h"

#include <thread>
#include <SFML/Graphics.hpp>
#include "../Graphics/Graphics-Handler.h"
#include "../Graphics/Vector-Generator.h"
#include "../Other/Vectors.h"

Game::Game()
    : game_activity(RUN_WITH_INPUT)
    , current_player(0)
    , last_game_player_count(0)
    , player_count(0)
    , high_score_table{}
    , name_entry_letter_pos(0)
    , player_HS_place{176, 176}
    , names_HS{}
    , player_score{}
    , player_lives{}
    , hyperspace_flag(0)
    , player_text_timer(0)
    , fast_timer(0)
    , slow_timer(0)
    , ship_direction(0)
    , ship_vel_x_minor(0)
    , ship_vel_y_minor(0)
    , credits(0)
    , pre_credit_coins(0)
    , ship_explosion_x{}
    , ship_explosion_y{}
{
    // window
    settings.create_startup_window(window);
    settings.output_settings();
    Vector_Generator vector_generator(settings);

    // settings
    option_switch = settings.get_option_switch();
    if (option_switch.starting_lives == 0)
        starting_lives = 4;
    else
        starting_lives = 3;

    // startup
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

    // main loop
    while (window.isOpen())
    {
        const steady_clock::time_point start_time = steady_clock::now();
        process_events(vector_generator, window);
        if (game_activity != PAUSE)
        {
            window.clear();

            if (game_activity == RUN_WITH_INPUT)
            {
                input.update(settings);
                if (input.on_press(TOGGLE_FULLSCREEN))
                {
                    settings.toggle_fullscreen(window);
                    vector_generator.set_resolution_scale(settings);
                }
                else if (input.on_press(EXIT))
                    window.close();
            }

            // do game stuff
            coin_input_to_credit();
            if (player_count == 0)
            {
                draw_player_score(0, player_score, vector_generator, window);
                draw_player_score(1, player_score, vector_generator, window);

                if (option_switch.coinage == 0)
                    credits = 2;

                if (player_HS_place[0] < 128 || player_HS_place[1] < 128)
                    handle_HS_entry(vector_generator);
                else
                    attract_mode(vector_generator);

                attempt_game_start();
            }
            else if (player_count == 1)
            {
                update_player(vector_generator);
                draw_player_score(0, player_score, vector_generator, window);
            }
            else // two-player mode
            {
                update_player(vector_generator);
                draw_multiplayer_scores(vector_generator);

                // draw player 2 lives
                set_position_and_size(207, 213, DIV_4, vector_generator, window);
                for (u8 i = 0; i < player_lives[1] && i < 13; i++)
                    vector_generator.process(LIVES_REMAINING_SHIP, window);
            }
            set_position_and_size(120, 219, MUL_1, vector_generator, window);
            draw_number(high_score_table[0].points, 2, vector_generator, window, true);
            draw_copyright(vector_generator);

            if (fast_timer == 255)
                slow_timer++;

            fast_timer++;

            window.display();
        }
        limit_FPS(start_time);
    }
}

void Game::process_events(Vector_Generator& vector_generator, RenderWindow& window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Resized)
        {
            settings.resize_window({event.size.width, event.size.height}, window);
            vector_generator.set_resolution_scale(settings);
        }
        else if (event.type == sf::Event::Closed)
            window.close();
        else if (settings.get_inactive_mode() == PAUSE)
        {
            if (event.type == sf::Event::LostFocus)
                game_activity = PAUSE;
            else if (event.type == sf::Event::GainedFocus)
                game_activity = RUN_WITH_INPUT;
        }
        else if (settings.get_inactive_mode() == RUN_WITHOUT_INPUT)
        {
            if (event.type == sf::Event::LostFocus)
            {
                game_activity = RUN_WITHOUT_INPUT;
                input.release_all_buttons();
            }
            else if (event.type == sf::Event::GainedFocus)
                game_activity = RUN_WITH_INPUT;
        }
    }
}

void Game::limit_FPS(steady_clock::time_point start_time) const
{
    if (settings.get_frame_limiter_mode() == SLEEPING)
    {
        const nanoseconds time_elapsed = steady_clock::now() - start_time;
        if (time_elapsed < TARGET_FRAME_TIME)
            std::this_thread::sleep_for(TARGET_FRAME_TIME - time_elapsed);
    }
    else
        while (steady_clock::now() - start_time < TARGET_FRAME_TIME);
}

void Game::draw_multiplayer_scores(Vector_Generator& vector_generator)
{
    // while waiting to spawn make the score of the current player flash
    // and otherwise brighten the score of the current player
    if (!hyperspace_flag && player[current_player].ship.status == INDISCERNIBLE)
    {
        if (current_player == 0)
        {
            if (fast_timer & 16)
                draw_player_score(0, player_score, vector_generator, window, true);

            draw_player_score(1, player_score, vector_generator, window);
        }
        else
        {
            if (fast_timer & 16)
                draw_player_score(1, player_score, vector_generator, window, true);

            draw_player_score(0, player_score, vector_generator, window);
        }
    }
    else
    {
        if (current_player == 0)
        {
            draw_player_score(0, player_score, vector_generator, window, true);
            draw_player_score(1, player_score, vector_generator, window);
        }
        else
        {
            draw_player_score(0, player_score, vector_generator, window);
            draw_player_score(1, player_score, vector_generator, window, true);
        }
    }
}

void Game::draw_copyright(Vector_Generator& vector_generator)
{
    vector_generator.process(COPYRIGHT_SYMBOL, window);

    // 2016_
    draw_character(3, vector_generator, window);
    draw_character(1, vector_generator, window);
    draw_character(2, vector_generator, window);
    draw_character(7, vector_generator, window);
    draw_character(0, vector_generator, window);

    // slx7R4GDZM
    draw_character(29, vector_generator, window);
    draw_character(22, vector_generator, window);
    draw_character(34, vector_generator, window);
    draw_character( 8, vector_generator, window);
    draw_character(28, vector_generator, window);
    draw_character( 5, vector_generator, window);
    draw_character(17, vector_generator, window);
    draw_character(14, vector_generator, window);
    draw_character(36, vector_generator, window);
    draw_character(23, vector_generator, window);
}

void Game::coin_input_to_credit()
{
    // input to pre-credits
    if (input.on_press(LEFT_COIN))
        pre_credit_coins++;

    if (input.on_press(CENTER_COIN))
    {
        if (option_switch.center_coin_multiplier == 0)
            pre_credit_coins++;
        else
            pre_credit_coins += 2;
    }
    if (input.on_press(RIGHT_COIN))
    {
        switch (option_switch.right_coin_multiplier)
        {
        case 0:
            pre_credit_coins++;
            break;
        case 1:
            pre_credit_coins += 4;
            break;
        case 2:
            pre_credit_coins += 5;
            break;
        case 3:
            pre_credit_coins += 6;
            break;
        }
    }

    // pre-credits to credits
    if (option_switch.coinage == 0)
        pre_credit_coins = 0;
    else if (option_switch.coinage == 3)
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

void Game::add_credit()
{
    switch (option_switch.coinage)
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

void Game::attempt_game_start()
{
    if (input.on_press(ONE_PLAYER_START))
    {
        if (credits >= 1)
        {
            credits--;
            player_count = 1;
            handle_game_start();
        }
    }
    else if (input.on_press(TWO_PLAYER_START))
    {
        if (credits >= 2)
        {
            credits -= 2;
            player_count = 2;
            handle_game_start();
        }
    }
}

void Game::handle_game_start()
{
    current_player = 0;
    // names_HS[MAX_HS_COUNT * HS_NAME_LENGTH - 1] = 0; // bug that sets the final initial of the 10th high score to space
    player_text_timer = 128;
    for (u8 i = 0; i < player_count; i++)
    {
        player_HS_place[i] = 255;
        player_score[i] = NO_SCORE;
        player_lives[i] = starting_lives;
        clear_space_objects(player[i]);
        player[i].ship.spawn();
        player[i].asteroids_per_wave = 2;
        player[i].asteroid_count = 0;
        player[i].saucer_spawn_and_shot_time = 146;
        player[i].saucer_spawn_time_start = 146;
        player[i].ship_spawn_timer = 1;
        player[i].asteroid_wave_spawn_time = 127;
        player[i].asteroids_to_slow_saucer_spawn = 5;
    }
}

void Game::end_game()
{
    current_player = player_count - 1;
    last_game_player_count = player_count;
    player_count = 0;
    name_entry_letter_pos = 0;

    // for each player compare their score to the hs table and get their hs placement
    for (u8 p = 0; p < MAX_PLAYERS; p++)
    {
        for (u8 s = 0; s < MAX_HS_COUNT; s++)
        {
            if (player_score[p] > high_score_table[s])
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
            if (player_score[0] > player_score[1])
                player_HS_place[1] += HS_NAME_LENGTH;
            else
                player_HS_place[0] += HS_NAME_LENGTH;
        }
        insert_any_new_high_scores();
    }
    else
        slow_timer = 255;
}

void Game::attract_mode(Vector_Generator& vector_generator)
{
    if (option_switch.coinage != 0)
    {
        set_position_and_size(80, 57, MUL_2, vector_generator, window);
        if (option_switch.coinage == 1)
            draw_text(ONE_COIN_2_PLAYS, option_switch.language, vector_generator, window);
        else if (option_switch.coinage == 2)
            draw_text(ONE_COIN_1_PLAY, option_switch.language, vector_generator, window);
        else
            draw_text(TWO_COINS_1_PLAY, option_switch.language, vector_generator, window);
    }

    // flash "push start" after enough credits
    if (credits >= 1 && (fast_timer & 32) == 0)
    {
        set_position_and_size(100, 198, MUL_2, vector_generator, window);
        draw_text(PUSH_START, option_switch.language, vector_generator, window);
    }

    if (high_score_table[0] > NO_SCORE && (slow_timer & 4) == 0)
    {
        set_position_and_size(100, 182, MUL_2, vector_generator, window);
        draw_text(HIGH_SCORES, option_switch.language, vector_generator, window);

        // draw highscore table
        u8 score_position[1] = {0};
        for (u8 i = 0; i < MAX_HS_COUNT && high_score_table[i] > NO_SCORE; i++)
        {
            // draw score position
            add_to_number(score_position, 1, 1, false);
            set_position_and_size(95, 167 - i * 8, MUL_2, vector_generator, window);
            draw_number(score_position, 1, vector_generator, window);
            vector_generator.process(DOT, window);
            draw_character(0, vector_generator, window);

            // draw score
            draw_number(high_score_table[i].points, 2, vector_generator, window, true);
            draw_character(0, vector_generator, window);

            // draw score initials
            for (u8 c = 0; c < HS_NAME_LENGTH; c++)
                draw_character(names_HS[i * HS_NAME_LENGTH + c], vector_generator, window);
        }
    }
    else
    {
        // show autodemo
        handle_saucer_stuff(player[0]);
        attempt_asteroid_wave_spawn(player[0]);
        update_space_objects(player[0], vector_generator);
    }
}

void Game::update_player(Vector_Generator& vector_generator)
{
    if (player_text_timer)
    {
        set_position_and_size(100, 182, MUL_2, vector_generator, window);
        draw_text(PLAYER_, option_switch.language, vector_generator, window);
        draw_digit(current_player + 1, vector_generator, window);
        player_text_timer--;
    }
    else if (!player_lives[current_player] && !Photon::any(player[current_player].ship_photon, MAX_SHIP_PHOTONS))
    {
        set_position_and_size(100, 157, MUL_2, vector_generator, window);
        draw_text(GAME_OVER, option_switch.language, vector_generator, window);
        if (player_count == 2)
        {
            set_position_and_size(100, 182, MUL_2, vector_generator, window);
            draw_text(PLAYER_, option_switch.language, vector_generator, window);
            draw_digit(current_player + 1, vector_generator, window);
        }
    }
    if (!player_text_timer)
        handle_ship_stuff(player[current_player]);

    attempt_asteroid_wave_spawn(player[current_player]);
    update_space_objects(player[current_player], vector_generator);

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
    set_position_and_size(40, 213, DIV_4, vector_generator, window);
    for (u8 i = 0; i < player_lives[0] && i < 55; i++)
        vector_generator.process(LIVES_REMAINING_SHIP, window);
}

void Game::add_points(u8 points, bool bonus)
{
    const u8 old_ten_thousandth = player_score[current_player].points[1] & 0xF0;
    add_to_number(player_score[current_player].points, 2, points, bonus);

    const u8 new_ten_thousandth = player_score[current_player].points[1] & 0xF0;
    if (new_ten_thousandth != old_ten_thousandth)
        player_lives[current_player]++;
}

void Game::insert_any_new_high_scores()
{
    // move the old scores and initals down and insert the new high scores
    for (u8 p = 0; p < MAX_PLAYERS; p++)
    {
        if (player_HS_place[p] == 255)
            continue;

        for (u8 s = 0; s < MAX_HS_COUNT; s++)
        {
            if (player_score[p] <= high_score_table[s])
                continue;

            // move the high score list down by one from where the new score is added
            for (u8 i = MAX_HS_COUNT - 1; i > s; i--)
            {
                high_score_table[i] = high_score_table[i - 1];

                // also move high score initials list down one
                for (u8 c = 0; c < HS_NAME_LENGTH; c++)
                    names_HS[i * HS_NAME_LENGTH + c] = names_HS[(i - 1) * HS_NAME_LENGTH + c];
            }
            // insert new score
            high_score_table[s] = player_score[p];

            // set initials to "A  "
            names_HS[s * HS_NAME_LENGTH] = 11;
            names_HS[s * HS_NAME_LENGTH + 1] = 0;
            names_HS[s * HS_NAME_LENGTH + 2] = 0;

            break;
        }
    }
}

void Game::handle_HS_entry(Vector_Generator& vector_generator)
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

void Game::draw_HS_entry_screen(Vector_Generator& vector_generator)
{
    // draw high score entry control help
    if (last_game_player_count > 1)
    {
        set_position_and_size(100, 182, MUL_2, vector_generator, window);
        draw_text(PLAYER_, option_switch.language, vector_generator, window);
        if (fast_timer & 16)
            draw_digit(current_player + 1, vector_generator, window);
    }
    set_position_and_size(12, 170, MUL_2, vector_generator, window);
    draw_text(YOUR_SCORE_IS_ONE_OF_THE_TEN_BEST,      option_switch.language, vector_generator, window);
    set_position_and_size(12, 162, MUL_2, vector_generator, window);
    draw_text(PEASE_ENTER_YOUR_INITIALS,              option_switch.language, vector_generator, window);
    set_position_and_size(12, 154, MUL_2, vector_generator, window);
    draw_text(PUSH_ROTATE_TO_SELECT_LETTER,           option_switch.language, vector_generator, window);
    set_position_and_size(12, 146, MUL_2, vector_generator, window);
    draw_text(PUSH_HYPERSPACE_WHEN_LETTER_IS_CORRECT, option_switch.language, vector_generator, window);

    // draw name entry
    set_position_and_size(100, 57, MUL_4, vector_generator, window);
    for (u8 c = 0; c < HS_NAME_LENGTH; c++)
    {
        if (names_HS[player_HS_place[current_player] + c] == 0)
            vector_generator.process(UNDERSCORE, window);
        else
            draw_character(names_HS[player_HS_place[current_player] + c], vector_generator, window);
    }
}

void Game::handle_HS_entry_input()
{
    if (input.on_press(HYPERSPACE))
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
        if (input.is_pressed(ROTATE_LEFT))
        {
            if (names_HS[player_HS_place[current_player] + name_entry_letter_pos] == 0)
                names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 11;
            else if (names_HS[player_HS_place[current_player] + name_entry_letter_pos] == 36)
                names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 0;
            else
                names_HS[player_HS_place[current_player] + name_entry_letter_pos]++;
        }
        else if (input.is_pressed(ROTATE_RIGHT))
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

void Game::update_space_objects(Player& player, Vector_Generator& vector_generator)
{
    for (u8 i = 0; i < MAX_ASTEROIDS; i++)
        player.asteroid[i].update(player.asteroid_count, player.asteroid_wave_spawn_time, vector_generator, window);

    player.ship.update(fast_timer, ship_direction, ship_explosion_x, ship_explosion_y, input.is_pressed(THRUST), vector_generator, window);
    player.saucer.update(fast_timer, player.saucer_spawn_and_shot_time, player.saucer_spawn_time_start, vector_generator, window);

    for (u8 i = 0; i < MAX_SAUCER_PHOTONS; i++)
        player.saucer_photon[i].update(fast_timer, vector_generator, window);

    for (u8 i = 0; i < MAX_SHIP_PHOTONS; i++)
        player.ship_photon[i].update(fast_timer, vector_generator, window);

    handle_collision(player);
}

void Game::handle_collision(Player& player)
{
    // ship
    for (u8 i = 0; player.ship.status == ALIVE && i < MAX_ASTEROIDS; i++)
    {
        if (player.ship.collide(player.asteroid[i], SHIP_HITBOX + player.asteroid[i].get_size()))
        {
            add_points(player.asteroid[i].get_points(), false);
            crash_asteroid(player, i);
            player.ship.crash(player_lives[current_player], player.ship_spawn_timer);
        }
    }

    // saucer
    for (u8 i = 0; (player.saucer.status == LARGE_SAUCER || player.saucer.status == SMALL_SAUCER) && i < MAX_ASTEROIDS; i++)
    {
        if (player.saucer.collide(player.asteroid[i], player.saucer.get_size(false) + player.asteroid[i].get_size()))
        {
            crash_asteroid(player, i);
            player.saucer.crash(player.saucer_spawn_and_shot_time, player.saucer_spawn_time_start);
        }
    }
    if (player.saucer.collide(player.ship, player.saucer.get_size(false) + SMALL_ASTEROID_HITBOX))
    {
        add_points(player.saucer.get_points(), player.saucer.status == SMALL_SAUCER);
        player.ship.crash(player_lives[current_player], player.ship_spawn_timer);
        player.saucer.crash(player.saucer_spawn_and_shot_time, player.saucer_spawn_time_start);
    }

    // saucer photon
    for (u8 i = 0; i < MAX_SAUCER_PHOTONS; i++)
    {
        bool crashed = false;
        for (u8 x = 0; x < MAX_ASTEROIDS && !crashed; x++)
        {
            if (player.saucer_photon[i].collide(player.asteroid[x], player.asteroid[x].get_size()))
            {
                crash_asteroid(player, x);
                player.saucer_photon[i].status = INDISCERNIBLE;
                crashed = true;
            }
        }
        if (player.saucer_photon[i].collide(player.ship, SMALL_ASTEROID_HITBOX))
        {
            player.ship.crash(player_lives[current_player], player.ship_spawn_timer);
            player.saucer_photon[i].status = INDISCERNIBLE;
        }
    }

    // ship photon
    for (u8 i = 0; i < MAX_SHIP_PHOTONS; i++)
    {
        bool crashed = false;
        for (u8 x = 0; x < MAX_ASTEROIDS && !crashed; x++)
        {
            if (player.ship_photon[i].collide(player.asteroid[x], player.asteroid[x].get_size()))
            {
                add_points(player.asteroid[x].get_points(), false);
                crash_asteroid(player, x);
                player.ship_photon[i].status = INDISCERNIBLE;
                crashed = true;
            }
        }
        if (player.ship_photon[i].collide(player.ship, SMALL_ASTEROID_HITBOX))
        {
            player.ship.crash(player_lives[current_player], player.ship_spawn_timer);
            player.ship_photon[i].status = INDISCERNIBLE;
        }
        if (player.ship_photon[i].collide(player.saucer, player.saucer.get_size(true)))
        {
            add_points(player.saucer.get_points(), player.saucer.status == SMALL_SAUCER);
            player.saucer.crash(player.saucer_spawn_and_shot_time, player.saucer_spawn_time_start);
            player.ship_photon[i].status = INDISCERNIBLE;
        }
    }
}

void Game::clear_space_objects(Player& player)
{
    for (u8 i = 0; i < MAX_ASTEROIDS; i++)
        player.asteroid[i].status = INDISCERNIBLE;

    player.ship.status = INDISCERNIBLE;
    player.saucer.status = INDISCERNIBLE;

    for (u8 i = 0; i < MAX_SAUCER_PHOTONS; i++)
        player.saucer_photon[i].status = INDISCERNIBLE;

    for (u8 i = 0; i < MAX_SHIP_PHOTONS; i++)
        player.ship_photon[i].status = INDISCERNIBLE;
}

void Game::attempt_asteroid_wave_spawn(Player& player)
{
    if (player.asteroid_wave_spawn_time)
        player.asteroid_wave_spawn_time--;

    if (player.asteroid_count == 0 && player.saucer.status == INDISCERNIBLE && !player.asteroid_wave_spawn_time)
    {
        if (player.asteroids_per_wave < 10)
            player.asteroids_per_wave += 2;
        else if (player.asteroids_per_wave == 10)
            player.asteroids_per_wave++;

        player.saucer_spawn_and_shot_time = 127;
        if (player.asteroids_to_slow_saucer_spawn < 10)
            player.asteroids_to_slow_saucer_spawn++;

        for (u8 i = 0; i < player.asteroids_per_wave; i++)
        {
            player.asteroid[i].spawn_wave_asteroid();
            player.asteroid_count++;
        }
    }
}

void Game::crash_asteroid(Player& player, u8 crashed_ast)
{
    const u8 ast_size = player.asteroid[crashed_ast].status & ASTEROID_SIZE;
    player.asteroid[crashed_ast].status = EXPLOSION_START;
    player.block_saucer_spawn_time = 80;
    if (ast_size == SMALL_ASTEROID)
        return;

    u8 new_ast;
    if (!spawn_asteroid(player, crashed_ast, ast_size, new_ast))
        return;

    Asteroid::offset_position(player.asteroid[new_ast].pos.x_minor,
                              player.asteroid[new_ast].vel_x_major);

    if (!spawn_asteroid(player, crashed_ast, ast_size, new_ast))
        return;

    Asteroid::offset_position(player.asteroid[new_ast].pos.y_minor,
                              player.asteroid[new_ast].vel_y_major);
}

bool Game::spawn_asteroid(Player& player, u8 crashed_ast, u8 ast_size, u8& new_ast)
{
    for (u8 i = 0; i < MAX_ASTEROIDS && player.asteroid_count < MAX_ASTEROIDS; i++)
    {
        if (player.asteroid[i].status == INDISCERNIBLE)
        {
            new_ast = i;
            player.asteroid[i].spawn_split_asteroid(ast_size,
                                                    player.asteroid[crashed_ast].vel_x_major,
                                                    player.asteroid[crashed_ast].vel_y_major,
                                                    player.asteroid[crashed_ast].pos);
            player.asteroid_count++;
            return true;
        }
    }
    return false;
}

void Game::handle_ship_stuff(Player& player)
{
    // on_press should be handled slightly differently, also need a limit on the photon shooting speed?
    if (input.on_press(FIRE) && !player_text_timer && !player.ship_spawn_timer)
        Photon::fire_photon(player.ship_photon, MAX_SHIP_PHOTONS, ship_direction, player.ship);
    if (input.is_pressed(HYPERSPACE) && !player_text_timer && !player.ship_spawn_timer)
    {
        player.ship.status = INDISCERNIBLE;
        player.ship.vel_x_major = 0;
        player.ship.vel_y_major = 0;
        player.ship_spawn_timer = 48;

        player.ship.pos.x_major = Space_Object::limit_position(random_byte() & 31, 28);
        hyperspace_flag = 1;
        u8 pos_y_major = random_byte() & 31;
        if (pos_y_major >= 24)
        {
            pos_y_major = (pos_y_major & 7) * 2 + 4;
            if (pos_y_major >= player.asteroid_count)
                hyperspace_flag = 128;
        }
        player.ship.pos.y_major = Space_Object::limit_position(pos_y_major, 20);
    }
    if (player.ship_spawn_timer > 0 && player.ship.status < TRUE_EXPLOSION_START)
    {
        if (player.ship_spawn_timer > 1)
            player.ship_spawn_timer--;
        else if (player.ship_spawn_timer == 1)
        {
            // ship trying to spawn
            if (hyperspace_flag == 0)
            {
                if (!Asteroid::blocking_spawn(player.asteroid))
                {
                    player.ship.status = ALIVE;
                    player.ship_spawn_timer = 0;
                }
            }
            // hyperspace re-entry
            else if (hyperspace_flag < 128)
            {
                player.ship.status = ALIVE;
                player.ship_spawn_timer = 0;
                hyperspace_flag = 0;
            }
            // hyperspace failure
            else
            {
                player.ship.crash(player_lives[current_player], player.ship_spawn_timer);
                hyperspace_flag = 0;
            }
        }
    }
    if (input.is_pressed(ROTATE_LEFT))
    {
        if (!player.ship_spawn_timer)
            ship_direction += 3;
    }
    else if (input.is_pressed(ROTATE_RIGHT))
    {
        if (!player.ship_spawn_timer)
            ship_direction -= 3;
    }
    if ((fast_timer & 1) == 0 && !player.ship_spawn_timer)
    {
        if (input.is_pressed(THRUST))
            player.ship.add_thrust(ship_direction, ship_vel_x_minor, ship_vel_y_minor);
        else
            player.ship.dampen_velocity(ship_vel_x_minor, ship_vel_y_minor);
    }
    if (player.ship.status && player.ship.status < TRUE_EXPLOSION_START)
        handle_saucer_stuff(player);
}

void Game::handle_saucer_stuff(Player& player)
{
    if (fast_timer & 3)
        return;

    player.saucer_spawn_and_shot_time--;
    if (player.saucer.status == INDISCERNIBLE)
    {
        if (player.block_saucer_spawn_time)
            player.block_saucer_spawn_time--;

        if (player.saucer_spawn_and_shot_time == 0)
        {
            player.saucer_spawn_and_shot_time = 18;
            if (player.asteroid_count == 0)
                return;

            if (player.block_saucer_spawn_time
             && player.asteroid_count >= player.asteroids_to_slow_saucer_spawn)
                return;

            const u8 new_saucer_spawn_time_start = player.saucer_spawn_time_start - 6;
            if (new_saucer_spawn_time_start >= MINIMUM_SAUCER_SPAWN_TIME)
                player.saucer_spawn_time_start = new_saucer_spawn_time_start;

            player.saucer.spawn(player_score[current_player], player.saucer_spawn_time_start);
        }
    }
    else if (player.saucer_spawn_and_shot_time == 0)
    {
        if (player.saucer.status == LARGE_SAUCER)
            saucer_direction = random_byte();
        else
        {
            saucer_direction = player.saucer.targeted_shot(player.ship.pos);
            const bool accurate_shot = player_score[current_player].points[1] >= 0x35;
            saucer_direction += Saucer::shot_offset(accurate_shot);
        }
        Photon::fire_photon(player.saucer_photon, MAX_SAUCER_PHOTONS, saucer_direction, player.saucer);
        player.saucer_spawn_and_shot_time = 10;
    }
}
