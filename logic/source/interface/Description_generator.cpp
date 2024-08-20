#include "Description_generator.hpp"

Description_generator::Card_descr Description_generator::get_card_instance(int index) // instantiate all cards in game 
{
    Card_descr card_desc = {};
    switch (index)
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
    card_desc.flavor_text = "Advanced augmentoid infantry. Kicks more ass than average.";

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
    case MACHINEPARTS:
    card_desc.name = "Machine Parts";
    card_desc.type = CTYPE_CONTRACT;
    card_desc.flavor_text = "Produces the nerve and bone of our age.";

    card_desc.cost = 4;
    card_desc.value = 6;
    card_desc.mnemosprite = {1, 1, 1,
                             1, 0, 0,
                             1, 1, 1,
                             0, 0, 1,
                             1, 1, 1};
    break;
    //______________________________
    case GUNKFOOD:
    card_desc.name = "Gunkfood";
    card_desc.type = CTYPE_CONTRACT;
    card_desc.flavor_text = "Synthetic food. It's got electrolytes.";

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
    card_desc.type = CTYPE_CONTRACT;
    card_desc.flavor_text = "Explosive expert with grave lack of fingers.";
    card_desc.ability_text = "Advantage -1";

    card_desc.cost = 2;
    card_desc.value = 6;
    card_desc.mnemosprite = {1, 1, 1,
                             0, 0, 1,
                             1, 0, 1,
                             1, 0, 0,
                             1, 0, 0};
    break;
    }
    return card_desc;
}
