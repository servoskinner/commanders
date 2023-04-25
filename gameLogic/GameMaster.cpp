#include "gameLogic.h"

#include <stdexcept>
#include <iostream>

GameMaster::GameMaster(std::vector<ControllerPtr> controllers, std::vector<DeckPtr> decks, int gridHeight, int gridWidth)
{
    if(controllers.size() > decks.size()) 
        throw std::runtime_error("Insufficient decks specified for game creation;");
    if(controllers.size() < decks.size())
        std::clog << "Redundant decks provided for GameMaster constructor. Ignoring last;" << std::endl;

    playerControllers = controllers;
    turn = 0;

    grid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth, Tile(*this)));

    for(int row = 0; row < gridHeight; row++)
        for(int column = 0; column < gridWidth; column ++)
            grid[row][column].y = column, grid[row][column].x = row; 

    for(int playerid = 0; playerid < controllers.size(); playerid++)
        {
        players.push_back(Player(*decks[playerid], playerid));

        controllers[playerid]->id = playerid;
        controllers[playerid]->master = (MasterPtr)this;
        }

    // deployZones = {}; //TBA
    // captureZone = {};
}

void GameMaster::MainLoop()
{
    PlayerAction action = playerControllers[turn]->getAction();
    int actionVerdict = ProcessAction(action);

    if(actionVerdict != GameMaster::NONE)
        playerControllers[turn]->handleActionError(actionVerdict);

    for(ControllerPtr pcp : playerControllers)
    {
        //updateStatus(pcp);
        pcp->applyUpdates();
    }
}

void GameMaster::EndTurn()
{
    turn = (turn + 1) % playerControllers.size();
    // restore
    // score points
    // income, decrement contracts
    // draw
    for(CardPtr cardp : players[turn].cardsInPlay)
    {
        if(cardp->type == Card::UNIT) //purge status effects
        {
            cardp->canMove       = true;
            cardp->canAttack     = true;
            cardp->isOverwhelmed = false;
        }
    }
};

int GameMaster::ProcessAction(const PlayerAction& action)
{
    int deltax, deltay; // Relative target postition for attacking and moving.
    int direction = -1; // Direction of attack or movement.

    switch(action.type)
    {

        //  PASS  _________________________________________________

        case PlayerAction::PASS:
        // self-explanatory.
        EndTurn();
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
            if     (!(deltax ==  1 && deltay ==  0))    direction = Tile::UP;
            else if(!(deltax == -1 && deltay ==  0))    direction = Tile::DOWN;
            else if(!(deltax ==  0 && deltay ==  1))    direction = Tile::RIGHT;
            else if(!(deltax ==  0 && deltay == -1))    direction = Tile::LEFT;
            else    //invalid
            return GameMaster::INVARGS;

        //check that an unit was selected
        if(grid[action.args[0]][action.args[1]].card == nullptr)                return GameMaster::NOSELECT;
        //check that target tile is vacant
        if(grid[action.args[2]][action.args[3]].card != nullptr)                return GameMaster::NOTARGET;
        //check for ownership permissions
        if(grid[action.args[0]][action.args[1]].card->owner->playerId != turn)  return GameMaster::PERMISSION;

        //Finally, perform the action
        grid[action.args[0]][action.args[1]].card->Move(direction);

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
            if     (!(deltax ==  1 && deltay ==  0))    direction = Tile::UP;
            else if(!(deltax == -1 && deltay ==  0))    direction = Tile::DOWN;
            else if(!(deltax ==  0 && deltay ==  1))    direction = Tile::RIGHT;
            else if(!(deltax ==  0 && deltay == -1))    direction = Tile::LEFT;
                     // valid diagonals
            else if(!(deltax ==  1 && deltay ==  1))    direction = Tile::UPRIGHT;
            else if(!(deltax ==  1 && deltay == -1))    direction = Tile::UPLEFT;
            else if(!(deltax == -1 && deltay ==  1))    direction = Tile::DOWNRIGHT;
            else if(!(deltax == -1 && deltay == -1))    direction = Tile::DOWNLEFT;
            else    //invalid
            return GameMaster::INVARGS;

        //check that an unit was selected
        if(grid[action.args[0]][action.args[1]].card == nullptr)                return GameMaster::NOSELECT;
        //check that a target is nonzero
        if(grid[action.args[2]][action.args[3]].card == nullptr)                return GameMaster::NOTARGET;
        //check for ownership permissions
        if(grid[action.args[0]][action.args[1]].card->owner->playerId != turn)  return GameMaster::PERMISSION;

        //Finally, perform the action
        grid[action.args[0]][action.args[1]].card->Attack(direction);

        std::cout << "Executing..." << std::endl;
        return GameMaster::NONE;
        break;

        //  INVALID  _________________________________________________

        default:
        return GameMaster::INVTYPE;
    }
    return GameMaster::INVTYPE; //Disable compiler warnings
}

//void GameMaster::updateStatus(PlayerController& controller);
