#include "GUI.h"
#include <iostream>

int main()
{
    GUI gui;
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