#pragma once

#include <string>
#include <vector>
#include <memory>

#include "gameLogic.h"

#define TOTAL_CARDS_IN_GAME 20

class Roster // All available cards will be listed here along with IDs.
{
public:
    inline const Card& get_card_instance(int index) { return cards[index]; }
    static Roster& get();

    enum cardIds
    {
        BOUNTYHUNTER,
        HENCHMAN,
        COMMANDO,
        OPPRESSOR,
        MAIMBOT,
        ARCHON,
        GUNKFOOD,
        MACHINEPARTS,
        FISSION,
        LOGISTICS,
        UNITANK,
        BIOWEAPONRD,
        BARGAIN
    };

private:
    Roster();
    static std::unique_ptr<Roster> instance{new Roster};

    Roster(const Roster&) = delete;
    Roster& operator=(const Roster&) = delete;

    std::array<Card, TOTAL_CARDS_IN_GAME> cards;
};