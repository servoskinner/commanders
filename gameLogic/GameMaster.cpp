#include "gameLogic.h"

GameMaster::GameMaster(int gridHeight = 6, int gridWidth = 8)
{
    players = {};
    playerControllers = {};

    grid = std::vector<std::vector<Tile>>(gridHeight);

    for(int row = 0; row < gridHeight; row++)
        grid[row] = std::vector<Tile>(gridWidth);

    deployZones = {}; //TBA
    captureZone = {};
}