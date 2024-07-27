#pragma once

#include "Game_master.hpp"

class Game_master::Player
    {
    public: // _____________________________________________________________________________
        Player(int playerid);

        const int id; // Unique player ID and turn no. on which it issues orders.
        bool is_active;
        std::vector<card_ref> hand;

        int points; //"Dominance points" scored by the player. 10 are required to win the game;
        int funds;  // Funds used to play cards and fire abilities.

        Commander::Player_info get_info(Deck &deck);

        // Event triggers
        //  std::vector<std::function<void(Game_master&)>> trig_turn_start;
        //  std::vector<std::function<void(Game_master&)>> trig_turn_end;
        //  std::vector<std::function<void(Game_master&, Card&)>> trig_draw;  // Card& drawn
        //  std::vector<std::function<void(Game_master&, Card&)>> trig_play; // Card& played
        //  std::vector<std::function<void(Game_master&, Card&)>> trig_order_given; //Card& target, int order
        //...
    };