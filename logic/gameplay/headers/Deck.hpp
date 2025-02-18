#ifndef _INCLUDE_DECK_HPP
#define _INCLUDE_DECK_HPP

#include "Game_master.hpp"
#include "Unique_map.hpp"
#include <list>

#ifdef LOGGER_ON
#include "Logger.hpp"
#endif

class Game_master::Deck : Unique
{
public: // _____________________________________________________________________________
    Deck(const std::vector<unsigned int>& deck_image, int player_id = -1);

    Unique entity_id;
    // Movable
    Deck(Deck&& other) noexcept = default;
    Deck& operator=(Deck&& other) noexcept = default;

    Unique_map<Card_ref> all;           /// All cards associated with this deck. Original cards are stored here.
    std::vector<Card> junk;    /// Cards that have been removed after being put into play.
    std::vector<Card> library; /// Cards that can be drawn.

    void shuffle(); /// Randomizes the order of cards in library.
    void refresh(); /// Move the contents of junk to library, then shuffle.
    // bool mill(int n_cards = 1);
private:
    // Non-copyable
    Deck(Deck& other) = delete;
    Deck& operator=(Deck& other) = delete;
};

#endif