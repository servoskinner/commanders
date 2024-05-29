#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>

#define GRID_WIDTH 		8
#define GRID_HEIGHT 	6

#define BASIC_INCOME 			1
#define BONUS_IF_ZERO_INCOME    1
#define STARTING_HAND_SIZE 		4
#define POINTS_REQ_FOR_VICTORY 	10

class Game_master; // Basically, a singular game session.
class Player;      // A player in context of game. Does not generate any input.
class Deck;        // A card pool that can be used by players.
class Card;        // A card in any state, with gameplay parameters.
class Tile;        // An area that cards can be placed onto.

class Roster;

// Info tags to conceal gameplay elements.
struct Card_info;
struct Player_info;

// Pointer type re-definition for clarity
typedef std::reference_wrapper<Game_master> master_ref;
typedef std::reference_wrapper<Player> player_ref;
typedef std::reference_wrapper<Deck> deck_ref;
typedef std::reference_wrapper<Card> card_ref;
typedef std::reference_wrapper<Tile> tile_ref;

#include "player_controller.hpp"

//Card abilities
template <int quantity>
void ability_draw(Game_master &gm, Card &activator);
template <int quantity>
void ability_gain_credits(Game_master &gm, Card &activator);

class Game_master
{
public: // _____________________________________________________________________________
    Game_master(const std::vector<pctrl_ref> &controllers, const std::vector<deck_ref> &ndecks);

    bool game_loop(); // Process player inputs and update status for everyone.
    void end_turn();  // Pass the turn to next player and process the necessary triggers.

    int getTurn() { return turn; }
    int get_absolute_turn() { return turnAbsolute; }

    int get_grid_width() { return grid[0].size(); }
    int get_grid_height() { return grid.size(); }

    enum order_type 
    {
        NOTHING, 
        PASS, 
        SURRENDER, 
        PLAY, 
        MOVE, 
        ATTACK, 
        CHOICE
    };
    enum event_types
    {
        ORDER_CONFIRM,
        ORDER_INVALID,
        BOARD_UPDATE,
        HAPPENING,
        TURN_PASS,
        GAME_ENDED,
    };
    enum invalid_event_codes
    {
        ORD_UNKNOWN,
        ORD_INVTYPE,
        ORD_INVARGS,
        ORD_NOSELECT,
        ORD_NOTARGET,
        ORD_PERMISSION,
        ORD_NOFUNDS,
        ORD_EXHAUSTED,
    };
    enum update_event_codes
    {
        DEPLOYED,
        MOVED,
        ATTACKED,
        DESTROYED,
        PLAYER_DRAWS,
        PLAYER_GAINS_FUNDS,
        PLAYER_DECK_REFRESH,
        PLAYER_DECK_NOREFRESH,
        PLAYER_DISCARDS
    };
    enum nearbyTiles
    {
        UP,
        RIGHT,
        DOWN,
        LEFT,
        UPRIGHT,
        DOWNRIGHT,
        DOWNLEFT,
        UPLEFT
    };

protected: // _____________________________________________________________________________
    Roster &roster;

    std::vector<Player> players;
    std::vector<deck_ref> decks;
    std::vector<pctrl_ref> player_controllers; // entities that provide player inputs.
                                              // E.G. User, AIs, network-connected players
    // Playing field
    std::vector<std::vector<Tile>> grid; // The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::vector<card_ref> active_cards;   // Cards that are currently on the playing field.

    bool resolve_action(const std::vector<int> &action);
    inline const void broadcast_event(const std::vector<int> &event);
    inline const void send_event(const std::vector<int> &event, int player_id);

    void update_status(int playerId);
    bool check_dominance(int playerId);

    int turn;
    int turnAbsolute;

    bool game_is_on;

    // Cards
    bool deploy_card(Card &card, std::optional<tile_ref> targer);         // Place a card in play.
    bool resolve_movement(Card &card, const int &direction);   // Move a card in specified direction.
    void resolve_destruction(Card &card);                      // Remove a card from play and discard it.
    bool resolve_attack(Card &card, const int &direction); // Resolve an attack from one tile to another.

    int resolve_combat(Card &attacker, Card &defender); // Resolve combat between two units.
    enum combatOutcome
    {
        DEFENDER_DEAD,
        BOTH_DEAD,
        ATTACKER_DEAD
    };
    // Tiles
    std::vector<std::optional<tile_ref>> get_4neighbors(const Tile &tile);
    std::vector<std::optional<tile_ref>> get_8neighbors(const Tile &tile);
    // Players
    bool resolve_draw(int playerId);                             // Returns whether deck is empty or not
    bool discard(int playerId, int handIndex);                // Returns whether discarding this card was possible
    bool play_card(int playerId, int handIndex, std::optional<tile_ref> target); // Perform rule checks and deploy card

public: // Card abitities
    template <int quantity>
    friend void ability_draw(Game_master &gm, Card &activator);
    template <int quantity>
    friend void ability_gain_credits(Game_master &gm, Card &activator);
};

class Player
{
public: // _____________________________________________________________________________
    Player(int playerid);

    const int id; // Unique player ID and turn no. on which it issues orders.
    std::vector<card_ref> hand;

    int points; //"Dominance points" scored by the player. 10 are required to win the game;
    int funds;  // Funds used to play cards and fire abilities.

    Player_info getInfo(Deck &deck);

    // Event triggers
    //  std::vector<std::function<void(Game_master&)>> trig_turn_start;
    //  std::vector<std::function<void(Game_master&)>> trig_turn_end;
    //  std::vector<std::function<void(Game_master&, Card&)>> trig_draw;  // Card& drawn
    //  std::vector<std::function<void(Game_master&, Card&)>> trig_play; // Card& played
    //  std::vector<std::function<void(Game_master&, Card&)>> trig_order_given; //Card& target, int order
    //...
};

struct Player_info
{
    // std::string name;
    int id;
    int points;
    int funds;

    int deck_size;
    int discard_size;
    int hand_size;
};

class Deck
{
public: // _____________________________________________________________________________
    Deck(const std::vector<Card> &cards);
    Deck(const Deck &original);
    Deck &operator=(const Deck &original);

    std::vector<Card> all;    // All cards associated with this deck. Original cards are stored here.
    std::vector<card_ref> discard; // Cards that have been removed after being put into play.
    std::vector<card_ref> library; // Cards that can be drawn.

    void shuffle();
    void refresh();
};

class Tile
{
public: // _____________________________________________________________________________
    Tile(int nx = -1, int ny = -1, int ntype = NORMAL) : x(nx), y(ny), type(ntype), card() {}

    int x, y;
    int type;

    enum tile_types
    {
        CAPTUREZONE = -2,
        NORMAL = -1
    }; // Non-negatives refer to deploy zones of players with same IDs

    std::optional<card_ref> card;
};

class Card
{
public: // _____________________________________________________________________________
    Card(int gid = -1, int mid = -1);
    Card& operator=(const Card& other);

    // Identification
    int owner_id;
    int global_id;
    int match_id;

    enum cardType
    {
        UNIT,
        CONTRACT,
        TACTIC
    };
    int type;
    enum cardStatus
    {
        UNDEFINED = -1,
        DECK = 0,
        HAND = 1,
        IN_PLAY = 2,
        DISCARD = 3
    };
    int status;

    Card_info get_info();
    // UI parameters
    std::string name;
    std::string text;
    //...
    // Gameplay
    int x, y;

    int cost;
    int value;
    int advantage;
    // Status effects
    bool can_attack;
    bool can_move;
    bool is_overwhelmed;

    // Event triggers
    std::vector<std::function<void(Game_master &, Card &)>> trig_ability;
    std::vector<std::function<void(Game_master &, Card &)>> trig_played;
    std::vector<std::function<void(Game_master &, Card &)>> trig_destroyed;

    // std::vector<std::function<void(Game_master&, Card&)>> trig_turn_start;
    // std::vector<std::function<void(Game_master&, Card&)>> trig_turn_end;

    // std::vector<std::function<void(Tile&)>> trig_moved; // Tile& destination
    // std::vector<std::function<void(Tile&, int)>> trig_attacks; // Tile& target, int damageDone
    // std::vector<std::function<void(Game_master&, Card&, Card&)>> trig_destroyed_other; // Card& target
    // std::vector<std::function<void(Card&, int)>> trig_attacked; // Card& damaged_by, int damage_sustained
    //...
};

struct Card_info // Out of game context info tag 
{
    int global_id;
    int match_id;
    int owner_id;
    int x, y;

    bool can_attack;
    bool can_move;
    bool is_overwhelmed;

    // the following are irrelevant and must be removed:
    std::string name;
    std::string text;

    int value;
    int cost;
    int advantage;
    int type;
};
