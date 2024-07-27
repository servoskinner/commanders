#include "Card_generator.hpp"

Game_master::Card_generator::Card_generator() // instantiate all cards in game 
{
    for(int i = 0; i < TOTAL_CARDS_IN_GAME; i++)
        cards[i].global_id = i;
    //______________________________
    cards[BOUNTYHUNTER].type = CTYPE_UNIT;

    cards[BOUNTYHUNTER].cost = 1;
    cards[BOUNTYHUNTER].value = 2;
    //______________________________
    cards[HENCHMAN].type = CTYPE_UNIT;

    cards[HENCHMAN].cost = 2;
    cards[HENCHMAN].value = 3;
    //______________________________
    cards[COMMANDO].type = CTYPE_UNIT;

    cards[COMMANDO].cost = 3;
    cards[COMMANDO].value = 4;
    //______________________________
    cards[OPPRESSOR].type = CTYPE_UNIT;

    cards[OPPRESSOR].cost = 4;
    cards[OPPRESSOR].value = 5;
    //______________________________
    cards[MAIMBOT].type = CTYPE_UNIT;

    cards[MAIMBOT].cost = 4;
    cards[MAIMBOT].value = 3;
    //______________________________
    cards[ARCHON].type = CTYPE_UNIT;

    cards[ARCHON].cost = 7;
    cards[ARCHON].value = 5;
    //______________________________
    cards[MACHINEPARTS].type = CTYPE_CONTRACT;

    cards[MACHINEPARTS].cost = 3;
    cards[MACHINEPARTS].value = 7;
    //______________________________
    cards[GUNKFOOD].type = CTYPE_CONTRACT;

    cards[GUNKFOOD].cost = 1;
    cards[GUNKFOOD].value = 4; 
    //______________________________
    cards[FISSION].type = CTYPE_CONTRACT;

    cards[FISSION].cost = 5;
    cards[FISSION].value = 10;
    //______________________________
    // cards[UNITANK].name = "Unicyclone";
    // cards[UNITANK].type = CTYPE_UNIT;
    // cards[UNITANK].text = "A unicycle tank.\n|   [GAIN $3 WHEN DESTROYED]";

    // cards[UNITANK].cost = 4;
    // cards[UNITANK].value = 3;
    // cards[UNITANK].trig_destroyed.push_back(ability_gain_credits<3>);
    //______________________________
    // cards[BIOWEAPONRD].name = "Bioweapons R&D";
    // cards[BIOWEAPONRD].type = Card::CONTRACT;
    // cards[BIOWEAPONRD].text = "\n|   [DRAW 1 CARD WHEN PUT INTO DISCARD PILE]";

    // cards[BIOWEAPONRD].cost = 3;
    // cards[BIOWEAPONRD].value = 4;
    // cards[BIOWEAPONRD].trig_destroyed.push_back(ability_draw<1>);
    //______________________________
    // cards[BARGAIN].name = "Bargain";
    // cards[BARGAIN].type = Card::TACTIC;
    // cards[BARGAIN].text = "Increase your funding by clever use of psychotronics.\n|   [GAIN 5 CREDITS IMMEDIATELY]";

    // cards[BARGAIN].cost = 3;
    // cards[BARGAIN].value = -1;
    // cards[BARGAIN].trig_played.push_back(ability_gain_credits<5>);
    //______________________________
    // cards[LOGISTICS].name = "Gray Logistics";
    // cards[LOGISTICS].type = Card::TACTIC;
    // cards[LOGISTICS].text = "Gain access to more war assets using shady transport channels.\n|   [DRAW 2 CARDS]";

    // cards[LOGISTICS].cost = 2;
    // cards[LOGISTICS].value = -1;
    // cards[LOGISTICS].trig_played.push_back(ability_draw<2>);
}
Game_master::Card_generator& Game_master::Card_generator::get() // Get active singleton instance of Description_generator;
{
    static std::unique_ptr<Card_generator> instance{new Card_generator};
    return *instance;
}
