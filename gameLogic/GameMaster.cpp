#include "gameLogic.h"
#include "misc.h"

#include <stdexcept>
#include <iostream>

GameMaster::GameMaster(const std::vector<PlayerController*> controllers, const std::vector<Deck>& ndecks)
{
    // Check argument validity
    if(controllers.size() > ndecks.size()) 
        throw std::runtime_error("Insufficient decks specified for game creation;");
    if(controllers.size() < ndecks.size())
        std::clog << "Redundant decks provided for GameMaster constructor. Ignoring last;" << std::endl;

    // Set up variables
    playerControllers = controllers;
    decks = ndecks;

    // Build grid
    grid = std::vector<std::vector<Tile>>(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));

    for(int row = 0; row < GRID_HEIGHT; row++)
        for(int column = 0; column < GRID_WIDTH; column ++)
            grid[row][column] = Tile(row, column, Tile::NORMAL); 

    // Assign IDs to various entities
    for(int playerid = 0; playerid < controllers.size(); playerid++)
        {
        players.emplace_back(playerid);

        controllers[playerid]->id = playerid;
        controllers[playerid]->master = this;

        for(Card& card : decks[playerid].roster)
            card.ownerId = playerid;
        }

    turn = 0;
    turnAbsolute = 0;
}

void GameMaster::mainLoop()
{
    //Send game status updates to every player
    for(int id = 0; id < playerControllers.size(); id++)
    {
        updateStatus(id);
        playerControllers[id]->applyUpdates();
    }
    
    PlayerAction action = playerControllers[turn]->getAction();
    int actionVerdict = processAction(action);

    if(actionVerdict != GameMaster::NONE) //Invalid action received;
        playerControllers[turn]->handleActionError(actionVerdict);
}

void GameMaster::endTurn()
{
    turn = (turn + 1) % playerControllers.size();
    turnAbsolute++;
    // restore
    // score points
    // income, decrement contracts
    // draw
    for(Card* cardp : activeCards)
    {
        if(cardp->type == Card::UNIT && cardp->ownerId == turn) // Purge next player's units' status effects
        {
            cardp->canMove       = true;
            cardp->canAttack     = true;
            cardp->isOverwhelmed = false;
        }
    }
};

int GameMaster::processAction(const PlayerAction& action)
{
    int deltax, deltay; // Relative target postition for attacking and moving.
    int direction = -1; // Direction of attack or movement.

    switch(action.type)
    {

        //  PASS  _________________________________________________

        case PlayerAction::PASS:
        // self-explanatory.
        endTurn();
        return GameMaster::NONE; //Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        break;

        //  MOVE  _________________________________________________

        case PlayerAction::MOVE:

        //check whether all arguments are present
        if(action.args[0] < 0 || action.args[1] < 0 || action.args[2] < 0 || action.args[3] < 0)
            return GameMaster::NOARGS;

        //check for out-of-bounds arguments
        if(action.args[0] >= grid.size() || action.args[1] >= grid[0].size() || \
           action.args[2] >= grid.size() || action.args[3] >= grid[0].size())
            return GameMaster::INVARGS;

            //determine direction and check if it is invalid
            deltax = action.args[2] - action.args[0];
            deltay = action.args[3] - action.args[1];
                    // valid
            if     (deltax ==  1 && deltay ==  0)    direction = GameMaster::DOWN;
            else if(deltax == -1 && deltay ==  0)    direction = GameMaster::UP; 
            else if(deltax ==  0 && deltay ==  1)    direction = GameMaster::RIGHT;
            else if(deltax ==  0 && deltay == -1)    direction = GameMaster::LEFT;
            else    //invalid
            return GameMaster::INVARGS;

        //check that an unit was selected
        if(grid[action.args[0]][action.args[1]].card == nullptr)            return GameMaster::NOSELECT;
        //check that target tile is vacant
        if(grid[action.args[2]][action.args[3]].card != nullptr)            return GameMaster::NOTARGET;
        //check for ownership permissions
        if(grid[action.args[0]][action.args[1]].card->ownerId != turn)      return GameMaster::PERMISSION;
        //check if ability was exhausted
        if(!grid[action.args[0]][action.args[1]].card->canMove)           return GameMaster::EXHAUSTED;

        //Finally, perform the action
        //grid[action.args[0]][action.args[1]].card->moveCard(direction);
        moveCard(*grid[action.args[0]][action.args[1]].card, direction);

        std::cout << "Executing..." << std::endl;
        return GameMaster::NONE;
        break;
        
        //  ATTACK  _________________________________________________

        case PlayerAction::ATTACK:

        //check whether all arguments are present
        if(action.args[0] < 0 || action.args[1] < 0 || action.args[2] < 0 || action.args[3] < 0)
            return GameMaster::NOARGS;

        //check for out-of-bounds arguments
        if(action.args[0] >= grid.size() || action.args[1] >= grid[0].size() || \
           action.args[2] >= grid.size() || action.args[3] >= grid[0].size())
            return GameMaster::INVARGS;

            //determine direction and check if it is invalid
            deltax = action.args[2] - action.args[0];
            deltay = action.args[3] - action.args[1];
                    // valid adjacent
            if     (deltax ==  1 && deltay ==  0)    direction = GameMaster::DOWN;
            else if(deltax == -1 && deltay ==  0)    direction = GameMaster::UP;
            else if(deltax ==  0 && deltay ==  1)    direction = GameMaster::RIGHT;
            else if(deltax ==  0 && deltay == -1)    direction = GameMaster::LEFT;
                     // valid diagonals
            else if(deltax ==  1 && deltay ==  1)    direction = GameMaster::DOWNRIGHT;
            else if(deltax ==  1 && deltay == -1)    direction = GameMaster::DOWNLEFT;
            else if(deltax == -1 && deltay ==  1)    direction = GameMaster::UPRIGHT;
            else if(deltax == -1 && deltay == -1)    direction = GameMaster::UPLEFT;
            else    //invalid
            return GameMaster::INVARGS;

        //check that an unit was selected
        if(grid[action.args[0]][action.args[1]].card == nullptr)            return GameMaster::NOSELECT;
        //check that a target is nonzero
        if(grid[action.args[2]][action.args[3]].card == nullptr)            return GameMaster::NOTARGET;
        //check for ownership permissions
        if(grid[action.args[0]][action.args[1]].card->ownerId != turn)      return GameMaster::PERMISSION;
        //check if ability was exhausted
        if(!grid[action.args[0]][action.args[1]].card->canAttack)           return GameMaster::EXHAUSTED;

        //Finally, perform the action
        //grid[action.args[0]][action.args[1]].card->attack(direction);
        attackWith(*grid[action.args[0]][action.args[1]].card, direction);

        std::cout << "Executing..." << std::endl;
        return GameMaster::NONE;
        break;

        //  INVALID  _________________________________________________

        default:
        return GameMaster::INVTYPE;
    }
    return GameMaster::INVTYPE; //Disable compiler warnings
}

void GameMaster::updateStatus(int playerId)
{
    PlayerController& controller = *playerControllers[playerId];
    
    controller.activeCards = std::vector<CardInfo>(activeCards.size());
    for(int i = 0; i < activeCards.size(); i++)
        controller.activeCards[i] = activeCards[i]->getInfo();

    controller.hand = std::vector<CardInfo>(players[playerId].hand.size());
    for(int i = 0; i < players[playerId].hand.size(); i++)
        controller.hand[i] = players[playerId].hand[i]->getInfo();

    controller.players = std::vector<PlayerInfo>(players.size());
    for(int i = 0; i < players.size(); i++)
        controller.players[i] = players[i].getInfo(decks[i]);
}

bool GameMaster::deployCard(Card& card, Tile* target)
{
    if(card.type == Card::UNIT && target == nullptr) throw std::runtime_error("No deployment site provided for unit");
    if(card.status == Card::IN_PLAY) std::clog << "WARNING: Deploying card marked as \"IN PLAY\"";

    if(card.type == Card::UNIT)
    {
        if(target->card)
            return false; 
        
        target->card = &card;

        card.x = target->x;
        card.y = target->y;
    }
    //TODO: fire events
    card.status = Card::IN_PLAY;
    activeCards.push_back(&card);

    card.canAttack = false;
    card.canMove = false;
    card.isOverwhelmed = false;

    return true;
}

bool GameMaster::moveCard(Card& card, const int& direction)
{   
    //
    if(direction < 0 || direction > 3) throw std::invalid_argument("invalid direction");
    if(grid[card.x][card.y].card != &card) throw std::runtime_error("Trying to move an off-grid card");

    auto options = getAdjacentTiles(grid[card.x][card.y]);

    //TODO add mutual exchange movement option
    // Check if tile is occupied
    if(!options[direction] || options[direction]->card)
        return false;

    // Exchange positions
    grid[card.x][card.y].card = NULL;
    options[direction]->card = &card;
    // Update coords
    card.x = options[direction]->x;
    card.y = options[direction]->y;

    card.canMove = false;

    return true;
}

void GameMaster::destroyCard(Card& card)
{
    if(card.x < 0 || card.y < 0)
        grid[card.x][card.y].card = NULL;


    if(!vectorPop(activeCards, &card))
        std::clog << "WARNING: killing card that was inactive" << std::endl;

    //TODO restore, fire events
    card.status = Card::DISCARD;
    decks[card.ownerId].discard.push_back(&card);
}

bool GameMaster::attackWith(Card& card, const int& direction)
{
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(grid[card.x][card.y].card != &card) throw std::runtime_error("Attacking with card with bad position");

    std::vector<Tile*> options = getSurroundingTiles(grid[card.x][card.y]);

    if(!options[direction] || !(options[direction]->card))
        return false;

    //combat damage resolution
    ResolveCombat(card, *options[direction]->card);
    return true;
}

int GameMaster::ResolveCombat(Card& attacker, Card& defender)
{
    int deltaAdvantage = attacker.advantage - defender.advantage;
    //resolve overwhelming mechanic
    if(defender.isOverwhelmed) deltaAdvantage++;
    //advantage resolution
    if(deltaAdvantage > 0)
        {
        defender.value -= deltaAdvantage;
        if(defender.value < 0) defender.value = 0;
        }
    else
        {
        attacker.value += deltaAdvantage;
        if(attacker.value < 0) attacker.value = 0;
        }

    attacker.canAttack = false;
    attacker.canMove   = false;
    defender.isOverwhelmed = true;
    //normal damage resolution
    if(attacker.value > defender.value)
    {
        attacker.value -= defender.value;
        defender.value = 0;
        destroyCard(defender);
        return GameMaster::WIN;
    }
    else if(attacker.value < defender.value)
    {
        defender.value -= attacker.value;
        attacker.value = 0;
        destroyCard(attacker);
        return GameMaster::LOSE;
    }
    else //attacker.value == defender.value
    {
        destroyCard(attacker);
        destroyCard(defender);
        return GameMaster::TIE;
    }
}

std::vector<Tile*> GameMaster::getAdjacentTiles(const Tile& tile)    //Get 4-neighbourhood of the tile.
{
    std::vector<Tile*> res(4);

    if(tile.x > 0)                           res[UP]    = &grid[tile.x-1][tile.y];
    if(tile.y < grid[0].size() - 1)          res[RIGHT] = &grid[tile.x][tile.y+1];
    if(tile.x < grid.size() - 1)             res[DOWN]  = &grid[tile.x+1][tile.y];
    if(tile.y > 0)                           res[LEFT]  = &grid[tile.x][tile.y-1];

    return res;
}

std::vector<Tile*> GameMaster::getSurroundingTiles(const Tile& tile) //Get 8-neighbourhood of the tile.
{
    std::vector<Tile*> res = std::vector<Tile*>(8);
    
    if(tile.x > 0)
    {
        res[UP] = &grid[tile.x-1][tile.y];

        if(tile.y > 0)                        res[UPLEFT]    = &grid[tile.x-1][tile.y-1];
        if(tile.y < grid[0].size() - 1)       res[UPRIGHT]   = &grid[tile.x-1][tile.y+1];
    }

    if(tile.x < grid.size() - 1)
    {
        res[DOWN] = &grid[tile.x+1][tile.y];

        if(tile.y > 0)                        res[DOWNLEFT]  = &grid[tile.x+1][tile.y-1];
        if(tile.y < grid[0].size() - 1)       res[DOWNRIGHT] = &grid[tile.x+1][tile.y+1];
    }

    if(tile.y > 0)                            res[LEFT]      = &grid[tile.x][tile.y-1];
    if(tile.y < grid[0].size() - 1)           res[RIGHT]     = &grid[tile.x][tile.y+1];

    return res;
}

