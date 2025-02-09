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

    #ifdef LOGGER_ON
        Logger::get().write("Created deck " + ref_to_string(this));
    #endif
}

void Game_master::Deck::shuffle()
{
    #ifdef LOGGER_ON
        Logger::get().write("Shuffling deck " + ref_to_string(this));
    #endif

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
    #ifdef LOGGER_ON
        Logger::get().write("Refreshing deck " + ref_to_string(this));
    #endif

    library.insert(library.end(), junk.begin(), junk.end()); // Move discard to library
    junk.clear();
   
    for (Card_ref cref : library)
        cref.get().status = Card::CSTATUS_LIBRARY;

    shuffle();
}