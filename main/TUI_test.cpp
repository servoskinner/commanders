#include "TUI.hpp"
#include "Focus.hpp"
#include "Animations.hpp"
#include "Sprite_storage.hpp"

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
    
    rect.x = 10, rect.y = 17;

    rect.tl_corner.foreground = COLOR_BRIGHT_WHITE;
    rect.t_border.foreground = COLOR_BRIGHT_WHITE;
    rect.tr_corner.foreground = COLOR_BRIGHT_WHITE;
    rect.set_vborders({ACS_VLINE, COLOR_CYAN});
    rect.bl_corner.foreground = COLOR_BLUE;
    rect.b_border.foreground = COLOR_BLUE;
    rect.br_corner.foreground = COLOR_BLUE;

    scroll.x = 11; scroll.y = 18;
    rect.width = 20, rect.height = 5;
    scroll.width = 18, scroll.height = 3;

    rolling_text.x = 15;
    rolling_text.y = 1;
    rolling_text.text.width = 30;
    rolling_text.text.height = 5;
    rolling_text.text.foreground = COLOR_WHITE;


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

    TUI::Sprite sprite = load_sprite("../sprites/grass").value();
    sprite.x = 12;
    sprite.y = 1;

    while (true) {
        unsigned input = tui.get_input();
        if(input == 'q' || input == 'Q') {
            break;
        }
        
        tui.clear();

        rect.draw(input);

        sprite.draw(0, 0, 0);
        sprite.draw(0, 0, 4);
        sprite.draw(0, 0, 8);
        sprite.draw(0, 4, 4);
        sprite.draw(0, 4, 8);
        sprite.draw(0, 8, 8);

        scroll.draw(input);
        rolling_text.draw(input);
        crazy_box.draw(input);

        tui.render();

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}