#pragma once

#include <vector>
#include <string>
#include <functional>

#include "gameLogic.h"

struct PlayerAction
{
    int type;
    enum PlayerActionType {NOTHING, PASS, SURRENDER, PLAY, MOVE, };
};

class PlayerController
{
    virtual PlayerAction getAction();
    
};