#include "gameLogic.h"

Player::Player(Deck& ndeck, int id) : deck(ndeck)
{
    playerId = id;
}