#include "GUI.h"
#include "gameLogic.h"
#include "playerController.h"
#include "Roster.h"

#include <thread>
#include <mutex>
#include <iostream>

void GameLoop(GameMaster& gm)
{
    while(gm.mainLoop()); 
}

void GUILoop(GUI& gui)
{
    while(gui.isActive())
    {
        gui.draw();
        while(gui.pollEvent())
        {
            std::array<int, 2> coords = gui.checkGridHit();
            // std::cout << coords[0] << " " << coords[1] << std::endl;

            if(coords[0] >= 0)
                gui.renderCard(coords[0], coords[1]);

            if(gui.checkPassHit())
            {   
                gui.pushHand();
                gui.updateCompactPositions();
                gui.playerFunds++;
                gui.updateText();
            }

            if(gui.checkMenuHit())
            {
                gui.clear();
                gui.playerDiscardSize++;
                gui.updateText();
            }
        }
    }
}

int main()
{
    Roster roster = Roster();

    TerminalControl TC1;
    TerminalControl TC2;

    Deck deck1({
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::ARCHON),
        roster.getOrigin(Roster::ARCHON),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::FISSION),
        roster.getOrigin(Roster::FISSION),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BIOWEAPONRD),
        roster.getOrigin(Roster::BIOWEAPONRD),
        roster.getOrigin(Roster::UNITANK),
        roster.getOrigin(Roster::UNITANK),
        roster.getOrigin(Roster::UNITANK)
        });

    Deck deck2({
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::BOUNTYHUNTER),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::HENCHMAN),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::COMMANDO),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::MAIMBOT),
        roster.getOrigin(Roster::ARCHON),
        roster.getOrigin(Roster::ARCHON),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::OPPRESSOR),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::GUNKFOOD),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::MACHINEPARTS),
        roster.getOrigin(Roster::FISSION),
        roster.getOrigin(Roster::FISSION),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::LOGISTICS),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BARGAIN),
        roster.getOrigin(Roster::BIOWEAPONRD),
        roster.getOrigin(Roster::BIOWEAPONRD),
        roster.getOrigin(Roster::UNITANK),
        roster.getOrigin(Roster::UNITANK),
        roster.getOrigin(Roster::UNITANK)
        });

    std::vector<PlayerController*> controllers = {&TC1, &TC2};
    std::vector<Deck*> decks = {&deck1, &deck2};

    GameMaster gm(controllers, decks, roster);
    GUI gui;

    std::thread GUIThread([&]{ GUILoop(gui);});
    std::thread masterThread([&]{ GameLoop(gm);});

    GUIThread.join();
    masterThread.join();

    return 0;
}