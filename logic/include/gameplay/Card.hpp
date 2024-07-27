#pragma once

#include "Game_master.hpp"

class Game_master::Card
{
public: // _____________________________________________________________________________
    Card(int gid = -1, int mid = -1);
    Card& operator=(const Card& other);

    // Identification
    int owner_id;
    // int controller_id;
    int global_id;
    int match_id;

    int type;
    enum card_status
    {
        CSTATUS_UNDEFINED = -1,
        CSTATUS_IN_DECK = 0,
        CSTATUS_IN_HAND = 1,
        CSTATUS_IN_PLAY = 2,
        CSTATUS_DISCARD = 3
    };
    int status;

    Commander::Card_info get_info();
    // Gameplay
    int x, y;

    int cost;
    int value;
    int advantage;
    // Status effects
    bool can_attack;
    bool can_move;
    bool is_overwhelmed;
    // Event triggers
    std::vector<std::function<void(void)>> trig_ability;
    std::vector<std::function<void(void)>> trig_played;
    std::vector<std::function<void(void)>> trig_destroyed;

    // std::vector<std::function<void(Game_master&, Card&)>> trig_turn_start;
    // std::vector<std::function<void(Game_master&, Card&)>> trig_turn_end;

    // std::vector<std::function<void(Tile&)>> trig_moved; // Tile& destination
    // std::vector<std::function<void(Tile&, int)>> trig_attacks; // Tile& target, int damageDone
    // std::vector<std::function<void(Game_master&, Card&, Card&)>> trig_destroyed_other; // Card& target
    // std::vector<std::function<void(Card&, int)>> trig_attacked; // Card& damaged_by, int damage_sustained
    //...
};