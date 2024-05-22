#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "gameLogic.h"
#include "playerController.h"

class Player_controller;
class CLI_control;
class TCP_control;

typedef std::shared_ptr<Player_controller> ControllerPtr;

struct PlayerAction
{
    int type;
    enum PlayerActionType {NOTHING, PASS, SURRENDER, PLAY, MOVE, ATTACK};
    int args[64];

    PlayerAction()
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

    std::vector<Card_info> activeCards;
    std::vector<Card_info> hand;
    std::vector<Player_info> players;

    virtual PlayerAction getAction() = 0;
    virtual void applyUpdates() = 0;
    virtual void handleControllerEvent(int eventCode) = 0;
    //virtual std::vector<int> chooseTile() = 0;
    //virtual std::vector<int> chooseContract() = 0;
    //virtual std::vector<int> choosePlayer() = 0;
};

class CLI_control : public Player_controller
{
    public:

    PlayerAction getAction() override;
    void handleControllerEvent(int errorCode) override;
    void applyUpdates() override;

    CLI_control() = default;

    private:
    void printUI();

    void higlightTileBold(std::string &buffer, int width, int height, int x, int y);
    void higlightTileLight(std::string &buffer, int width, int height, int x, int y);
    void higlightTileFunky(std::string &buffer, int width, int height, int x, int y);
};

class TCP_control : public Player_controller
{
    public:

    PlayerAction getAction() override;
    void handleControllerEvent(int errorCode) override;
    void applyUpdates() override;

    TCP_control() = default;

    private:
    
};