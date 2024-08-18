#pragma once

#include "Game_master.hpp"
#include <list>

class Game_master::Deck
{
public: // _____________________________________________________________________________
    Deck(Game_master& master, const std::vector<int>& deck_image, int player_id = -1);

    std::list<Card> all;           // All cards associated with this deck. Original cards are stored here.
    std::vector<card_ref> graveyard; // Cards that have been removed after being put into play.
    std::vector<card_ref> library;   // Cards that can be drawn.

    void shuffle();
    void refresh();
    // bool mill(int n_cards = 1);
};