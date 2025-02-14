#include "Game_master.hpp"

#include <stdexcept>
#include <iostream>
#include <optional>
#include <memory>

Game_master::Game_master(const std::vector<std::vector<unsigned int>>& deck_images,
                         Gamemode gamemode,
                         int nplayers,
                         const std::vector<std::vector<int>>& terrain)
{
    #ifdef LOGGER_ON
        Logger::get().write("Initializing game...");
    #endif
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
        n_terrain = {GRID_WIDTH, std::vector<int>(GRID_HEIGHT, NORMAL)};
        for(int y = 0; y < GRID_HEIGHT; y++) 
        {
        // Deploy zones
        n_terrain[0][y] = 0;
        n_terrain[GRID_WIDTH-1][y] = 1;
        // Capture zone
        n_terrain[(GRID_WIDTH-1)/2][y] = OBJECTIVE;
        n_terrain[(GRID_WIDTH)/2][y] = OBJECTIVE;
        }
    }
    else
    {
        n_terrain = terrain;
    }
    // Set grid size
    grid_size = {n_terrain[0].size(), n_terrain.size()};

    #ifdef LOGGER_ON
        Logger::get().write("Setting up grid, height " + std::to_string(grid_size.y) + ", width " + std::to_string(grid_size.x));
    #endif
    // Build tiles
    grid = {grid_size.x, std::vector<Tile>(grid_size.y)};

    for(int x = 0; x < grid_size.x; x++) {
        for(int y = 0; y < grid_size.y; y++) {
            grid[x][y] = Tile({x, y}, n_terrain[x][y]);
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
            #ifdef LOGGER_ON
                Logger::get().write("Setting up player " + std::to_string(playerid));
            #endif
            decks.emplace_back(*this, deck_images[playerid], playerid);
            players.emplace_back(playerid, decks.back());
        }
    }
    else {
        #ifdef LOGGER_ON
            Logger::get().write("Setting up skirmish");
        #endif
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
    #ifdef LOGGER_ON
        Logger::get().write("Starting init draw");
    #endif
    // Deal cards TODO add re-draw
    for(int id = 0; id < players.size(); id++) {
        // Deal starting hand
        for(int card = 0; card < STARTING_HAND_SIZE; card++) {
            resolve_draw(id);
        }
    }
    game_is_ongoing = true;
    end_turn(); // end -1st turn
}

std::optional<Game_master::Card_ref> Game_master::find_card(unsigned int entity_id)
{
    std::optional<Card_ref> card = {};
    // Search in decks
    for (Deck& deck : decks)
    {
        auto iter = std::find_if(deck.all.begin(), deck.all.end(), [entity_id](Card& card){ return card.entity_id == entity_id;} );
        if (iter != deck.all.end()) {
            card.emplace(*iter);
            break;
        }
    }
    // Try to find in temporary cards
    if (!card.has_value()) {
        auto iter = std::find_if(tokens.begin(), tokens.end(), [entity_id](Card& card){ return card.entity_id == entity_id;} );
        if (iter != tokens.end()) {
            Card& ref = *iter;
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
    #ifdef LOGGER_ON
        Logger::get().write("Ending turn " + std::to_string(turn) + "(absolute " + std::to_string(turn_absolute) + ")");
    #endif
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
        #ifdef LOGGER_ON
        Logger::get().write("Dominance condition met!");
        #endif
        players[turn].points++;
        // Check if player wins on this turn
        if(players[turn].points >= POINTS_REQ_FOR_VICTORY)
        {
            game_is_ongoing = false;
            // Notify everyone that game ended
            Commander::Event event;
            event.type = Commander::Event::EV_GAME_WON_BY;
            event.data.push_back(turn);
            broadcast_event(event);
        }
    }

    #ifdef LOGGER_ON
        Logger::get().write("Rewinding card abilities");
    #endif
    for(Card_ref cref : active_cards)
    {
        if(cref.get().type == CTYPE_UNIT && cref.get().controller_id == turn) // Un-overwhelm units and refresh move and attack
        {
            cref.get().can_move       = true;
            cref.get().can_attack     = true;
            cref.get().is_overwhelmed = false;
            #ifdef LOGGER_ON
            Logger::get().write("Card " + ref_to_string(&cref.get()) + " refreshed");
            Logger::get().write("(EID: " + std::to_string(cref.get().entity_id) + ", CID: " + std::to_string(cref.get().card_id) + ")");
            #endif
        }
        if(cref.get().type == CTYPE_CONTRACT && cref.get().controller_id == turn) // Gain profit for each contract and decrement their duration
        {
            #ifdef LOGGER_ON
            Logger::get().write("Contract " + ref_to_string(&cref.get()) + " tapped");
            Logger::get().write("(EID: " + std::to_string(cref.get().entity_id) + ", CID: " + std::to_string(cref.get().card_id) + ")");
            #endif

            inflict_damage(cref.get(), 1);
            players[turn].funds++;
        }
    }
};

Commander::Order_result Game_master::exec_order(int player_id, const Commander::Order& order)
{
    #ifdef LOGGER_ON
        Logger::get().write("\nExecuting ord type " + std::to_string(order.type) + " from player " + std::to_string(player_id));
    #endif

    switch(order.type)
    {
        //  EMPTY _________________________________________________
        case Commander::ORD_DO_NOTHING:
        return Commander::ORD_SUCCESS;
        //  PASS  _________________________________________________
        case Commander::ORD_PASS:
        { // ends turn: self-explanatory
        #ifdef LOGGER_ON
        Logger::get().write("Success!");
        #endif
        end_turn(); // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        return Commander::ORD_SUCCESS;
        }
        break;

        //  MOVE  _________________________________________________
        case Commander::ORD_MOVE:
        {   
            // Check if all args are present
            if (order.data.size() < 4) {
                return Commander::ORD_INV_ARGS;
            }
            // Extract args
            Vector2i from_pos = {order.data[0], order.data[1]};
            Vector2i to_pos = {order.data[2], order.data[3]};
            
            // Check bounds (necessary only for first pos)
            if(!(from_pos >= Vector2i(0, 0) && from_pos < grid_size)) {
                return Commander::ORD_INV_ARGS;
			}
            Tile_ref from_tile = grid[from_pos.x][from_pos.y];
            // check that a unit was selected
            if(!from_tile.get().occupier.has_value()) { 
                return Commander::ORD_NO_ACTOR; 	
            }
            Card_ref moving_card = from_tile.get().occupier.value();

            // check ownership
            if (moving_card.get().owner_id != player_id) {
                return Commander::ORD_NO_PERMISSION;
            }
            // check ability cooldown
            if (!moving_card.get().can_move) {
                return Commander::ORD_EXHAUSTED;
            }
            
            // determine direction and check if it is invalid
            Direction neighbor = delta_to_neighbor(to_pos - from_pos);

            // Refuse attempts to move diagonally.
            if (!is_4neighbor(neighbor)) {
                return Commander::ORD_OUT_OF_RANGE;
            }
            std::optional<Tile_ref> to_tile = get_4neighbors(from_tile)[neighbor];

            // check that destination tile exists
            if (!to_tile.has_value()) {
                return Commander::ORD_INV_ARGS;
            }

            // check that target tile is vacant
            if(to_tile->get().occupier.has_value()) { 
                return Commander::ORD_NO_TARGET; 	
            }

            // Execute order
            if(!resolve_movement(moving_card, neighbor)) {
                return Commander::ORD_UNKNOWN;
            }
            // push feedback
            // ...
            #ifdef LOGGER_ON
            Logger::get().write("Success!");
            #endif
            return Commander::ORD_SUCCESS;
        }
        break;
        
        //  ATTACK  _________________________________________________
        case Commander::ORD_ATTACK:
        {
            // Check no. of arguments
            if(order.data.size() < 4) {
                return Commander::ORD_INV_ARGS;
            }

            Vector2i attacker_pos = {order.data[0], order.data[1]};
            Vector2i defender_pos = {order.data[2], order.data[3]};
           
            // check for out-of-bounds arguments
            if(!(attacker_pos >= Vector2i(0, 0) && attacker_pos < grid_size)) {
                return Commander::ORD_INV_ARGS;
			}

            Tile_ref attacker_tile = grid[attacker_pos.x][attacker_pos.y];
            // check if attacker exists
            if (!attacker_tile.get().occupier.has_value()) {
                return Commander::ORD_NO_ACTOR;
            }

            Card_ref attacking_card = attacker_tile.get().occupier.value();
            // check ownership
            if (attacking_card.get().owner_id != player_id) {
                return Commander::ORD_NO_PERMISSION;
            }

            // check ability cooldown
            if (!attacking_card.get().can_attack) {
                return Commander::ORD_EXHAUSTED;
            }

            // determine direction and check if it is invalid
            Direction neighbor = delta_to_neighbor(defender_pos - attacker_pos);
            if (neighbor == NOT_A_NEIGHBOR) {
                return Commander::ORD_OUT_OF_RANGE;
            }

            std::optional<Tile_ref> defender_tile = get_8neighbors(attacker_tile)[neighbor];
            // check that defender tile is OK
            if(!defender_tile.has_value()) { 
                return Commander::ORD_INV_ARGS; 	
            }

            // check that target is present
            if(!defender_tile->get().occupier.has_value()) { 
                return Commander::ORD_NO_TARGET; 	
            }

            // check friendly fire
            if(defender_tile->get().occupier->get().owner_id == attacking_card.get().owner_id) {
                return Commander::ORD_FRIENDLY_FIRE;
            }

            // process order
            if(!resolve_attack(attacking_card, neighbor)) {
            	return Commander::ORD_UNKNOWN;
			}

            // push feedback
            // ...
            #ifdef LOGGER_ON
            Logger::get().write("Success!");
            #endif
            return Commander::ORD_SUCCESS;
        }
        break;

        //  PLAY  _________________________________________________
        case Commander::ORD_PLAY_CARD:
        {
            // check no. of args
            if (order.data.size() < 1) {
                return Commander::ORD_INV_ARGS;
            }

            int hand_index = order.data[0];
            std::optional<Tile_ref> target_tile;

            if(order.data[0] < 0 || order.data[0] >= players[turn].hand.size()) {                
            	return Commander::ORD_INV_ARGS;
			}
            // Check for out-of-bounds arguments for unit deployment
            std::optional<Tile_ref> deployment_site; // Get tile reference for unit deployment
            if(players[turn].hand[order.data[0]].get().type == CTYPE_UNIT)
            {
                if(order.data[1] < 0 || order.data[2] < 0) { 
                    return Commander::ORD_INV_ARGS;
                }
                if(order.data[1] >= grid.size() || order.data[2] >= grid[0].size()) {
                    return Commander::ORD_INV_ARGS;
				}
                deployment_site = std::ref(grid[order.data[1]][order.data[2]]);
            }
            // Ensure player has sufficient money
            if(players[turn].hand[order.data[0]].get().cost > players[turn].funds) {
                return Commander::ORD_LOW_FUNDS;
			}
            // Finally, perform the action and check for proper execution.
            if(!play_card(turn, order.data[0], deployment_site)) {
                return Commander::ORD_NO_TARGET;
			}
            #ifdef LOGGER_ON
            Logger::get().write("Success!");
            #endif
            return Commander::ORD_SUCCESS; // Mind that even if player ID is changed BEFORE returning, the error handler is not triggered.
        }
        break;

        //  INVALID TYPE ____________________________________________
        default:
            return Commander::ORD_UNKNOWN_TYPE;
        break;
    }
}

void Game_master::fire_trigger(Cause& trigger, std::vector<int> args = {}) 
{
    for (std::pair<int, Effect> bound : trigger) {
        bound.second(args);
        #ifdef LOGGER_ON
        Logger::get().write("Firing trigger " + ref_to_string(&trigger) + ": binding " + std::to_string(bound.first));
        #endif
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
    Vector2i grid_size = get_grid_size();

    for (Deck& deck : decks) {
        for (Card& card : deck.all) {
            manifest.insert({card.entity_id, card.get_info()});
        }
    }
    for (Card& card : tokens) {
        manifest.insert({card.entity_id, card.get_info()});
    }

    return {manifest, grid_size};
}

bool Game_master::check_dominance(int playerId) const
{
    #ifdef LOGGER_ON
    Logger::get().write("Checking dominance for " + std::to_string(playerId));
    #endif
    int unit_count_difference = 0;
    for(Card_ref cref : active_cards) // Find active cards within capture zone
        if(cref.get().type == CTYPE_UNIT && grid[cref.get().pos.x][cref.get().pos.y].type == OBJECTIVE)
        {
            if(cref.get().controller_id == turn)
                unit_count_difference++;
            else
                unit_count_difference--;
        }
    return unit_count_difference > 0;
}
Commander::Order_result Game_master::deploy_card(Card& card, int player, std::optional<Tile_ref> target)
{
    #ifdef LOGGER_ON
    Logger::get().write("Player " + std::to_string(player) + " deploys card " + ref_to_string(&card));
    Logger::get().write("(EID: " + std::to_string(card.entity_id) + " CID: " + std::to_string(card.card_id));
    #endif

    if(card.type == CTYPE_UNIT && !target.has_value()) {
        return Commander::ORD_NO_TARGET;
    }

    if(card.status == Card::CSTATUS_BATTLEFIELD) {
        std::clog << "WARNING: Deploying card marked as \"IN PLAY\"";
    }

    if(card.type == CTYPE_UNIT)
    {   // Check if 
        if(target->get().occupier)
            return false; 
        
        target->get().occupier = std::ref(card);
        card.pos = target->get().pos;

        card.can_attack = true;
        card.can_move = false;
        card.is_overwhelmed = false;

        #ifdef LOGGER_ON
        Logger::get().write("Unit deployed");
        #endif
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
    return Commander::ORD_SUCCESS;
}

Commander::Order_result Game_master::resolve_movement(Card& card, Direction direction)
{   
    #ifdef LOGGER_ON
    Logger::get().write("Moving card " + ref_to_string(&card) + "to direction " + std::to_string(direction));
    Logger::get().write("(EID: " + std::to_string(card.entity_id) + " CID: " + std::to_string(card.card_id));
    #endif

    fire_trigger(card.before_move, {direction});

    // Compose event message before coordinates change
    Commander::Event move_event = {Commander::EV_CARD_MOVED, {card.pos.x, card.pos.y, direction}};

    // Exchange positions
    grid[card.pos.x][card.pos.y].occupier.reset();
    options[direction]->get().occupier = std::ref(card);
    // Update coords
    card.pos = options[direction]->get().pos;
    card.can_move = false;

    fire_trigger(card.after_move, {direction});
    broadcast_event(move_event);

    return Commander::ORD_SUCCESS;
}

void Game_master::resolve_destruction(Card& card)
{   
    #ifdef LOGGER_ON
    Logger::get().write("Destroying card " + ref_to_string(&card));
    Logger::get().write("(EID: " + std::to_string(card.entity_id) + " CID: " + std::to_string(card.card_id));
    #endif

    if(card.type != CTYPE_TACTIC)
    {
        if(card.pos != Vector2i(-1, -1))
            grid[card.pos.x][card.pos.y].occupier.reset();

        std::function<bool(Card_ref&, Card&)> comparison = \
                    [](Card_ref& cr, Card& c){ return cr.get() == c; };
            
        if(!pop_element(active_cards, card, comparison))
            std::clog << "WARNING: killing card that was not in activeCards" << std::endl;

        if(card.status != Card::CSTATUS_BATTLEFIELD)
            std::clog << "WARNING: killing card that was not \"in play\"" << std::endl;
        #ifdef LOGGER_ON
        Logger::get().write("Removed card from field");
        #endif
    }

    fire_trigger(card.leaves_play);

    // Send to discard
    card.status = Card::CSTATUS_GRAVEYARD;
    decks[card.owner_id].junk.push_back(std::ref(card));

    // Restore default power and linked triggers
    card.reset();
    card.controller_id = -1;

    if (card.type != CTYPE_TACTIC) {
        // Broadcast event if the card is not a tactic (in that case, deployment event is enough)
        Commander::Event destruction_event = {Commander::Event::EV_CARD_DESTROYED, {(int)card.entity_id}};
        broadcast_event(destruction_event);
    }
}

Commander::Order_result Game_master::resolve_attack(Card& card, Direction direction)
{
    #ifdef LOGGER_ON
    Logger::get().write("Attacking with card " + ref_to_string(&card) + " in direction " + std::to_string(direction));
    Logger::get().write("(EID: " + std::to_string(card.entity_id) + " CID: " + std::to_string(card.card_id));
    #endif
    // Check if card's position on grid is OK
    if(direction < 0 || direction > 7) throw std::invalid_argument("Invalid direction");
    if(!grid[card.x][card.y].card.has_value() || \
        grid[card.x][card.y].card->get().entity_id != card.entity_id) throw std::runtime_error("Attacking with card with bad position");

    std::vector<std::optional<Tile_ref>> options = get_8neighbors(grid[card.x][card.y]);
    // Check if there is a target in this direction.
    if(!options[direction].has_value() || !(options[direction]->get().occupier.has_value()))
        return false;

    int combat_result = resolve_combat(card, options[direction]->get().occupier.value());

    // Broadcast event
    Commander::Event attack_event = {Commander::Event::EV_CARD_ATTACKS, {card.x, card.y, options[direction]->get().x, options[direction]->get().y}};
    broadcast_event(attack_event);
    // Process triggers

    return true;
}

int Game_master::resolve_combat(Card& attacker, Card& defender)
{
    #ifdef LOGGER_ON
    Logger::get().write("Resolving combat...");
    #endif
    fire_trigger(attacker.before_attack, {(int)defender.entity_id});
    fire_trigger(defender.before_attacked, {(int)attacker.entity_id});

    int advantage_difference = attacker.advantage - defender.advantage;
    // set advantage bonus if defender is overwhelmed
    if(defender.is_overwhelmed) { advantage_difference++; }
    // tile type advantage
    if(defender.x >= 0 && defender.y >= 0)
    {
        if(grid[defender.x][defender.y].type = TERR_ADV)    { advantage_difference--; }
        if(grid[defender.x][defender.y].type = TERR_DISADV) { advantage_difference++; }
    }
    if(attacker.x >= 0 && attacker.y >= 0)
    {
        if(grid[attacker.x][attacker.y].type = Tile::TERR_ADV)    { advantage_difference--; }
        if(grid[attacker.x][attacker.y].type = Tile::TERR_DISADV) { advantage_difference++; }
    }
    #ifdef LOGGER_ON
    Logger::get().write("Advantage diff " + std::to_string(advantage_difference));
    #endif

    // apply statuses
    attacker.can_attack = false;
    attacker.can_move   = false;
    defender.is_overwhelmed = true;
    
    // advantage resolution
    if(advantage_difference > 0)
    {
        bool defender_destroyed = inflict_damage(defender, advantage_difference);
        if (defender_destroyed) {
            fire_trigger(attacker.after_attack,   {(int)defender.entity_id, Game_master::ATTACKER_LIVES});
            fire_trigger(defender.after_attacked, {(int)attacker.entity_id, Game_master::DEFENDER_LIVES});
            return Game_master::ATTACKER_LIVES;
        }
    }
    else if(advantage_difference < 0)
    {
        bool attacker_destroyed = inflict_damage(attacker, -advantage_difference);
        if (attacker_destroyed) {
            fire_trigger(attacker.after_attack,   {(int)defender.entity_id, Game_master::DEFENDER_LIVES});
            fire_trigger(defender.after_attacked, {(int)attacker.entity_id, Game_master::ATTACKER_LIVES});
            return Game_master::DEFENDER_LIVES;
        }
    }
    int attacker_value = attacker.value;
    int defender_value = defender.value;

    bool defender_destroyed = inflict_damage(defender, attacker_value);
    bool attacker_destroyed = inflict_damage(attacker, defender_value);

    int combat_outcome;

    if (defender_destroyed && !attacker_destroyed) {
        combat_outcome = Game_master::ATTACKER_LIVES;
    }
    else if (attacker_destroyed && !defender_destroyed) {
        combat_outcome = Game_master::DEFENDER_LIVES;
    }
    else {
        combat_outcome = Game_master::BOTH_DIE;
    }
    // set status effects

    fire_trigger(attacker.after_attack,   {(int)defender.entity_id, combat_outcome});
    fire_trigger(defender.after_attacked, {(int)attacker.entity_id, -combat_outcome});

    return combat_outcome;
}

std::array<std::optional<Game_master::Tile_ref>, 4> Game_master::get_4neighbors(const Tile& tile)
{
    std::array<std::optional<Tile_ref>, 4> res;
    Vector2i pos = tile.pos;
    // Check boundary conditions and add all possible options to res.
    if(pos.x > 0)                  { res[TOP]    = std::ref(grid[pos.x-1][pos.y]); }
    if(pos.y < grid_size.y - 1)    { res[RIGHT]  = std::ref(grid[pos.x][pos.y+1]); }
    if(pos.x < grid_size.x - 1)    { res[BOTTOM] = std::ref(grid[pos.x+1][pos.y]); }
    if(pos.y > 0)                  { res[LEFT]   = std::ref(grid[pos.x][pos.y-1]); }

    return res;
}

std::array<std::optional<Game_master::Tile_ref>, 8> Game_master::get_8neighbors(const Tile& tile)
{
    std::array<std::optional<Tile_ref>, 8> res;
    Vector2i pos = tile.pos;
    // Check boundary conditions and add all possible options to res.
    if(pos.x > 0)
    {
        res[TOP] = std::ref(grid[pos.x-1][pos.y]);

        if(pos.y > 0)               { res[TOP_LEFT]    = std::ref(grid[pos.x-1][pos.y-1]); }
        if(pos.y < grid_size.y - 1) { res[TOP_RIGHT]   = std::ref(grid[pos.x-1][pos.y+1]); }
    }

    if(pos.x < grid_size.x - 1)
    {
        res[BOTTOM] = std::ref(grid[pos.x+1][pos.y]);

        if(pos.y > 0)               { res[BOTTOM_LEFT]  = std::ref(grid[pos.x+1][pos.y-1]); }
        if(pos.y < grid_size.y - 1) { res[BOTTOM_RIGHT] = std::ref(grid[pos.x+1][pos.y+1]); }
    }

    if(pos.y > 0)                   { res[LEFT]      = std::ref(grid[pos.x][pos.y-1]); }
    if(pos.y < grid_size.y - 1)     { res[RIGHT]     = std::ref(grid[pos.x][pos.y+1]); }

    return res;
}

Game_master::Direction Game_master::delta_to_neighbor(Vector2i delta) const {
    if (delta == Vector2i(0, 0) || !(delta >= Vector2i(-1, -1)) || !(delta <= Vector2i(1, 1)) ) {
        return NOT_A_NEIGHBOR;
    }
    const Direction neighbor_map[] = {TOP_LEFT, TOP, TOP_RIGHT, LEFT, NOT_A_NEIGHBOR, RIGHT, BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT};
    int index = (delta.x + 1) + (delta.y + 1) * 3;

    return neighbor_map[index];
}

bool Game_master::inflict_damage(Game_master::Card& card, int amount)
{
    #ifdef LOGGER_ON
    Logger::get().write(std::to_string(amount) + " damage inflicted to " + ref_to_string(&card));
    #endif
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
    #ifdef LOGGER_ON
    Logger::get().write(std::to_string(player_id) + " draws");
    #endif
    Player& player = players[player_id];

    if(player.deck.get().library.size() == 0) {
        // Library is empty, refill it by shuffling graveyard
        if(player.deck.get().junk.size() == 0) {
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

    int discarded_entity_id = (int)players[player_id].hand[hand_index].get().entity_id;

    decks[player_id].junk.push_back(players[player_id].hand[hand_index]);
    players[player_id].hand[hand_index].get().status = Card::CSTATUS_GRAVEYARD;
    pop_index(players[player_id].hand, hand_index);

    // Broadcast event
    Commander::Event discard_event = {Commander::Event::EV_PLAYER_DISCARDS, {player_id, discarded_entity_id}};
    broadcast_event(discard_event);

    return true;
}

bool Game_master::play_card(int player_id, int hand_index, std::optional<Tile_ref> target) // Returns false if there are insufficient funds.
{
    #ifdef LOGGER_ON
    Logger::get().write(std::to_string(player_id) + " plays " + std::to_string(hand_index) + "th card");
    #endif
    Card& played = players[player_id].hand[hand_index];
    // Check index correctness
    if(player_id < 0 || player_id >= players.size()) throw std::out_of_range("Player ID out of range");
    if(hand_index < 0 || hand_index >= players[player_id].hand.size()) throw std::out_of_range("Hand index out of range");

    // Check whether a player has enough funds to play the card (subtract them if success)
    if(players[player_id].funds < played.cost) {
        return false;
    }

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
                   tptr->get().occupier.has_value() && \
                   tptr->get().occupier->get().controller_id == player_id)
                    nearFriendly = true;

            if(!nearFriendly) {
                return false;
            }
            // Check if there are no enemies on surrounding tiles
            for(std::optional<Tile_ref> tptr : get_8neighbors(target.value()))
                if(tptr.has_value() && \
                   tptr->get().occupier.has_value() && 
                   tptr->get().occupier->get().controller_id != player_id) {
                        return false;
                   }
        }
    }

    if(played.status != Card::CSTATUS_HAND)
        std::clog << "Warning: trying to play card that has not been marked as \"In hand\"" << std::endl;

    bool res = deploy_card(played, player_id, target);

    if (res) 
    {
        #ifdef LOGGER_ON
        Logger::get().write("Success, removing card from hand...");
        #endif
        players[player_id].funds -= players[player_id].hand[hand_index].get().cost;
        pop_index(players[player_id].hand, hand_index);

        fire_trigger(players[player_id].deploys);

        Vector2i target_pos = {-1, -1};
        if (target.has_value()) {
            target_pos = target->get().pos;
        }

        Commander::Event deploy_event = {Commander::Event::EV_PLAYER_DEPLOYS, {player_id, (int)played.entity_id, target_pos.x, target_pos.y}};
        broadcast_event(deploy_event);
    }
    return res;
}

