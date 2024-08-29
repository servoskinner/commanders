#include "Card_index.hpp"
#include "CLI_commander.hpp"
#include "Game_master.hpp"

#include <iostream>
#include <vector>

int main()
{
    std::vector<int> deck1 =
    {
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        TERRORGUARD,
        TERRORGUARD,
        MEGALO,
        MAIMDROID,
        MAIMDROID,
        PEPTANE,
        PEPTANE,
        MACHINESHOP,
        FISSION
    };
    std::vector<int> deck2 =
    {
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        TERRORGUARD,
        TERRORGUARD,
        MEGALO,
        MAIMDROID,
        MAIMDROID,
        PEPTANE,
        PEPTANE,
        MACHINESHOP,
        FISSION,
        LOGISTICS

    };

    CLI_commander commander;
    std::vector<std::vector<int>> deck_images = {deck1, deck2};

    Game_master gm(deck_images);

    commander.set_params(gm.get_static_game_info());

    while(gm.is_on())
    {
        int turn = gm.get_turn();
        commander.update_state(gm.get_game_state(turn));
        commander.active_id = turn;
        commander.apply_updates();

        int order_code = gm.exec_order(turn, commander.get_order());
        commander.process_order_feedback(order_code);
    }
    
    return 0;
}
