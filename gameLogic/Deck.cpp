#include "gameLogic.h"

Deck::Deck(const std::vector<Card>& cards)
{
    discard = std::vector<Card*>(0);

    for(const Card& card : cards)
    {
        roster.push_back(card);
        library.push_back(&roster.back());
    }
}
Deck::Deck(const Deck& original)
{
    discard = std::vector<Card*>(0);
    
    for(const Card& card : original.roster)
    {
        roster.push_back(card);
        library.push_back(&roster.back());
    }
}