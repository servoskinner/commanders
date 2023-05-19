#include "GUI.h"
#include "gameLogic.h"
#include "playerController.h"

#include <thread>
#include <mutex>
#include <iostream>

int gameLoop(GameMaster& gm)
{
    while(gm.mainLoop());
}



int main()
{
    
}