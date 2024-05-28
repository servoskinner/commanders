#include <vector>
#include <random>

#include "game_logic.hpp"

Deck::Deck(const std::vector<Card> &cards) : all(cards), library(), discard()
{
    for(Card& card : all)
    {   
        card.status = Card::DECK;
        library.emplace_back(std::ref(card));
    }
}

void Deck::shuffle()
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

void Deck::refresh()
{
    library.insert(library.end(), discard.begin(), discard.end()); // Move discard to library
    discard = std::vector<card_ref>();

    for (card_ref cref : library)
        cref.get().status = Card::DECK;

    shuffle();
}

Deck::Deck(const Deck &original) : all(original.all), library(), discard()
{
    for(Card& card : all)
    {   
        card.status = Card::DECK;
        library.emplace_back(std::ref(card));
    }
}

Deck &Deck::operator=(const Deck &original)
{
    library = std::vector<card_ref>();
    discard = std::vector<card_ref>();

    all = original.all;

    for(Card& card : all)
    {   
        card.status = Card::DECK;
        library.emplace_back(std::ref(card));
    }

    return *this;
}