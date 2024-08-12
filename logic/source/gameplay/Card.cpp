#include "Game_master.hpp"

#include <stdexcept>
#include <iostream>

#include "Misc_functions.hpp"
class Game_master;

Game_master::Card::Card(int id, int oid) : entity_id(), card_id(id), owner_id(oid)
{
    // Initialize abilities
    switch (card_id)
    {
        case BOUNTYHUNTER:
        case HENCHMAN:
        case COMMANDO:
        case OPPRESSOR:
        case MAIMBOT:
        case ARCHON:
        case GUNKFOOD:
        case MACHINEPARTS:
        case FISSION:
        break;
    }

    reset();
}

void Game_master::Card::reset() 
{
    can_attack = false;
    can_move = false;
    is_overwhelmed = false;
    x = -1, y = -1;

    status = CSTATUS_UNDEFINED;

    switch (card_id)
    {
    case BOUNTYHUNTER:
        type = CTYPE_UNIT;

        cost = 1;
        value = 2;
        break;
    case HENCHMAN:
        type = CTYPE_UNIT;

        cost = 2;
        value = 3;
        break;
    case COMMANDO:
        type = CTYPE_UNIT;

        cost = 3;
        value = 4;
        break;
    case OPPRESSOR:
        type = CTYPE_UNIT;

        cost = 4;
        value = 5;
        break;
    case MAIMBOT:
        type = CTYPE_UNIT;

        cost = 4;
        value = 3;
        advantage = 1;
        break;
    case ARCHON:
        type = CTYPE_UNIT;

        cost = 7;
        value = 5;
        advantage = 1;
        break;
    case GUNKFOOD:
        type = CTYPE_CONTRACT;

        cost = 1;
        value = 4; 
    case MACHINEPARTS:
        type = CTYPE_CONTRACT;

        cost = 3;
        value = 7;
        break;
    case FISSION:
        type = CTYPE_CONTRACT;

        cost = 5;
        value = 10;
        break;
    }
}

Commander::Card_info Game_master::Card::get_info()
{
    Commander::Card_info info;

    info.owner_id = owner_id;
    info.card_id = card_id;
    info.entity_id = entity_id.get_id();

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