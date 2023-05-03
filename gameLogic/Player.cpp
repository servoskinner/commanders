#include "gameLogic.h"

#include <stdexcept>
#include <iostream>

Player::Player(int playerid) : id(playerid)
{
    std::cout << " created player " << this << " with ID " << id << std::endl;

    funds = 0;
    points = 0;
}