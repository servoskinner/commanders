#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Card_index.hpp"

class Description_generator // A singleton that returns a card's description for its ID.
{
public:
    struct Card_descr
    {
        int global_id = -1;
        std::string name = "unknown";
        std::string ability_text = "";
        std::string flavor_text = "";

        int value = 0;
        int cost = 0;
        int type = -1;
    };
    inline const Card_descr& get_card_instance(int index) 
    { return (index >= 0 && index < TOTAL_CARDS_IN_GAME) ? cards[index] : unknown_card; }
    static Description_generator& get();

private:
    Description_generator();

    Description_generator(const Description_generator&) = delete;
    Description_generator& operator=(const Description_generator&) = delete;
    std::array<Card_descr, TOTAL_CARDS_IN_GAME> cards;
    Card_descr unknown_card = {};
};