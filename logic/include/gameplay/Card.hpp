#pragma once

#include "Game_master.hpp"
#include "Unique.hpp"
#include "Ability.hpp"

/// @brief A gameplay-wise representation of a card.
class Game_master::Card
{
public: // _____________________________________________________________________________
    Card(Game_master& master, int id, int oid = -1);
    /// @brief Re-initializes everything that could have changed when the card was on field (Power, etc.)
    void reset();

    // Identification
    const int owner_id; /// ID of the player in whose deck was this card when the game began.
    const int card_id; /// Which card in game it is
    int controller_id; /// ID of the player who can give orders to this card.

    Unique entity_id; /// Unique ID used to compare identities of different objects.

    Card_type type;
    enum Card_status
    {
        CSTATUS_UNDEFINED = -1,
        CSTATUS_LIBRARY = 0,
        CSTATUS_HAND = 1,
        CSTATUS_BATTLEFIELD = 2,
        CSTATUS_GRAVEYARD = 3
    };
    Card_status status;

    Commander::Card_info get_info();
    // Gameplay
    int x, y; /// The card's position on the grid, if it is there.

    int cost; /// Funds required to deploy the card.
    int value; /// The unit's Combat Power or the contract's duration.
    int advantage;
    // Status effects
    bool can_attack;
    bool can_move;
    bool is_overwhelmed;

    std::list<std::unique_ptr<Ability>> abilities;
    // Event triggers

    Cause enters_play; /// Triggered when card is deployed.
    Cause leaves_play; /// Triggered when card is put into junk.

    Cause before_move; /// Triggered after the card has been issued a valid move order and before it moves.
    Cause after_move; /// Triggered after the card makes a valid move.

    Cause before_attack; /// Triggered after the card has been issued a valid attack order and before it attacks.
    Cause after_attack; /// Triggered after the card successfully executes an attack order. 
    Cause before_attacked; /// Triggered after an attack has been declared on the card and before the combat is resolved.
    Cause after_attacked; /// Triggered after the card was attacked, after the resolution of combat.
};