#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "gameLogic.h"

typedef std::shared_ptr<PlayerController> ControllerPtr;

struct PlayerAction
{
    int type;
    enum PlayerActionType {NOTHING, PASS, SURRENDER, PLAY, MOVE, ATTACK};
    int args[64];
};

class PlayerController //Abstract player interface class
{
    public:
    GameMaster master;
    int id;

    std::vector<CardInfo> inPlay;
    std::vector<CardInfo> hand;

    virtual PlayerAction getAction() = 0;
    //virtual std::vector<int> chooseTile() = 0;
    //virtual std::vector<int> chooseContract() = 0;
    //virtual std::vector<int> choosePlayer() = 0;
};

class TerminalControl : public PlayerController
{
    public:

    void printUI();
    PlayerAction getAction() override;

    private:
    void higlightTileBold(std::string &buffer, int width, int height, int x, int y);
    void higlightTileLight(std::string &buffer, int width, int height, int x, int y);
    void higlightTileHazard(std::string &buffer, int width, int height, int x, int y);
};