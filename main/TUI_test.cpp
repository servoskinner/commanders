#include "TUI.hpp"
#include "Focus.hpp"

#include <ncurses.h>
#include <thread>
#include <string>

int main()
{
    TUI::Rect rect;
    TUI::Scrollable_text scroll;
    TUI::Text_input input_box;
    TUI& tui = TUI::get();

    tui.set_color_pair(1, COLOR_BRIGHT_WHITE, COLOR_BLACK);
    tui.set_color_pair(2, COLOR_BRIGHT_BLUE, COLOR_BLACK);
    tui.set_color_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    
    rect.x = 10, rect.y = 10;

    rect.tl_corner.color = 1;
    rect.t_border.color = 1;
    rect.tr_corner.color = 1;
    rect.set_vborders({ACS_VLINE, 2});
    rect.bl_corner.color = 3;
    rect.b_border.color = 3;
    rect.br_corner.color = 3;

    scroll.x = 11; scroll.y = 11;
    rect.width = 20, rect.height = 5;
    scroll.width = 18, scroll.height = 3;

    input_box.x = 1;
    input_box.y = 17;
    input_box.width = 30;
    input_box.height = 5;
    input_box.color = 3;


    scroll.text = \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna \
    aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco \
    laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure \
    dolor in reprehenderit in voluptate velit esse cillum dolore eu \
    fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, \
    sunt in culpa qui officia deserunt mollit anim id est laborum.";

    while (true) {
        tui.clear();

        unsigned input = tui.get_input();
        if(input == 'q' || input == 'Q') {
            break;
        }
        rect.draw(input);
        scroll.draw(input);
        input_box.draw(input);

        tui.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}