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
        BOUNTYHUNTER,
        TERRORGUARD,
        TERRORGUARD,
        MAIMDROID,
        MAIMDROID,
        PEPTANE,
        PEPTANE,
        MACHINESHOP,
        MACHINESHOP,
        DEMOLITIONIST,
        LOGISTICS,
        LOGISTICS,
        LOGISTICS,
        UNICYCLONE,
        UNICYCLONE
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
        DEMOLITIONIST,
        LOGISTICS,
        LOGISTICS,
        LOGISTICS,
        UNICYCLONE,
        UNICYCLONE
    };

    std::vector<std::vector<int>> deck_images = {deck1, deck2};
    Game_master gm(deck_images);

    commander.set_params(gm.get_static_game_info());

    while(commander.is_on())
    {
        int turn = gm.get_turn();
        commander.update_state(gm.get_game_state(turn));

        // process all events
        std::optional<Commander::Event> ev = gm.get_event(turn);
        while (ev.has_value()) {
            commander.process_event(ev.value());
            gm.pop_event(turn);
            ev = gm.get_event(turn);
        }

        unsigned input = TUI::get().get_input();
        commander.active_id = turn;
        commander.draw(input);

        std::optional<Commander::Order> ord = commander.get_order();
        if (ord.has_value()) {
            int order_code = gm.exec_order(turn, ord.value());
            commander.process_order_feedback(order_code);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}