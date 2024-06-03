#pragma once
#include "Game_master.hpp"
#include <array>

class Game_master::Card_generator
{
public:
    inline const Card& get_card_instance(int index) { return cards[index]; }
    static Card_generator& get();

private:
    Card_generator();

    Card_generator(const Card_generator&) = delete;
    Card_generator& operator=(const Description_generator&) = delete;

    std::array<Card, TOTAL_CARDS_IN_GAME> cards;
};