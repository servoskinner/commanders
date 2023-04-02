#include "gameLogic.h"

#include <iostream>

int printUI(const GameMaster& gm);
void higlightTileBold(std::string &buffer, int width, int height, int x, int y);

int main()
{
    GameMaster master = GameMaster();
    Card littleGuy = Card();
    littleGuy.name = "test";

    master.grid[0][1].card = &littleGuy;
    
    printUI(master);

    return 0;
}