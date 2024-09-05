#include "TUI.hpp"
#include "Focus.hpp"
#include "Animations.hpp"

#include <ncurses.h>
#include <thread>
#include <string>

int main()
{
    TUI::Rect rect;
    TUI::Scrollable_text scroll;
    Rolling_text rolling_text(0.05);
    TUI& tui = TUI::get();

    Crazy_box crazy_box(0.1);

    tui.set_color_pair(1, COLOR_BRIGHT_WHITE, COLOR_BLACK);
    tui.set_color_pair(2, COLOR_BRIGHT_BLUE, COLOR_BLACK);
    tui.set_color_pair(3, COLOR_MAGENTA, COLOR_BLACK);

    tui.set_color_pair(4, COLOR_BLACK, COLOR_BRIGHT_YELLOW);
    tui.set_color_pair(5, COLOR_BLACK, COLOR_BRIGHT_BLACK);
    
    rect.x = 10, rect.y = 17;

    rect.tl_corner.color = 1;
    rect.t_border.color = 1;
    rect.tr_corner.color = 1;
    rect.set_vborders({ACS_VLINE, 2});
    rect.bl_corner.color = 3;
    rect.b_border.color = 3;
    rect.br_corner.color = 3;

    scroll.x = 11; scroll.y = 18;
    rect.width = 20, rect.height = 5;
    scroll.width = 18, scroll.height = 3;

    rolling_text.x = 15;
    rolling_text.y = 1;
    rolling_text.text.width = 30;
    rolling_text.text.height = 5;
    rolling_text.text.color = 3;


    scroll.text = \
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, \
    sed do eiusmod tempor incididunt ut labore et dolore magna \
    aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco \
    laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure \
    dolor in reprehenderit in voluptate velit esse cillum dolore eu \
    fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, \
    sunt in culpa qui officia deserunt mollit anim id est laborum.";

    rolling_text.origin_text = "wake up neo.... yuo shat yuor pants..";

    crazy_box.text = "crazy box!!";
    crazy_box.rect.x = 1;
    crazy_box.rect.y = 1;
    crazy_box.rect.width = 10;
    crazy_box.rect.height = 8;

    while (true) {
        unsigned input = tui.get_input();
        if(input == 'q' || input == 'Q') {
            break;
        }
        
        tui.clear();

        rect.draw(input);
        scroll.draw(input);
        rolling_text.draw(input);
        crazy_box.draw(input);

        tui.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}