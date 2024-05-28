#include "game_logic.hpp"

#include <stdexcept>
#include <iostream>

Player::Player(int playerid) : id(playerid)
{
    std::cout << "created player with ID " << id << " at " << this <<  std::endl;

    funds = 0;
    points = 0;
}

Player_info Player::getInfo(Deck& deck)
{
    Player_info info;

    info.id = id;
    info.funds = funds;
    info.points = points;

    info.deck_size = deck.library.size();
    info.discard_size = deck.discard.size();
    info.hand_size = hand.size();

    return info;
}
