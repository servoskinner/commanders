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

#define LOGGER_ON

#ifdef LOGGER_ON
#include "Logger.hpp"
#endif

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

    typedef std::function<void(std::vector<int>)> Effect;
    typedef std::vector<int> EArgs;
    typedef std::unordered_map<int, Effect> Cause;
    typedef std::reference_wrapper<Game_master::Cause>   Cause_ref;

    class Ability;
    class Binding;

    class Ability_simple;

public: // _____________________________________________________________________________
    enum Gamemode {
        GM_STANDARD,
        GM_SKIRMISH,
        GM_2V2
    };

    Game_master(const std::vector<std::vector<int>> &deck_images,
                Gamemode gamemode = GM_STANDARD,
                int nplayers = 2,
                const std::vector<std::vector<int>>& terrain = {});

    // Handle commander requests

    /// @brief Returns game state as seen from perspective of given player.
    /// @param player_id id of player, the available information for whom is returned.
    /// @return The current game state.
    const Commander::Game_state get_game_state(int player_id);

    /// @return the id of player who goes now.
    int get_turn() { return turn; }

    /// @return the total number of turns that have passed since the beginning of match.
    int get_absolute_turn() { return turn_absolute; }

    /// @return Whether a game is active.
    bool is_on() { return game_is_on; }

    /// @return game information that does not change over time and is available to each player.
    Commander::Game_params get_static_game_info();

    /// @return height and width of the battlefield.
    std::pair<int, int> get_grid_params() { return {grid.size(), grid[0].size()}; }
    
    /// @brief executes an order from the perspective of given player.
    /// @param player_id id of player who issues the order.
    /// @param order order to execute.
    /// @return 0 if order was executed successfully, or an error code describing why it failed.
    int exec_order(int player_id, const Commander::Order &order);
    
    /// @brief Gets the least recent event that given player was notified of.
    /// @param player_id id of player who received the event.
    /// @return game event.
    std::optional<Commander::Event> get_event(int player_id);

    /// @brief Discards the least recent event that given player was notified of.
    /// Call this after the event has been fully processed.
    /// @param player_id id of player who received the event.
    inline void pop_event(int player_id)
    {
        if (!event_queues[player_id].empty()) {
            event_queues[player_id].pop();
        }
    }

    /// @brief Callback for whenever an event visible to all players happens.
    std::vector<std::function<void(Commander::Event)>> on_event_broadcast;

private: // _____________________________________________________________________________
    std::optional<Commander::Order> processed_order;

    std::vector<std::queue<Commander::Event>> event_queues;

    std::list<Card> tokens;
    std::vector<Player> players;
    std::vector<Deck> decks;
    // Playing field

    std::vector<std::vector<Tile>> grid; /// The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::vector<Game_master::Card_ref> active_cards;   /// Cards that are currently on the playing field.

    // Global triggers

    Cause any_turn_start; /// Fires on start of each turn (and on the end of previous one)

    Cause any_enters_play; /// Fires whenever any card is deployed.
    Cause any_leaves_play; /// Fires whenever any card is put into anyone's junk.

    Cause any_before_attack;
    Cause any_after_attack;

    Cause any_before_move;
    Cause any_after_move;

    int turn; /// The id of the player who moves now.
    int turn_absolute; /// Total number of turns that have passed.

    bool game_is_on;

    /// @brief Pass the turn to next player and process the necessary triggers.
    void end_turn();

    /// @brief Attempts to find a card by its entity id
    /// @return reference to the card if it was found, nothing if it was not.
    std::optional<Card_ref> find_card(unsigned int entity_id);

    /// @brief Fire all events bound to given Cause.
    inline void fire_trigger(Cause& trigger, std::vector<int> args);

    /// @returns If player with given id has the most units within the capture zone.
    bool check_dominance(int player_id);

    /// @brief Notifies a single player about given event.
    inline void push_event(int player_id, const Commander::Event& event);
    /// @brief  

    inline void broadcast_event(const Commander::Event& event);

    // Card actions
    bool deploy_card(Card &card, int player, std::optional<Tile_ref> target = {});         // Place a card in play.
    bool resolve_movement(Card &card, int direction);   // Move a card in specified direction.
    void resolve_destruction(Card &card);                      // Remove a card from play and discard it.
    bool resolve_attack(Card &card, int direction); // Resolve an attack from one tile to another.
    bool inflict_damage(Card &card, int amount);

    int resolve_combat(Card &attacker, Card &defender); // Resolve combat between two units.
    enum Combat_outcome
    {
        COMBAT_WIN = 1,
        COMBAT_TIE = 0,
        COMBAT_LOSE = -1
    };
    // Tile

    /// @return References to Tiles adjoined to Tile, excluding diagonals.
    std::vector<std::optional<Game_master::Tile_ref>> get_4neighbors(const Tile &tile);
    /// @return References to Tiles adjoined to Tile, including diagonals.
    std::vector<std::optional<Game_master::Tile_ref>> get_8neighbors(const Tile &tile);

    // Players

    /// @brief resolve a player drawing one card from the top of their deck's library.
    /// @param player_id The player that draws a card.
    /// @return Whether the deck was empty or not.
    bool resolve_draw(int player_id);      

    /// @brief resolve a player drawing multiple cards from the top of their deck's library.
    /// @param player_id The player that draws a card.
    /// @return Whether the deck was empty or not.                
    bool resolve_draw_multi(int player_id, int n_cards);

    bool discard(int player_id, int hand_index);                /// Returns whether discarding this card was possible

    /// @brief Perform all rule checks and deploy a card from hand
    /// @param player_id The id of player who plays the card.
    /// @param hand_index Position of card in given player's hand.
    /// @param target Tile to play the card on.
    /// @return Whether playing the card was successful.
    bool play_card(int player_id, int hand_index, std::optional<Tile_ref> target);
};

#include "Card.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Ability.hpp"