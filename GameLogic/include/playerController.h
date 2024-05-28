#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>

#include "gameLogic.h"
#include "playerController.h"

class Player_controller;
class CLI_control;
class TCP_control;

typedef std::reference_wrapper<Player_controller> pctrl_ref;

struct Order
{
    int type;
    enum PlayerActionType {NOTHING, PASS, SURRENDER, PLAY, MOVE, ATTACK};
    int args[64];

    Order()
    {
        type = NOTHING;
        std::fill_n(args, 64, -1);
    }
};

class Player_controller //Abstract player interface class (slave of GameMaster)
{
    public:
    Game_master* master;
    int id;
    
    inline int getId() { return id;}

    std::vector<Card_info> active_cards;
    std::vector<Card_info> hand;
    std::vector<Player_info> players;

    virtual Order get_action() = 0;
    virtual void apply_updates() = 0;
    virtual void handle_controller_event(int eventCode) = 0;
    //virtual std::vector<int> chooseTile() = 0;
    //virtual std::vector<int> chooseContract() = 0;
    //virtual std::vector<int> choosePlayer() = 0;
};

class CLI_control : public Player_controller
{
    public:

    Order get_action() override;
    void handle_controller_event(int errorCode) override;
    void apply_updates() override;

    CLI_control() = default;

    private:
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int width, int height, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int width, int height, int x, int y);
    void highlight_tile_funky(std::string &buffer, int width, int height, int x, int y);
};

class TCP_control : public Player_controller
{
    public:

    Order get_action() override;
    void handle_controller_event(int errorCode) override;
    void apply_updates() override;

    TCP_control() = default;

    private:
    
};