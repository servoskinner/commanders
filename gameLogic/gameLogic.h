#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>

#include "playerController.h"

class GameMaster;
class Player;
class Deck;
class Card;
class Tile;

struct CardInfo; 
struct PlayerInfo;

typedef std::shared_ptr<GameMaster> MasterPtr;
typedef std::shared_ptr<Player> PlayerPtr;
typedef std::shared_ptr<Deck> DeckPtr;
typedef std::shared_ptr<Card> CardPtr;
typedef std::shared_ptr<Tile> TilePtr;

class GameMaster
{
    public:
    std::vector<Player> players;
    std::vector<ControllerPtr> playerControllers; //entities that provide player inputs.
                                                      //E.G. User, AIs, network-connected players

    //grid
    std::vector<std::vector<Tile>> grid;

    // std::vector<std::vector<Tile>> deployZones;
    // std::vector<Tile> captureZone;

    void switchTurn();

    GameMaster(std::vector<ControllerPtr> controllers, std::vector<DeckPtr> decks, int gridHeight = 6, int gridWidth = 8);

    private:

    int turn;
};

class Player
{
    public:
    int playerId;

    Deck& deck;
    std::vector<CardPtr> hand;

    int points;
    int funds;
    std::vector<CardPtr> cardsInPlay;

    Player(Deck& ndeck, int playerid);

    int draw(Deck& targetDeck); //should return whether deck was refreshed or not
    void discard(int handIndex);
    void play(int handIndex, Tile& target);
    /*void commandMove(Card& object, Tile& destination);
    void commandAttack(Card& object, Tile& target);
    void commandActivateAbility(Card& object, Card& target);*/
    void endTurn();

    //Event triggers
    std::vector<std::function<void()>> onTurnStart;
    std::vector<std::function<void()>> onTurnEnd;
    std::vector<std::function<void(Card&)>> onDraw;  // Card& drawn
    std::vector<std::function<void(Card&)>> onPlay; // Card& played
    std::vector<std::function<void(Card&)>> onCommandAbilityActivate; //Card& target
    //...
};

struct PlayerInfo
{
    int id;

    //std::string name;

    int points;
    int money;
    int deckSize;
    int discardSize;
};

class Deck
{
     public:
     std::vector<Card> cards;
     std::vector<Card> discard;

     std::vector<std::shared_ptr<Card>> associated;

     Deck();
     
     void shuffle();
     void refresh();
};

class Tile
{
    public:
    GameMaster& master;

    int x, y;
    enum tileTypes {CAPTUREZONE = -2, NORMAL = -1}; //Non-negatives refer to deploy zones of players with same IDs
    int type;

    CardPtr card; //NULL if empty

    enum nearbyTiles {UP, RIGHT, DOWN, LEFT, UPRIGHT, DOWNRIGHT, DOWNLEFT, UPLEFT};
    std::vector<TilePtr> getAdjacent();
    std::vector<TilePtr> getSurrounding();

    Tile(GameMaster& new_master, int x = -1, int y = -1);
};

class Card
{
    public:
    // General game parameters
    PlayerPtr owner;
    int id;
    //int type
    enum cardType {UNIT, CONTRACT, TACTIC};

    enum playPositions {UNDEFINED = -1, DECK = 0, HAND = 1, IN_PLAY = 2, DISCARD = 3};
    int playPosition;
    TilePtr gridPosition;

    int playCost;
    int value;
    int advantage;

    bool canAttack;
    bool canMove;
    bool isOverwhelmed;

    //Actions
    bool Deploy(TilePtr destination = NULL);
    bool Move(int direction);
    void Kill();
    bool Attack(int direction);

    int ResolveCombat(Card& target);
    enum combatOutcome {WIN, TIE, LOSE};

    Card();

    // Event triggers
    // std::vector<std::function<void(Tile*)>> onPlay; // Tile* deployLocation

    // std::vector<std::function<void()>> onTurnStart;
    // std::vector<std::function<void()>> onTurnEnd;

    // std::vector<std::function<void(Tile&)>> onMove; // Tile& destination
    // std::vector<std::function<void(Tile&, int)>> onAttack; // Tile& target, int damageDone
    // std::vector<std::function<void(Card&)>> onKill; // Card& target
    // std::vector<std::function<void(Card&, int)>> onReceiveDamage; // Card& damagedBy, int damageSustained
    // std::vector<std::function<void(Card&)>> onDeath; // Card& killedBy
    // std::vector<std::function<void(CardPtr target)>> onAbilityActivate; // CardPtr target
    //...


    // Graphic & UI parameters
    std::string name;
    std::string text;
    //...

};

struct CardInfo //Out of game context info tag
{
    int ownerid;
    int playPosition;

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
