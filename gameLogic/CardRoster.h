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

Roster::Roster() : cards(TOTAL_CARDS_IN_GAME)
{
    for(int i = 0; i < TOTAL_CARDS_IN_GAME; i++)
        cards[i].id = i;
    //______________________________
    cards[BOUNTYHUNTER].name = "Bounty Hunter";
    cards[BOUNTYHUNTER].type = Card::UNIT;
    cards[BOUNTYHUNTER].text = "Poorly armed rabble hired for pocket money. Cannon fodder.";

    cards[BOUNTYHUNTER].cost = 1;
    cards[BOUNTYHUNTER].value = 2;
    //______________________________
    cards[HENCHMAN].name = "Henchman";
    cards[HENCHMAN].type = Card::UNIT;
    cards[HENCHMAN].text = "Standard infantry. Most standard of them all.";

    cards[HENCHMAN].cost = 2;
    cards[HENCHMAN].value = 3;
    //______________________________
    cards[COMMANDO].name = "Commando";
    cards[COMMANDO].type = Card::UNIT;
    cards[COMMANDO].text = "Elite goons with a little more muscle.";

    cards[COMMANDO].cost = 3;
    cards[COMMANDO].value = 4;
    //______________________________
    cards[OPPRESSOR].name = "Oppressor";
    cards[OPPRESSOR].type = Card::UNIT;
    cards[OPPRESSOR].text = "Superheavy assault unit. Kicks ass.";

    cards[OPPRESSOR].cost = 4;
    cards[OPPRESSOR].value = 5;
    //______________________________
    cards[MAIMBOT].name = "Maimbot";
    cards[MAIMBOT].type = Card::UNIT;
    cards[MAIMBOT].text = "ADVANTAGE: (1) Autonomous weapons platform.";

    cards[MAIMBOT].cost = 4;
    cards[MAIMBOT].value = 3;
    cards[MAIMBOT].advantage = 1;
    //______________________________
    cards[ARCHON].name = "Archon";
    cards[ARCHON].type = Card::UNIT;
    cards[ARCHON].text = "ADVANTAGE: (1) Frontline mech, best in class. Banned in 43 countries.";

    cards[ARCHON].cost = 7;
    cards[ARCHON].value = 5;
    cards[ARCHON].advantage = 1;
    //______________________________
    cards[MACHINEPARTS].name = "Machine parts";
    cards[MACHINEPARTS].type = Card::CONTRACT;
    cards[MACHINEPARTS].text = "The nerves and bones of current age.";

    cards[MACHINEPARTS].cost = 3;
    cards[MACHINEPARTS].value = 7;
    //______________________________
    cards[GUNKFOOD].name = "Gunkfood";
    cards[GUNKFOOD].type = Card::CONTRACT;
    cards[GUNKFOOD].text = "Synthetic food. It's got electrolytes.";

    cards[GUNKFOOD].cost = 1;
    cards[GUNKFOOD].value = 4; 
    //______________________________
    cards[FISSION].name = "Nuclear fission";
    cards[FISSION].type = Card::CONTRACT;
    cards[FISSION].text = "Lasts for months. Indistinguishable from sorcery.";

    cards[FISSION].cost = 5;
    cards[FISSION].value = 10;
}