#include "Game_master.hpp"
#include "Commander.hpp"

#include <stdexcept>
#include <iostream>

Game_master::Player::Player(int playerid) : id(playerid)
{
    std::cout << "created player with ID " << id << " at " << this <<  std::endl;

    funds = 0;
    points = 0;
}

Commander::Player_info Game_master::Player::get_info(Deck& deck)
{
    Commander::Player_info info;

    info.id = id;
    info.funds = funds;
    info.points = points;

    info.deck_size = deck.library.size();
    info.discard_size = deck.discard.size();
    info.hand_size = hand.size();

    return info;
}
