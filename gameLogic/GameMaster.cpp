#include "gameLogic.h"

GameMaster::GameMaster(int gridHeight, int gridWidth)
{
    //players = {};
    //playerControllers = {};

    grid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth, Tile(*this)));

    for(int row = 0; row < gridHeight; row++)
        for(int column = 0; column < gridWidth; column ++)
            grid[row][column].x = column, grid[row][column].y = row;

    // deployZones = {}; //TBA
    // captureZone = {};
}