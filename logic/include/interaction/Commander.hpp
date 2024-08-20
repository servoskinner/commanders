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
        char id;
        short points;
        short funds;

        unsigned short deck_total_size;
        unsigned short library_size;
        unsigned short discard_size;
        unsigned short hand_size;

        Player_info() = default;
    };
    struct Card_info // Card information to be exchanged between commanders and the master
    {
        unsigned short card_id;
        unsigned short entity_id;
        char owner_id;

        unsigned short x, y;

        bool can_attack;
        bool can_move;
        bool is_overwhelmed;

        unsigned short value;
        unsigned short cost;
        short advantage;
        char type;

        Card_info() = default; 
    };
    struct Commander_message // Data structure that represents the player's in-game actions.
    {
        unsigned short type;
        enum order_type 
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
        enum order_feedback
        {
            ORD_SUCCESS,
            ORD_UNKNOWN,
            ORD_INVTYPE,
            ORD_INVARGS,
            ORD_PERMISSION,
            ORD_NOSELECT,
            ORD_NOTARGET,
            ORD_EXHAUSTED,
            ORD_NOFUNDS
        };
        enum event_type 
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
        std::vector<int> data;

        Commander_message() : type(ORD_DO_NOTHING), data(0) {}
        Commander_message(unsigned short type, std::vector<int> data) : type(type), data(data) {}
        const std::vector<char> packed();
        Commander_message(const std::vector<char>& packed);
    };

    struct Game_params
    {
    std::unordered_map<unsigned int, Card_info> card_manifest;
    std::pair<int, int> grid_size;
    };

    struct Game_status
    {
        int turn, turn_absolute;
        std::vector<Card_info> active_cards;
        std::vector<Player_info> players;
        std::vector<std::vector<Card_info>> hands;
    };

    typedef Commander_message Order; // Command issued by player
    typedef Commander_message Event; // Message describing a happening in game.

    std::vector<int> controlled_ids; // All players controlled by the commander
    int active_id;    

    Game_params game_params;
    Game_status game_status;

    virtual Order get_order() = 0;         // Called by the Game Master to receive player input.
    virtual void process_event(Event event) = 0; // Called by the Game Master to communicate a game event
};