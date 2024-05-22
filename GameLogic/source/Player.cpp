#include "gameLogic.h"

#include <stdexcept>
#include <iostream>

Player::Player(int playerid) : id(playerid)
{
    std::cout << "created player with ID " << id << " at " << this <<  std::endl;

    funds = 0;
    points = 0;
}

PlayerInfo Player::getInfo(Deck& deck)
{
    PlayerInfo info;

    info.id = id;
    info.funds = funds;
    info.points = points;

    info.deckSize = deck.library.size();
    info.discardSize = deck.discard.size();
    info.handSize = hand.size();

    return info;
}
