#include "Game_master.hpp"
#include "Commander.hpp"

#include <stdexcept>
#include <iostream>

Game_master::Player::Player(int player_id, Deck& p_deck) : id(player_id), deck(p_deck)
{
    #ifdef LOGGER_ON
        Logger::get().write("Created player (id " + std::to_string(player_id) + ")");
    #endif

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
