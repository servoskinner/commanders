#include "gameLogic.h"
#include "playerController.h"

#include <iostream>

int main()
{
    TerminalControl TC1;
    TerminalControl TC2;
    Deck deck(std::vector<Card>(0));

    std::vector<ControllerPtr> controllers = {(ControllerPtr)&TC1, (ControllerPtr)&TC2};
    std::vector<DeckPtr> decks = {(DeckPtr)&deck, (DeckPtr)&deck};

    GameMaster gm(controllers, decks);

    while(true)
    {
        gm.MainLoop();
    }
    
    return 0;
}