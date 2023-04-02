#include "gameLogic.h"

Card::Card()
{
    owner = NULL;
    id = -1;

    playPosition = Card::UNDEFINED;

    playCost = 0;
    value = 0;
    advantage = 0;

    name = "unknown";
    text = "";
}