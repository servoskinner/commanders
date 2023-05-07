#pragma once

#include <string>
#include <vector>

#include "gameLogic.h"

#define TOTAL_CARDS_IN_GAME 20

class Roster // All available cards will be listed here along with IDs.
{
public:
    Roster();
    const Card& getOrigin(int index) { return cards[index]; }

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
    std::vector<Card> cards;
};