#pragma once

#include <string>
#include <vector>
#include <memory>
#include <array>

#include "Card_index.hpp"

class Description_generator // A singleton that returns a card's description for its ID.
{
public:
    struct Card_descr
    {
        int entity_id = -1;
        std::string name = "unknown";
        std::string ability_text = "";
        std::string flavor_text = "";

        int value = 0;
        int cost = 0;
        int type = -1;
        std::array<bool, 15> mnemosprite = {};
    };
    Card_descr get_card_instance(int index);
    static Description_generator& get();

private:
    Description_generator();

    Description_generator(const Description_generator&) = delete;
    Description_generator& operator=(const Description_generator&) = delete;
};