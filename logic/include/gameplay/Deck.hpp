#pragma once

#include "Game_master.hpp"
#include <list>

#ifdef LOGGER_ON
#include "Logger.hpp"
#endif

class Game_master::Deck
{
public: // _____________________________________________________________________________
    Deck(Game_master& master, const std::vector<int>& deck_image, int player_id = -1);

    std::list<Card> all;           /// All cards associated with this deck. Original cards are stored here.
    std::vector<Card_ref> junk;    /// Cards that have been removed after being put into play.
    std::vector<Card_ref> library; /// Cards that can be drawn.

    void shuffle(); /// Randomizes the order of cards in library.
    void refresh(); /// Move the contents of junk to library, then shuffle.
    // bool mill(int n_cards = 1);
};