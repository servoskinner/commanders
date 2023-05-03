#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

#define GRID_WIDTH 8
#define GRID_HEIGHT 6

class GameMaster; // Basically, a singular game session. 
class Player; // A player in context of game. Does not generate any input.
class Deck; // A card pool that can be used by players.
class Card; // A card in any state, with gameplay parameters.
class Tile; // An area that cards can be placed onto.

// Info tags to conceal gameplay elements.
struct CardInfo; 
struct PlayerInfo;

// Pointer type re-definition for clarity
typedef std::shared_ptr<GameMaster> MasterPtr;
typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<Deck> DeckPtr;
typedef std::shared_ptr<Card> CardPtr;
typedef std::shared_ptr<Tile> TilePtr;

#include "playerController.h"

class GameMaster
{
    public: // _____________________________________________________________________________
    GameMaster(const std::vector<PlayerController*> controllers, const std::vector<Deck*>& decks);

    std::vector<Player> players; 
    std::vector<Deck*> decks;
    std::vector<PlayerController*> playerControllers; // entities that provide player inputs.
                                                      // E.G. User, AIs, network-connected players
    //grid
    std::vector<std::vector<Tile>> grid; //The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::vector<Card*> activeCards; // Cards that are currently on the playing field.

    void mainLoop(); // Process player inputs and update status for everyone.
    void endTurn(); // Pass the turn to next player and process the necessary triggers.

    int getTurn() { return turn;}
    int getTurnAbsolute() { return turnAbsolute;}


    //protected: // _____________________________________________________________________________
    int processAction(const PlayerAction& action);
    enum invalidAction {NONE, INVTYPE, NOARGS, INVARGS, PERMISSION, NOSELECT, NOTARGET, EXHAUSTED, NOFUNDS};
    //void GameMaster::updateStatus(PlayerController& controller);
    int turn;
    int turnAbsolute;

    // Cards
    bool deployCard(Card& card, Tile* target); // Place a card in play.
    bool moveCard(Card& card, const int& direction); // Move a card in specified direction.
    void destroyCard(Card& card); // Remove a card from play and discard it.
    bool attackWith(Card& card, const int& direction); // Resolve an attack from one tile to another.

    int ResolveCombat(Card& attacker, Card& defender); // Resolve combat between two units.
    enum combatOutcome {WIN, TIE, LOSE};
    // Tiles
    enum nearbyTiles {UP, RIGHT, DOWN, LEFT, UPRIGHT, DOWNRIGHT, DOWNLEFT, UPLEFT};
    std::vector<Tile*> getAdjacentTiles(const Tile& tile);
    std::vector<Tile*> getSurroundingTiles(const Tile& tile);
    // Players
    bool giveCard(int player); // Returns whether deck was refreshed or not
    bool discard(int player, int handIndex); // 
    bool playCard(int handIndex, Tile& target);
};

class Player
{
    public: // _____________________________________________________________________________
    Player(int playerid);
    
    const int id; //Unique player ID and turn no. on which it issues orders.
    std::vector<Card*> hand;

    int points; //"Dominance points" scored by the player. 10 are required to win the game;
    int funds; // Funds used to play cards and fire abilities.

    PlayerInfo getInfo();
    
    /*void commandMove(Card& object, Tile& destination);
    void commandAttack(Card& object, Tile& target);
    void commandActivateAbility(Card& object, Card& target);*/

    //Event triggers
    // std::vector<std::function<void()>> onTurnStart;
    // std::vector<std::function<void()>> onTurnEnd;
    // std::vector<std::function<void(Card&)>> onDraw;  // Card& drawn
    // std::vector<std::function<void(Card&)>> onPlay; // Card& played
    // std::vector<std::function<void(Card&)>> onCommandAbilityActivate; //Card& target
    //...
};

struct PlayerInfo
{
    //std::string name;
    int id;
    int points;
    int money;
    int deckSize;
    int discardSize;
};

class Deck
{
    public: // _____________________________________________________________________________
    Deck(const std::vector<Card>& cards);
    Deck(const Deck& original);
 
    std::vector<Card> roster; //All cards associated with this deck. Original cards are stored here.
    std::vector<Card*> discard; //Cards that have been removed after being put into play.
    std::vector<Card*> library; //Cards that can be drawn.
     
    void shuffle();
    void refresh();
};

class Tile
{
    public: // _____________________________________________________________________________
    Tile(int nx, int ny, int ntype = -1) : x(nx), y(ny), type(ntype), card(nullptr) {}
    Tile(Tile& original) = default;

    int x, y;
    int type;
    
    enum tileTypes {CAPTUREZONE = -2, NORMAL = -1}; //Non-negatives refer to deploy zones of players with same IDs
    
    Card* card; //NULL if empty
};

class Card
{
    public: // _____________________________________________________________________________
    Card(int nid);
    Card(const Card& original) = default;
    // Identification 
    int ownerId;
    const int id;

    enum cardType {UNIT, CONTRACT, TACTIC};
    int type;
    enum cardStatus {UNDEFINED = -1, DECK = 0, HAND = 1, IN_PLAY = 2, DISCARD = 3};
    int status;

    CardInfo getInfo();
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
    // std::vector<std::function<void(Tile*)>> onPlay; // Tile* deployLocation

    // std::vector<std::function<void()>> onTurnStart;
    // std::vector<std::function<void()>> onTurnEnd;

    // std::vector<std::function<void(Tile&)>> onMove; // Tile& destination
    // std::vector<std::function<void(Tile&, int)>> onAttack; // Tile& target, int damageDone
    // std::vector<std::function<void(Card&)>> onKill; // Card& target
    // std::vector<std::function<void(Card&, int)>> onReceiveDamage; // Card& damagedBy, int damageSustained
    // std::vector<std::function<void(Card&)>> onDeath; // Card& killedBy
    // std::vector<std::function<void(Card* target)>> onAbilityActivate; // Card* target
    //...

};

struct CardInfo //Out of game context info tag
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
