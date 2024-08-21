#include "NCurses_commander.hpp"
#include "Card_index.hpp"
#include "Game_master.hpp"

#include <thread>


int main()
{
    NCurses_commander commander;

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
        MEGALO,
        MAIMDROID,
        MAIMDROID,
        GUNKFOOD,
        GUNKFOOD,
        MACHINEPARTS,
        FISSION
    };

    std::vector<std::vector<int>> deck_images = {deck1, deck2};
    Game_master gm(deck_images);

    commander.set_params(gm.get_static_game_info());

    while(gm.is_on())
    {
        int turn = gm.get_turn();
        commander.update_state(gm.get_game_state(turn));
        commander.active_id = turn;
        commander.run();

        int order_code = gm.exec_order(turn, commander.get_order());
        commander.process_order_feedback(order_code);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}