#pragma once

#include <string>
#include <vector>
#include <memory>
#include <array>

#include "Card_index.hpp"

class Description_generator // A singleton that returns a card's description for its ID.
{
public:
    /// @brief Everything that would be written on a paper card
    struct Card_descr
    {
        std::string name = "unknown";
        std::string ability_text = "";
        std::string flavor_text = "";

        int value = 0;
        int cost = 0;
        int type = -1;
        std::array<bool, 15> mnemosprite = {};
    };
    static Card_descr get_desc(int card_id);

private:
    Description_generator() = delete;
    Description_generator(const Description_generator&) = delete;
    Description_generator& operator=(const Description_generator&) = delete;
};