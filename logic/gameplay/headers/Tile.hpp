#ifndef _INCLUDE_TILE_HPP
#define _INCLUDE_TILE_HPP

#include "Game_master.hpp"

class Game_master::Tile
{
public: // _____________________________________________________________________________
    Tile(Vector2i pos = {-1, -1}, int type = NORMAL) : pos(pos), type(type), occupier() {}

    Vector2i pos;
    int type;

    std::optional<Card> occupier; /// The card that was placed on tile.
};

#endif