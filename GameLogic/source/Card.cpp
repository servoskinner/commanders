#include "game_logic.hpp"

#include <stdexcept>
#include <iostream>

#include "misc.hpp"

Card::Card(int gid, int mid)
{
    match_id = mid;
    global_id = gid;
    owner_id = -1;

    value = 0;
    advantage = 0;
    cost = 0;

    type = Card::UNIT;
    status = Card::UNDEFINED; 
    x = -1, y = -1;

    name = "unknown";
    text = "";

    can_attack = false;
    can_move = false;
    is_overwhelmed = false;
}

Card& Card::operator=(const Card& other)
{
    //Status, ID and ownerID stay the same;
    type = other.type;

    x = other.x;
    y = other.y;

    name = other.name;
    text = other.text;

    cost = other.cost;
    value = other.value;
    advantage = other.advantage;
    
    trig_ability = other.trig_ability;
    trig_played = other.trig_played;
    trig_destroyed = other.trig_destroyed;

    return *this;
}

Card_info Card::get_info()
{
    Card_info info;

    info.owner_id = owner_id;
    info.global_id = global_id;
    info.match_id = match_id;

    // these can be stored on player's side
    info.name = name;
    info.text = text;

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
