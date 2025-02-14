#include <vector>
#include <random>

#include "Game_master.hpp"

Game_master::Deck::Deck(const std::vector<unsigned int>& deck_image, int player_id)
{
    for(int id : deck_image) {   
        library.emplace_back(id, player_id);
        all.push_back(all.back());
        library.back().status = Card::CSTATUS_LIBRARY;
    }

    Unique first;
    Unique second;

    first = std::move(second);

    #ifdef LOGGER_ON
        Logger::get() << "Created deck" << this;
    #endif
}

void Game_master::Deck::shuffle()
{
    #ifdef LOGGER_ON
        Logger::get() << "Shuffling deck " << this;
    #endif

    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (int i = library.size() - 1; i > 0; --i)
    {
        std::uniform_int_distribution<> distrib(0, i);
        int j = distrib(gen);
        std::swap(std::move(library[i]), std::move(library[j]));
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