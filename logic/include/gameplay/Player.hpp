#pragma once

#include "Game_master.hpp"

class Game_master::Player
    {
    public: // _____________________________________________________________________________
        Player(int playerid, Deck& deck);

        const int id; // Unique player ID and turn no. on which it issues orders.
        bool is_active;
        std::vector<card_ref> hand;
        deck_ref deck;

        int points; //"Dominance points" scored by the player. 10 are required to win the game;
        int funds;  // Funds used to play cards and fire abilities.

        Commander::Player_info get_info();

        Trigger turn_start;
        Trigger turn_end;
        Trigger deploys;
    };