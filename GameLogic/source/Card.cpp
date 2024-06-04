#include "Game_master.hpp"

#include <stdexcept>
#include <iostream>

#include "Misc_functions.hpp"
class GameMaster;

Game_master::Card::Card(int gid, int mid)
{
    match_id = mid;
    global_id = gid;
    owner_id = -1;

    value = 0;
    advantage = 0;
    cost = 0;

    type = CTYPE_UNIT;
    status = Card::CSTATUS_UNDEFINED; 
    x = -1, y = -1;

    can_attack = false;
    can_move = false;
    is_overwhelmed = false;
}

Commander::Card_info Game_master::Card::get_info()
{
    Commander::Card_info info;

    info.owner_id = owner_id;
    info.global_id = global_id;
    info.match_id = match_id;

    info.x = x, info.y = y;

    info.value = value;
    info.cost = cost;
    info.advantage = advantage;
    info.type = type;

    info.can_attack = can_attack;
    info.can_move = can_move;
    info.is_overwhelmed = is_overwhelmed;

    return info;
}

Game_master::Card& Game_master::Card::operator=(const Game_master::Card& other)
{
    // keep match_id and abilities
    global_id = other.global_id;
    
    cost = other.cost;
    value = other.value;
    advantage = other.advantage;

    trig_ability = other.trig_ability;
    trig_played = other.trig_played;
    trig_destroyed = other.trig_destroyed;
    
    return *this;
}
