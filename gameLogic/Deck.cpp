#include <vector>
#include <random>

#include "gameLogic.h"

Deck::Deck(const std::vector<Card> &cards) : roster(0), library(0), discard(0)
{
    roster = cards;
    for(Card& card : roster)
    {   
        card.status = Card::DECK;
        library.push_back(&card);
    }
}

void Deck::shuffle()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    for (int i = library.size() - 1; i > 0; --i)
    {
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(gen);
        std::swap(library[i], library[j]);
    }
}

void Deck::refresh()
{
    library.insert(library.end(), discard.begin(), discard.end()); // Move discard to library
    discard = std::vector<Card *>(0);

    for (Card *cptr : library)
        cptr->status = Card::DECK;

    shuffle();
}

Deck::Deck(const Deck &original) : roster(0), library(0), discard(0)
{
    roster = original.roster;
    for(Card& card : roster)
    {   
        card.status = Card::DECK;
        library.push_back(&card);
    }
}

Deck &Deck::operator=(const Deck &original)
{
    roster = std::vector<Card>(0);
    library = std::vector<Card *>(0);
    discard = std::vector<Card *>(0);

    roster = original.roster;
    for(Card& card : roster)
    {   
        card.status = Card::DECK;
        library.push_back(&card);
    }

    return *this;
}