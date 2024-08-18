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
        OPPRESSOR,
        MAIMDROID,
        MAIMDROID,
        GUNKFOOD,
        GUNKFOOD,
        MACHINEPARTS,
        FISSION
    };
    std::vector<int> deck2 =
    {
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        TERRORGUARD,
        TERRORGUARD,
        OPPRESSOR,
        MAIMDROID,
        MAIMDROID,
        GUNKFOOD,
        GUNKFOOD,
        MACHINEPARTS,
        FISSION
    };

    CLI_commander cmd1, cmd2;
    std::vector<std::vector<int>> deck_images = {deck1, deck2};

    Game_master gm(deck_images);

    cmd1.static_game_info = gm.get_static_game_info();
    cmd2.static_game_info = gm.get_static_game_info();

    while(true)
    {
        cmd1.game_status = gm.get_status(0);
        cmd2.game_status = gm.get_status(1);
        if (gm.get_turn() % 2 == 0){
            int order_code = gm.exec_order(0, cmd1.get_order());
        }
        else {
            int order_code = gm.exec_order(1, cmd2.get_order());
        }
    }
    
    return 0;
}
