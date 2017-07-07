// Copyright slx7R4GDZM
// Distributed under the terms of the MIT License.
// Refer to the License.txt file for details.

#pragma once

#include "Space_Object/Asteroid.h"
#include "Space_Object/Ship.h"
#include "Space_Object/Saucer.h"
#include "Space_Object/Photon.h"
#include "../Graphics/Vector_Generator.h"
#include "../Input/Input_Handler.h"
#include "../Other/Common_Things.h"
#include "../Other/Constants.h"
#include <SFML/Graphics.hpp>

class Game
{
private:
    struct Player
    {
        Asteroid asteroid[MAX_ASTEROIDS];
        Ship ship;
        Saucer saucer;
        Photon saucer_photon[MAX_SAUCER_PHOTONS];
        Photon ship_photon[MAX_SHIP_PHOTONS];
        u8 asteroids_per_wave;
        u8 asteroid_count;
        u8 saucer_spawn_and_shot_time;
        u8 saucer_spawn_time_start;
        u8 ship_spawn_timer;
        u8 asteroid_wave_spawn_time;
    };

    Settings_Handler settings;
    sf::RenderWindow window;
    Input_Handler input;
    Inactive_Mode game_activity;
    bool draw_thrust; // this is a kinda hacky variable to deal with how the dvg is simulated super inaccurately

    u8 current_player;
    u8 last_game_player_count;
    u8 player_count;
    u16 high_score_table[MAX_HS_COUNT];
    u8 name_entry_letter_pos;
    u8 player_HS_place[MAX_PLAYERS];
    u8 names_HS[MAX_HS_COUNT * HS_NAME_LENGTH];
    u16 player_score[MAX_PLAYERS];
    u8 starting_lives;
    u8 player_lives[MAX_PLAYERS];
    u8 hyperspace_flag;
    u8 player_text_timer;
    u8 fast_timer;
    u8 slow_timer;
    u8 ship_direction;
    u8 ship_vel_x_minor;
    u8 ship_vel_y_minor;
    // sound stuff will go here
    u8 credits;
    u8 pre_credit_coins;
    Option_Switch option_switch;
    Player player[MAX_PLAYERS];

    void process_events(Vector_Generator& vector_generator, sf::RenderWindow& window);
    void limit_FPS(const high_resolution_clock::time_point start_time) const;

    void draw_multiplayer_scores(Vector_Generator& vector_generator);
    void draw_copyright(Vector_Generator& vector_generator);

    void coin_input_to_credit();
    void add_credit();

    void attempt_game_start();
    void handle_game_start();
    void end_game();

    void attract_mode(Vector_Generator& vector_generator);
    void update_player(Vector_Generator& vector_generator);
    void add_points(const u8 points);

    void insert_any_new_high_scores();
    void handle_HS_entry(Vector_Generator& vector_generator);
    void draw_HS_entry_screen(Vector_Generator& vector_generator);
    void handle_HS_entry_input();

    void update_space_objects(Player& player, Vector_Generator& vector_generator);
    void handle_collision(Player& player);
    static void clear_space_objects(Player& player);

    static void attempt_asteroid_wave_spawn(Player& player);
    static void spawn_asteroids_from_wreckage(Player& player, const u8 iteration);
    void handle_ship_stuff(Player& player);
    void handle_saucer_stuff(Player& player) const;
public:
    Game();
};
