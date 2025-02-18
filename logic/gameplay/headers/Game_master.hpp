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

#include "Vector2i.hpp"
#include "Card_index.hpp"
#include "Misc_functions.hpp"
#include "Unique_map.hpp"

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
    typedef std::reference_wrapper<Card> Card_ref;

    class Ability;
    typedef std::reference_wrapper<Ability> Ability_ref;
    typedef std::unique_ptr<Ability> Anon_ability;

    class Deck;
    typedef std::reference_wrapper<Deck> Deck_ref;

    class Player;
    typedef std::reference_wrapper<Player> Player_ref;
    
    class Tile;
    typedef std::reference_wrapper<Tile> Tile_ref;

    typedef std::function<void(std::vector<int>)> Effect;
    typedef std::vector<int> EArgs;
    typedef std::unordered_map<int, Effect> Cause;
    typedef std::reference_wrapper<Game_master::Cause>   Cause_ref;

public: // _____________________________________________________________________________
    enum Gamemode {
        GM_STANDARD,
        GM_SKIRMISH,
        GM_2V2
    };
    enum Direction
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT,
        TOP_RIGHT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        TOP_LEFT,
        NOT_A_NEIGHBOR
    };
    enum Tile_type
    {
        NORMAL      = -1,
        OBJECTIVE   = -2,
        // Special terrain has too much impact on gameplay and its use is discouraged
        OBSTACLE    = -3,
        NO_DEPLOY   = -4,
        TERR_ADV    = -5,
        TERR_DISADV = -6
    }; // Non-negatives refer to deploy zones of players with same IDs
    /// @brief Player info without implementation details.
    struct Player_info
    {
        uint8_t id;
        int16_t points;
        int16_t funds;

        uint16_t deck_total_size;
        uint16_t library_size;
        uint16_t discard_size;
        uint16_t hand_size;

        Player_info() = default;
    };
    struct Card_info // Card information to be exchanged between commanders and the master
    {
        uint16_t card_id;
        uint32_t entity_id;
        uint8_t owner_id;

        Vector2i pos;

        uint8_t can_attack;
        uint8_t can_move;
        uint8_t is_overwhelmed;

        uint16_t value;
        uint16_t cost;
        int8_t advantage; // may be negative!
        uint8_t type;

        Card_info() = default; 
    };

    enum Order_type : uint8_t
    {
        ORD_DO_NOTHING, 
        ORD_PASS, 
        ORD_SURRENDER, 
        ORD_PLAY_CARD, 
        ORD_MOVE, 
        ORD_ATTACK, 
        ORD_CHOICE,
        ORD_ABILITY
    };

    enum Order_result : uint8_t
    {
        ORD_SUCCESS,
        ORD_UNKNOWN,
        ORD_UNKNOWN_TYPE,
        ORD_INV_ARGS,
        ORD_OUT_OF_RANGE,
        ORD_NO_PERMISSION,
        ORD_FRIENDLY_FIRE,
        ORD_NO_ACTOR,
        ORD_NO_TARGET,
        ORD_EXHAUSTED,
        ORD_LOW_FUNDS
    };

    enum Event_type : uint8_t
    {
        EV_DUMMY,
        EV_ORDER_FEEDBACK,
        EV_GAME_WON_BY,
        EV_TURN_PASSED_TO,
        EV_PLAYER_DEPLOYS,
        EV_CARD_MOVED,
        EV_CARD_ATTACKS,
        EV_CARD_DESTROYED,
        EV_PLAYER_DRAWS,
        EV_PLAYER_DISCARDS,
        EV_DECK_REFRESH,
        EV_DECK_NOREFRESH,
        EV_CHOICE_PROMPT
    };
    
    struct Message // Data structure that represents the player's in-game actions.
    {
        uint8_t type;

        std::vector<int32_t> data;

        Message() : type(ORD_DO_NOTHING), data(0) {}
        Message(unsigned short type, std::vector<int32_t> data) : type(type), data(data) {}
        const std::vector<uint8_t> packed();
        Message(const std::vector<uint8_t>& packed);
    };

    struct Game_params
    {
        std::unordered_map<uint32_t, Card_info> card_manifest;
        Vector2i grid_size;
    };

    struct Game_state
    {
        uint32_t turn, turn_absolute;
        std::vector<Card_info> active_cards;
        std::vector<Player_info> players;
        std::vector<std::vector<Card_info>> hands;
    };

    typedef Message Order; // Command issued by player
    typedef Message Event; // Message describing a happening in game.

    Game_master(const std::vector<std::vector<unsigned int>> &deck_images,
                Gamemode gamemode = GM_STANDARD,
                int nplayers = 2,
                const std::vector<std::vector<int>>& terrain = {});

    // Handle commander requests

    /// @brief Returns game state as seen from perspective of given player.
    /// @param player_id id of player, the available information for whom is returned.
    /// @return The current game state.
    Game_state get_game_state(int player_id) const;

    /// @return the id of player who goes now.
    int get_turn() { return turn; }

    /// @return the total number of turns that have passed since the beginning of match.
    int get_absolute_turn() { return turn_absolute; }

    /// @return Whether a game is active.
    bool is_ongoing() { return game_is_ongoing; }

    /// @return game information that does not change over time and is available to each player.
    Game_params get_static_game_info() const;


    /// @brief Dimensions of battlefield.
    Vector2i grid_size;
    /// @return dimensions of the battlefield.
    Vector2i get_grid_size() { return grid_size; }
    
    /// @brief executes an order from the perspective of given player.
    /// @param player_id id of player who issues the order.
    /// @param order order to execute.
    /// @return 0 if order was executed successfully, or an error code describing why it failed.
    Order_result exec_order(int player_id, const Order &order);
    
    /// @brief Gets the least recent event that given player was notified of.
    /// @param player_id id of player who received the event.
    /// @return game event.
    std::optional<Event> get_event(int player_id);

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
    std::vector<std::function<void(Event)>> on_event_broadcast;

private: // _____________________________________________________________________________
    std::optional<Order> processed_order;

    std::vector<std::queue<Event>> event_queues;

    std::vector<Player> players;
    // Indexed by EIDs
    std::unordered_map<unsigned int, Card> tokens;
    std::unordered_map<unsigned int, Deck> decks;
    // Playing field

    std::vector<std::vector<Tile>> grid; /// The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::unordered_map<unsigned int, Card_ref> active_cards;   /// Cards that are currently on the playing field.

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

    bool game_is_ongoing;

    /// @brief Pass the turn to next player and process the necessary triggers.
    void end_turn();

    /// @brief Attempts to find a card by its entity id
    /// @return reference to the card if it was found, nothing if it was not.
    std::optional<Card_ref> find_card(unsigned int entity_id);

    /// @brief Fire all events bound to given Cause.
    inline void fire_trigger(Cause& trigger, std::vector<int> args);

    /// @returns If player with given id has the most units within the capture zone.
    bool check_dominance(int player_id) const;

    /// @brief Notifies a single player about given event.
    inline void push_event(int player_id, const Event& event);
    /// @brief  

    inline void broadcast_event(const Event& event);

    // Card functions

    /// @brief Try to place a card in play.
    Order_result deploy_card(Card &card, int player, std::optional<Tile_ref> target = {});
    /// @brief Try to move a card in specified direction.
    Order_result resolve_movement(Card &card, Direction direction); // Move a card in specified direction.
    /// @brief Try to attack an adjacent tile with card.
    Order_result resolve_attack(Card &card, Direction direction); // Resolve an attack from one tile to another.
    /// @brief Resolve dealing damage to a card, making destruction checks and 
    /// @returns true if card was destroyed.
    bool inflict_damage(Card &card, unsigned int amount);
    /// @brief Put a card from battlefield to junk.
    void resolve_destruction(Card &card);

    enum Combat_outcome
    {
        ATTACKER_LIVES = 1,
        BOTH_DIE = 0,
        DEFENDER_LIVES = -1
    };
    /// @brief Resolve combat between two units.
    Combat_outcome resolve_combat(Card &attacker, Card &defender);

    // Tile functions

    /// @return References to Tiles adjoined to Tile, excluding diagonals.
    std::array<std::optional<Game_master::Tile_ref>, 4> get_4neighbors(const Tile &tile);
    /// @return References to Tiles adjoined to Tile, including diagonals.
    std::array<std::optional<Game_master::Tile_ref>, 8> get_8neighbors(const Tile &tile);

    /// @brief Returns a Tile_neighbor value corresponding to given delta.
    Direction delta_to_neighbor(Vector2i delta) const;

    inline bool is_4neighbor(Direction neighbor) { return neighbor < 4; }
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