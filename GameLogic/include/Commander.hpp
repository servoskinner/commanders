#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>

#include <cstring>

class Commander;

class CLI_commander;
class NCurses_commander;
class Network_commander;

typedef std::reference_wrapper<Commander> commander_ref;

class Commander // Acts as an interface between the Game Master and whoever is providing the player input.
{
public:
    struct Player_info // Information about other players seen by everyone.
    {
        int id;
        int points;
        int funds;

        int deck_total_size;
        int library_size;
        int discard_size;
        int hand_size;

        Player_info() = default;
        const std::vector<char> packed();
        Player_info(const std::vector<char>& packed);
    };
    struct Card_info // Gameplay-wise card information excerpt
    {
        int global_id;
        int match_id;
        int owner_id;

        int x, y;

        bool can_attack;
        bool can_move;
        bool is_overwhelmed;

        int value;
        int cost;
        int advantage;
        int type;

        Card_info() = default; 
        const std::vector<char> packed(); // Packs the structure to send it via network.
        Card_info(const std::vector<char>& packed); // Unpacks the structure from byte sequence
    };
    struct Commander_message // Data structure that represents the player's in-game actions.
    {
        int type;
        enum order_type 
        {
            ORD_NOTHING, 
            ORD_PASS, 
            ORD_SURRENDER, 
            ORD_PLAY, 
            ORD_MOVE, 
            ORD_ATTACK, 
            ORD_CHOICE,
            ORD_ABILITY
        };
        enum order_error_codes
        {
            INVORD_NONE,
            INVORD_UNKNOWN,
            INVORD_INVTYPE,
            INVORD_INVARGS,
            INVORD_PERMISSION,
            INVORD_NOSELECT,
            INVORD_NOTARGET,
            INVORD_EXHAUSTED,
            INVORD_NOFUNDS
        };
        enum event_type 
        {
            EV_DUMMY,
            EV_ORDER_CONFIRM,
            EV_ORDER_INVALID,
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
        std::vector<int> data;

        Commander_message() : type(ORD_NOTHING), data(0) {}
        const std::vector<char> packed();
        Commander_message(const std::vector<char>& packed);
    };
    typedef Commander_message Order;
    typedef Commander_message Event; // Message describing a happening in game.

    int id; // Associated player's unique identifier

    int grid_width, grid_height;         
    int turn, turn_absolute;     

    std::vector<Card_info>   active_cards;  // Cards placed on the battlefield and visible to everyone.
    std::vector<Card_info>   hand;          // Associated player's hand.
    std::vector<Player_info> players;       // Known data about other players: their funds, hand size, graveyard, etc.

    virtual Order get_order() = 0;         // Called by the Game Master to receive player input.
    virtual void process_event(const Event& event) = 0; // Called by the Game Master to communicate a game event
};