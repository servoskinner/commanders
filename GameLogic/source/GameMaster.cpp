#include "gameLogic.h"
#include "misc.h"
#include "Roster.h"

#include <stdexcept>
#include <iostream>

Game_master::Game_master(const std::vector<pctrl_ref>& controllers, \
                       const std::vector<deck_ref>& ndecks) : roster({})
    // TODO add singleton roster
{

    enableGameLoop = true;
    // Check argument validity
    if(controllers.size() > ndecks.size()) {
        throw std::runtime_error("Insufficient decks specified for game creation;");
    }
    if(controllers.size() < ndecks.size()) {
        std::clog << "Redundant decks provided for Game_master constructor. Ignoring last;" << std::endl;
	}
    // Set up variables
    playerControllers = controllers;
    decks = ndecks; 

    // Build grid
    grid = std::vector<std::vector<Tile>>(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));

    for(int row = 0; row < GRID_HEIGHT; row++) {
        for(int column = 0; column < GRID_WIDTH; column ++) {
            grid[row][column] = Tile(row, column);
        }
	}
    // Mark up special tiles (All are aligned vertically)
    for(int row = 0; row < GRID_HEIGHT; row++) {
        // Deploy zone
        grid[row][0].type = 0;
        grid[row][GRID_WIDTH-1].type = 1;
        // Capture zone
        grid[row][(GRID_WIDTH-1)/2].type = Tile::CAPTUREZONE;
        grid[row][(GRID_WIDTH)/2].type = Tile::CAPTUREZONE;
    }

    // Assign IDs to various entities
    for(int playerid = 0; playerid < controllers.size(); playerid++) {
        players.emplace_back(playerid);

        controllers[playerid].get().id = playerid;
        controllers[playerid].get().master = this;

        for(Card& card : decks[playerid].get().all) {
            card.ownerId = playerid;
        }
    }

    //There is a non-existent turn before the first one.
    turn = -1;
    turnAbsolute = -1;

    // Deal cards TODO add re-draw
    for(int id = 0; id < players.size(); id++)
    {
    	// Shuffle decks
        decks[id].get().shuffle();
        // Deal starting hand
        for(int card = 0; card < STARTING_HAND_SIZE; card++) {
            forceDraw(id);
        }
    }

    endTurn();
}

bool Game_master::mainLoop() 
{
    //Send game status updates to every player
    for(int id = 0; id < playerControllers.size(); id++)
    {
        updateStatus(id);
        playerControllers[id].get().applyUpdates();
    }
    
    PlayerAction action = playerControllers[turn]->getAction();
    int actionVerdict = processAction(action);

    if(actionVerdict != Game_master::NONE) { //Invalid action received;
        playerControllers[turn].get().handleControllerEvent(actionVerdict);
    }

    return enableGameLoop;
}

void Game_master::endTurn()
{
    // Increment turn counter (also is playerid of whoever takes the turn)
    turn = (turn + 1) % playerControllers.size();
    turnAbsolute++;

	// Resolve resource decay 
	if (players[turn].funds > 0) {
		players[turn].funds -= FUNDS_DECAY;
	}
    // Add basic income
    players[turn].funds += BASIC_INCOME;
    // Draw a card, fire event if both discard and deck are exhausted
    if(!forceDraw(turn)) {
        playerControllers[turn]->handleControllerEvent(DECK_NOREFRESH);
    }
	// Check if player should gain victory points
    if(checkDominance(turn))
    {
        players[turn].points++;
        // Check if player wins on this turn
        if(players[turn].points >= POINTS_REQ_FOR_VICTORY)
        {
            enableGameLoop = false;
            // Notify everyone that game ended
            for(int i = 0; i < playerControllers.size(); i++) {
                playerControllers[i]->handleControllerEvent(i == turn ? Game_master::GAME_WIN : Game_master::GAME_LOSE);
			}
        }
    }


    for(card_ref cref : activeCards)
    {
        if(cref.get().type == Card::UNIT && cref.get().ownerId == turn) // Un-overwhelm units and refresh move and attack
        {
            cref.get().canMove       = true;
            cref.get().canAttack     = true;
            cref.get().isOverwhelmed = false;
        }
        if(cref.get().type == Card::CONTRACT && cref.get().ownerId == turn) // Gain profit for each contract and decrement their duration
        {
            cref.get().value--;
            players[turn].funds++;

            if(cref.get().value <= 0) {
                destroyCard(cref);
            } 
        }
    }
};

int Game_master::processAction(const PlayerAction& action)
{
    int deltax, deltay; // Relative target postition for attacking and moving.
    int direction = -1; // Direction of attack or movement.

    switch(action.type)
    {

        //  PASS  _________________________________________________

        case PlayerAction::PASS:
        // self-explanatory.
        endTurn();
        return Game_master::NONE; //Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        break;

        //  MOVE  _________________________________________________

        case PlayerAction::MOVE:
        {

            //check whether all arguments are present
            if(action.args[0] < 0 || action.args[1] < 0 || action.args[2] < 0 || action.args[3] < 0) {
                return Game_master::ACT_INVARGS;
			}
            //check for out-of-bounds arguments
            if(action.args[0] >= grid.size() || action.args[1] >= grid[0].size() || \
            action.args[2] >= grid.size() || action.args[3] >= grid[0].size()) {
                return Game_master::ACT_INVARGS;
			}
                //determine direction and check if it is invalid
                deltax = action.args[2] - action.args[0];
                deltay = action.args[3] - action.args[1];
                        // valid
                if     (deltax ==  1 && deltay ==  0)    direction = Game_master::DOWN;
                else if(deltax == -1 && deltay ==  0)    direction = Game_master::UP; 
                else if(deltax ==  0 && deltay ==  1)    direction = Game_master::RIGHT;
                else if(deltax ==  0 && deltay == -1)    direction = Game_master::LEFT;
                else    //invalid
                return Game_master::ACT_INVARGS;

            //check that an unit was selected
            if(!grid[action.args[0]][action.args[1]].card.has_value())       { return Game_master::ACT_NOSELECT; 	}
            //check that target tile is vacant
            if(grid[action.args[2]][action.args[3]].card.has_value())       { return Game_master::ACT_NOTARGET; 	}
            //check for ownership permissions
            if(grid[action.args[0]][action.args[1]].card->get().ownerId != turn) { return Game_master::ACT_PERMISSION; }
            //check if ability was exhausted
            if(!grid[action.args[0]][action.args[1]].card->get().canMove)        { return Game_master::ACT_EXHAUSTED; 	}

            //Finally, perform the action

            if(!moveCard(*grid[action.args[0]][action.args[1]].card, direction)) {
                return Game_master::UNKNOWN;
            }

            std::cout << "Executing..." << std::endl;
            return Game_master::NONE;
        }
        break;
        
        //  ATTACK  _________________________________________________

        case PlayerAction::ATTACK:
        {

            //check whether all arguments are present
            if(action.args[0] < 0 || action.args[1] < 0 || action.args[2] < 0 || action.args[3] < 0) {
                return Game_master::ACT_INVARGS;
			}
            //check for out-of-bounds arguments
            if(action.args[0] >= grid.size() || action.args[1] >= grid[0].size() || \
            action.args[2] >= grid.size() || action.args[3] >= grid[0].size()) {
                return Game_master::ACT_INVARGS;
			}
            //grid[action.args[0]][action.args[1]].card->moveCard(direction);                //determine direction and check if it is invalid
                deltax = action.args[2] - action.args[0];
                deltay = action.args[3] - action.args[1];
                        // valid adjacent
                if     (deltax ==  1 && deltay ==  0) { direction = Game_master::DOWN; 		}
                else if(deltax == -1 && deltay ==  0) { direction = Game_master::UP; 		}
                else if(deltax ==  0 && deltay ==  1) { direction = Game_master::RIGHT; 		}
                else if(deltax ==  0 && deltay == -1) { direction = Game_master::LEFT; 		}
                        // valid diagonals
                else if(deltax ==  1 && deltay ==  1) { direction = Game_master::DOWNRIGHT; 	}
                else if(deltax ==  1 && deltay == -1) { direction = Game_master::DOWNLEFT;	}
                else if(deltax == -1 && deltay ==  1) { direction = Game_master::UPRIGHT;	}
                else if(deltax == -1 && deltay == -1) { direction = Game_master::UPLEFT;		}
                else    //invalid
                return Game_master::ACT_INVARGS;

            //check that an unit was selected
            if(!grid[action.args[0]][action.args[1]].card.has_value())       { return Game_master::ACT_NOSELECT; 	}
            //check that a target is nonzero
            if(!grid[action.args[2]][action.args[3]].card.has_value())       { return Game_master::ACT_NOTARGET; 	}
            //check for ownership permissions
            if(grid[action.args[0]][action.args[1]].card->get().ownerId != turn) { return Game_master::ACT_PERMISSION; }
            //check if ability was exhausted
            if(!grid[action.args[0]][action.args[1]].card->get().canAttack)      { return Game_master::ACT_EXHAUSTED; 	}

            // Finally, perform the action and check for proper execution.
            if(!attackWith(grid[action.args[0]][action.args[1]].card.value(), direction)) {
            	return Game_master::UNKNOWN;
			}
            std::cout << "Executing..." << std::endl;
            return Game_master::NONE;
        }
        break;

        //  PLAY  _________________________________________________

        case PlayerAction::PLAY:
        {
        
            if(action.args[0] < 0 || action.args[0] >= players[turn].hand.size()) {                
            	return Game_master::ACT_INVARGS;
			}
            // Check for out-of-bounds arguments for unit deployment
            std::optional<tile_ref> deploySite; // Get tile pointer for unit deployment
            if(players[turn].hand[action.args[0]].get().type == Card::UNIT)
            {
                if(action.args[1] < 0 || action.args[2] < 0) {
                    return Game_master::ACT_INVARGS;
                }
                if(action.args[1] >= grid.size() || action.args[2] >= grid[0].size()) {
                    return Game_master::ACT_INVARGS;
				}
                deploySite = std::ref(grid[action.args[1]][action.args[2]]);
            }
            // Check for insufficient funds
            if(players[turn].hand[action.args[0]].get().cost > players[turn].funds) {
                return Game_master::ACT_NOFUNDS;
			}
            // Finally, perform the action and check for proper execution.
            if(!playCard(turn, action.args[0], deploySite)) {
                return Game_master::ACT_NOTARGET;
			}
            return Game_master::NONE; //Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        }
        break;

        //  INVALID  _________________________________________________

        default:
            return Game_master::ACT_INVTYPE;
        break;
    }
    return Game_master::ACT_INVTYPE; //Disable compiler warnings
}

void Game_master::updateStatus(int playerId)
{
    pctrl_ref controller = std::ref(playerControllers[playerId]);
    
    controller.get().activeCards = std::vector<Card_info>(activeCards.size());
    for(int i = 0; i < activeCards.size(); i++)
        controller.get().activeCards[i] = activeCards[i].get().getInfo();

    controller.get().hand = std::vector<Card_info>(players[playerId].hand.size());
    for(int i = 0; i < players[playerId].hand.size(); i++)
        controller.get().hand[i] = players[playerId].hand[i].get().getInfo();

    controller.get().players = std::vector<Player_info>(players.size());
    for(int i = 0; i < players.size(); i++)
        controller.get().players[i] = players[i].getInfo(decks[i]);
}

bool Game_master::checkDominance(int playerId)
{
    int delta = 0;
    for(card_ref cref : activeCards) // Find active cards that are in capture zone
        if(cref.get().type == Card::UNIT && grid[cref.get().x][cref.get().y].type == Tile::CAPTUREZONE)
        {
            if(cref.get().ownerId == turn)
                delta++;
            else
                delta--;
        }
    return delta > 0;
}

bool Game_master::deployCard(Card& card, std::optional<tile_ref> target)
{
    if(card.type == Card::UNIT && !target.has_value()) throw std::runtime_error("No deployment site provided for unit");
    if(card.status == Card::IN_PLAY) std::clog << "WARNING: Deploying card marked as \"IN PLAY\"";

    if(card.type == Card::UNIT)
    {   // Set grid position
        if(target->get().card)
            return false; 
        
        target->get().card = std::ref(card);

        card.x = target->get().x;
        card.y = target->get().y;

        card.canAttack = true;
        card.canMove = false;
        card.isOverwhelmed = false;
    }
    
    if(card.type != Card::TACTIC)
    {   // Update status
        card.status = Card::IN_PLAY;
        activeCards.push_back(std::ref(card));
    }

    // Fire onPlay events
    for(const auto& deployEvent : card.onPlay)
        deployEvent(*this, card);

    if(card.type == Card::TACTIC)
        destroyCard(card);

    return true;
}

bool Game_master::moveCard(Card& card, const int& direction)
{   
    //
    if(direction < 0 || direction > 3) throw std::invalid_argument("invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card.value() != std::ref(card)) throw std::runtime_error("Trying to move an off-grid card");

    auto options = getAdjacentTiles(grid[card.x][card.y]);

    //TODO add mutual exchange movement option
    // Check if tile is occupied
    if(options[direction]->get().card)
        return false;

    // Exchange positions
    grid[card.x][card.y].card.reset();
    options[direction]->get().card = std::ref(card);
    // Update coords
    card.x = options[direction]->get().x;
    card.y = options[direction]->get().y;

    card.canMove = false;

    return true;
}

void Game_master::destroyCard(Card& card)
{   
    if(card.type != Card::TACTIC)
    {
        if(card.x > 0 && card.y > 0)
            grid[card.x][card.y].card.reset();

        if(!vectorPop(activeCards, std::ref(card)))
            std::clog << "WARNING: killing card that was not in activeCards" << std::endl;

        if(card.status != Card::IN_PLAY)
            std::clog << "WARNING: killing card that was not \"in play\"" << std::endl;
    }

    // Fire death events
    for(const auto& deathEvent : card.onDeath)
        deathEvent(*this, card);

    // Send to discard
    card.status = Card::DISCARD;
    decks[card.ownerId].get().discard.push_back(std::ref(card));

    // Restore
    card = roster.getOrigin(card.id);
}

bool Game_master::attackWith(Card& card, const int& direction)
{
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card.value() != std::ref(card)) throw std::runtime_error("Attacking with card with bad position");

    std::vector<std::optional<tile_ref>> options = getSurroundingTiles(grid[card.x][card.y]);

    if(!options[direction].has_value() || !(options[direction]->get().card.has_value()))
        return false;

    //combat damage resolution
    ResolveCombat(card, options[direction]->get().card.value());
    return true;
}

int Game_master::ResolveCombat(Card& attacker, Card& defender)
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
        return Game_master::WIN;
    }
    else if(attacker.value < defender.value)
    {
        defender.value -= attacker.value;
        attacker.value = 0;
        destroyCard(attacker);
        return Game_master::LOSE;
    }
    else //attacker.value == defender.value
    {
        destroyCard(attacker);
        destroyCard(defender);
        return Game_master::TIE;
    }
}

std::vector<std::optional<tile_ref>> Game_master::getAdjacentTiles(const Tile& tile)    //Get 4-neighbourhood of the tile.
{
    std::vector<std::optional<tile_ref>> res(4);

    if(tile.x > 0)                           res[UP]    = std::ref(grid[tile.x-1][tile.y]);
    if(tile.y < grid[0].size() - 1)          res[RIGHT] = std::ref(grid[tile.x][tile.y+1]);
    if(tile.x < grid.size() - 1)             res[DOWN]  = std::ref(grid[tile.x+1][tile.y]);
    if(tile.y > 0)                           res[LEFT]  = std::ref(grid[tile.x][tile.y-1]);

    return res;
}

std::vector<std::optional<tile_ref>> Game_master::getSurroundingTiles(const Tile& tile) //Get 8-neighbourhood of the tile.
{
    std::vector<std::optional<tile_ref>> res(8);
    
    if(tile.x > 0)
    {
        res[UP] = std::ref(grid[tile.x-1][tile.y]);

        if(tile.y > 0)                        res[UPLEFT]    = std::ref(grid[tile.x-1][tile.y-1]);
        if(tile.y < grid[0].size() - 1)       res[UPRIGHT]   = std::ref(grid[tile.x-1][tile.y+1]);
    }

    if(tile.x < grid.size() - 1)
    {
        res[DOWN] = std::ref(grid[tile.x+1][tile.y]);

        if(tile.y > 0)                        res[DOWNLEFT]  = std::ref(grid[tile.x+1][tile.y-1]);
        if(tile.y < grid[0].size() - 1)       res[DOWNRIGHT] = std::ref(grid[tile.x+1][tile.y+1]);
    }

    if(tile.y > 0)                            res[LEFT]      = std::ref(grid[tile.x][tile.y-1]);
    if(tile.y < grid[0].size() - 1)           res[RIGHT]     = std::ref(grid[tile.x][tile.y+1]);

    return res;
}

bool Game_master::forceDraw(int playerId)
{
    if(decks[playerId].get().library.size() == 0)
        if(decks[playerId].get().discard.size() == 0)
            return false;
        else
            decks[playerId].get().refresh();

    if(decks[playerId].get().library.back().get().status != Card::DECK)
        std::clog << "Warning: drawing card that has not been marked as \"In deck\"" << std::endl;

    players[playerId].hand.push_back(decks[playerId].get().library.back());
    decks[playerId].get().library.back().get().status = Card::HAND;
    decks[playerId].get().library.pop_back();

    return true;
}

bool Game_master::discard(int playerId, int handIndex)
{
    if(playerId < 0 || playerId >= players.size()) throw std::out_of_range("playerId out of range");
    if(handIndex < 0 || handIndex >= players[playerId].hand.size()) throw std::out_of_range("handIndex out of range");

    if(players[playerId].hand[handIndex].get().status != Card::HAND)
        std::clog << "Warning: discarding card that has not been marked as \"In hand\"" << std::endl;

    decks[playerId].get().discard.push_back(players[playerId].hand[handIndex]);
    players[playerId].hand[handIndex].get().status = Card::DISCARD;
    vectorPopIndex(players[playerId].hand, handIndex);

    return true;
}

bool Game_master::playCard(int playerId, int handIndex, std::optional<tile_ref> target) //Returns false if there are insufficient funds.
{
    Card& played = players[playerId].hand[handIndex];
    // Check for index correctness
    if(playerId < 0 || playerId >= players.size()) throw std::out_of_range("playerId out of range");
    if(handIndex < 0 || handIndex >= players[playerId].hand.size()) throw std::out_of_range("handIndex out of range");

    // Check whether a player has enough funds to play the card (subtract them if success)
    if(players[playerId].funds < played.cost)
        return false;

    // Check that a target tile was provided for units
    if(played.type == Card::UNIT)
    {
        if(!target.has_value())
            std::runtime_error("No deployment site provided for playing a unit");

        // Check surroundings or deployment zones
        if(target->get().type != playerId) //If deploying outside of Base
        {
            // Check if there is at least one friendly unit on adjacent tiles
            bool nearFriendly = false;
            for(std::optional<tile_ref> tptr : getAdjacentTiles(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && \
                   tptr->get().card->get().ownerId == playerId)
                    nearFriendly = true;

            if(!nearFriendly) return Game_master::ACT_NOTARGET;

            // Check if there are no enemies on surrounding tiles
            for(std::optional<tile_ref> tptr : getSurroundingTiles(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && 
                   tptr->get().card->get().ownerId != playerId)
                    return Game_master::ACT_NOTARGET;
        }
    }

    if(played.status != Card::HAND)
        std::clog << "Warning: trying to play card that has not been marked as \"In hand\"" << std::endl;

    players[playerId].funds -= players[playerId].hand[handIndex].get().cost;
    
    vectorPopIndex(players[playerId].hand, handIndex);
    bool res = deployCard(played, target);

    return res;
}

