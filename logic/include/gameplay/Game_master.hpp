#pragma once

#include <vector>
#include <list>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <queue>

#include "Commander.hpp"
#include "Card_index.hpp"
#include "Misc_functions.hpp"

#define GRID_WIDTH 		8
#define GRID_HEIGHT 	6

#define BASIC_INCOME 			1
#define BONUS_INCOME            1
#define BONUS_INCOME_THRESHOLD  2
#define STARTING_HAND_SIZE 		4 
#define POINTS_REQ_FOR_VICTORY 	10

#define EVENT_QUEUE_CAPACITY    1024

class Game_master
{
protected:

    class Card;
    typedef std::reference_wrapper<Game_master::Card>   Card_ref;
    class Deck;
    typedef std::reference_wrapper<Game_master::Deck>   Deck_ref;
    class Player;
    typedef std::reference_wrapper<Game_master::Player> Player_ref;
    class Tile;
    typedef std::reference_wrapper<Game_master::Tile>   Tile_ref;

    typedef std::function<void(std::vector<int>)> Reaction;
    typedef std::unordered_map<int, Reaction> Trigger;
    typedef std::reference_wrapper<Game_master::Trigger>   Trigger_ref;

    class Ability;
    class Binding;

    class Ability_simple;

public: // _____________________________________________________________________________
    Game_master(const std::vector<std::vector<int>> &deck_images,
                int gamemode = GM_STANDARD,
                int nplayers = 2,
                const std::vector<std::vector<int>>& terrain = {});

    enum gamemodes {
        GM_STANDARD,
        GM_SKIRMISH,
        GM_2V2
    };
    // Handle commander requests
    const Commander::Game_state get_game_state(int player_id);
    int get_turn() { return turn; }
    int get_absolute_turn() { return turn_absolute; }
    bool is_on() { return game_is_on; }
    Commander::Game_params get_static_game_info();
    std::pair<int, int> get_grid_params() { return {grid.size(), grid[0].size()}; }

    int exec_order(int player_id, const Commander::Order &action);
    
    std::optional<Commander::Event> get_event(int player_id);
    inline void pop_event(int player_id)
    {
        if (!event_queues[player_id].empty()) {
            event_queues[player_id].pop();
        }
    }


    std::vector<std::function<void(Commander::Event)>> on_event_broadcast;

private: // _____________________________________________________________________________
    std::vector<std::queue<Commander::Event>> event_queues;

    std::list<Card> tokens;
    std::vector<Player> players;
    std::vector<Deck> decks;
    // Playing field
    std::vector<std::vector<Tile>> grid; // The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::vector<Game_master::Card_ref> active_cards;   // Cards that are currently on the playing field.

    // Global triggers
    Trigger any_turn_start;

    Trigger any_enters_play;
    Trigger any_leaves_play;

    Trigger any_before_attack;
    Trigger any_after_attack;

    Trigger any_before_move;
    Trigger any_after_move;

    int turn;
    int turn_absolute;

    bool game_is_on;

    void end_turn();  // Pass the turn to next player and process the necessary triggers.

    std::optional<Card_ref> find_card(unsigned int entity_id);
    inline void fire_trigger(Trigger& trigger, std::vector<int> args);
    bool check_dominance(int playerId);

    inline void push_event(int player_id, const Commander::Event& event);
    inline void broadcast_event(const Commander::Event& event);

    // Card actions
    bool deploy_card(Card &card, int player, std::optional<Tile_ref> target = {});         // Place a card in play.
    bool resolve_movement(Card &card, const int &direction);   // Move a card in specified direction.
    void resolve_destruction(Card &card);                      // Remove a card from play and discard it.
    bool resolve_attack(Card &card, const int &direction); // Resolve an attack from one tile to another.

    int resolve_combat(Card &attacker, Card &defender); // Resolve combat between two units.
    enum combat_outcome
    {
        COMBAT_WIN,
        COMBAT_TIE,
        COMBAT_LOSE
    };
    // Tile functions
    std::vector<std::optional<Game_master::Tile_ref>> get_4neighbors(const Tile &tile);
    std::vector<std::optional<Game_master::Tile_ref>> get_8neighbors(const Tile &tile);

    // Players
    bool resolve_draw(int player_id);                             // Returns whether deck is empty or not
    bool resolve_draw_multi(int player_id, int n_cards);
    bool discard(int playerId, int handIndex);                // Returns whether discarding this card was possible
    bool play_card(int playerId, int handIndex, std::optional<Tile_ref> target); // Perform rule checks and deploy card
};

#include "Card.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Ability.hpp"