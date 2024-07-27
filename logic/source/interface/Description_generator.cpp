#include "Description_generator.hpp"

Description_generator::Description_generator() // instantiate all cards in game 
{
    for(int i = 0; i < TOTAL_CARDS_IN_GAME; i++)
        cards[i].global_id = i;
    //______________________________
    cards[BOUNTYHUNTER].name = "Small-Time Merc";
    cards[BOUNTYHUNTER].type = CTYPE_UNIT;
    cards[BOUNTYHUNTER].flavor_text = "Idiots we hired to fight for pocket money.";

    cards[BOUNTYHUNTER].cost = 1;
    cards[BOUNTYHUNTER].value = 2;
    cards[BOUNTYHUNTER].mnemosprite = {0, 0, 1,
                                       1, 1, 1,
                                       0, 0, 1,
                                       0, 1, 0,
                                       1, 1, 0};
    //______________________________
    cards[HENCHMAN].name = "Henchman";
    cards[HENCHMAN].type = CTYPE_UNIT;
    cards[HENCHMAN].flavor_text = "Standard infantry. Most standard of them all.";

    cards[HENCHMAN].cost = 2;
    cards[HENCHMAN].value = 3;
    cards[HENCHMAN].mnemosprite =     {1, 1, 1,
                                       0, 0, 0,
                                       0, 0, 1,
                                       1, 0, 1,
                                       1, 0, 1};
    //______________________________
    cards[COMMANDO].name = "Terrorguard";
    cards[COMMANDO].type = CTYPE_UNIT;
    cards[COMMANDO].flavor_text = "Advanced augmentoid infantry. Kicks more ass than average.";

    cards[COMMANDO].cost = 3;
    cards[COMMANDO].value = 4;
    cards[COMMANDO].mnemosprite =     {1, 1, 1,
                                       1, 0, 1,
                                       0, 1, 1,
                                       0, 1, 1,
                                       1, 1, 0};
    //______________________________
    cards[OPPRESSOR].name = "Oppressor";
    cards[OPPRESSOR].type = CTYPE_UNIT;
    cards[OPPRESSOR].flavor_text = "Superheavy assault unit. Ideal for shock-and-awe tactics.";

    cards[OPPRESSOR].cost = 4;
    cards[OPPRESSOR].value = 5;
    cards[OPPRESSOR].mnemosprite =    {0, 1, 1,
                                       1, 1, 1,
                                       1, 0, 0,
                                       0, 1, 0,
                                       1, 1, 1};
    //______________________________
    cards[MAIMBOT].name = "Maimbot";
    cards[MAIMBOT].type = CTYPE_UNIT;
    cards[MAIMBOT].flavor_text = "Autonomous weapons platform.";
    cards[MAIMBOT].ability_text = "Advantage 1";

    cards[MAIMBOT].cost = 4;
    cards[MAIMBOT].value = 3;
    cards[MAIMBOT].mnemosprite =      {1, 1, 1,
                                       1, 1, 0,
                                       0, 1, 1,
                                       1, 1, 0,
                                       1, 0, 0};
    //______________________________
    cards[ARCHON].name = "Omnitank";
    cards[ARCHON].type = CTYPE_UNIT;
    cards[ARCHON].flavor_text = "High-performance Death Mech. Banned in 43 countries.";
    cards[ARCHON].ability_text = "Advantage 1";

    cards[ARCHON].cost = 7;
    cards[ARCHON].value = 5;
    cards[ARCHON].mnemosprite =       {1, 0, 1,
                                       1, 1, 0,
                                       1, 1, 1,
                                       1, 0, 1,
                                       1, 0, 1};
    //______________________________
    cards[MACHINEPARTS].name = "Machine Shop";
    cards[MACHINEPARTS].type = CTYPE_CONTRACT;
    cards[MACHINEPARTS].flavor_text = "Produces the nerve and bone of our age.";

    cards[MACHINEPARTS].cost = 3;
    cards[MACHINEPARTS].value = 7;
    cards[MACHINEPARTS].mnemosprite = {1, 1, 0,
                                       1, 1, 1,
                                       0, 0, 0,
                                       1, 1, 0,
                                       0, 1, 0};
    //______________________________
    cards[GUNKFOOD].name = "Gunkfood";
    cards[GUNKFOOD].type = CTYPE_CONTRACT;
    cards[GUNKFOOD].flavor_text = "Synthetic food. It's got electrolytes.";

    cards[GUNKFOOD].cost = 1;
    cards[GUNKFOOD].value = 4; 
    cards[GUNKFOOD].mnemosprite =     {0, 0, 1,
                                       1, 0, 0,
                                       1, 1, 0,
                                       1, 1, 0,
                                       0, 1, 0};
    //______________________________
    cards[FISSION].name = "Nuclear Fission";
    cards[FISSION].type = CTYPE_CONTRACT;
    cards[FISSION].flavor_text = "Get pure energy from thin air.";

    cards[FISSION].cost = 5;
    cards[FISSION].value = 10;
    cards[FISSION].mnemosprite =      {1, 1, 1,
                                       0, 1, 1,
                                       0, 0, 1,
                                       1, 1, 0,
                                       1, 0, 0};
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
Description_generator& Description_generator::get() // Get active singleton instance of Description_generator;
{
    static std::unique_ptr<Description_generator> instance{new Description_generator};
    return *instance;
}
