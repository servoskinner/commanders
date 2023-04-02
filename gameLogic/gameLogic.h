#pragma once
#include <vector>
#include <string>
#include <functional>

#include "playerController.h"

class GameMaster;
// class Player;
// class Deck;
class Card;
class Tile;

class GameMaster
{
    public:
    // std::vector<Player> players;
    // std::vector<std::reference_wrapper<PlayerController>> playerControllers; //entities that provide player inputs.
                                                      //E.G. User, AIs, network-connected players

    //grid
    std::vector<std::vector<Tile>> grid;

    // std::vector<std::vector<Tile>> deployZones;
    // std::vector<Tile> captureZone;

    // void setup(); 
    // void switchTurn();

    GameMaster(int gridHeight = 6, int gridWidth = 8);
};

// class Player //inherited by different player agent types
// {
//     public:
//     int playerId;

//     Deck& deck;
//     std::vector<std::reference_wrapper<Card>> hand;

//     int points;
//     int funds;
//     std::vector<std::reference_wrapper<Card>> cardsInPlay;

//     int draw(Deck& targetDeck); //should return whether deck was refreshed or not
//     void discard(int handIndex);
//     void play(int handIndex, Tile& target);
//     /*void commandMove(Card& object, Tile& destination);
//     void commandAttack(Card& object, Tile& target);
//     void commandActivateAbility(Card& object, Card& target);*/
//     void endTurn();

//     //Event triggers
//     std::vector<std::function<void()>> onTurnStart;
//     std::vector<std::function<void()>> onTurnEnd;
//     std::vector<std::function<void(Card&)>> onDraw;  // Card& drawn
//     std::vector<std::function<void(Card&)>> onPlay; // Card& played
//     std::vector<std::function<void(Card&)>> onCommandAbilityActivate; //Card& target
//     //...
// };

// class Deck
// {
//     public:
//     std::vector<Card> cards;
//     std::vector<Card> discard;

//     void shuffle();
//     void refresh();
// };

class Tile
{
    public:
    GameMaster& master;

    int x, y;
    enum tileTypes {CAPTUREZONE = -2, NORMAL = -1}; //Non-negatives refer to deploy zones of players with same IDs
    int type;

    Card* card; //NULL if empty

    enum nearbyTiles {UP, RIGHT, DOWN, LEFT, UPRIGHT, DOWNRIGHT, DOWNLEFT, UPLEFT};
    std::vector<Tile*> getAdjacent();
    std::vector<Tile*> getSurrounding();

    Tile(GameMaster& new_master, int x = -1, int y = -1);
};

class Card
{
    public:
    // General game parameters
    // Player* owner;
    int id;
    //int type
    enum cardType {UNIT, CONTRACT, TACTIC};

    enum playPositions {UNDEFINED = -1, DECK = 0, HAND = 1, IN_PLAY = 2, DISCARD = 3};
    int playPosition;
    Tile *gridPosition;

    int playCost;
    int value;
    int advantage;

    //Actions
    bool Deploy(Tile* destination = NULL);
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
    // std::vector<std::function<void(Card* target)>> onAbilityActivate; // Card* target
    //...


    // Graphic & UI parameters
    std::string name;
    std::string text;
    //...

    
};