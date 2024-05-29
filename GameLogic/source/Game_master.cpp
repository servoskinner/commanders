#include "game_logic.hpp"
#include "misc.hpp"
#include "roster.hpp"

#include <stdexcept>
#include <iostream>

Game_master::Game_master(const std::vector<pctrl_ref>& controllers, \
                       const std::vector<deck_ref>& ndecks) : roster(Roster::get())
{
    game_is_on = true;
    // Check argument validity
    if(controllers.size() > ndecks.size()) {
        throw std::runtime_error("Insufficient decks specified for game creation;");
    }
    if(controllers.size() < ndecks.size()) {
        std::clog << "Redundant decks provided for Game_master constructor. Ignoring last;" << std::endl;
	}
    // Set up variables
    player_controllers = controllers;
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
    int cards_in_match = 0; // TODO reserve id space for tokens

    for(int playerid = 0; playerid < controllers.size(); playerid++) {
        players.emplace_back(playerid);

        controllers[playerid].get().id = playerid;

        for(Card& card : decks[playerid].get().all) {
            card.owner_id = playerid;

            card.match_id = cards_in_match;
            cards_in_match++;
        }
    }

    turn = -1;
    turnAbsolute = -1;

    // Deal cards TODO add re-draw
    for(int id = 0; id < players.size(); id++)
    {
    	// Shuffle decks
        decks[id].get().shuffle();
        // Deal starting hand
        for(int card = 0; card < STARTING_HAND_SIZE; card++) {
            resolve_draw(id);
        }
    }

    end_turn(); // end -1st turn
}

inline const void Game_master::broadcast_event(const std::vector<int>& event)
{
    for(pctrl_ref pctrl : player_controllers)
    {
        pctrl.get().process_event(event);
    }
}

inline const void Game_master::send_event(const std::vector<int>& event, int player_id)
{
    player_controllers[player_id].get().process_event(event);
}

bool Game_master::game_loop() 
{
    // Send game status updates to every player
    for(int id = 0; id < player_controllers.size(); id++)
    {
        update_status(id);
        player_controllers[id].get().apply_updates();
    }
    
    std::vector<int> action = player_controllers[turn].get().get_action();
    resolve_action(action);

    return game_is_on;
}

void Game_master::end_turn()
{
    // Increment turn counter (also is playerid of whoever takes the turn)
    turn = (turn + 1) % player_controllers.size();
    turnAbsolute++;

	// Players starting their turn with zero money get a bonus
	if (players[turn].funds == 0) {
		players[turn].funds += BONUS_IF_ZERO_INCOME;
	}
    // Add basic income
    players[turn].funds += BASIC_INCOME;
    // Draw a card, fire event if both discard and deck are exhausted
    if(!resolve_draw(turn)) {
        broadcast_event({Game_master::HAPPENING, Game_master::PLAYER_DECK_NOREFRESH, turn});
    }
	// Check if player should gain victory points
    if(check_dominance(turn))
    {
        players[turn].points++;
        // Check if player wins on this turn
        if(players[turn].points >= POINTS_REQ_FOR_VICTORY)
        {
            game_is_on = false;
            // Notify everyone that game ended
            broadcast_event({Game_master::GAME_ENDED, turn});
        }
    }


    for(card_ref cref : active_cards)
    {
        if(cref.get().type == Card::UNIT && cref.get().owner_id == turn) // Un-overwhelm units and refresh move and attack
        {
            cref.get().can_move       = true;
            cref.get().can_attack     = true;
            cref.get().is_overwhelmed = false;
        }
        if(cref.get().type == Card::CONTRACT && cref.get().owner_id == turn) // Gain profit for each contract and decrement their duration
        {
            cref.get().value--;
            players[turn].funds++;

            if(cref.get().value <= 0) {
                resolve_destruction(cref);
            } 
        }
    }
};

bool Game_master::resolve_action(const std::vector<int> &action)
{
    int delta_x, delta_y; // Relative target postition for attacking and moving.
    int direction = -1; // Direction of attack or movement.
    char action_type = action[0];

    switch(action_type)
    {

        //  PASS  _________________________________________________

        case Game_master::PASS:
        // self-explanatory
        send_event({Game_master::ORDER_CONFIRM}, turn);
        end_turn(); 
        return true;
        // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        break;

        //  MOVE  _________________________________________________

        case Game_master::MOVE:
        {

            // check whether all arguments are present
            if(action[1] < 0 || action[2] < 0 || action[3] < 0 || action[4] < 0) {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                return false;
			}
            // check for out-of-bounds arguments
            if(action[1] >= grid.size() || action[2] >= grid[0].size() || \
            action[3] >= grid.size() || action[4] >= grid[0].size()) 
            {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                return false;
			}
                // determine direction and check if it is invalid
                delta_x = action[3] - action[1];
                delta_y = action[4] - action[2];
                        // valid
                if     (delta_x ==  1 && delta_y ==  0)    direction = Game_master::DOWN;
                else if(delta_x == -1 && delta_y ==  0)    direction = Game_master::UP; 
                else if(delta_x ==  0 && delta_y ==  1)    direction = Game_master::RIGHT;
                else if(delta_x ==  0 && delta_y == -1)    direction = Game_master::LEFT;
                else    //invalid
                {
                    send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                    return false;
                }

            // check that an unit was selected
            if(!grid[action[1]][action[2]].card.has_value())       
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOSELECT}, turn);
                return false;	
            }
            // check that target tile is vacant
            if(grid[action[3]][action[4]].card.has_value())       
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOTARGET}, turn);
                return false;	
            }
            // check ownership permissions
            if(grid[action[1]][action[2]].card->get().owner_id != turn) 
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_PERMISSION}, turn);
                return false;
            }
            // check if ability was exhausted
            if(!grid[action[1]][action[2]].card->get().can_move)        
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_EXHAUSTED}, turn);
                return false;
            }

            // exec order

            if(!resolve_movement(grid[action[1]][action[2]].card.value(), direction)) {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_UNKNOWN}, turn);
                return false;
            }

            std::cout << "Executing..." << std::endl;
            send_event({Game_master::ORDER_CONFIRM}, turn);
            return true;
        }
        break;
        
        //  ATTACK  _________________________________________________

        case Game_master::ATTACK:
        {

            // ensure all arguments are present
            if(action.size() != 5) {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                return false;
			}
            // check for out-of-bounds arguments
            if(action[1] >= grid.size() || action[2] >= grid[0].size() || \
            action[3] >= grid.size() || action[4] >= grid[0].size()) 
            {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                return false;
			}
            // determine direction and check if it is invalid
                delta_x = action[3] - action[1];
                delta_y = action[4] - action[2];
                        // valid adjacent
                if     (delta_x ==  1 && delta_y ==  0) { direction = Game_master::DOWN; 		}
                else if(delta_x == -1 && delta_y ==  0) { direction = Game_master::UP; 		    }
                else if(delta_x ==  0 && delta_y ==  1) { direction = Game_master::RIGHT; 		}
                else if(delta_x ==  0 && delta_y == -1) { direction = Game_master::LEFT; 		}
                        // valid diagonals
                else if(delta_x ==  1 && delta_y ==  1) { direction = Game_master::DOWNRIGHT; 	}
                else if(delta_x ==  1 && delta_y == -1) { direction = Game_master::DOWNLEFT;	}
                else if(delta_x == -1 && delta_y ==  1) { direction = Game_master::UPRIGHT;	    }
                else if(delta_x == -1 && delta_y == -1) { direction = Game_master::UPLEFT;		}
                else    //invalid
                {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                return false;
                }

            // check that unit was selected
            if(!grid[action[1]][action[2]].card.has_value())
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOSELECT}, turn);
                return false;	
            }
            // check target legality
            if(!grid[action[3]][action[4]].card.has_value()) 
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOTARGET}, turn);
                return false; 	
            }
            // check ownership permissions
            if(grid[action[1]][action[2]].card->get().owner_id != turn) 
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_PERMISSION}, turn);
                return false;
            }
            // check if ability was exhausted
            if(!grid[action[1]][action[2]].card->get().can_attack)      
            { 
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_EXHAUSTED}, turn);
                return false; 	
            }

            // process order
            if(!resolve_attack(grid[action[1]][action[2]].card.value(), direction)) {
            	send_event({Game_master::ORDER_INVALID, Game_master::ORD_UNKNOWN}, turn);
                return false;
			}
            send_event({Game_master::ORDER_CONFIRM}, turn);
            return true;
        }
        break;

        //  PLAY  _________________________________________________

        case Game_master::PLAY:
        {
        
            if(action[1] < 0 || action[1] >= players[turn].hand.size()) {                
            	send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                return false;
			}
            // Check for out-of-bounds arguments for unit deployment
            std::optional<tile_ref> deploySite; // Get tile reference for unit deployment
            if(players[turn].hand[action[1]].get().type == Card::UNIT)
            {
                if(action[2] < 0 || action[3] < 0) {
                    send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVARGS}, turn);
                    return false;
                }
                if(action[2] >= grid.size() || action[3] >= grid[0].size()) {
                    send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOFUNDS}, turn);
                    return false;
				}
                deploySite = std::ref(grid[action[2]][action[3]]);
            }
            // Ensure player has sufficient money
            if(players[turn].hand[action[1]].get().cost > players[turn].funds) {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOFUNDS}, turn);
                return false;
			}
            // Finally, perform the action and check for proper execution.
            if(!play_card(turn, action[1], deploySite)) {
                send_event({Game_master::ORDER_INVALID, Game_master::ORD_NOTARGET}, turn);
                return false;
			}
            send_event({Game_master::ORDER_CONFIRM}, turn); 
            return true; // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        }
        break;

        //  INVALID  _________________________________________________
        default:
            send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVTYPE}, turn);
            return false;
        break;
    }
    send_event({Game_master::ORDER_INVALID, Game_master::ORD_INVTYPE}, turn);
    return false;
}

void Game_master::update_status(int player_id)
{
    pctrl_ref controller = std::ref(player_controllers[player_id]);
    
    controller.get().active_cards = std::vector<Card_info>(active_cards.size());
    for(int i = 0; i < active_cards.size(); i++)
        controller.get().active_cards[i] = active_cards[i].get().get_info();

    controller.get().hand = std::vector<Card_info>(players[player_id].hand.size());
    for(int i = 0; i < players[player_id].hand.size(); i++)
        controller.get().hand[i] = players[player_id].hand[i].get().get_info();

    controller.get().players = std::vector<Player_info>(players.size());
    for(int i = 0; i < players.size(); i++)
        controller.get().players[i] = players[i].getInfo(decks[i]);
}

bool Game_master::check_dominance(int playerId)
{
    int unit_count_difference = 0;
    for(card_ref cref : active_cards) // Find active cards within capture zone
        if(cref.get().type == Card::UNIT && grid[cref.get().x][cref.get().y].type == Tile::CAPTUREZONE)
        {
            if(cref.get().owner_id == turn)
                unit_count_difference++;
            else
                unit_count_difference--;
        }
    return unit_count_difference > 0;
}

bool Game_master::deploy_card(Card& card, std::optional<tile_ref> target)
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

        card.can_attack = true;
        card.can_move = false;
        card.is_overwhelmed = false;
    }
    
    if(card.type != Card::TACTIC)
    {   // Update status
        card.status = Card::IN_PLAY;
        active_cards.push_back(std::ref(card));
    }
    broadcast_event({Game_master::HAPPENING, Game_master::DEPLOYED, card.match_id, card.x, card.y});
    // Fire trig_played events
    for(const auto& event_played : card.trig_played) {
        event_played(*this, card);
    }
    if(card.type == Card::TACTIC) {
        resolve_destruction(card);
    }
    return true;
}

bool Game_master::resolve_movement(Card& card, const int& direction)
{   
    // Check if direction is valid
    if(direction < 0 || direction > 3) throw std::invalid_argument("invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card->get().match_id != card.match_id) throw std::runtime_error("Trying to move an off-grid card");

    auto options = get_4neighbors(grid[card.x][card.y]);

    // TODO add mutual exchange movement option (stack movement operations up until a card is moved on an empty space)
    // Check if tile is occupied
    if(options[direction]->get().card)
        return false;

    // Exchange positions
    grid[card.x][card.y].card.reset();
    options[direction]->get().card = std::ref(card);
    // Update coords
    card.x = options[direction]->get().x;
    card.y = options[direction]->get().y;

    card.can_move = false;
    broadcast_event({Game_master::HAPPENING, Game_master::MOVED, card.match_id, direction});

    return true;
}

void Game_master::resolve_destruction(Card& card)
{   
    if(card.type != Card::TACTIC)
    {
        if(card.x > 0 && card.y > 0)
            grid[card.x][card.y].card.reset();

        std::function<bool(const card_ref&, const Card&)> compare_match_ids = \
            [](const card_ref& cr, const Card& c){ return cr.get().match_id == c.match_id; };
            
        if(!pop_element(active_cards, card, compare_match_ids))
            std::clog << "WARNING: killing card that was not in activeCards" << std::endl;

        if(card.status != Card::IN_PLAY)
            std::clog << "WARNING: killing card that was not \"in play\"" << std::endl;
    }
    broadcast_event({Game_master::HAPPENING, Game_master::DESTROYED, card.match_id});

    // Fire death events
    for(const auto& deathEvent : card.trig_destroyed)
        deathEvent(*this, card);

    // Send to discard
    card.status = Card::DISCARD;
    decks[card.owner_id].get().discard.push_back(std::ref(card));

    // Restore default power and linked triggers
    card = roster.get_card_instance(card.global_id);
}

bool Game_master::resolve_attack(Card& card, const int& direction)
{
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card->get().match_id != card.match_id) throw std::runtime_error("Attacking with card with bad position");

    std::vector<std::optional<tile_ref>> options = get_8neighbors(grid[card.x][card.y]);

    if(!options[direction].has_value() || !(options[direction]->get().card.has_value()))
        return false;

    // combat damage resolution
    resolve_combat(card, options[direction]->get().card.value());
    return true;
}

int Game_master::resolve_combat(Card& attacker, Card& defender)
{
    int advantage_difference = attacker.advantage - defender.advantage;
    // resolve advantage bonus if defender is overwhelmed
    if(defender.is_overwhelmed) advantage_difference++;
    // advantage resolution
    if(advantage_difference > 0)
        {
        defender.value -= advantage_difference;
        if(defender.value < 0) defender.value = 0;
        }
    else
        {
        attacker.value += advantage_difference;
        if(attacker.value < 0) attacker.value = 0;
        }
    // set status effects
    attacker.can_attack = false;
    attacker.can_move   = false;
    defender.is_overwhelmed = true;
    // normal damage resolution
    if(attacker.value > defender.value)
    {
        attacker.value -= defender.value;
        defender.value = 0;
        resolve_destruction(defender);
        return Game_master::DEFENDER_DEAD;
    }
    else if(attacker.value < defender.value)
    {
        defender.value -= attacker.value;
        attacker.value = 0;
        resolve_destruction(attacker);
        return Game_master::ATTACKER_DEAD;
    }
    else // attacker.value == defender.value
    {
        resolve_destruction(attacker);
        resolve_destruction(defender);
        return Game_master::BOTH_DEAD;
    }
}

std::vector<std::optional<tile_ref>> Game_master::get_4neighbors(const Tile& tile)    // Get 4-neighbourhood of a tile.
{
    std::vector<std::optional<tile_ref>> res(4);

    if(tile.x > 0)                           res[UP]    = std::ref(grid[tile.x-1][tile.y]);
    if(tile.y < grid[0].size() - 1)          res[RIGHT] = std::ref(grid[tile.x][tile.y+1]);
    if(tile.x < grid.size() - 1)             res[DOWN]  = std::ref(grid[tile.x+1][tile.y]);
    if(tile.y > 0)                           res[LEFT]  = std::ref(grid[tile.x][tile.y-1]);

    return res;
}

std::vector<std::optional<tile_ref>> Game_master::get_8neighbors(const Tile& tile) // Get 8-neighbourhood of a tile.
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

bool Game_master::resolve_draw(int player_id)
{
    if(decks[player_id].get().library.size() == 0)
        if(decks[player_id].get().discard.size() == 0)
            return false;
        else
            decks[player_id].get().refresh();
            broadcast_event({Game_master::HAPPENING, Game_master::PLAYER_DECK_REFRESH, player_id});

    if(decks[player_id].get().library.back().get().status != Card::DECK)
        std::clog << "Warning: drawing card that has not been marked as \"In deck\"" << std::endl;

    players[player_id].hand.push_back(decks[player_id].get().library.back());
    decks[player_id].get().library.back().get().status = Card::HAND;
    decks[player_id].get().library.pop_back();
    // send updates to player controllers
    broadcast_event({Game_master::HAPPENING, Game_master::PLAYER_DRAWS, player_id});

    return true;
}

bool Game_master::discard(int player_id, int hand_index)
{
    if(player_id < 0 || player_id >= players.size()) throw std::out_of_range("playerId out of range");
    if(hand_index < 0 || hand_index >= players[player_id].hand.size()) throw std::out_of_range("handIndex out of range");

    card_ref card_to_discard = players[player_id].hand[hand_index];
    if(card_to_discard.get().status != Card::HAND)
        std::clog << "Warning: discarding card that has not been marked as \"In hand\"" << std::endl;

    decks[player_id].get().discard.push_back(card_to_discard);
    card_to_discard.get().status = Card::DISCARD;
    pop_index(players[player_id].hand, hand_index);
    // send updates to controllers
    broadcast_event({Game_master::HAPPENING, Game_master::PLAYER_DISCARDS, player_id, card_to_discard.get().match_id});

    return true;
}

bool Game_master::play_card(int player_id, int hand_index, std::optional<tile_ref> target) // Returns false if there are insufficient funds.
{
    Card& played = players[player_id].hand[hand_index];
    // Check index correctness
    if(player_id < 0 || player_id >= players.size()) throw std::out_of_range("Player ID out of range");
    if(hand_index < 0 || hand_index >= players[player_id].hand.size()) throw std::out_of_range("Hand index out of range");

    // Check whether a player has enough funds to play the card (subtract them if success)
    if(players[player_id].funds < played.cost)
        return false;

    // Check that deployment location was provided if the card is a unit
    if(played.type == Card::UNIT)
    {
        if(!target.has_value())
            std::runtime_error("No deployment site provided for a unit");

        // Check surroundings or deployment zones
        if(target->get().type != player_id) //If deploying outside of Base
        {
            // Check if there is at least one friendly unit on adjacent tiles
            bool nearFriendly = false;
            for(std::optional<tile_ref> tptr : get_4neighbors(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && \
                   tptr->get().card->get().owner_id == player_id)
                    nearFriendly = true;

            if(!nearFriendly) return Game_master::ORD_NOTARGET;

            // Check if there are no enemies on surrounding tiles
            for(std::optional<tile_ref> tptr : get_8neighbors(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && 
                   tptr->get().card->get().owner_id != player_id)
                    return Game_master::ORD_NOTARGET;
        }
    }

    if(played.status != Card::HAND)
        std::clog << "Warning: trying to play card that has not been marked as \"In hand\"" << std::endl;

    players[player_id].funds -= players[player_id].hand[hand_index].get().cost;
    
    pop_index(players[player_id].hand, hand_index);
    bool res = deploy_card(played, target);

    return res;
}

