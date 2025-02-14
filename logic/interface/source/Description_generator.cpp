#include "Description_generator.hpp"

Description_generator::Card_descr Description_generator::get_desc(int card_id) // instantiate all cards in game 
{
    Card_descr card_desc = {};
    
    switch (card_id)
    {
    //______________________________
    case BOUNTYHUNTER:
        card_desc.name = "Small-Time Merc";
        card_desc.type = CTYPE_UNIT;
        card_desc.flavor_text = "Idiots we hired to fight for pocket money.";

        card_desc.cost = 1;
        card_desc.value = 2;
        card_desc.mnemosprite = {0, 0, 0,
                                 1, 1, 1,
                                 1, 0, 1,
                                 0, 1, 0,
                                 1, 0, 0};
    break;
    //______________________________
    case TERRORGUARD:
    card_desc.name = "Terrorguard";
    card_desc.type = CTYPE_UNIT;
    card_desc.flavor_text = "Advanced cyborg infantry. Kicks more ass than average.";

    card_desc.cost = 3;
    card_desc.value = 5;
    card_desc.mnemosprite = {1, 1, 1,
                             0, 0, 1,
                             1, 1, 1,
                             1, 1, 0,
                             0, 1, 0};
    break;
    //______________________________
    case MEGALO:
    card_desc.name = "Megalotrooper";
    card_desc.type = CTYPE_UNIT;
    card_desc.flavor_text = "Superheavy assault unit. Ideal for shock-and-awe tactics.";

    card_desc.cost = 5;
    card_desc.value = 8;
    card_desc.mnemosprite = {0, 1, 1,
                             1, 1, 1,
                             1, 1, 1,
                             1, 0, 0,
                             0, 1, 0};
    break;
    //______________________________
    case MAIMDROID:
    card_desc.name = "Maimdroid";
    card_desc.type = CTYPE_UNIT;
    card_desc.flavor_text = "Autonomous weapons platform.";
    card_desc.ability_text = "Advantage 1";

    card_desc.cost = 3;
    card_desc.value = 3;
    card_desc.mnemosprite = {0, 1, 1,
                             1, 1, 0,
                             0, 1, 1,
                             1, 0, 1,
                             1, 0, 0};
    break;
    //______________________________
    case OMNITANK:
    card_desc.name = "Omnitank";
    card_desc.type = CTYPE_UNIT;
    card_desc.flavor_text = "High-performance Death Mech. Banned in 43 countries.";
    card_desc.ability_text = "Advantage 1";

    card_desc.cost = 7;
    card_desc.value = 5;
    card_desc.mnemosprite = {1, 0, 1,
                             1, 1, 0,
                             1, 1, 1,
                             1, 0, 1,
                             1, 0, 1};
    break;
    //______________________________
    case MACHINESHOP:
    card_desc.name = "Machine Shop";
    card_desc.type = CTYPE_CONTRACT;
    card_desc.flavor_text = "Wire and steel are the new flesh and bone.";

    card_desc.cost = 4;
    card_desc.value = 6;
    card_desc.mnemosprite = {1, 1, 1,
                             1, 0, 0,
                             1, 1, 1,
                             0, 0, 1,
                             1, 1, 1};
    break;
    //______________________________
    case PEPTANE:
    card_desc.name = "Peptane Fields";
    card_desc.type = CTYPE_CONTRACT;
    card_desc.flavor_text = "Bacterial fuel. It's got electrolytes.";

    card_desc.cost = 2;
    card_desc.value = 3; 
    card_desc.mnemosprite = {0, 0, 1,
                             0, 1, 1,
                             0, 1, 1,
                             0, 0, 1,
                             1, 1, 0};
    break;
    //______________________________
    case FISSION:
    card_desc.name = "Nuclear Fission";
    card_desc.type = CTYPE_CONTRACT;
    card_desc.flavor_text = "Get pure energy from thin air.";

    card_desc.cost = 6;
    card_desc.value = 9;
    card_desc.mnemosprite = {1, 0, 0,
                             1, 1, 0,
                             0, 0, 1,
                             0, 1, 1,
                             1, 1, 1};
    break;
    //______________________________
    case DEMOLITIONIST:
    card_desc.name = "Trashpunk Brute";
    card_desc.type = CTYPE_UNIT;
    card_desc.flavor_text = "Explosives expert with grave lack of fingers.";
    card_desc.ability_text = "Advantage -1";

    card_desc.cost = 2;
    card_desc.value = 6;
    card_desc.mnemosprite = {1, 1, 1,
                             0, 0, 1,
                             1, 0, 1,
                             1, 0, 0,
                             1, 0, 0};
    break;
    //______________________________
    case UNICYCLONE:
    card_desc.name = "Unicyclone";
    card_desc.type = CTYPE_UNIT;
    card_desc.flavor_text = "Unicycle tank.";
    card_desc.ability_text = "When destroyed, gain #2.";

    card_desc.cost = 3;
    card_desc.value = 3;
    card_desc.mnemosprite = {1, 1, 1,
                             1, 0, 0,
                             0, 1, 0,
                             0, 0, 1,
                             1, 0, 0};
    break;
    //______________________________
    case LOGISTICS:
    card_desc.name = "Gray Logistics";
    card_desc.type = CTYPE_TACTIC;
    card_desc.flavor_text = "Acquire state-of-the-art weaponry by questionable means.";
    card_desc.ability_text = "Draw three cards.";

    card_desc.cost = 1;
    card_desc.mnemosprite = {0, 0, 1,
                             1, 0, 0,
                             1, 0, 0,
                             1, 0, 0,
                             1, 1, 1};
    break;
    }
    return card_desc;
}
