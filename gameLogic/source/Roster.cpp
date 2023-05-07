#include "Roster.h"
#include "gameLogic.h"

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
    cards[FISSION].name = "Nuclear Fission";
    cards[FISSION].type = Card::CONTRACT;
    cards[FISSION].text = "Lasts for months. Indistinguishable from sorcery.";

    cards[FISSION].cost = 5;
    cards[FISSION].value = 10;
    //______________________________
    cards[LOGISTICS].name = "Gray Logistics";
    cards[LOGISTICS].type = Card::TACTIC;
    cards[LOGISTICS].text = "DRAW 2 CARDS.";

    cards[LOGISTICS].cost = 2;
    cards[LOGISTICS].value = -1;
    cards[LOGISTICS].onPlay.push_back(abilityDrawCards<2>);
    //______________________________
    cards[UNITANK].name = "Unicycle tank";
    cards[UNITANK].type = Card::UNIT;
    cards[UNITANK].text = "GAIN $2 WHEN THIS UNIT IS DESTROYED.";

    cards[UNITANK].cost = 4;
    cards[UNITANK].value = 2;
    cards[UNITANK].onDeath.push_back(abilityGainCredits<2>);
    //______________________________
    cards[BIOWEAPONRD].name = "Bioweapons R&D";
    cards[BIOWEAPONRD].type = Card::CONTRACT;
    cards[BIOWEAPONRD].text = "DRAW 1 CARD WHEN THIS CONTRACT IS TERMINATED.";

    cards[BIOWEAPONRD].cost = 3;
    cards[BIOWEAPONRD].value = 4;
    cards[BIOWEAPONRD].onDeath.push_back(abilityDrawCards<1>);
    //______________________________
    cards[BARGAIN].name = "Bargain";
    cards[BARGAIN].type = Card::TACTIC;
    cards[BARGAIN].text = "GAIN 5 CREDITS. DEAL!";

    cards[BARGAIN].cost = 3;
    cards[BARGAIN].value = -1;
    cards[BARGAIN].onPlay.push_back(abilityGainCredits<5>);
}

template <int quantity>
void abilityDrawCards(GameMaster& gm, Card& activator)
{
    for(int i = 0; i < quantity; i++)
        gm.forceDraw(activator.ownerId);
}
template <int quantity>
void abilityGainCredits(GameMaster& gm, Card& activator)
{
    gm.players[activator.ownerId].funds += quantity;
}