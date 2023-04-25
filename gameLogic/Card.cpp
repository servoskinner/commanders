#include "gameLogic.h"

#include <stdexcept>
#include <iostream>

Card::Card()
{
    owner = nullptr;
    id = -1;

    type = Card::UNIT;
    playPosition = Card::UNDEFINED;
    gridPosition = NULL;

    name = "unknown";
    text = "";
}

bool Card::Move(int direction)
{   
    //
    if(direction < 0 || direction > 3) throw std::invalid_argument("invalid direction");
    if(!gridPosition) throw std::runtime_error("Trying to move an off-grid card");

    auto options = gridPosition->getAdjacent();

    //TODO add mutual exchange movement option
    // Check if tile is occupied
    if(!options[direction] || options[direction]->card)
        return false;

    // Exchange positions
    options[direction]->card = (CardPtr)this;
    this->gridPosition->card = NULL;
    this->gridPosition = options[direction];

    canMove = false;

    return true;
}

bool Card::Deploy(TilePtr destination)
{
    if(gridPosition) throw std::runtime_error("Card has been already deployed");
    if(playPosition == IN_PLAY) std::clog << "WARNING: Deploying card marked as \"IN PLAY\"";

    if(destination)
    {
        if(destination->card)
            return false; 
        
        destination->card = (CardPtr)this;
        gridPosition = destination;
    }
    //TODO: fire events
    playPosition = IN_PLAY;
    return true;
}

void Card::Kill()
{
    if(gridPosition)
    {
        gridPosition->card = NULL;
        gridPosition = NULL;
    }
    playPosition = DISCARD;

    //TODO restore
}

bool Card::Attack(int direction)
{
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(!gridPosition) throw std::runtime_error("Trying to attack with an off-grid card");

    auto options = gridPosition->getSurrounding();

    if(!options[direction] || !(options[direction]->card))
        return false;

    //combat damage resolution
    ResolveCombat(*options[direction]->card);
    return true;
}

int Card::ResolveCombat(Card& target)
{
    int deltaAdvantage = advantage - target.advantage;
    //resolve overwhelming mechanic
    if(target.isOverwhelmed) deltaAdvantage++;
    //advantage resolution
    if(deltaAdvantage > 0)
        {
        target.value -= deltaAdvantage;
        if(target.value < 0) target.value = 0;
        }
    else
        {
        value += deltaAdvantage;
        if(value < 0) value = 0;
        }

    canAttack = false;
    canMove   = false;
    target.isOverwhelmed = true;
    //normal damage resolution
    if(value > target.value)
    {
        value -= target.value;
        target.value = 0;
        target.Kill();
        return Card::WIN;
    }
    else if(value < target.value)
    {
        target.value -= value;
        value = 0;
        Kill();
        return Card::LOSE;
    }
    else //value == target.value
    {
        target.Kill();
        Kill();
        return Card::TIE;
    }
}