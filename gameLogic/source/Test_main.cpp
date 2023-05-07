#include "gameLogic.h"
#include "playerController.h"
#include "Roster.h"

#include <iostream>

int main()
{
    Roster roster = Roster();

    TerminalControl TC1;
    TerminalControl TC2;

    Deck deck1({
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BIOWEAPONRD),
        roster.getOrigin(Roster::UNITANK),
        roster.getOrigin(Roster::UNITANK)
        });

    Deck deck2({
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BIOWEAPONRD),
        roster.getOrigin(Roster::UNITANK),
        roster.getOrigin(Roster::UNITANK)
        });

    std::vector<PlayerController*> controllers = {&TC1, &TC2};
    std::vector<Deck*> decks = {&deck1, &deck2};

    GameMaster gm(controllers, decks, roster);

    while(gm.mainLoop());
    
    return 0;
}