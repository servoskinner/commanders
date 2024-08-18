#include "NCurses_commander.hpp"
#include "Card_index.hpp"

int main()
{
    NCurses_commander commander;
    commander.active_id = 0;
    commander.grid_height = 6;
    commander.grid_width = 8;

    commander.hands.push_back({});

    Commander::Card_info merc;
    merc.type = CTYPE_UNIT;
    merc.card_id = BOUNTYHUNTER;
    merc.value = 2;
    merc.advantage = 0;
    merc.x = 1;
    merc.y = 2;
    merc.owner_id = 1;

    commander.active_cards.push_back(merc);
    merc.y = 3;
    merc.owner_id = 0;
    commander.active_cards.push_back(merc);
    commander.hands[0].push_back(merc);
    merc.x = 2;
    merc.y = 4;
    merc.owner_id = 1;
    commander.active_cards.push_back(merc);

    merc.owner_id = 0;
    merc.value = 3;
    merc.advantage = 1;
    merc.x = 5;
    merc.y = 5;
    merc.card_id = MAIMDROID;
    commander.active_cards.push_back(merc);
    commander.hands[0].push_back(merc);

    merc.type = CTYPE_CONTRACT;
    merc.card_id = FISSION;
    commander.hands[0].push_back(merc);
    merc.card_id = MACHINEPARTS;
    commander.hands[0].push_back(merc);
    merc.card_id = OMNITANK;
    merc.type = CTYPE_UNIT;
    commander.hands[0].push_back(merc);

    bool is_running = true;
    while (is_running)
    {
        commander.apply_updates();
        is_running = commander.is_on();
    }
}