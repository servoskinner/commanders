#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "gameLogic.h"
#include "playerController.h"

class PlayerController;
class TerminalControl;

typedef std::shared_ptr<PlayerController> ControllerPtr;

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

class PlayerController //Abstract player interface class (slave of GameMaster)
{
    public:
    GameMaster* master;
    int id;
    
    inline int getId() { return id;}

    std::vector<CardInfo> activeCards;
    std::vector<CardInfo> hand;
    std::vector<PlayerInfo> players;

    virtual PlayerAction getAction() = 0;
    virtual void applyUpdates() = 0;
    virtual void handleActionError(int errorCode) = 0;
    //virtual std::vector<int> chooseTile() = 0;
    //virtual std::vector<int> chooseContract() = 0;
    //virtual std::vector<int> choosePlayer() = 0;
};

class TerminalControl : public PlayerController
{
    public:

    PlayerAction getAction() override;
    void handleActionError(int errorCode) override;
    void applyUpdates() override;

    TerminalControl() = default;

    private:
    void printUI();

    void higlightTileBold(std::string &buffer, int width, int height, int x, int y);
    void higlightTileLight(std::string &buffer, int width, int height, int x, int y);;
};