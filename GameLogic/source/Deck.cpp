#include <vector>
#include <random>

#include "gameLogic.h"

Deck::Deck(const std::vector<Card> &cards) : all(0), library(0), discard(0)
{
    all = cards;

    for(Card& card : all)
    {   
        card.status = Card::DECK;
        library.push_back(std::ref(card));
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
    discard = std::vector<card_ref>(0);

    for (card_ref cref : library)
        cref->get().status = Card::DECK;

    shuffle();
}

Deck::Deck(const Deck &original) : all(0), library(0), discard(0)
{
    all = original.all;

    for(Card& card : all)
    {   
        card.status = Card::DECK;
        library.push_back(&card);
    }
}

Deck &Deck::operator=(const Deck &original)
{
    library = std::vector<Card *>(0);
    discard = std::vector<Card *>(0);

    all = original.all;

    for(Card& card : all)
    {   
        card.status = Card::DECK;
        library.push_back(&card);
    }

    return *this;
}