#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <ncurses.h>
#include <queue>

#include "Description_generator.hpp"

class Commander;

class CLI_commander;
class NCurses_commander;
class Network_commander;

typedef std::reference_wrapper<Commander> pctrl_ref;

class Commander // Acts as an interface between the Game Master and whoever is providing the player input.
{
public:
    struct Player_info
    {
        int id;
        int points;
        int funds;

        int deck_size;
        int discard_size;
        int hand_size;
    };
    struct Card_info // Out of game context info tag 
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
    };
    struct Order // Data structure that represents the player's in-game actions.
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
        std::vector<int> args;

        Order() : type(ORD_NOTHING), args(0) {}
    };
    struct Event // Message describing a happening in game.
    {
        int type;
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

        Event() : type(EV_DUMMY), data(0) {}
    };

    int id;

    int grid_width, grid_height;         
    int turn, turn_absolute;     

    std::vector<Card_info>   active_cards;  // Cards placed on the battlefield and visible to everyone.
    std::vector<Card_info>   hand;          // The associated player's hand.
    std::vector<Player_info> players;       // Known data about other players: their funds, hand size, graveyard, etc.

    virtual Order get_action() = 0;         // Called by the Game Master to receive player input.
    virtual void apply_updates() = 0;       // Called by the Game Master after communicating data to the Controller
    virtual void handle_controller_event(const Event& event) = 0;
};

class CLI_commander : public Commander
{
    public:

    Order get_action() override;
    void handle_controller_event(const Event& event) override;
    void apply_updates() override;

    CLI_commander();

    private:
    Description_generator& desc_gen;
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_funky(std::string &buffer, int g_width, int x, int y);
};

class NCurses_commander : public Commander
{
    public:

    Order get_action() override;
    void handle_controller_event(const Event& event) override;
    void apply_updates() override;

    NCurses_commander();
    ~NCurses_commander();

    private:
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_funky(std::string &buffer, int g_width, int x, int y);
};

class Network_commander : public Commander
{
    public:

    Order get_action() override;
    void handle_controller_event(const Event& event) override;
    void apply_updates() override;

    Network_commander() = default;

    private:
    
};