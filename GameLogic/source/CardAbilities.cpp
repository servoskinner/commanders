#include "gameLogic.h"

template <int quantity>
void ability_draw(Game_master& gm, Card& activator)
{
    for(int i = 0; i < quantity; i++) {
        gm.resolve_draw(activator.owner_id);
    }
}

template <int quantity>
void ability_gain_credits(Game_master& gm, Card& activator)
{
    gm.players[activator.owner_id].funds += quantity;
}

