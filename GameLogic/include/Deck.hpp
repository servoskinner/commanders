#pragma once
#include "Game_master.hpp"
#include "Card_generator.hpp"

class Game_master::Deck
{
public: // _____________________________________________________________________________
    Deck(const std::vector<Card> &cards);
    Deck(const Deck &original);
    Deck(const std::vector<int> &deck_image);

    Deck &operator=(const Deck &original);

    std::vector<Card> all;    // All cards associated with this deck. Original cards are stored here.
    std::vector<card_ref> discard; // Cards that have been removed after being put into play.
    std::vector<card_ref> library; // Cards that can be drawn.

    void shuffle();
    void refresh();
};