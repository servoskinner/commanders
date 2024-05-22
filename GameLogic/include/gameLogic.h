#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>

#define GRID_WIDTH 		8
#define GRID_HEIGHT 	6

#define FUNDS_DECAY 			1
#define BASIC_INCOME 			2
#define STARTING_HAND_SIZE 		4
#define POINTS_REQ_FOR_VICTORY 	10

class Game_master; // Basically, a singular game session.
class Player;     // A player in context of game. Does not generate any input.
class Deck;       // A card pool that can be used by players.
class Card;       // A card in any state, with gameplay parameters.
class Tile;       // An area that cards can be placed onto.

class Roster;

// Info tags to conceal gameplay elements.
struct Card_info;
struct Player_info;

// Pointer type re-definition for clarity
typedef std::reference_wrapper<Game_master> master_ref;
typedef std::reference_wrapper<Player_controller> pctrl_ref;

typedef std::reference_wrapper<Player> player_ref;
typedef std::reference_wrapper<Deck> deck_ref;
typedef std::reference_wrapper<Card> card_ref;
typedef std::reference_wrapper<Tile> tile_ref;

#include "playerController.h"

//Card abilities
template <int quantity>
void abilityDrawCards(Game_master &gm, Card &activator);
template <int quantity>
void abilityGainCredits(Game_master &gm, Card &activator);

class Game_master
{
public: // _____________________________________________________________________________
    Game_master(const std::vector<pctrl_ref> &controllers, const std::vector<deck_ref> &ndecks);

    bool mainLoop(); // Process player inputs and update status for everyone.
    void endTurn();  // Pass the turn to next player and process the necessary triggers.

    int getTurn() { return turn; }
    int getTurnAbsolute() { return turnAbsolute; }

    int getGridWidth() { return grid[0].size(); }
    int getGridHeight() { return grid.size(); }

    enum eventCodes
    {
        NONE,
        UNKNOWN,
        GAME_WIN,
        GAME_LOSE,
        ACT_INVTYPE,
        ACT_INVARGS,
        ACT_PERMISSION,
        ACT_NOSELECT,
        ACT_NOTARGET,
        ACT_EXHAUSTED,
        ACT_NOFUNDS,
        DECK_NOREFRESH
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
    std::vector<pctrl_ref> playerControllers; // entities that provide player inputs.
                                              // E.G. User, AIs, network-connected players
    // Playing field
    std::vector<std::vector<Tile>> grid; // The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::vector<card_ref> activeCards;   // Cards that are currently on the playing field.

    int processAction(const PlayerAction &action);

    void updateStatus(int playerId);
    bool checkDominance(int playerId);

    int turn;
    int turnAbsolute;

    bool enableGameLoop;

    // Cards
    bool deployCard(Card &card, std::optional<tile_ref> targer);         // Place a card in play.
    bool moveCard(Card &card, const int &direction);   // Move a card in specified direction.
    void destroyCard(Card &card);                      // Remove a card from play and discard it.
    bool attackWith(Card &card, const int &direction); // Resolve an attack from one tile to another.

    int ResolveCombat(Card &attacker, Card &defender); // Resolve combat between two units.
    enum combatOutcome
    {
        WIN,
        TIE,
        LOSE
    };
    // Tiles
    std::vector<std::optional<tile_ref>> getAdjacentTiles(const Tile &tile);
    std::vector<std::optional<tile_ref>> getSurroundingTiles(const Tile &tile);
    // Players
    bool forceDraw(int playerId);                             // Returns whether deck is empty or not
    bool discard(int playerId, int handIndex);                // Returns whether discarding this card was possible
    bool playCard(int playerId, int handIndex, std::optional<tile_ref> target); // Perform rule checks and deploy card

public: // Card abitities
    template <int quantity>
    friend void abilityDrawCards(Game_master &gm, Card &activator);
    template <int quantity>
    friend void abilityGainCredits(Game_master &gm, Card &activator);
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
    //  std::vector<std::function<void(Game_master&)>> onTurnStart;
    //  std::vector<std::function<void(Game_master&)>> onTurnEnd;
    //  std::vector<std::function<void(Game_master&, Card&)>> onDraw;  // Card& drawn
    //  std::vector<std::function<void(Game_master&, Card&)>> onPlay; // Card& played
    //  std::vector<std::function<void(Game_master&, Card&)>> onCommandAbilityActivate; //Card& target
    //...
};

struct Player_info
{
    // std::string name;
    int id;
    int points;
    int funds;

    int deckSize;
    int discardSize;
    int handSize;
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
    Tile(int nx = -1, int ny = -1, int ntype = NORMAL) : x(nx), y(ny), type(ntype), card(nullptr) {}

    int x, y;
    int type;

    enum tileTypes
    {
        CAPTUREZONE = -2,
        NORMAL = -1
    }; // Non-negatives refer to deploy zones of players with same IDs

    std::optional<card_ref> card;
};

class Card
{
public: // _____________________________________________________________________________
    Card(int nid = -1);
    Card& operator=(const Card& other);

    // Identification
    int ownerId;
    int id;

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

    Card_info getInfo();
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
    bool canAttack;
    bool canMove;
    bool isOverwhelmed;

    // Event triggers
    std::vector<std::function<void(Game_master &, Card &)>> onAbilityActivate;
    std::vector<std::function<void(Game_master &, Card &)>> onPlay;
    std::vector<std::function<void(Game_master &, Card &)>> onDeath;

    // std::vector<std::function<void(Game_master&, Card&)>> onTurnStart;
    // std::vector<std::function<void(Game_master&, Card&)>> onTurnEnd;

    // std::vector<std::function<void(Tile&)>> onMove; // Tile& destination
    // std::vector<std::function<void(Tile&, int)>> onAttack; // Tile& target, int damageDone
    // std::vector<std::function<void(Game_master&, Card&, Card&)>> onKill; // Card& target
    // std::vector<std::function<void(Card&, int)>> onReceiveDamage; // Card& damagedBy, int damageSustained
    //...
};

struct Card_info // Out of game context info tag 
{
    int ownerId;

    std::string name;
    std::string text;

    int x, y;
    int value;
    int cost;
    int advantage;
    int type;

    bool canAttack;
    bool canMove;
    bool isOverwhelmed;
};
