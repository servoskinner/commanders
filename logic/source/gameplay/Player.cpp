#include "Game_master.hpp"
#include "Commander.hpp"

#include <stdexcept>
#include <iostream>

Game_master::Player::Player(int player_id, Deck& p_deck) : id(player_id), deck(p_deck)
{
    std::cout << "created player with ID " << id << " at " << this <<  std::endl;

    funds = 0;
    points = 0;
    is_active = true;
}

Commander::Player_info Game_master::Player::get_info()
{
    Commander::Player_info info;

    info.id = id;
    info.funds = funds;
    info.points = points;

    info.deck_total_size = deck.get().all.size();
    info.library_size = deck.get().library.size();
    info.discard_size = deck.get().junk.size();
    info.hand_size = hand.size();

    return info;
}
