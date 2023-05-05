#include "gameLogic.h"
#include "playerController.h"
#include "CardRoster.h"

#include <iostream>

int main()
{
    Roster roster = Roster();

    TerminalControl TC1;
    TerminalControl TC2;

    Deck deck1({
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::ARCHON),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::FISSION)
        });

    Deck deck2({
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::ARCHON),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::FISSION)
        });

    std::vector<PlayerController*> controllers = {&TC1, &TC2};
    std::vector<Deck*> decks = {&deck1, &deck2};

    GameMaster gm(controllers, decks);

    while(true)
    {
        gm.mainLoop();
    }
    
    return 0;
}