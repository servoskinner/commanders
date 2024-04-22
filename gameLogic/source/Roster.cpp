#include "Roster.h"
#include "gameLogic.h"

Roster::Roster() : cards(TOTAL_CARDS_IN_GAME)
{
    for(int i = 0; i < TOTAL_CARDS_IN_GAME; i++)
        cards[i].id = i;
    //______________________________
    cards[BOUNTYHUNTER].name = "Small-Time Merc";
    cards[BOUNTYHUNTER].type = Card::UNIT;
    cards[BOUNTYHUNTER].text = "Idiots we hired to fight for pocket money.";

    cards[BOUNTYHUNTER].cost = 1;
    cards[BOUNTYHUNTER].value = 2;
    //______________________________
    cards[HENCHMAN].name = "Henchman";
    cards[HENCHMAN].type = Card::UNIT;
    cards[HENCHMAN].text = "Standard infantry. Most standard of them all.";

    cards[HENCHMAN].cost = 2;
    cards[HENCHMAN].value = 3;
    //______________________________
    cards[COMMANDO].name = "Terrorguard";
    cards[COMMANDO].type = Card::UNIT;
    cards[COMMANDO].text = "Advanced augmentoid infantry. Kicks more ass than average.";

    cards[COMMANDO].cost = 3;
    cards[COMMANDO].value = 4;
    //______________________________
    cards[OPPRESSOR].name = "Oppressor";
    cards[OPPRESSOR].type = Card::UNIT;
    cards[OPPRESSOR].text = "Superheavy assault unit. Ideal for shock-and-awe tactics.";

    cards[OPPRESSOR].cost = 4;
    cards[OPPRESSOR].value = 5;
    //______________________________
    cards[MAIMBOT].name = "Maimbot";
    cards[MAIMBOT].type = Card::UNIT;
    cards[MAIMBOT].text = "Autonomous weapons platform.\n|   [ADVANTAGE 1]";

    cards[MAIMBOT].cost = 4;
    cards[MAIMBOT].value = 3;
    cards[MAIMBOT].advantage = 1;
    //______________________________
    cards[ARCHON].name = "Omnitank";
    cards[ARCHON].type = Card::UNIT;
    cards[ARCHON].text = "High-performance Death Mech. Banned in 43 countries.\n|   [ADVANTAGE 1]";

    cards[ARCHON].cost = 7;
    cards[ARCHON].value = 5;
    cards[ARCHON].advantage = 1;
    //______________________________
    cards[MACHINEPARTS].name = "Machine Shop";
    cards[MACHINEPARTS].type = Card::CONTRACT;
    cards[MACHINEPARTS].text = "Produces the nerve and bone of our age.";

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
    cards[FISSION].text = "Get raw energy from almost thin air.";

    cards[FISSION].cost = 5;
    cards[FISSION].value = 10;
    //______________________________
    cards[UNITANK].name = "Unicyclone";
    cards[UNITANK].type = Card::UNIT;
    cards[UNITANK].text = "A unicycle tank.\n|   [GAIN $3 WHEN DESTROYED]";

    cards[UNITANK].cost = 4;
    cards[UNITANK].value = 3;
    cards[UNITANK].onDeath.push_back(abilityGainCredits<3>);
    //______________________________
    cards[BIOWEAPONRD].name = "Bioweapons R&D";
    cards[BIOWEAPONRD].type = Card::CONTRACT;
    cards[BIOWEAPONRD].text = "\n|   [DRAW 1 CARD WHEN PUT INTO DISCARD PILE]";

    cards[BIOWEAPONRD].cost = 3;
    cards[BIOWEAPONRD].value = 4;
    cards[BIOWEAPONRD].onDeath.push_back(abilityDrawCards<1>);
    //______________________________
    cards[BARGAIN].name = "Bargain";
    cards[BARGAIN].type = Card::TACTIC;
    cards[BARGAIN].text = "Increase your funding by clever use of psychotronics.\n|   [GAIN 5 CREDITS IMMEDIATELY]";

    cards[BARGAIN].cost = 3;
    cards[BARGAIN].value = -1;
    cards[BARGAIN].onPlay.push_back(abilityGainCredits<5>);
    //______________________________
    cards[LOGISTICS].name = "Gray Logistics";
    cards[LOGISTICS].type = Card::TACTIC;
    cards[LOGISTICS].text = "Gain access to more war assets using shady transport channels.\n|   [DRAW 2 CARDS]";

    cards[LOGISTICS].cost = 2;
    cards[LOGISTICS].value = -1;
    cards[LOGISTICS].onPlay.push_back(abilityDrawCards<2>);
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
