#include "Game_master.hpp"

#include <stdexcept>
#include <iostream>
#include <optional>
#include <memory>

Game_master::Game_master(const std::vector<pctrl_ref>& commanders_ref, \
                         const std::vector<std::vector<int>>& deck_images,
                         const std::vector<std::vector<int>>& terrain) : card_generator(Game_master::Card_generator::get())
{
    // Check argument validity
    if(commanders_ref.size() > deck_images.size()) {
        throw std::runtime_error("Insufficient decks specified for game creation;");
    }
    if(commanders_ref.size() < deck_images.size()) {
        std::clog << "Redundant decks provided for Game_master constructor. Ignoring last;" << std::endl;
	}

    std::vector<std::vector<int>> n_terrain;
    if(terrain.size() == 0 || terrain[0].size() == 0)   // use default terrain
    {
        n_terrain = std::vector<std::vector<int>>(GRID_HEIGHT, std::vector<int>(GRID_WIDTH, Tile::NORMAL));
        for(int row = 0; row < GRID_HEIGHT; row++) 
        {
        // Deploy zone
        n_terrain[row][0] = 0;
        n_terrain[row][GRID_WIDTH-1] = 1;
        // Capture zone
        n_terrain[row][(GRID_WIDTH-1)/2] = Tile::OBJECTIVE;
        n_terrain[row][(GRID_WIDTH)/2] = Tile::OBJECTIVE;
        }
    }
    else
    {
        n_terrain = terrain;
    }
    int grid_height = n_terrain.size();
    int grid_width = n_terrain[0].size();
    // Unpack decks
    commanders = commanders_ref;
    for(const std::vector<int>& image : deck_images)
    {
        decks.push_back(image);
    }
    // Build grid
    grid = std::vector<std::vector<Tile>>(GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH));

    for(int row = 0; row < GRID_HEIGHT; row++) {
        for(int column = 0; column < GRID_WIDTH; column ++) {
            grid[row][column] = Tile(row, column, n_terrain[row][column]);
        }
	}

    // Assign IDs to various entities
    int cards_in_match = 0; // TODO reserve id space for tokens

    for(int playerid = 0; playerid < commanders.size(); playerid++) {
        players.emplace_back(playerid);

        commanders[playerid].get().id = playerid;

        commanders[playerid].get().grid_height = GRID_HEIGHT,
        commanders[playerid].get().grid_width = GRID_WIDTH;

        for(Card& card : decks[playerid].all) {
            card.owner_id = playerid;

            card.match_id = cards_in_match;
            cards_in_match++;
        }
    }

    turn = -1;
    turn_absolute = -1;

    // Deal cards TODO add re-draw
    for(int id = 0; id < players.size(); id++)
    {
    	// Shuffle decks
        decks[id].shuffle();
        // Deal starting hand
        for(int card = 0; card < STARTING_HAND_SIZE; card++) {
            resolve_draw(id);
        }
    }

    game_is_on = true;
    end_turn(); // end -1st turn
}

bool Game_master::game_loop() 
{
    // Send game status updates to every player
    for(int id = 0; id < commanders.size(); id++)
    {
        update_status(id);
    }

    std::function<bool(Player&)> check_activity = [](Player& p){ return p.is_active; };

    if(std::find_if(players.begin(), players.end(), check_activity) == players.end()) // End game if all players are inactive
    {
        game_is_on = false;
        // Announce game end with no winner
        Commander::Event event;
        event.type = Commander::Event::EV_GAME_WON_BY;
        event.data.push_back(-1);
        broadcast_event(event);

        return game_is_on;
    }
    while (!players[turn].is_active) // Skip inactive players
    {
        turn++;
    }
    Commander::Order action = commanders[turn].get().get_order();
    int action_verdict = exec_order(action);

    Commander::Event order_response = {};
    if(action_verdict != Commander::Order::INVORD_NONE) { // Invalid action received;
        order_response.type = Commander::Event::EV_ORDER_INVALID,
        order_response.data.push_back(action_verdict);
    }
    
    commanders[turn].get().process_event(order_response);

    return game_is_on;
}

inline void Game_master::broadcast_event(const Commander::Event& event)
{
    for (pctrl_ref pc : commanders)
    {
        pc.get().process_event(event);
    }
}

void Game_master::end_turn()
{
    // Increment turn counter (also is playerid of whoever takes the turn)
    turn = (turn + 1) % commanders.size();
    turn_absolute++;

	// Players starting their turn with zero money get a bonus
	if (players[turn].funds == 0) 
    {
		players[turn].funds += BONUS_IF_ZERO_INCOME;
	}
    // Add basic income
    players[turn].funds += BASIC_INCOME;
    // Draw a card, fire event if both discard and deck are exhausted
    if (!resolve_draw(turn)) // move this cond to resolve_draw
    {
        Commander::Event event;
        event.type = Commander::Event::EV_DECK_NOREFRESH;
        event.data.push_back(turn);
        broadcast_event(event);
    }
	// Check if player should gain victory points
    if (check_dominance(turn))
    {
        players[turn].points++;
        // Check if player wins on this turn
        if(players[turn].points >= POINTS_REQ_FOR_VICTORY)
        {
            for(int id = 0; id < commanders.size(); id++)
            {
                update_status(id); // Send last game status update
            }
            game_is_on = false;
            // Notify everyone that game ended
            Commander::Event event;
            event.type = Commander::Event::EV_GAME_WON_BY;
            event.data.push_back(turn);
            broadcast_event(event);
        }
    }

    for(card_ref cref : active_cards)
    {
        if(cref.get().type == CTYPE_UNIT && cref.get().owner_id == turn) // Un-overwhelm units and refresh move and attack
        {
            cref.get().can_move       = true;
            cref.get().can_attack     = true;
            cref.get().is_overwhelmed = false;
        }
        if(cref.get().type == CTYPE_CONTRACT && cref.get().owner_id == turn) // Gain profit for each contract and decrement their duration
        {
            cref.get().value--;
            players[turn].funds++;

            if(cref.get().value <= 0) {
                resolve_destruction(cref);
            } 
        }
    }
};

int Game_master::exec_order(const Commander::Order& order)
{
    int delta_x, delta_y; // Relative target postition for attacking and moving.
    int direction = -1; // Direction of attack or movement.

    switch(order.type)
    {

        //  PASS  _________________________________________________

        case Commander::Order::ORD_PASS:
        // self-explanatory
        end_turn();
        return Commander::Order::INVORD_NONE; // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        break;

        //  MOVE  _________________________________________________

        case Commander::Order::ORD_MOVE:
        {

            // check whether all arguments are present
            if(order.data[0] < 0 || order.data[1] < 0 || order.data[2] < 0 || order.data[3] < 0) {
                return Commander::Order::INVORD_INVARGS;
			}
            // check for out-of-bounds arguments
            if(order.data[0] >= grid.size() || order.data[1] >= grid[0].size() || \
            order.data[2] >= grid.size() || order.data[3] >= grid[0].size()) {
                return Commander::Order::INVORD_INVARGS;
			}
                // determine direction and check if it is invalid
                delta_x = order.data[2] - order.data[0];
                delta_y = order.data[3] - order.data[1];
                        // valid
                if     (delta_x ==  1 && delta_y ==  0) { direction = Tile::DOWN; }
                else if(delta_x == -1 && delta_y ==  0) { direction = Tile::UP; }
                else if(delta_x ==  0 && delta_y ==  1) { direction = Tile::RIGHT; }
                else if(delta_x ==  0 && delta_y == -1) { direction = Tile::LEFT; }
                else    //invalid
                return Commander::Order::INVORD_INVARGS;

            // check that an unit was selected
            if(!grid[order.data[0]][order.data[1]].card.has_value())       { return Commander::Order::INVORD_NOSELECT; 	}
            // check that target tile is vacant
            if(grid[order.data[2]][order.data[3]].card.has_value())       { return Commander::Order::INVORD_NOTARGET; 	}
            // check ownership permissions
            if(grid[order.data[0]][order.data[1]].card->get().owner_id != turn) { return Commander::Order::INVORD_PERMISSION; }
            // check if ability was exhausted
            if(!grid[order.data[0]][order.data[1]].card->get().can_move)        { return Commander::Order::INVORD_EXHAUSTED; 	}

            // exec order

            if(!resolve_movement(grid[order.data[0]][order.data[1]].card.value(), direction)) {
                return Commander::Order::INVORD_UNKNOWN;
            }

            std::cout << "Executing..." << std::endl;
            return Commander::Order::INVORD_NONE;
        }
        break;
        
        //  ATTACK  _________________________________________________

        case Commander::Order::ORD_ATTACK:
        {

            // ensure all arguments are present
            if(order.data[0] < 0 || order.data[1] < 0 || order.data[2] < 0 || order.data[3] < 0) {
                return Commander::Order::INVORD_INVARGS;
			}
            // check for out-of-bounds arguments
            if(order.data[0] >= grid.size() || order.data[1] >= grid[0].size() || \
            order.data[2] >= grid.size() || order.data[3] >= grid[0].size()) {
                return Commander::Order::INVORD_INVARGS;
			}
            // determine direction and check if it is invalid
                delta_x = order.data[2] - order.data[0];
                delta_y = order.data[3] - order.data[1];
                        // valid adjacent
                if     (delta_x ==  1 && delta_y ==  0) { direction = Tile::DOWN;   }
                else if(delta_x == -1 && delta_y ==  0) { direction = Tile::UP;     }
                else if(delta_x ==  0 && delta_y ==  1) { direction = Tile::RIGHT;  }
                else if(delta_x ==  0 && delta_y == -1) { direction = Tile::LEFT; 	}
                        // valid diagonals
                else if(delta_x ==  1 && delta_y ==  1) { direction = Tile::DOWNRIGHT;  }
                else if(delta_x ==  1 && delta_y == -1) { direction = Tile::DOWNLEFT;	}
                else if(delta_x == -1 && delta_y ==  1) { direction = Tile::UPRIGHT;	}
                else if(delta_x == -1 && delta_y == -1) { direction =   Tile::UPLEFT;	}
                else    //invalid
                return Commander::Order::INVORD_INVARGS;

            // check that unit was selected
            if(!grid[order.data[0]][order.data[1]].card.has_value())       { return Commander::Order::INVORD_NOSELECT; 	}
            // check target legality
            if(!grid[order.data[2]][order.data[3]].card.has_value())       { return Commander::Order::INVORD_NOTARGET; 	}
            // check ownership permissions
            if(grid[order.data[0]][order.data[1]].card->get().owner_id != turn) { return Commander::Order::INVORD_PERMISSION; }
            // check if ability was exhausted
            if(!grid[order.data[0]][order.data[1]].card->get().can_attack)      { return Commander::Order::INVORD_EXHAUSTED; 	}

            // process order
            if(!resolve_attack(grid[order.data[0]][order.data[1]].card.value(), direction)) {
            	return Commander::Order::INVORD_UNKNOWN;
			}
            std::cout << "Executing..." << std::endl;
            return Commander::Order::INVORD_NONE;
        }
        break;

        //  PLAY  _________________________________________________

        case Commander::Order::ORD_PLAY:
        {
        
            if(order.data[0] < 0 || order.data[0] >= players[turn].hand.size()) {                
            	return Commander::Order::INVORD_INVARGS;
			}
            // Check for out-of-bounds arguments for unit deployment
            std::optional<tile_ref> deploySite; // Get tile reference for unit deployment
            if(players[turn].hand[order.data[0]].get().type == CTYPE_UNIT)
            {
                if(order.data[1] < 0 || order.data[2] < 0) {
                    return Commander::Order::INVORD_INVARGS;
                }
                if(order.data[1] >= grid.size() || order.data[2] >= grid[0].size()) {
                    return Commander::Order::INVORD_INVARGS;
				}
                deploySite = std::ref(grid[order.data[1]][order.data[2]]);
            }
            // Ensure player has sufficient money
            if(players[turn].hand[order.data[0]].get().cost > players[turn].funds) {
                return Commander::Order::INVORD_NOFUNDS;
			}
            // Finally, perform the action and check for proper execution.
            if(!play_card(turn, order.data[0], deploySite)) {
                return Commander::Order::INVORD_NOTARGET;
			}
            return Commander::Order::INVORD_NONE; // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        }
        break;

        //  INVALID  _________________________________________________

        default:
            return Commander::Order::INVORD_INVTYPE;
        break;
    }
    //return Commander::Order::INVORD_INVTYPE;
}

void Game_master::update_status(int player_id)
{
    pctrl_ref controller = commanders[player_id];

    controller.get().turn = turn;
    controller.get().turn_absolute = turn_absolute;
    
    controller.get().active_cards.resize(active_cards.size());
    for(int i = 0; i < active_cards.size(); i++)
    {
        controller.get().active_cards[i] = active_cards[i].get().get_info();
    }

    controller.get().hand.resize(players[player_id].hand.size());
    for(int i = 0; i < players[player_id].hand.size(); i++)
    {
        controller.get().hand[i] = players[player_id].hand[i].get().get_info();
    }

    controller.get().players.resize(players.size());
    for(int i = 0; i < players.size(); i++)
    {
        controller.get().players[i] = players[i].get_info(decks[i]);
    }
}

bool Game_master::check_dominance(int playerId)
{
    int unit_count_difference = 0;
    for(card_ref cref : active_cards) // Find active cards within capture zone
        if(cref.get().type == CTYPE_UNIT && grid[cref.get().x][cref.get().y].type == Tile::OBJECTIVE)
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
    if(card.type == CTYPE_UNIT && !target.has_value()) throw std::runtime_error("No deployment site provided for unit");
    if(card.status == Card::CSTATUS_IN_PLAY) std::clog << "WARNING: Deploying card marked as \"IN PLAY\"";

    if(card.type == CTYPE_UNIT)
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
    
    if(card.type != CTYPE_TACTIC)
    {   // Update status
        card.status = Card::CSTATUS_IN_PLAY;
        active_cards.push_back(std::ref(card));
    }

    // Fire trig_played events
    for(const auto& event_played : card.trig_played) {
        event_played();
    }
    if(card.type == CTYPE_TACTIC) {
        resolve_destruction(card);
    }
    return true;
}

bool Game_master::resolve_movement(Card& card, const int& direction)
{   
    // Check if direction is valid
    if(direction < 0 || direction > 3) throw std::invalid_argument("invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        &grid[card.x][card.y].card->get() != &card) throw std::runtime_error("Trying to move an off-grid card");

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

    return true;
}

void Game_master::resolve_destruction(Card& card)
{   
    if(card.type != CTYPE_TACTIC)
    {
        if(card.x > 0 && card.y > 0)
            grid[card.x][card.y].card.reset();

        std::function<bool(card_ref&, Card&)> compare_origins = \
                    [](card_ref& cr, Card& c){ return &cr.get() == &c; };
            
        if(!pop_element(active_cards, card, compare_origins))
            std::clog << "WARNING: killing card that was not in activeCards" << std::endl;

        if(card.status != Card::CSTATUS_IN_PLAY)
            std::clog << "WARNING: killing card that was not \"in play\"" << std::endl;
    }

    // Fire death events
    for(const auto& death_event : card.trig_destroyed)
        death_event();

    // Send to discard
    card.status = Card::CSTATUS_DISCARD;
    decks[card.owner_id].discard.push_back(std::ref(card));

    // Restore default power and linked triggers
    card = card_generator.get_card_instance(card.global_id);
}

bool Game_master::resolve_attack(Card& card, const int& direction)
{
    // Check if card's position on grid is OK
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card->get().match_id != card.match_id) throw std::runtime_error("Attacking with card with bad position");

    std::vector<std::optional<tile_ref>> options = get_8neighbors(grid[card.x][card.y]);
    // Check if there is a target in this direction.
    if(!options[direction].has_value() || !(options[direction]->get().card.has_value()))
        return false;

    resolve_combat(card, options[direction]->get().card.value());
    return true;
}

int Game_master::resolve_combat(Card& attacker, Card& defender)
{
    int advantage_difference = attacker.advantage - defender.advantage;
    // Advantage bonus if defender is overwhelmed
    if(defender.is_overwhelmed) { advantage_difference++; }
    // Environmental advantage
    if(defender.x > 0 && defender.y > 0)
    {
    if(grid[defender.x][defender.y].type = Tile::TERR_ADV)    { advantage_difference--; }
    if(grid[defender.x][defender.y].type = Tile::TERR_DISADV) { advantage_difference++; }
    }
    if(attacker.x > 0 && attacker.y > 0)
    {
    if(grid[attacker.x][attacker.y].type = Tile::TERR_ADV)    { advantage_difference--; }
    if(grid[attacker.x][attacker.y].type = Tile::TERR_DISADV) { advantage_difference++; }
    }
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

std::vector<std::optional<Game_master::tile_ref>> Game_master::get_4neighbors(const Tile& tile)    
{
    std::vector<std::optional<Game_master::tile_ref>> res(4);

    if(tile.x > 0)                  { res[Tile::UP]    = std::ref(grid[tile.x-1][tile.y]); }
    if(tile.y < grid[0].size() - 1) { res[Tile::RIGHT] = std::ref(grid[tile.x][tile.y+1]); }
    if(tile.x < grid.size() - 1)    { res[Tile::DOWN]  = std::ref(grid[tile.x+1][tile.y]); }
    if(tile.y > 0)                  { res[Tile::LEFT]  = std::ref(grid[tile.x][tile.y-1]); }

    return res;
}

std::vector<std::optional<Game_master::tile_ref>> Game_master::get_8neighbors(const Tile& tile)
{
    std::vector<std::optional<tile_ref>> res(8);
    
    if(tile.x > 0)
    {
        res[Tile::UP] = std::ref(grid[tile.x-1][tile.y]);

        if(tile.y > 0)                  { res[Tile::UPLEFT]    = std::ref(grid[tile.x-1][tile.y-1]); }
        if(tile.y < grid[0].size() - 1) { res[Tile::UPRIGHT]   = std::ref(grid[tile.x-1][tile.y+1]); }
    }

    if(tile.x < grid.size() - 1)
    {
        res[Tile::DOWN] = std::ref(grid[tile.x+1][tile.y]);

        if(tile.y > 0)                  { res[Tile::DOWNLEFT]  = std::ref(grid[tile.x+1][tile.y-1]); }
        if(tile.y < grid[0].size() - 1) { res[Tile::DOWNRIGHT] = std::ref(grid[tile.x+1][tile.y+1]); }
    }

    if(tile.y > 0)                      { res[Tile::LEFT]      = std::ref(grid[tile.x][tile.y-1]); }
    if(tile.y < grid[0].size() - 1)     { res[Tile::RIGHT]     = std::ref(grid[tile.x][tile.y+1]); }

    return res;
}

bool Game_master::resolve_draw(int player_id)
{
    if(decks[player_id].library.size() == 0)
    {
        if(decks[player_id].discard.size() == 0)
        {
            return false;
        }
        else
        {
            decks[player_id].refresh();
        }
    }
    if(decks[player_id].library.back().get().status != Card::CSTATUS_IN_DECK)
    {
        std::clog << "Warning: drawing card that has not been marked as \"In deck\"" << std::endl;
    }
    players[player_id].hand.push_back(decks[player_id].library.back());
    decks[player_id].library.back().get().status = Card::CSTATUS_IN_HAND;
    decks[player_id].library.pop_back();

    return true;
}

bool Game_master::discard(int player_id, int hand_index)
{
    if(player_id < 0 || player_id >= players.size()) throw std::out_of_range("playerId out of range");
    if(hand_index < 0 || hand_index >= players[player_id].hand.size()) throw std::out_of_range("handIndex out of range");

    if(players[player_id].hand[hand_index].get().status != Card::CSTATUS_IN_HAND)
        std::clog << "Warning: discarding card that has not been marked as \"In hand\"" << std::endl;

    decks[player_id].discard.push_back(players[player_id].hand[hand_index]);
    players[player_id].hand[hand_index].get().status = Card::CSTATUS_DISCARD;
    pop_index(players[player_id].hand, hand_index);

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
    if(played.type == CTYPE_UNIT)
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

            if(!nearFriendly) return Commander::Order::INVORD_NOTARGET;

            // Check if there are no enemies on surrounding tiles
            for(std::optional<tile_ref> tptr : get_8neighbors(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && 
                   tptr->get().card->get().owner_id != player_id)
                    return Commander::Order::INVORD_NOTARGET;
        }
    }

    if(played.status != Card::CSTATUS_IN_HAND)
        std::clog << "Warning: trying to play card that has not been marked as \"In hand\"" << std::endl;

    players[player_id].funds -= players[player_id].hand[hand_index].get().cost;
    
    pop_index(players[player_id].hand, hand_index);
    bool res = deploy_card(played, target);

    return res;
}

