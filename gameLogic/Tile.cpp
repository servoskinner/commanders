#include "gameLogic.h"

Tile::Tile(GameMaster& new_master, int new_x, int new_y) : master(new_master)
{
    x = new_x;
    y = new_y;
    int type = Tile::NORMAL;
    card = NULL;
}

std::vector<TilePtr> Tile::getAdjacent()    //Get 4-neighbourhood of the tile.
{
    std::vector<TilePtr> res = std::vector<TilePtr>(4);

    if(x > 0)                           res[UP]    = (TilePtr)&(master.grid[x-1][y]);
    if(y < master.grid[0].size() - 1)   res[RIGHT] = (TilePtr)&(master.grid[x][y+1]);
    if(x < master.grid.size() - 1)      res[DOWN]  = (TilePtr)&(master.grid[x+1][y]);
    if(y > 0)                           res[LEFT]  = (TilePtr)&(master.grid[x][y-1]);

    return res;
}

std::vector<TilePtr> Tile::getSurrounding() //Get 8-neighbourhood of the tile.
{
    std::vector<TilePtr> res = std::vector<TilePtr>(8);
    
    if(x > 0)
    {
        res[UP] = (TilePtr)&(master.grid[x-1][y]);

        if(y > 0)                           res[UPLEFT]    = (TilePtr)&(master.grid[x-1][y-1]);
        if(y < master.grid[0].size() - 1)   res[UPRIGHT]   = (TilePtr)&(master.grid[x-1][y+1]);
    }

    if(x < master.grid.size() - 1)
    {
        res[DOWN] = (TilePtr)&(master.grid[x+1][y]);

        if(y > 0)                           res[DOWNLEFT]  = (TilePtr)&(master.grid[x+1][y-1]);
        if(y < master.grid[0].size() - 1)   res[DOWNRIGHT] = (TilePtr)&(master.grid[x+1][y+1]);
    }

    if(y > 0)                               res[LEFT]      = (TilePtr)&(master.grid[x][y-1]);
    if(y < master.grid[0].size() - 1)       res[RIGHT]     = (TilePtr)&(master.grid[x][y+1]);

    return res;
}
