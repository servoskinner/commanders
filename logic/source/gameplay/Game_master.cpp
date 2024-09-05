#include "Game_master.hpp"

#include <stdexcept>
#include <iostream>
#include <optional>
#include <memory>

Game_master::Game_master(const std::vector<std::vector<int>>& deck_images,
                         int gamemode,
                         int nplayers,
                         const std::vector<std::vector<int>>& terrain)
{
    // for now, assuming that gamemode is GM_STANDARD
    // Check argument validity
    if(gamemode == GM_STANDARD && deck_images.size() < 1 ||
       deck_images.size() == 0) {
        throw std::runtime_error("Insufficient decks specified for game creation;");
    }
    if(gamemode == GM_STANDARD && deck_images.size() > 2 ||
       gamemode == GM_SKIRMISH && deck_images.size() > 1 ||
       gamemode == GM_2V2 && (deck_images.size() == 3 || deck_images.size() > 4)) {
        std::clog << "Redundant decks provided for Game_master constructor. Ignoring last;" << std::endl;
	}

    // Set up grid
    std::vector<std::vector<int>> n_terrain;
    if(terrain.size() < 2 || terrain[0].size() < 2)   // Use default terrain if none was provided
    {
        n_terrain = {GRID_HEIGHT, std::vector<int>(GRID_WIDTH, Tile::NORMAL)};
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
    
    // Build grid
    grid = {GRID_HEIGHT, std::vector<Tile>(GRID_WIDTH)};

    for(int row = 0; row < GRID_HEIGHT; row++) {
        for(int column = 0; column < GRID_WIDTH; column ++) {
            grid[row][column] = Tile(row, column, n_terrain[row][column]);
        }
	}

    switch (gamemode)
    {
    case GM_STANDARD:
        nplayers = 2;
        break;
    case GM_2V2:
        nplayers = 4;
    }
    // Reserve space for all entities to prevent re-allocation
    event_queues = std::vector<std::queue<Commander::Event>>(nplayers, std::queue<Commander::Event>());
    decks.reserve(deck_images.size());
    players.reserve(nplayers);

    // Assign IDs to various entities
    if (gamemode != GM_SKIRMISH) {
        for(int playerid = 0; playerid < nplayers; playerid++) {
            decks.emplace_back(*this, deck_images[playerid], playerid);
            players.emplace_back(playerid, decks.back());
        }
    }
    else {
        decks.emplace_back(*this, deck_images[0]);
        for (Player& p : players) {
            p.deck = decks.back();
        }
    }

    turn = -1;
    turn_absolute = -1;
    // shuffle decks
    for (Deck& deck : decks) {
        deck.shuffle();
    }
    // Deal cards TODO add re-draw
    for(int id = 0; id < players.size(); id++) {
        // Deal starting hand
        for(int card = 0; card < STARTING_HAND_SIZE; card++) {
            resolve_draw(id);
        }
    }
    game_is_on = true;
    end_turn(); // end -1st turn
}

std::optional<Game_master::Card_ref> Game_master::find_card(unsigned int entity_id)
{
    std::optional<Card_ref> card = {};
    for (Deck& deck : decks)
    {
        auto iter = std::find_if(deck.all.begin(), deck.all.end(), [entity_id](Card& card){ return card.entity_id == entity_id;} );
        if (iter != deck.all.end()) {
            card.emplace(*iter);
            break;
        }
    }
    if (!card.has_value()) {
        auto iter = std::find_if(tokens.begin(), tokens.end(), [entity_id](Card& card){ return card.entity_id == entity_id;} );
        if (iter != tokens.end()) {
            card.emplace(*iter);
        }
    }
    return card;
}

std::optional<Commander::Event> Game_master::get_event(int player_id)
{
    if (event_queues[player_id].empty()) {
        return {};
    }
    else {
        return {event_queues[player_id].back()};
    }
}

inline void Game_master::push_event(int player_id, const Commander::Event& event)
{
    if (event_queues[player_id].size() < EVENT_QUEUE_CAPACITY || EVENT_QUEUE_CAPACITY < 0) {
        event_queues[player_id].push(event);
    }
}

inline void Game_master::broadcast_event(const Commander::Event& event)
{
    for (std::queue<Commander::Event>& queue : event_queues) {
        if (queue.size() < EVENT_QUEUE_CAPACITY || EVENT_QUEUE_CAPACITY < 0) {
            queue.push(event);
        }
    }
    for (std::function<void(Commander::Event)> callback : on_event_broadcast) {
        callback(event);
    }
}

void Game_master::end_turn()
{
    // Increment turn counter (also is playerid of whoever takes the turn)
    turn = (turn + 1) % players.size();
    turn_absolute++;

	// Players starting their turn with zero money get a bonus
	if (players[turn].funds <= BONUS_INCOME_THRESHOLD) 
    {
		players[turn].funds += BONUS_INCOME;
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
            game_is_on = false;
            // Notify everyone that game ended
            Commander::Event event;
            event.type = Commander::Event::EV_GAME_WON_BY;
            event.data.push_back(turn);
            broadcast_event(event);
        }
    }

    for(Card_ref cref : active_cards)
    {
        if(cref.get().type == CTYPE_UNIT && cref.get().controller_id == turn) // Un-overwhelm units and refresh move and attack
        {
            cref.get().can_move       = true;
            cref.get().can_attack     = true;
            cref.get().is_overwhelmed = false;
        }
        if(cref.get().type == CTYPE_CONTRACT && cref.get().controller_id == turn) // Gain profit for each contract and decrement their duration
        {
            inflict_damage(cref.get(), 1);
            players[turn].funds++;
        }
    }
};

int Game_master::exec_order(int player_id, const Commander::Order& order)
{
    int delta_x, delta_y; // Relative target postition for attacking and moving.
    int direction = -1; // Direction of attack or movement.

    switch(order.type)
    {
        //  EMPTY _________________________________________________
        case Commander::Order::ORD_DO_NOTHING:
        return Commander::Order::ORD_SUCCESS;
        //  PASS  _________________________________________________
        case Commander::Order::ORD_PASS:
        { // ends turn: self-explanatory
        end_turn(); // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        return Commander::Order::ORD_SUCCESS;
        }
        break;

        //  MOVE  _________________________________________________
        case Commander::Order::ORD_MOVE:
        {

            // check whether all arguments are present
            if(order.data[0] < 0 || order.data[1] < 0 || order.data[2] < 0 || order.data[3] < 0) {
                return Commander::Order::ORD_INVARGS;
			}
            // check for out-of-bounds arguments
            if(order.data[0] >= grid.size() || order.data[1] >= grid[0].size() || \
            order.data[2] >= grid.size() || order.data[3] >= grid[0].size()) {
                return Commander::Order::ORD_INVARGS;
			}
            // determine direction and check if it is invalid
            delta_x = order.data[2] - order.data[0];
            delta_y = order.data[3] - order.data[1];
                    // valid
            if     (delta_x ==  1 && delta_y ==  0) { direction = Tile::DOWN; }
            else if(delta_x == -1 && delta_y ==  0) { direction = Tile::UP; }
            else if(delta_x ==  0 && delta_y ==  1) { direction = Tile::RIGHT; }
            else if(delta_x ==  0 && delta_y == -1) { direction = Tile::LEFT; }
            else {
                return Commander::Order::ORD_INVARGS;
            }

            // check that an unit was selected
            if(!grid[order.data[0]][order.data[1]].card.has_value()) { 
                return Commander::Order::ORD_NOSELECT; 	
            }
            // check that target tile is vacant
            if(grid[order.data[2]][order.data[3]].card.has_value()) { 
                return Commander::Order::ORD_NOTARGET; 	
            }
            // check ownership permissions
            if(grid[order.data[0]][order.data[1]].card->get().controller_id != turn) {
                return Commander::Order::ORD_PERMISSION; 
            }
            // check if ability was exhausted
            if(!grid[order.data[0]][order.data[1]].card->get().can_move) { 
                return Commander::Order::ORD_EXHAUSTED; 	
            }
            // execute order
            if(!resolve_movement(grid[order.data[0]][order.data[1]].card.value(), direction)) {
                return Commander::Order::ORD_UNKNOWN;
            }
            // push feedback
            return Commander::Order::ORD_SUCCESS;
        }
        break;
        
        //  ATTACK  _________________________________________________
        case Commander::Order::ORD_ATTACK:
        {
            // ensure all arguments are present
            if(order.data[0] < 0 || order.data[1] < 0 || order.data[2] < 0 || order.data[3] < 0) {
                return Commander::Order::ORD_INVARGS;
			}
            // check for out-of-bounds arguments
            if(order.data[0] >= grid.size() || order.data[1] >= grid[0].size() || \
            order.data[2] >= grid.size() || order.data[3] >= grid[0].size()) {
                return Commander::Order::ORD_INVARGS;
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
                else   { //invalid
                    return Commander::Order::ORD_INVARGS;
                }

            // check that unit was selected
            if(!grid[order.data[0]][order.data[1]].card.has_value()) { 
                return Commander::Order::ORD_NOSELECT; 	
            }
            // check target legality
            if(!grid[order.data[2]][order.data[3]].card.has_value()) { 
                return Commander::Order::ORD_NOTARGET; 	
            }
            // check ownership permissions
            if(grid[order.data[0]][order.data[1]].card->get().controller_id != turn) { 
                return Commander::Order::ORD_PERMISSION; 
            }
            // check if ability was exhausted
            if(!grid[order.data[0]][order.data[1]].card->get().can_attack) { 
                return Commander::Order::ORD_EXHAUSTED; 	
            }

            // process order
            if(!resolve_attack(grid[order.data[0]][order.data[1]].card.value(), direction)) {
            	return Commander::Order::ORD_UNKNOWN;
			}
            // push feedback
            return Commander::Order::ORD_SUCCESS;
        }
        break;

        //  PLAY  _________________________________________________
        case Commander::Order::ORD_PLAY_CARD:
        {
        
            if(order.data[0] < 0 || order.data[0] >= players[turn].hand.size()) {                
            	return Commander::Order::ORD_INVARGS;
			}
            // Check for out-of-bounds arguments for unit deployment
            std::optional<Tile_ref> deployment_site; // Get tile reference for unit deployment
            if(players[turn].hand[order.data[0]].get().type == CTYPE_UNIT)
            {
                if(order.data[1] < 0 || order.data[2] < 0) { 
                    return Commander::Order::ORD_INVARGS;
                }
                if(order.data[1] >= grid.size() || order.data[2] >= grid[0].size()) {
                    return Commander::Order::ORD_INVARGS;
				}
                deployment_site = std::ref(grid[order.data[1]][order.data[2]]);
            }
            // Ensure player has sufficient money
            if(players[turn].hand[order.data[0]].get().cost > players[turn].funds) {
                return Commander::Order::ORD_NOFUNDS;
			}
            // Finally, perform the action and check for proper execution.
            if(!play_card(turn, order.data[0], deployment_site)) {
                return Commander::Order::ORD_NOTARGET;
			}
            return Commander::Order::ORD_SUCCESS; // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        }
        break;

        //  INVALID TYPE ____________________________________________
        default:
            return Commander::Order::ORD_INVTYPE;
        break;
    }
}

void Game_master::fire_trigger(Trigger& trigger, std::vector<int> args = {}) 
{
    for (std::pair<int, Reaction> bound : trigger) {
        bound.second(args);
    }
}

const Commander::Game_state Game_master::get_game_state(int player_id)
{
    Commander::Game_state status;
    status.turn = turn;
    status.turn_absolute = turn_absolute;
        
    status.active_cards = std::vector<Commander::Card_info>(active_cards.size());
    for(int i = 0; i < active_cards.size(); i++) {
        status.active_cards[i] = active_cards[i].get().get_info();
    }

    status.hands = std::vector<std::vector<Commander::Card_info>>(players.size());
    status.hands[player_id].resize(players[player_id].hand.size());
    for(int i = 0; i < players[player_id].hand.size(); i++) {
        status.hands[player_id][i] = players[player_id].hand[i].get().get_info();
    }

    status.players = std::vector<Commander::Player_info>(players.size());
    for(int i = 0; i < players.size(); i++) {
        status.players[i] = players[i].get_info();
    }

    return status;
}

Commander::Game_params Game_master::get_static_game_info()
{
    std::unordered_map<unsigned int, Commander::Card_info> manifest;
    std::pair<int, int> grid_size = {grid.size(), grid[0].size()};

    for (Deck& deck : decks) {
        for (Card& card : deck.all) {
            manifest.insert({card.entity_id.get_id(), card.get_info()});
        }
    }
    for (Card& card : tokens) {
        manifest.insert({card.entity_id.get_id(), card.get_info()});
    }

    return {manifest, grid_size};
}

bool Game_master::check_dominance(int playerId)
{
    int unit_count_difference = 0;
    for(Card_ref cref : active_cards) // Find active cards within capture zone
        if(cref.get().type == CTYPE_UNIT && grid[cref.get().x][cref.get().y].type == Tile::OBJECTIVE)
        {
            if(cref.get().controller_id == turn)
                unit_count_difference++;
            else
                unit_count_difference--;
        }
    return unit_count_difference > 0;
}
bool Game_master::deploy_card(Card& card, int player, std::optional<Tile_ref> target)
{
    if(card.type == CTYPE_UNIT && !target.has_value()) throw std::runtime_error("No deployment site provided for unit");
    if(card.status == Card::CSTATUS_BATTLEFIELD) std::clog << "WARNING: Deploying card marked as \"IN PLAY\"";

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
    
    card.controller_id = player;

    if(card.type != CTYPE_TACTIC)
    {   // Update status
        card.status = Card::CSTATUS_BATTLEFIELD;
        active_cards.push_back(std::ref(card));
        card.controller_id = player;
    }

    fire_trigger(card.enters_play, {});

    if(card.type == CTYPE_TACTIC) {
        resolve_destruction(card);
    }
    return true;
}

bool Game_master::resolve_movement(Card& card, int direction)
{   
    // Check if direction is valid
    if(direction < 0 || direction > 3) throw std::invalid_argument("invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        &grid[card.x][card.y].card->get() != &card) throw std::runtime_error("Trying to move an off-grid card");

    auto options = get_4neighbors(grid[card.x][card.y]);

    // TODO add mutual exchange movement option (stack movement operations up until a card is moved on an empty space)

    // Check if tile is occupied
    if(options[direction]->get().card.has_value())
        return false;

    fire_trigger(card.before_move, {direction});

    // Broadcast event before coordinates have changed 
    Commander::Event move_event = {Commander::Event::EV_CARD_MOVED, {card.x, card.y, options[direction]->get().x, options[direction]->get().y}};
    broadcast_event(move_event);

    // Exchange positions
    grid[card.x][card.y].card.reset();
    options[direction]->get().card = std::ref(card);
    // Update coords
    card.x = options[direction]->get().x;
    card.y = options[direction]->get().y;

    card.can_move = false;

    fire_trigger(card.after_move, {direction});

    return true;
}

void Game_master::resolve_destruction(Card& card)
{   
    if(card.type != CTYPE_TACTIC)
    {
        if(card.x >= 0 && card.y >= 0)
            grid[card.x][card.y].card.reset();

        std::function<bool(Card_ref&, Card&)> compare_origins = \
                    [](Card_ref& cr, Card& c){ return &cr.get() == &c; };
            
        if(!pop_element(active_cards, card, compare_origins))
            std::clog << "WARNING: killing card that was not in activeCards" << std::endl;

        if(card.status != Card::CSTATUS_BATTLEFIELD)
            std::clog << "WARNING: killing card that was not \"in play\"" << std::endl;
    }

    fire_trigger(card.leaves_play);

    // Send to discard
    card.status = Card::CSTATUS_GRAVEYARD;
    decks[card.owner_id].graveyard.push_back(std::ref(card));

    // Restore default power and linked triggers
    card.reset();
    card.controller_id = -1;

    if (card.type != CTYPE_TACTIC) {
        // Broadcast event if the card is not a tactic (in that case, deployment event is enough)
        Commander::Event destruction_event = {Commander::Event::EV_CARD_DESTROYED, {(int)card.entity_id.get_id()}};
        broadcast_event(destruction_event);
    }
}

bool Game_master::resolve_attack(Card& card, int direction)
{
    // Check if card's position on grid is OK
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card->get().entity_id != card.entity_id) throw std::runtime_error("Attacking with card with bad position");

    std::vector<std::optional<Tile_ref>> options = get_8neighbors(grid[card.x][card.y]);
    // Check if there is a target in this direction.
    if(!options[direction].has_value() || !(options[direction]->get().card.has_value()))
        return false;

    int combat_result = resolve_combat(card, options[direction]->get().card.value());

    // Broadcast event
    Commander::Event attack_event = {Commander::Event::EV_CARD_ATTACKS, {card.x, card.y, options[direction]->get().x, options[direction]->get().y}};
    broadcast_event(attack_event);
    // Process triggers

    return true;
}

int Game_master::resolve_combat(Card& attacker, Card& defender)
{
    fire_trigger(attacker.before_attack, {(int)defender.entity_id.get_id()});
    fire_trigger(defender.before_attacked, {(int)attacker.entity_id.get_id()});

    int advantage_difference = attacker.advantage - defender.advantage;
    // set advantage bonus if defender is overwhelmed
    if(defender.is_overwhelmed) { advantage_difference++; }
    // tile type advantage
    if(defender.x >= 0 && defender.y >= 0)
    {
        if(grid[defender.x][defender.y].type = Tile::TERR_ADV)    { advantage_difference--; }
        if(grid[defender.x][defender.y].type = Tile::TERR_DISADV) { advantage_difference++; }
    }
    if(attacker.x >= 0 && attacker.y >= 0)
    {
        if(grid[attacker.x][attacker.y].type = Tile::TERR_ADV)    { advantage_difference--; }
        if(grid[attacker.x][attacker.y].type = Tile::TERR_DISADV) { advantage_difference++; }
    }

    // apply statuses
    attacker.can_attack = false;
    attacker.can_move   = false;
    defender.is_overwhelmed = true;
    
    // advantage resolution
    if(advantage_difference > 0)
    {
        bool defender_destroyed = inflict_damage(defender, advantage_difference);
        if (defender_destroyed) {
            fire_trigger(attacker.after_attack,   {(int)defender.entity_id.get_id(), Game_master::COMBAT_WIN});
            fire_trigger(defender.after_attacked, {(int)attacker.entity_id.get_id(), Game_master::COMBAT_LOSE});
            return Game_master::COMBAT_WIN;
        }
    }
    else if(advantage_difference < 0)
    {
        bool attacker_destroyed = inflict_damage(attacker, -advantage_difference);
        if (attacker_destroyed) {
            fire_trigger(attacker.after_attack,   {(int)defender.entity_id.get_id(), Game_master::COMBAT_LOSE});
            fire_trigger(defender.after_attacked, {(int)attacker.entity_id.get_id(), Game_master::COMBAT_WIN});
            return Game_master::COMBAT_LOSE;
        }
    }
    int attacker_value = attacker.value;
    int defender_value = defender.value;

    bool defender_destroyed = inflict_damage(defender, attacker_value);
    bool attacker_destroyed = inflict_damage(attacker, defender_value);

    int combat_outcome;

    if (defender_destroyed && !attacker_destroyed) {
        combat_outcome = Game_master::COMBAT_WIN;
    }
    else if (attacker_destroyed && !defender_destroyed) {
        combat_outcome = Game_master::COMBAT_LOSE;
    }
    else {
        combat_outcome = Game_master::COMBAT_TIE;
    }
    // set status effects

    fire_trigger(attacker.after_attack,   {(int)defender.entity_id.get_id(), combat_outcome});
    fire_trigger(defender.after_attacked, {(int)attacker.entity_id.get_id(), -combat_outcome});

    return combat_outcome;
}

std::vector<std::optional<Game_master::Tile_ref>> Game_master::get_4neighbors(const Tile& tile)    
{
    std::vector<std::optional<Tile_ref>> res(4);
    // Check boundary conditions and add all possible options to res.
    if(tile.x > 0)                  { res[Tile::UP]    = std::ref(grid[tile.x-1][tile.y]); }
    if(tile.y < grid[0].size() - 1) { res[Tile::RIGHT] = std::ref(grid[tile.x][tile.y+1]); }
    if(tile.x < grid.size() - 1)    { res[Tile::DOWN]  = std::ref(grid[tile.x+1][tile.y]); }
    if(tile.y > 0)                  { res[Tile::LEFT]  = std::ref(grid[tile.x][tile.y-1]); }

    return res;
}

std::vector<std::optional<Game_master::Tile_ref>> Game_master::get_8neighbors(const Tile& tile)
{
    std::vector<std::optional<Tile_ref>> res(8);
    // Check boundary conditions and add all possible options to res.
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

bool Game_master::inflict_damage(Game_master::Card& card, int amount)
{
    if (card.type == CTYPE_TACTIC) {
        throw std::runtime_error("inflict_damage(): cannot inflict damage to tactic");
    }

    card.value -= amount;
    if (card.value <= 0) {
        resolve_destruction(card);
        return true;
    }
    return false;
}

bool Game_master::resolve_draw(int player_id)
{
    Player& player = players[player_id];

    if(player.deck.get().library.size() == 0) {
        // Library is empty, refill it by shuffling graveyard
        if(player.deck.get().graveyard.size() == 0) {
            // Graveyard is empty (everything is in hand or on battlefield?) -- draw is not resolved
            return false;
        }
        else {
            player.deck.get().refresh();

            Commander::Event refresh_event = {Commander::Event::EV_DECK_REFRESH, {player_id}};
            broadcast_event(refresh_event);
        }
    }
    if(player.deck.get().library.back().get().status != Card::CSTATUS_LIBRARY) {
       // std::clog << "Warning: drawing card that has not been marked as \"In deck\"" << std::endl;
    }

    player.hand.push_back(player.deck.get().library.back());
    player.deck.get().library.back().get().status = Card::CSTATUS_HAND;
    player.deck.get().library.pop_back();
    
    // Process triggers
    fire_trigger(player.draws);
    // Broadcast event
    Commander::Event draw_event = {Commander::Event::EV_PLAYER_DRAWS, {player_id}};
    broadcast_event(draw_event);

    return true;
}

bool Game_master::resolve_draw_multi(int player_id, int n_cards)
{
    for (int i = 0; i < n_cards; i++) {
        if (!resolve_draw(player_id)) {
            return false;
        }
    }
    return true;
}

bool Game_master::discard(int player_id, int hand_index)
{
    if(player_id < 0 || player_id >= players.size()) throw std::out_of_range("playerId out of range");
    if(hand_index < 0 || hand_index >= players[player_id].hand.size()) throw std::out_of_range("handIndex out of range");

    if(players[player_id].hand[hand_index].get().status != Card::CSTATUS_HAND)
        std::clog << "Warning: discarding card that has not been marked as \"In hand\"" << std::endl;

    int discarded_entity_id = (int)players[player_id].hand[hand_index].get().entity_id.get_id();

    decks[player_id].graveyard.push_back(players[player_id].hand[hand_index]);
    players[player_id].hand[hand_index].get().status = Card::CSTATUS_GRAVEYARD;
    pop_index(players[player_id].hand, hand_index);

    // Broadcast event
    Commander::Event discard_event = {Commander::Event::EV_PLAYER_DISCARDS, {player_id, discarded_entity_id}};
    broadcast_event(discard_event);

    return true;
}

bool Game_master::play_card(int player_id, int hand_index, std::optional<Tile_ref> target) // Returns false if there are insufficient funds.
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
            std::runtime_error("No deployment site provided for a unit card");

        // Check surroundings or deployment zones
        if(target->get().type != player_id) //If deploying outside of Base
        {
            // Check if there is at least one friendly unit on adjacent tiles
            bool nearFriendly = false;
            for(std::optional<Tile_ref> tptr : get_4neighbors(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && \
                   tptr->get().card->get().controller_id == player_id)
                    nearFriendly = true;

            if(!nearFriendly) {
                return false;
            }
            // Check if there are no enemies on surrounding tiles
            for(std::optional<Tile_ref> tptr : get_8neighbors(target.value()))
                if(tptr.has_value() && \
                   tptr->get().card.has_value() && 
                   tptr->get().card->get().controller_id != player_id) {
                        return false;
                   }
        }
    }

    if(played.status != Card::CSTATUS_HAND)
        std::clog << "Warning: trying to play card that has not been marked as \"In hand\"" << std::endl;

    bool res = deploy_card(played, player_id, target);

    if (res) 
    {
        players[player_id].funds -= players[player_id].hand[hand_index].get().cost;
        pop_index(players[player_id].hand, hand_index);

        fire_trigger(players[player_id].deploys);

        int target_x = -1, target_y = -1;
        if (target.has_value()) {
            target_x = target->get().x;
            target_y = target->get().y;
        }

        Commander::Event deploy_event = {Commander::Event::EV_PLAYER_DEPLOYS, {player_id, (int)played.entity_id.get_id(), target_x, target_y}};
        broadcast_event(deploy_event);
    }
    return res;
}

