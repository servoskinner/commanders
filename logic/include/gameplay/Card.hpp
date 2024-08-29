#pragma once

#include "Game_master.hpp"
#include "Unique.hpp"
#include "Ability.hpp"

class Game_master::Card
{
public: // _____________________________________________________________________________
    Card(Game_master& master, int id, int oid = -1);
    void reset();

    // Identification
    const int owner_id;
    const int card_id;
    int controller_id;

    Unique entity_id;

    int type;
    enum card_status
    {
        CSTATUS_UNDEFINED = -1,
        CSTATUS_LIBRARY = 0,
        CSTATUS_HAND = 1,
        CSTATUS_BATTLEFIELD = 2,
        CSTATUS_GRAVEYARD = 3
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

    std::list<std::unique_ptr<Ability>> abilities;
    // Event triggers
    Trigger enters_play;
    Trigger leaves_play;

    Trigger turn_start;
    Trigger turn_end;

    Trigger before_move;
    Trigger after_move;

    Trigger before_attack;
    Trigger after_attack;
    Trigger before_attacked;
    Trigger after_attacked;
};