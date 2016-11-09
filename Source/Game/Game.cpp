// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#include "Game.h"

#include "../Graphics/Graphics_Handler.h"
#include "../Other/Vectors.h"
#include "../Settings/Settings_Handler.h"
#include <thread>

const u8 EXPLOSION_START = 160;
const u8 INDISCERNIBLE = 0;
const u8 PHOTON_SIZE = 43;
const u8 BONUS_SIZE_1 = 71;
const u16 MAX_SCORE = 10000;
const u8 MAX_ASTEROID_COUNT = 27;
const u8 MAX_SHIP_PHOTON_COUNT = 4;
const nanoseconds MAX_FRAME_TIME = 16666667ns;

enum Buttons : u8
{
    HYPERSPACE,
    FIRE,
    LEFT_COIN,
    CENTER_COIN,
    RIGHT_COIN,
    ONE_PLAYER_START,
    TWO_PLAYER_START,
    THRUST,
    ROTATE_RIGHT,
    ROTATE_LEFT,
    EXIT
};

enum Text : u8
{
    HIGH_SCORES,
    PLAYER_,
    YOUR_SCORE_IS_ONE_OF_THE_TEN_BEST,
    PEASE_ENTER_YOUR_INITIALS,
    PUSH_ROTATE_TO_SELECT_LETTER,
    PUSH_HYPERSPACE_WHEN_LETTER_IS_CORRECT,
    PUSH_START,
    GAME_OVER,
    ONE_COIN_2_PLAYS,
    ONE_COIN_1_PLAY,
    TWO_COINS_1_PLAY
};

Game::Game()
    : game_activity(RUN_WITH_INPUT)
    , draw_thrust_graphic(false) // hacky, don't use this var
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
{
    // window
    settings.apply_window_settings(window);
    settings.output_settings();
    Vector_Generator vector_generator(settings);

    // settings
    option_switch = settings.get_option_switch();
    if (option_switch.starting_lives == 0)
        starting_lives = 4;
    else
        starting_lives = 3;

    // startup
    player[0].asteroids_per_wave = 2;
    player[0].saucer_spawn_and_shot_time = 127; // maybe don't do this because it's done in asteroid spawn?
    player[0].saucer_spawn_time_start = 0;
    player[0].asteroid_wave_spawn_time = 0;

    // main loop
    while (window.isOpen())
    {
        high_resolution_clock::time_point start_time = high_resolution_clock::now();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
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
                    game_activity = RUN_WITHOUT_INPUT;
                else if (event.type == sf::Event::GainedFocus)
                    game_activity = RUN_WITH_INPUT;
            }
        }
        if (game_activity != PAUSE)
        {
            window.clear();

            if (game_activity == RUN_WITH_INPUT)
            {
                input.update(settings);
                if (input.on_press(EXIT))
                    window.close();
            }

            // do game stuff
            coin_input_to_credit();
            if (player_count == 0)
            {
                Graphics_Handler::draw_score(0, player_score[0], vector_generator, window);
                Graphics_Handler::draw_score(1, player_score[1], vector_generator, window);

                if (player_HS_place[0] < 128 || player_HS_place[1] < 128)
                    handle_HS_entry(vector_generator);
                else
                    attract_mode(vector_generator);

                attempt_game_start();
            }
            else if (player_count == 1)
            {
                update_player(vector_generator);
                Graphics_Handler::draw_score(0, player_score[0], vector_generator, window);
            }
            else // two-player mode
            {
                update_player(vector_generator);
                draw_multiplayer_scores(vector_generator);

                // draw player 2 lives
                vector_generator.load_absolute(207, 43, -2);
                for (u8 i = 0; i < player_lives[1] && i < 13; i++)
                    vector_generator.process(LIVES_REMAINING_SHIP, window);
            }
            Graphics_Handler::draw_score(high_score_table[0], 132, 37, 0, vector_generator, window);
            draw_copyright(vector_generator);

            if (fast_timer == 255)
                slow_timer++;
            fast_timer++;
        }
        window.display();

        if (settings.get_frame_limiter_mode() == SLEEPING)
        {
            nanoseconds frame_time = high_resolution_clock::now() - start_time;
            if (frame_time < MAX_FRAME_TIME)
                std::this_thread::sleep_for(MAX_FRAME_TIME - frame_time);
        }
        else
            while (high_resolution_clock::now() - start_time < MAX_FRAME_TIME);
    }
}

void Game::draw_multiplayer_scores(Vector_Generator& vector_generator)
{
    // make score flash and brighten score for current player
    if (player[current_player].ship.empty())
    {
        if (current_player == 0)
        {
            if (fast_timer % 32 >= 16)
                Graphics_Handler::draw_score(0, player_score[0], vector_generator, window, true);
            Graphics_Handler::draw_score(1, player_score[1], vector_generator, window);
        }
        else
        {
            if (fast_timer % 32 >= 16)
                Graphics_Handler::draw_score(1, player_score[1], vector_generator, window, true);
            Graphics_Handler::draw_score(0, player_score[0], vector_generator, window);
        }
    }
    else
    {
        if (current_player == 0)
        {
            Graphics_Handler::draw_score(0, player_score[0], vector_generator, window, true);
            Graphics_Handler::draw_score(1, player_score[1], vector_generator, window);
        }
        else
        {
            Graphics_Handler::draw_score(0, player_score[0], vector_generator, window);
            Graphics_Handler::draw_score(1, player_score[1], vector_generator, window, true);
        }
    }
}

void Game::draw_copyright(Vector_Generator& vector_generator)
{
    vector_generator.load_absolute(100, 224, 0);
    vector_generator.process(COPYRIGHT_SYMBOL, window);

    // 2016_
    Graphics_Handler::draw_character(3, vector_generator, window);
    Graphics_Handler::draw_character(1, vector_generator, window);
    Graphics_Handler::draw_character(2, vector_generator, window);
    Graphics_Handler::draw_character(7, vector_generator, window);
    Graphics_Handler::draw_character(0, vector_generator, window);

    // slx7R4GDZM
    Graphics_Handler::draw_character(29, vector_generator, window);
    Graphics_Handler::draw_character(22, vector_generator, window);
    Graphics_Handler::draw_character(34, vector_generator, window);
    Graphics_Handler::draw_character(8, vector_generator, window);
    Graphics_Handler::draw_character(28, vector_generator, window);
    Graphics_Handler::draw_character(5, vector_generator, window);
    Graphics_Handler::draw_character(17, vector_generator, window);
    Graphics_Handler::draw_character(14, vector_generator, window);
    Graphics_Handler::draw_character(36, vector_generator, window);
    Graphics_Handler::draw_character(23, vector_generator, window);
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
    clear_space_objects(player[0]);
    clear_space_objects(player[1]); // maybe? untested
    current_player = 0; // maybe? untested
    player_score[0] = 0;
    player_score[1] = 0;
    player_HS_place[0] = 255;
    player_HS_place[1] = 255;
    player_text_timer = 128;
    for (u8 i = 0; i < player_count; i++)
    {
        player_lives[i] = starting_lives;
        player[i].saucer_spawn_and_shot_time = 146;
        player[i].saucer_spawn_time_start = player[i].saucer_spawn_and_shot_time;
        player[i].asteroids_per_wave = 2;
        player[i].ship_spawn_timer = 1;
    }
    player[0].asteroid_wave_spawn_time = 127;
    if (last_game_player_count == 1)
        player[1].asteroid_wave_spawn_time = 127;
    else
        player[1].asteroid_wave_spawn_time = 0;
}

void Game::end_game()
{
    current_player = player_count - 1;
    last_game_player_count = player_count;
    player_count = 0;
    name_entry_letter_pos = 0;

    // for each player compare their score to the hs table and get their hs placement
    for (u8 p = 0; p < MAX_PLAYER_COUNT; p++)
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

    // deal with players having the same score
    if (player_HS_place[0] != 255 && player_HS_place[1] != 255)
    {
        if (player_score[0] > player_score[1])
            player_HS_place[1] += HS_NAME_LENGTH;
        else
            player_HS_place[0] += HS_NAME_LENGTH;
    }

    // move the old scores and initals down and insert the new high scores
    for (u8 p = 0; p < MAX_PLAYER_COUNT; p++)
    {
        if (player_HS_place[p] != 255)
        {
            for (u8 s = 0; s < MAX_HS_COUNT; s++)
            {
                if (player_score[p] > high_score_table[s])
                {
                    // move the high score list down by one from where the new score is added
                    for (u8 i = MAX_HS_COUNT - 1; i > s; i--)
                    {
                        high_score_table[i] = high_score_table[i - 1];

                        // also move high score initials list down one
                        for (u8 c = 0; c < HS_NAME_LENGTH; c++)
                            names_HS[i * HS_NAME_LENGTH + c] = names_HS[(i - 1) * HS_NAME_LENGTH + c];
                    }
                    high_score_table[s] = player_score[p];

                    // set initials to "A  "
                    names_HS[s * HS_NAME_LENGTH] = 11;
                    names_HS[s * HS_NAME_LENGTH + 1] = 0;
                    names_HS[s * HS_NAME_LENGTH + 2] = 0;

                    s = MAX_HS_COUNT; // cheeky loop escape
                }
            }
        }
    }
}

void Game::attract_mode(Vector_Generator& vector_generator)
{
    if (option_switch.coinage == 0)
        credits = 2;
    else
    {
        vector_generator.load_absolute(80, 199, 1);
        if (option_switch.coinage == 1)
            Graphics_Handler::draw_text(ONE_COIN_2_PLAYS, option_switch.language, vector_generator, window);
        else if (option_switch.coinage == 2)
            Graphics_Handler::draw_text(ONE_COIN_1_PLAY, option_switch.language, vector_generator, window);
        else
            Graphics_Handler::draw_text(TWO_COINS_1_PLAY, option_switch.language, vector_generator, window);
    }

    // flash "push start" after enough credits
    if (credits >= 1 && fast_timer % 64 < 32)
    {
        vector_generator.load_absolute(100, 58, 1);
        Graphics_Handler::draw_text(PUSH_START, option_switch.language, vector_generator, window);
    }

    if (high_score_table[0] > 0 && slow_timer % 8 < 4)
    {
        vector_generator.load_absolute(100, 74, 1);
        Graphics_Handler::draw_text(HIGH_SCORES, option_switch.language, vector_generator, window);

        // draw highscore table
        for (u8 i = 0; i < MAX_HS_COUNT; i++)
        {
            if (high_score_table[i] > 0) // verify this
            {
                // draw score position
                vector_generator.load_absolute(101, 89 + i * 8, 1);
                Graphics_Handler::draw_digit(i + 1, vector_generator, window);
                vector_generator.process(DOT, window);

                // draw score
                Graphics_Handler::draw_score(high_score_table[i], 137, 89 + i * 8, 1, vector_generator, window);

                // draw score initials
                vector_generator.load_absolute(149, 89 + i * 8, 1);
                for (u8 c = 0; c < HS_NAME_LENGTH; c++)
                    Graphics_Handler::draw_character(names_HS[i * 3 + c], vector_generator, window);
            }
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
        vector_generator.load_absolute(100, 74, 1);
        Graphics_Handler::draw_text(PLAYER_, option_switch.language, vector_generator, window);
        Graphics_Handler::draw_digit(current_player + 1, vector_generator, window);
        player_text_timer--;
    }
    else if (!player_lives[current_player] && player[current_player].ship_photon.empty())
    {
        vector_generator.load_absolute(100, 99, 1);
        Graphics_Handler::draw_text(GAME_OVER, option_switch.language, vector_generator, window);
        if (player_count == 2)
        {
            vector_generator.load_absolute(100, 74, 1);
            Graphics_Handler::draw_text(PLAYER_, option_switch.language, vector_generator, window);
            Graphics_Handler::draw_digit(current_player + 1, vector_generator, window);
        }
    }
    if (!player_text_timer && !player[current_player].ship.empty())
        handle_ship_stuff(player[current_player]);

    attempt_asteroid_wave_spawn(player[current_player]);
    update_space_objects(player[current_player], vector_generator);

    if (!player_text_timer && player[current_player].ship.empty())
    {
        if (player[current_player].ship_spawn_timer < 128)
        {
            //if (spawn_area_clear())
                player[current_player].ship.push_back(Ship());
        }
        else if (player_count == 1)
        {
            if (player_lives[0])
            {
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
                player[current_player].saucer_spawn_and_shot_time = player[current_player].saucer_spawn_time_start;
                player[current_player].ship_spawn_timer = 16;
            }
            else if (player_lives[current_player])
            {
                player_text_timer = 128;
                player[current_player].saucer_spawn_and_shot_time = player[current_player].saucer_spawn_time_start;
                player[current_player].ship_spawn_timer = 16;
            }
            else
                end_game();
        }
    }
    vector_generator.load_absolute(40, 43, -2);
    for (u8 i = 0; i < player_lives[0] && i < 55; i++)
        vector_generator.process(LIVES_REMAINING_SHIP, window);
}

void Game::add_points(const u8 points)
{
    u16 old_thousandth = player_score[current_player] / 1000;
    player_score[current_player] += points;

    if (player_score[current_player] / 1000 != old_thousandth)
        player_lives[current_player]++;

    if (player_score[current_player] >= MAX_SCORE)
        player_score[current_player] -= MAX_SCORE;
}

void Game::handle_HS_entry(Vector_Generator& vector_generator)
{
    // if p1 gets a hs in mp and p2 doesn't then just skip p2 entering their name
    if (player_HS_place[current_player] == 255)
        current_player--;

    // draw high score entry control help
    if (last_game_player_count > 1)
    {
        vector_generator.load_absolute(100, 74, 1);
        Graphics_Handler::draw_text(PLAYER_, option_switch.language, vector_generator, window);
        if (fast_timer % 32 >= 16)
            Graphics_Handler::draw_digit(current_player + 1, vector_generator, window);
    }
    vector_generator.load_absolute(12, 86, 1);
    Graphics_Handler::draw_text(YOUR_SCORE_IS_ONE_OF_THE_TEN_BEST, option_switch.language, vector_generator, window);
    vector_generator.load_absolute(12, 94, 1);
    Graphics_Handler::draw_text(PEASE_ENTER_YOUR_INITIALS, option_switch.language, vector_generator, window);
    vector_generator.load_absolute(12, 102, 1);
    Graphics_Handler::draw_text(PUSH_ROTATE_TO_SELECT_LETTER, option_switch.language, vector_generator, window);
    vector_generator.load_absolute(12, 110, 1);
    Graphics_Handler::draw_text(PUSH_HYPERSPACE_WHEN_LETTER_IS_CORRECT, option_switch.language, vector_generator, window);

    // draw name entry
    vector_generator.load_absolute(100, 199, 2);
    for (u8 c = 0; c < HS_NAME_LENGTH; c++)
    {
        if (names_HS[player_HS_place[current_player] + c] == 0)
            vector_generator.process(UNDERSCORE, window);
        else
            Graphics_Handler::draw_character(names_HS[player_HS_place[current_player] + c], vector_generator, window);
    }

    // handle input
    if (input.on_press(HYPERSPACE))
    {
        name_entry_letter_pos++;
        if (name_entry_letter_pos == HS_NAME_LENGTH)
        {
            name_entry_letter_pos = 0;
            player_HS_place[current_player] = 255;
            if (current_player > 0)
                current_player--;
        }
        else
            names_HS[player_HS_place[current_player] + name_entry_letter_pos] = 11;
    }
    if (fast_timer % 8 == 0) // i wonder if this is off by 1 because I didn't freeze the value of fast timer when I checked it
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
    if (option_switch.coinage == 0)
        credits = 2;
}

void Game::update_space_objects(Player& player, Vector_Generator& vector_generator)
{
    // could do this with a remove_if? idk
    for (u8 i = 0; i < player.asteroid.size(); i++)
    {
        if (!player.asteroid[i].update(vector_generator, window))
        {
            player.asteroid.erase(player.asteroid.begin() + i);
            if (player.asteroid.empty())
                player.asteroid_wave_spawn_time = 127;
            i--;
        }
    }
    for (u8 i = 0; i < player.ship.size(); i++)
    {
        if (!player.ship[i].update(vector_generator, window, fast_timer, ship_direction, draw_thrust_graphic) && player.ship_spawn_timer == 128)
        {
            player.ship.erase(player.ship.begin() + i);
            i--;
        }
    }
    for (u8 i = 0; i < player.saucer.size(); i++)
    {
        if (!player.saucer[i].update(vector_generator, window))
        {
            player.saucer.erase(player.saucer.begin() + i);
            i--;
        }
    }
    for (u8 i = 0; i < player.saucer_photon.size(); i++)
    {
        if (!player.saucer_photon[i].update(vector_generator, window))
        {
            player.saucer_photon.erase(player.saucer_photon.begin() + i);
            i--;
        }
    }
    for (u8 i = 0; i < player.ship_photon.size(); i++)
    {
        if (!player.ship_photon[i].update(vector_generator, window))
        {
            player.ship_photon.erase(player.ship_photon.begin() + i);
            i--;
        }
    }

    handle_collision(player);
}

// not sure if points are added or if lives are reduced first, gotta test
void Game::handle_collision(Player& player)
{
    // ship
    for (auto& ship : player.ship)
    {
        bool crashed = false;
        for (u8 i = 0; i < player.asteroid.size() && !crashed; i++)
        {
            if (Space_Object::collide(ship, player.asteroid[i], BONUS_SIZE_1 + player.asteroid[i].get_size()))
            {
                player_lives[current_player]--;
                add_points(player.asteroid[i].get_points());
                spawn_asteroids_from_wreckage(player.asteroid, i);
                ship.set_status(EXPLOSION_START);
                player.ship_spawn_timer = 129;
                crashed = true;
            }
        }
    }

    // saucer
    for (auto& saucer : player.saucer)
    {
        bool crashed = false;
        for (u8 i = 0; i < player.asteroid.size() && !crashed; i++)
        {
            if (Space_Object::collide(saucer, player.asteroid[i], saucer.get_size(true) + player.asteroid[i].get_size()))
            {
                spawn_asteroids_from_wreckage(player.asteroid, i);
                saucer.set_status(EXPLOSION_START);
                crashed = true;
            }
        }
        for (u8 i = 0; i < player.ship.size() && !crashed; i++)
        {
            if (Space_Object::collide(saucer, player.ship[i], saucer.get_size(true)))
            {
                player_lives[current_player]--;
                add_points(saucer.get_points());
                player.ship[i].set_status(EXPLOSION_START);
                player.ship_spawn_timer = 129;
                saucer.set_status(EXPLOSION_START);
                crashed = true;
            }
        }
    }

    // saucer photon
    for (auto& saucer_photon : player.saucer_photon)
    {
        bool crashed = false;
        for (u8 i = 0; i < player.asteroid.size() && !crashed; i++)
        {
            if (Space_Object::collide(saucer_photon, player.asteroid[i], PHOTON_SIZE + player.asteroid[i].get_size()))
            {
                spawn_asteroids_from_wreckage(player.asteroid, i);
                saucer_photon.set_status(0);
                crashed = true;
            }
        }
        for (u8 i = 0; i < player.ship.size() && !crashed; i++)
        {
            if (Space_Object::collide(saucer_photon, player.ship[i], PHOTON_SIZE))
            {
                player_lives[current_player]--;
                player.ship[i].set_status(EXPLOSION_START);
                player.ship_spawn_timer = 129;
                saucer_photon.set_status(0);
                crashed = true;
            }
        }
    }

    // ship photon
    for (auto& ship_photon : player.ship_photon)
    {
        bool crashed = false;
        for (u8 i = 0; i < player.asteroid.size() && !crashed; i++)
        {
            if (Space_Object::collide(ship_photon, player.asteroid[i], PHOTON_SIZE + player.asteroid[i].get_size()))
            {
                add_points(player.asteroid[i].get_points());
                spawn_asteroids_from_wreckage(player.asteroid, i);
                ship_photon.set_status(0);
                crashed = true;
            }
        }
        for (u8 i = 0; i < player.saucer.size() && !crashed; i++)
        {
            if (Space_Object::collide(ship_photon, player.saucer[i], PHOTON_SIZE + player.saucer[i].get_size(false)))
            {
                add_points(player.saucer[i].get_points());
                player.saucer[i].set_status(EXPLOSION_START);
                ship_photon.set_status(0);
                crashed = true;
            }
        }
    }
}

void Game::clear_space_objects(Player& player)
{
    player.asteroid.clear();
    player.ship.clear();
    player.saucer.clear();
    player.saucer_photon.clear();
    player.ship_photon.clear();
}

void Game::attempt_asteroid_wave_spawn(Player& player)
{
    if (player.asteroid_wave_spawn_time)
        player.asteroid_wave_spawn_time--;
    if (player.asteroid.empty() && player.saucer.empty() && !player.asteroid_wave_spawn_time)
    {
        if (player.asteroids_per_wave < 10)
            player.asteroids_per_wave += 2;
        else if (player.asteroids_per_wave == 10)
            player.asteroids_per_wave++;

        for (u8 i = 0; i < player.asteroids_per_wave; i++)
            player.asteroid.push_back(Asteroid());

        player.saucer_spawn_and_shot_time = 127;
    }
}

void Game::spawn_asteroids_from_wreckage(vector<Asteroid>& asteroid, const u8 iteration)
{
    u8 status = asteroid[iteration].get_status();
    if (status >= 16)
    {
        Position pos = asteroid[iteration].get_position();
        asteroid.push_back(Asteroid(status, pos));
        if (asteroid.size() < MAX_ASTEROID_COUNT)
            asteroid.push_back(Asteroid(status, pos));
    }
    asteroid[iteration].set_status(EXPLOSION_START);
}

void Game::handle_ship_stuff(Player& player)
{
    if (input.is_pressed(THRUST) && fast_timer % 8 >= 4)
        draw_thrust_graphic = true;

    // on_press should be handled slightly differently, also need a limit on the photon shooting speed?
    if (input.on_press(FIRE) && !player_text_timer && !player.ship_spawn_timer && player.ship_photon.size() < MAX_SHIP_PHOTON_COUNT)
    //if (input.is_pressed(FIRE) && !player_text_timer && !player.ship_spawn_timer && player.ship_photon.size() < 255)
        player.ship_photon.push_back(Photon(ship_direction,
                                            player.ship[0].get_vel_x(),
                                            player.ship[0].get_vel_y(),
                                            player.ship[0].get_position()));

    if (input.is_pressed(HYPERSPACE) && !player_text_timer && !player.ship_spawn_timer)
    {
        player.ship[0].set_status(INDISCERNIBLE);
        player.ship_spawn_timer = 48;

        u8 pos_x_major = Space_Object::limit_position(random_byte() % 32, 28);
        u8 pos_y_major = random_byte() % 32;

        if (pos_y_major >= 24)
        {
            pos_y_major -= 22;
            pos_y_major *= 2;
            hyperspace_flag = 128;
        }
        else
        {
            pos_y_major = Space_Object::limit_position(pos_y_major, 20);
            hyperspace_flag = 1;
        }
        player.ship[0].set_position_major(pos_x_major, pos_y_major);
    }
    if (player.ship_spawn_timer > 0 && player.ship[0].get_status() < TRUE_EXPLOSION_START)
    {
        player.ship_spawn_timer--;
        if (player.ship_spawn_timer == 0)
        {
            if (hyperspace_flag < 128)
                player.ship[0].set_status(ALIVE);
            else
            {
                player_lives[current_player]--;
                player.ship[0].set_status(EXPLOSION_START);
                player.ship_spawn_timer = 129;
            }
            hyperspace_flag = 0;
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
    if (fast_timer % 2 == 0 && !player.ship_spawn_timer)
    {
        // this could be better done with pointers but ehhhh
        s8 vel_x_major = player.ship[0].get_vel_x();
        s8 vel_y_major = player.ship[0].get_vel_y();
        if (input.is_pressed(THRUST))
            Ship::add_thrust(ship_direction, vel_x_major, ship_vel_x_minor, vel_y_major, ship_vel_y_minor);
        else
            Ship::dampen_velocity(vel_x_major, ship_vel_x_minor, vel_y_major, ship_vel_y_minor);

        player.ship[0].set_velocity(vel_x_major, vel_y_major);
    }
    u8 status = player.ship[0].get_status();
    if (status && status < TRUE_EXPLOSION_START)
        handle_saucer_stuff(player);
}

void Game::handle_saucer_stuff(Player& player) const
{
    if (fast_timer % 4 == 0)
    {
        player.saucer_spawn_and_shot_time--;
        if (player.saucer.empty())
        {
            if (player.saucer_spawn_and_shot_time == 0)
            {
                u8 new_saucer_spawn_time_start = player.saucer_spawn_time_start - 6;
                if (new_saucer_spawn_time_start >= 32)
                    player.saucer_spawn_time_start = new_saucer_spawn_time_start;

                player.saucer.push_back(Saucer());
            }
        }
        else
        {
            // shoot photons
        }
    }
}
