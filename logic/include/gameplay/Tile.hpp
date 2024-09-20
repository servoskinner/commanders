#pragma once

#include "Game_master.hpp"

class Game_master::Tile
{
public: // _____________________________________________________________________________
    Tile(int nx = -1, int ny = -1, int ntype = NORMAL) : x(nx), y(ny), type(ntype), card() {}

    int x, y;
    int type;

    enum Tile_neighbor
    {
        UP,
        RIGHT,
        DOWN,
        LEFT,
        UPRIGHT,
        DOWNRIGHT,
        DOWNLEFT,
        UPLEFT
    };
    enum Tile_type
    {
        NORMAL      = -1,
        OBJECTIVE   = -2,
        // Special terrain has too much impact on gameplay and its use is discouraged
        OBSTACLE    = -3,
        NO_DEPLOY   = -4,
        TERR_ADV    = -5,
        TERR_DISADV = -6
    }; // Non-negatives refer to deploy zones of players with same IDs

    std::optional<Card_ref> card; /// The card that was placed on tile.
};