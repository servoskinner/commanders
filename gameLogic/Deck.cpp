#include "gameLogic.h"

Deck::Deck(const std::vector<Card>& cards) : roster(0), library(0), discard(0)
{
    for(const Card& card : cards)
    {
        roster.push_back(card);
        library.push_back(&roster.back());
    }
}
Deck::Deck(const Deck& original) : roster(0), library(0), discard(0)
{
    for(const Card& card : original.roster)
    {
        roster.push_back(card);
        library.push_back(&roster.back());
    }
}
Deck& Deck::operator=(const Deck& original)
{
    roster  = std::vector<Card>(0);
    library = std::vector<Card*>(0);
    discard = std::vector<Card*>(0);

    for(const Card& card : original.roster)
    {
        roster.push_back(card);
        library.push_back(&roster.back());
    }

    return *this;
}