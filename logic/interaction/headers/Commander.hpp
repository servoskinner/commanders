#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

#include <cstring>
#include "Vector2i.hpp"

class Commander;

typedef std::reference_wrapper<Commander> commander_ref;

/// @brief Generates standardized Orders from interface input and processes Events in the opposite direction.
class Commander // Acts as an interface between the Game Master and whoever is providing the player input.
{
public:
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
    
    struct Commander_message // Data structure that represents the player's in-game actions.
    {
        uint8_t type;

        std::vector<int32_t> data;

        Commander_message() : type(ORD_DO_NOTHING), data(0) {}
        Commander_message(unsigned short type, std::vector<int32_t> data) : type(type), data(data) {}
        const std::vector<uint8_t> packed();
        Commander_message(const std::vector<uint8_t>& packed);
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

    typedef Commander_message Order; // Command issued by player
    typedef Commander_message Event; // Message describing a happening in game.

    std::vector<int> controlled_ids; // All players controlled by the commander
    int active_id;    

    Game_params game_params;
    Game_state game_state;

    virtual std::optional<Order> get_order() = 0;         
    virtual void process_event(Event event) = 0;
    virtual void process_order_feedback(int code) = 0;

    virtual void update_state(Game_state state) { game_state = state;}
    virtual void set_params(Game_params params) { game_params = params;}
};