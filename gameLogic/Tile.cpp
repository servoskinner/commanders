#include "gameLogic.h"

Tile::Tile(GameMaster& new_master, int new_x, int new_y)
{
    master = new_master;
    x = new_x;
    y = new_y;
    int type = Tile::NORMAL;
    card = NULL;
}

// std::vector<Tile&> Tile::getAdjacent();
// std::vector<Tile&> Tile::getSurrounding();
