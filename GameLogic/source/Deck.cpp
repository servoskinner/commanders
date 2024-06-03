#include <vector>
#include <random>

#include "Card_generator.hpp"
#include "Game_master.hpp"

Game_master::Deck::Deck(const std::vector<Card> &cards) : all(cards), library(), discard()
{
    for(Card& card : all)
    {   
        card.status = Card::CSTATUS_IN_DECK;
        library.emplace_back(std::ref(card));
    }
}

void Game_master::Deck::shuffle()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (int i = library.size() - 1; i > 0; --i)
    {
        std::uniform_int_distribution<>distrib(0, i);
        int j = distrib(gen);
        std::swap(library[i], library[j]);
    }
}

void Game_master::Deck::refresh()
{
    library.insert(library.end(), discard.begin(), discard.end()); // Move discard to library
    discard = std::vector<card_ref>();

    for (card_ref cref : library)
        cref.get().status = Card::CSTATUS_IN_DECK;

    shuffle();
}

Game_master::Deck::Deck(const Deck &original) : all(original.all), library(), discard()
{
    for(Card& card : all)
    {   
        card.status = Game_master::Card::CSTATUS_IN_DECK;
        library.emplace_back(std::ref(card));
    }
}

Game_master::Deck::Deck(const std::vector<int> &deck_image) : all(0), library(), discard()
{
    Card_generator& card_generator = Card_generator::get(); 
    for(int id : deck_image)
    {   
        all.emplace_back(card_generator.get_card_instance(id));
        library.emplace_back(std::ref(all.back()));
    }
}

Game_master::Deck &Game_master::Deck::operator=(const Deck &original)
{
    library = std::vector<card_ref>();
    discard = std::vector<card_ref>();

    all = original.all;

    for(Card& card : all)
    {   
        card.status = Card::CSTATUS_IN_DECK;
        library.emplace_back(std::ref(card));
    }

    return *this;
}