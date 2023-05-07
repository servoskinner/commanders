#include "gameLogic.h"

#include <stdexcept>
#include <iostream>

#include "misc.h"

Card::Card(int nid)
{
    id = nid;
    ownerId = -1;

    value = 0;
    advantage = 0;
    cost = 0;

    type = Card::UNIT;
    status = Card::UNDEFINED;
    x = -1, y = -1;

    name = "unknown";
    text = "";

    canAttack = false;
    canMove = false;
    isOverwhelmed = false;
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
    
    onAbilityActivate = other.onAbilityActivate;
    onPlay = other.onPlay;
    onDeath = other.onDeath;

    return *this;
}

CardInfo Card::getInfo()
{
    CardInfo info;

    info.ownerId = ownerId;
    info.name = name;
    info.text = text;

    info.x = x, info.y = y;
    info.value = value;
    info.cost = cost;
    info.advantage = advantage;
    info.type = type;

    info.canAttack = canAttack;
    info.canMove = canMove;
    info.isOverwhelmed = isOverwhelmed;

    return info;
}
