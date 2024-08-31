#include <vector>
#include <random>

#include "Game_master.hpp"

Game_master::Deck::Deck(Game_master& master, const std::vector<int>& deck_image, int player_id)
{
    for(int id : deck_image) {   
        all.emplace_back(master, id, player_id);
        library.push_back(all.back());
        all.back().status = Card::CSTATUS_LIBRARY;
    }
}

void Game_master::Deck::shuffle()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (int i = library.size() - 1; i > 0; --i)
    {
        std::uniform_int_distribution<> distrib(0, i);
        int j = distrib(gen);
        std::swap(library[i], library[j]);
    }
}

void Game_master::Deck::refresh()
{
    library.insert(library.end(), graveyard.begin(), graveyard.end()); // Move discard to library
    graveyard.clear();
   
    for (Card_ref cref : library)
        cref.get().status = Card::CSTATUS_LIBRARY;

    shuffle();
}