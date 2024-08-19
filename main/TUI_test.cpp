#include "TUI.hpp"
#include "Focus.hpp"

#include <ncurses.h>
#include <thread>
#include <string>

int main()
{
    TUI::Rect rect;
    TUI::Scroll_box scroll;
    TUI& tui = TUI::get();

    tui.set_color_pair(1, COLOR_BLACK, COLOR_CYAN);
    
    rect.x = 1, rect.y = 1;
    rect.border_color = 1;

    scroll.x = 2; scroll.y = 2;
    rect.width = 20, rect.height = 5;
    scroll.width = 18, scroll.height = 3;


    scroll.text = \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna \
    aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco \
    laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure \
    dolor in reprehenderit in voluptate velit esse cillum dolore eu \
    fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, \
    sunt in culpa qui officia deserunt mollit anim id est laborum.";
                   
    scroll.focus.claim_control();

    while (true) {
        tui.clear();

        unsigned input = tui.get_input();
        if(input == 'q' || input == 'Q') {
            break;
        }
        rect.draw(input);
        scroll.draw(input);

        tui.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}