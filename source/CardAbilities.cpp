#include "gameLogic.h"

template <int quantity>
void abilityDrawCards(GameMaster& gm, Card& activator)
{
    for(int i = 0; i < quantity; i++)
        gm.forceDraw(activator.ownerId);
}

template <int quantity>
void abilityGainCredits(GameMaster& gm, Card& activator)
{
    gm.players[activator.ownerId].funds += quantity;
}

