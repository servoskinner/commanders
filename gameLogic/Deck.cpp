#include "gameLogic.h"

Deck::Deck(const std::vector<Card>& cards) //Takes a <Card> container
{
    
    for(Card card : cards)
    {
        CardPtr cardptr = std::make_shared<Card>(card);
        roster.push_back(cardptr);
        library.push_back(cardptr);
    }
}