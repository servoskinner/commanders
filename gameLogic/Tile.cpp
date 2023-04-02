#include "gameLogic.h"

Tile::Tile(GameMaster& new_master, int new_x, int new_y) : master(new_master)
{
    x = new_x;
    y = new_y;
    int type = Tile::NORMAL;
    card = NULL;
}

std::vector<Tile*> Tile::getAdjacent()
{
    std::vector<Tile*> res = std::vector<Tile*>(4);

    if(x > 0) res[UP] = &(master.grid[x-1][y]);
    if(y < master.grid[0].size() - 1) res[RIGHT] = &(master.grid[x][y+1]);
    if(x < master.grid.size() - 1) res[DOWN] = &(master.grid[x+1][y]);
    if(y > 0) res[LEFT] = &(master.grid[x][y-1]);

    return res;
}

std::vector<Tile*> Tile::getSurrounding()
{
    std::vector<Tile*> res = std::vector<Tile*>(8);

    if(x > 0)
    {
        res[UP] = &(master.grid[x-1][y]);

        if(y > 0) res[UPLEFT] = &(master.grid[x-1][y-1]);
        if(y < master.grid[0].size() - 1) res[UPRIGHT] = &(master.grid[x-1][y+1]);
    }

    if(x < master.grid.size() - 1)
    {
        res[DOWN] = &(master.grid[x+1][y]);

        if(y > 0) res[DOWNLEFT] = &(master.grid[x+1][y-1]);
        if(y < master.grid[0].size() - 1) res[DOWNRIGHT] = &(master.grid[x+1][y+1]);
    }

    if(y > 0) res[LEFT] = &(master.grid[x][y-1]);
    if(y < master.grid[0].size() - 1) res[RIGHT] = &(master.grid[x][y+1]);

    return res;
}
