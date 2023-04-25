#include "gameLogic.h"
#include "playerController.h"

#include <iostream>

int main()
{
    TerminalControl TC;
    Deck deck(std::vector<Card>(0));

    std::vector<ControllerPtr> controllers = {(ControllerPtr)&TC, (ControllerPtr)&TC};
    std::vector<DeckPtr> decks = {(DeckPtr)&deck, (DeckPtr)&deck};

    GameMaster gm(controllers, decks);

    while(true)
    {
        gm.MainLoop();
    }
    
    return 0;
}