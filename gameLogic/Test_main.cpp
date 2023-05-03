#include "gameLogic.h"
#include "playerController.h"

#include <iostream>

int main()
{
    TerminalControl TC1;
    TerminalControl TC2;
    TerminalControl TC3;

    Card card1(1), card2(2), card3(3);
    
    card1.name = "one";
    card1.value = 4;
    card1.cost = 0;
    
    card2.name = "two";
    card2.value = 3;
    card2.cost = 0;

    card3.name = "three";
    card3.value = 2;
    card3.cost = 0;

    Deck deck1({card1}), deck2({card2}), deck3({card3});

    std::vector<PlayerController*> controllers = {&TC1, &TC2, &TC3};
    std::vector<Deck> decks = {deck1, deck2, deck3};

    GameMaster gm(controllers, decks);

    gm.deployCard(gm.decks[0].roster[0], &gm.grid[3][3]);
    gm.deployCard(gm.decks[1].roster[0], &gm.grid[5][0]);
    gm.deployCard(gm.decks[2].roster[0], &gm.grid[2][6]);
    //TODO check permission validations

    while(true)
    {
        gm.mainLoop();
    }
    
    return 0;
}