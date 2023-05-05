#pragma once

#include <string>
#include <vector>

#include "gameLogic.h"

#define TOTAL_CARDS_IN_GAME 10

class Roster // All available cards will be listed here along with IDs.
{
public:
    Roster();
    Card getOrigin(int index) { return cards[index]; }

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
        FISSION
    };

private:
    std::vector<Card> cards;
};