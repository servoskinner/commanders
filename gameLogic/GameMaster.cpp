#include "gameLogic.h"
#include "misc.h"

#include <stdexcept>
#include <iostream>

GameMaster::GameMaster(const std::vector<PlayerController*>& controllers, const std::vector<Deck*>& ndecks)
{
    enableGameLoop = true;
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
            grid[row][column] = Tile(row, column);

    // Mark up special tiles (All are aligned vertically)
    for(int row = 0; row < GRID_HEIGHT; row++)
    {
        // Deploy zone
        grid[row][0].type = 0;
        grid[row][GRID_WIDTH-1].type = 1;
        // Capture zone
        grid[row][(GRID_WIDTH-1)/2].type = Tile::CAPTUREZONE;
        grid[row][(GRID_WIDTH)/2].type = Tile::CAPTUREZONE;
    }


    // Assign IDs to various entities
    for(int playerid = 0; playerid < controllers.size(); playerid++)
        {
        players.emplace_back(playerid);

        controllers[playerid]->id = playerid;
        controllers[playerid]->master = this;

        for(Card& card : decks[playerid]->roster)
            card.ownerId = playerid;
        }

    //There is a non-existent turn before the first one.
    turn = -1;
    turnAbsolute = -1;

    //Deal cards TODO add re-draw
    for(int id = 0; id < players.size(); id++)
    {
        decks[id]->shuffle();
        for(int card = 0; card < STARTING_HAND_SIZE; card++)
            forceDraw(id);
    }

    endTurn();
}

bool GameMaster::mainLoop() //To be put in while(). 
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
        playerControllers[turn]->handleControllerEvent(actionVerdict);

    return enableGameLoop;
}

void GameMaster::endTurn()
{
    // Change turn number (also is playerid of whoever takes the turn)
    turn = (turn + 1) % playerControllers.size();
    turnAbsolute++;

    // Basic income
    players[turn].funds += BASIC_INCOME;
    // Draw a card
    forceDraw(turn); //TOOD: if false fire controller event

    if(checkDominance(turn))
    {
        players[turn].points++;
        if(players[turn].points >= POINTS_REQ_FOR_VICTORY)
        {
            enableGameLoop = false;
            for(int i = 0; i < playerControllers.size(); i++)
                playerControllers[i]->handleControllerEvent(i == turn ? GameMaster::GAME_WIN : GameMaster::GAME_LOSE);

        }
    }


    for(Card* cardp : activeCards)
    {
        if(cardp->type == Card::UNIT && cardp->ownerId == turn) // Purge next player's units' status effects
        {
            cardp->canMove       = true;
            cardp->canAttack     = true;
            cardp->isOverwhelmed = false;
        }
        if(cardp->type == Card::CONTRACT && cardp->ownerId == turn) // Gain profit for each contract and decrement them.
        {
            cardp->value--;
            players[turn].funds++;

            if(cardp->value <= 0)
                destroyCard(*cardp);
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
        {

            //check whether all arguments are present
            if(action.args[0] < 0 || action.args[1] < 0 || action.args[2] < 0 || action.args[3] < 0)
                return GameMaster::ACT_INVARGS;

            //check for out-of-bounds arguments
            if(action.args[0] >= grid.size() || action.args[1] >= grid[0].size() || \
            action.args[2] >= grid.size() || action.args[3] >= grid[0].size())
                return GameMaster::ACT_INVARGS;

                //determine direction and check if it is invalid
                deltax = action.args[2] - action.args[0];
                deltay = action.args[3] - action.args[1];
                        // valid
                if     (deltax ==  1 && deltay ==  0)    direction = GameMaster::DOWN;
                else if(deltax == -1 && deltay ==  0)    direction = GameMaster::UP; 
                else if(deltax ==  0 && deltay ==  1)    direction = GameMaster::RIGHT;
                else if(deltax ==  0 && deltay == -1)    direction = GameMaster::LEFT;
                else    //invalid
                return GameMaster::ACT_INVARGS;

            //check that an unit was selected
            if(grid[action.args[0]][action.args[1]].card == nullptr)            return GameMaster::ACT_NOSELECT;
            //check that target tile is vacant
            if(grid[action.args[2]][action.args[3]].card != nullptr)            return GameMaster::ACT_NOTARGET;
            //check for ownership permissions
            if(grid[action.args[0]][action.args[1]].card->ownerId != turn)      return GameMaster::ACT_PERMISSION;
            //check if ability was exhausted
            if(!grid[action.args[0]][action.args[1]].card->canMove)           return GameMaster::ACT_EXHAUSTED;

            //Finally, perform the action
            //grid[action.args[0]][action.args[1]].card->moveCard(direction);
            if(!moveCard(*grid[action.args[0]][action.args[1]].card, direction))
                return GameMaster::UNKNOWN;

            std::cout << "Executing..." << std::endl;
            return GameMaster::NONE;
        }
        break;
        
        //  ATTACK  _________________________________________________

        case PlayerAction::ATTACK:
        {

            //check whether all arguments are present
            if(action.args[0] < 0 || action.args[1] < 0 || action.args[2] < 0 || action.args[3] < 0)
                return GameMaster::ACT_INVARGS;

            //check for out-of-bounds arguments
            if(action.args[0] >= grid.size() || action.args[1] >= grid[0].size() || \
            action.args[2] >= grid.size() || action.args[3] >= grid[0].size())
                return GameMaster::ACT_INVARGS;

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
                return GameMaster::ACT_INVARGS;

            //check that an unit was selected
            if(grid[action.args[0]][action.args[1]].card == nullptr)            return GameMaster::ACT_NOSELECT;
            //check that a target is nonzero
            if(grid[action.args[2]][action.args[3]].card == nullptr)            return GameMaster::ACT_NOTARGET;
            //check for ownership permissions
            if(grid[action.args[0]][action.args[1]].card->ownerId != turn)      return GameMaster::ACT_PERMISSION;
            //check if ability was exhausted
            if(!grid[action.args[0]][action.args[1]].card->canAttack)           return GameMaster::ACT_EXHAUSTED;

            // Finally, perform the action and check for proper execution.
            if(!attackWith(*grid[action.args[0]][action.args[1]].card, direction))
                return GameMaster::UNKNOWN;

            std::cout << "Executing..." << std::endl;
            return GameMaster::NONE;
        }
        break;

        //  PLAY  _________________________________________________

        case PlayerAction::PLAY:
        {
        
            if(action.args[0] < 0 || action.args[0] >= players[turn].hand.size())
                return GameMaster::ACT_INVARGS;

            // Check for out-of-bounds arguments for unit deployment
            Tile* deploySite = nullptr; // Get tile pointer for unit deployment
            if(players[turn].hand[action.args[0]]->type == Card::UNIT)
            {
                if(action.args[1] < 0 || action.args[2] < 0)
                    return GameMaster::ACT_INVARGS;
                if(action.args[1] >= grid.size() || action.args[2] >= grid[0].size())
                    return GameMaster::ACT_INVARGS;

                deploySite = &grid[action.args[1]][action.args[2]];
            }
            // Check for insufficient funds
            if(players[turn].hand[action.args[0]]->cost > players[turn].funds)
                return GameMaster::ACT_NOFUNDS;

            // Finally, perform the action and check for proper execution.
            if(!playCard(turn, action.args[0], deploySite))
                return GameMaster::ACT_NOTARGET;

            return GameMaster::NONE; //Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        }
        break;

        //  INVALID  _________________________________________________

        default:
            return GameMaster::ACT_INVTYPE;
        break;
    }
    return GameMaster::ACT_INVTYPE; //Disable compiler warnings
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
        controller.players[i] = players[i].getInfo(*decks[i]);
}

bool GameMaster::checkDominance(int playerId)
{
    int delta = 0;
    for(Card* cptr : activeCards) // Find active cards that are in capture zone
        if(cptr->type == Card::UNIT && grid[cptr->x][cptr->y].type == Tile::CAPTUREZONE)
        {
            if(cptr->ownerId == turn)
                delta++;
            else
                delta--;
        }
    return delta > 0;
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
    if(card.x > 0 && card.y > 0)
        grid[card.x][card.y].card = NULL;

    if(!vectorPop(activeCards, &card))
        std::clog << "WARNING: killing card that was not in activeCards" << std::endl;

    if(card.status != Card::IN_PLAY)
        std::clog << "WARNING: killing card that was not \"in play\"" << std::endl;

    //TODO restore, fire events
    card.status = Card::DISCARD;
    decks[card.ownerId]->discard.push_back(&card);
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

bool GameMaster::forceDraw(int playerId)
{
    if(decks[playerId]->library.size() == 0)
        if(decks[playerId]->discard.size() == 0)
            return false;
        else
            decks[playerId]->refresh();

    if(decks[playerId]->library.back()->status != Card::DECK)
        std::clog << "Warning: drawing card that has not been marked as \"In deck\"" << std::endl;

    players[playerId].hand.push_back(decks[playerId]->library.back());
    decks[playerId]->library.back()->status = Card::HAND;
    decks[playerId]->library.pop_back();

    return true;
}

bool GameMaster::discard(int playerId, int handIndex)
{
    if(playerId < 0 || playerId >= players.size()) throw std::out_of_range("playerId out of range");
    if(handIndex < 0 || handIndex >= players[playerId].hand.size()) throw std::out_of_range("handIndex out of range");

    if(players[playerId].hand[handIndex]->status != Card::HAND)
        std::clog << "Warning: discarding card that has not been marked as \"In hand\"" << std::endl;

    decks[playerId]->discard.push_back(players[playerId].hand[handIndex]);
    players[playerId].hand[handIndex]->status = Card::DISCARD;
    vectorPopIndex(players[playerId].hand, handIndex);

    return true;
}

bool GameMaster::playCard(int playerId, int handIndex, Tile* target) //Returns false if there are insufficient funds.
{
    // Check for index correctness
    if(playerId < 0 || playerId >= players.size()) throw std::out_of_range("playerId out of range");
    if(handIndex < 0 || handIndex >= players[playerId].hand.size()) throw std::out_of_range("handIndex out of range");

    // Check that a target tile was provided for units
    if(players[playerId].hand[handIndex]->type == Card::UNIT)
    {
        if(target == nullptr)
            std::runtime_error("No deployment site provided for playing a unit");

        // Check surroundings or deployment zones
        if(target->type != playerId) //If deploying outside of DZ
        {
            // At least one friendly unit on adjacent tiles
            bool nearFriendly = false;
            for(Tile* tptr : getAdjacentTiles(*target))
                if(tptr != nullptr && tptr->card != nullptr && tptr->card->ownerId == playerId)
                    nearFriendly = true;

            if(!nearFriendly) return GameMaster::ACT_NOTARGET;

            // No enemies on surrounding tiles
            for(Tile* tptr : getSurroundingTiles(*target))
                if(tptr != nullptr && tptr->card != nullptr && tptr->card->ownerId != playerId)
                    return GameMaster::ACT_NOTARGET;
        }
    }

    if(players[playerId].hand[handIndex]->status != Card::HAND)
        std::clog << "Warning: trying to play card that has not been marked as \"In hand\"" << std::endl;

    // Check whether a player has enough funds to play the card (subtract them if success)
    if(players[playerId].funds < players[playerId].hand[handIndex]->cost)
        return false;
    players[playerId].funds -= players[playerId].hand[handIndex]->cost;
    
    bool res = deployCard(*players[playerId].hand[handIndex], target);
    vectorPopIndex(players[playerId].hand, handIndex);

    return res;
}

