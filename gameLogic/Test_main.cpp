#include "gameLogic.h"
#include "playerController.h"

#include <iostream>

int main()
{
    TerminalControl TC1;
    TerminalControl TC2;
    TerminalControl TC3;

    Card unit_generic2(1), unit_generic3(2), unit_generic4(3);
    Card contract_generic4(4), contract_generic7(7);
    
    unit_generic2.name = "Henchman";
    unit_generic2.value = 2;
    unit_generic2.cost = 1;
    
    unit_generic3.name = "Soldier";
    unit_generic3.value = 3;
    unit_generic3.cost = 2;

    unit_generic4.name = "Commando";
    unit_generic4.value = 4;
    unit_generic4.cost = 3;

    contract_generic4.name = "Gunkfood";
    contract_generic4.value = 4;
    contract_generic4.cost = 1;
    contract_generic4.type = Card::CONTRACT;

    contract_generic7.name = "Machine parts";
    contract_generic7.value = 7;
    contract_generic7.cost = 3;
    contract_generic7.type = Card::CONTRACT;

    Deck deck1({unit_generic2, unit_generic2, unit_generic3, unit_generic3, unit_generic4, \
                contract_generic7, contract_generic4});
    Deck deck2({unit_generic2, unit_generic3, unit_generic3, unit_generic4, unit_generic4, \
                contract_generic4, contract_generic4});

    std::vector<PlayerController*> controllers = {&TC1, &TC2};
    std::vector<Deck*> decks = {&deck1, &deck2};

    GameMaster gm(controllers, decks);

    while(true)
    {
        gm.mainLoop();
    }
    
    return 0;
}