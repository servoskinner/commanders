#pragma once

#include "Game_master.hpp"
#include "Ability.hpp"

class Game_master::Player
    {
    public: // _____________________________________________________________________________
        Player(int player_id, Deck& p_deck);

        const int id; // Unique player ID and turn no. on which it issues orders.
        bool is_active;
        std::vector<Card_ref> hand;
        Deck_ref deck;

        int points; //"Dominance points" scored by the player. 10 are required to win the game;
        int funds;  // Funds used to play cards and fire abilities.

        Commander::Player_info get_info();

        Trigger turn_start;
        Trigger turn_end;
        Trigger deploys;
        Trigger draws;
    };