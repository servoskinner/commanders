#pragma once
#include <vector>
#include <string>

#include "playerController.h"

class GameMaster
{
    std::vector<Player> players;
    std::vector<PlayerController&> playerControllers; //entities that provide player inputs.
                                                      //E.G. User, AIs, network-connected players
    std::vector<std::vector<Tile>> grid;

    std::vector<std::vector<Tile>> deployZones;
    std::vector<Tile> captureZone;

    void setup();
    void switchTurn();
};

class Player //inherited by different player agent types
{
    int playerId;

    Deck& deck;
    std::vector<Card&> hand;

    int points;
    int money;
    std::vector<Card&> cardsInPlay;

    int draw(Deck& targetDeck); //should return whether deck was refreshed or not
    void discard(int handIndex);

    void play(int handIndex, Tile& target);
    void commandMove(Card& object, Tile& destination);
    void commandAttack(Card& object, Tile& target);
    void commandActivateAbility(Card& object, Card& target);
    void endTurn();

    //Event triggers
    std::vector<void()> onTurnStart;
    std::vector<void()> onTurnEnd;
    std::vector<void(Card& drawn)> onDraw;
    std::vector<void(Card& played)> onPlay;
    std::vector<void(Card& target)> onCommandAbilityActivate;
    //...
};

class Deck
{
    std::vector<Card> cards;
    std::vector<Card> discard;

    void shuffle();
    void refresh();
};

class Tile
{
    GameMaster& master;
    int x, y;
    enum tileTypes {CAPTUREZONE = -2, NORMAL = -1}; //Non-negatives refer to deploy zones of players with same IDs
    int type;

    Card& card; //NULL if empty

    std::vector<Tile&> getAdjacent();
    std::vector<Tile&> getSurrounding();
};

class Card
{
    // Game general
    Player& owner;
    int id;

    enum playPositions {DECK = 0, HAND = 1, IN_PLAY = 2, DISCARD = 3};
    int playPosition;
    Tile &gridPosition;

    //Actions
    void moveTo(Tile& destination);
    void attack(Tile& target);

    // Event triggers
    std::vector<void(Tile& deployLocation)> onPlay;
    std::vector<void()> onTurnStart;
    std::vector<void()> onTurnEnd;

    std::vector<void(Tile& destination)> onMove;
    std::vector<void(Tile& target, int damageDone)> onAttack;
    std::vector<void(Card& target)> onKill;
    std::vector<void(Card& damagedBy, int damageSustained)> onReceiveDamage;
    std::vector<void(Card& killedBy)> onDeath;
    std::vector<void(Card& target)> onAbilityActivate;
    //...


    // Graphic & UI parameters
    std::string name;
    std::string text;
    //...

    
};