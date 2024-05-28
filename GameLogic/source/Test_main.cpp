#include "gameLogic.h"
#include "playerController.h"
#include "Roster.h"

#include <iostream>

int main()
{
    Roster& roster = Roster::get();
    CLI_control TC1;
    CLI_control TC2;

    Deck deck1({
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::ARCHON),
        roster.get_card_instance(Roster::ARCHON),
        roster.get_card_instance(Roster::OPPRESSOR),
        roster.get_card_instance(Roster::OPPRESSOR),
        roster.get_card_instance(Roster::GUNKFOOD),
        roster.get_card_instance(Roster::GUNKFOOD),
        roster.get_card_instance(Roster::GUNKFOOD),
        roster.get_card_instance(Roster::MACHINEPARTS),
        roster.get_card_instance(Roster::MACHINEPARTS),
        roster.get_card_instance(Roster::MACHINEPARTS),
        roster.get_card_instance(Roster::FISSION),
        roster.get_card_instance(Roster::FISSION),
        roster.get_card_instance(Roster::LOGISTICS),
        roster.get_card_instance(Roster::LOGISTICS),
        roster.get_card_instance(Roster::LOGISTICS),
        roster.get_card_instance(Roster::BARGAIN),
        roster.get_card_instance(Roster::BARGAIN),
        roster.get_card_instance(Roster::BIOWEAPONRD),
        roster.get_card_instance(Roster::BIOWEAPONRD),
        roster.get_card_instance(Roster::UNITANK),
        roster.get_card_instance(Roster::UNITANK),
        roster.get_card_instance(Roster::UNITANK)
        });

    Deck deck2({
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::BOUNTYHUNTER),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::HENCHMAN),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::COMMANDO),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::MAIMBOT),
        roster.get_card_instance(Roster::ARCHON),
        roster.get_card_instance(Roster::ARCHON),
        roster.get_card_instance(Roster::OPPRESSOR),
        roster.get_card_instance(Roster::OPPRESSOR),
        roster.get_card_instance(Roster::GUNKFOOD),
        roster.get_card_instance(Roster::GUNKFOOD),
        roster.get_card_instance(Roster::GUNKFOOD),
        roster.get_card_instance(Roster::MACHINEPARTS),
        roster.get_card_instance(Roster::MACHINEPARTS),
        roster.get_card_instance(Roster::MACHINEPARTS),
        roster.get_card_instance(Roster::FISSION),
        roster.get_card_instance(Roster::FISSION),
        roster.get_card_instance(Roster::LOGISTICS),
        roster.get_card_instance(Roster::LOGISTICS),
        roster.get_card_instance(Roster::LOGISTICS),
        roster.get_card_instance(Roster::BARGAIN),
        roster.get_card_instance(Roster::BARGAIN),
        roster.get_card_instance(Roster::BIOWEAPONRD),
        roster.get_card_instance(Roster::BIOWEAPONRD),
        roster.get_card_instance(Roster::UNITANK),
        roster.get_card_instance(Roster::UNITANK),
        roster.get_card_instance(Roster::UNITANK)
        });

    std::vector<pctrl_ref> controllers = {std::ref(TC1), std::ref(TC2)};
    std::vector<deck_ref> decks = {std::ref(deck1), std::ref(deck2)};
    deck_ref deck = deck1;

    Game_master gm(controllers, decks);

    while(gm.game_loop());
    
    return 0;
}