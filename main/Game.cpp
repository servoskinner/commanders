#include "TUI.hpp"
#include "Animations.hpp"
#include "Event_timer.hpp"
#include "Sprite_storage.hpp"
#include "Misc_functions.hpp"

#include <string>
#include <vector>
#include <thread>
#include <functional>

int main()
{
    // -----------------
    // Global variables
    // -----------------

    Storage_manager& storage = Storage_manager::get_default();

    enum game_screen {
        INTRO,
        CALLSIGN_PROMPT,
        MAIN_MENU,
        GAME,
        DECK_ED,
        SETTINGS
    };

    game_screen cur_screen = INTRO;
    bool run_loop = true;
    std::string callsign;

    // ------
    // Intro
    // ------

    Event_timer intro_timer;
    intro_timer.skip_stalled = false;

    TUI::Rect intro_rect(28, 54, 8, 3);
    intro_rect.set_all({' ', COLOR_BLACK, COLOR_RED});
    int drawing_phase = 0;

    TUI::Text intro_text1("HEGEMON OS   54021", 1, 0, 6, 3, COLOR_RED);
    intro_text1.visible = false;
    TUI::Text intro_text2("37.0/2A (MU-11)", 1, 0, 6, 26, COLOR_RED);
    intro_text2.visible = false;

    Rolling_text intro_rolling1(0.0001);
    intro_rolling1.text.x = 59;
    intro_rolling1.text.y = 9;
    intro_rolling1.text.width = 24;
    intro_rolling1.text.foreground = COLOR_RED;
    intro_rolling1.origin_text = \
    "COMMAND SUBROUTINE STARTING UP ............ DONE \
     PSYCHOTRONIC LINK VERSION..54.93 CHECKSUM \
     72A02FDD INITIATING STARTUP SEQUENCE ............ DONE \
     PARTICLE BEAM WARMUP PROCEDURE ............... DONE \
     2A 96 97 DF GENETIC SIGNATURE CHECK PASSED - INITIATING CONTROL AUTHORITY TRANSFER ........... DONE";
    intro_rolling1.visible = false;

    Rolling_text intro_rolling2(0.0001);
    intro_rolling2.text.x = 59;
    intro_rolling2.text.y = 25;
    intro_rolling2.text.width = 24;
    intro_rolling2.text.foreground = COLOR_RED;
    intro_rolling2.origin_text = "another text blah blah blah blah blah blah blah blah blah blah blah";
    intro_rolling2.visible = false;

    intro_timer.events.push_back({[&intro_text1](){ intro_text1.visible = true;}, 0.15, 1});
    intro_timer.events.push_back({[&intro_text2](){ intro_text2.visible = true;}, 0.3, 1});
    intro_timer.events.push_back({[&intro_rolling1](){ intro_rolling1.visible = true;}, 0.5, 1});
    intro_timer.events.push_back({[&intro_rolling2](){ intro_rolling2.visible = true;}, 2.4, 1});

    std::function<void(void)> end_intro = [&cur_screen, &storage, &callsign]() {
        std::optional<std::vector<char>> entry = storage.get_vector<char>("callsign");
        if (entry.has_value()) {
            callsign = {entry.value().begin(), entry.value().end()};
            cur_screen = MAIN_MENU;
        }
        else {
            cur_screen = CALLSIGN_PROMPT;
        }
    };
    
    intro_timer.events.push_back({end_intro, 3.7, 1});

    // ----------------
    // Callsign prompt
    // ----------------

    TUI& tui = TUI::get();
    TUI::Rect callsign_prompt_frame(3, 50, tui.get_size().first/2-2, tui.get_size().second/2-25, COLOR_RED);

    TUI::Text callsign_prompt_title("ENTER-CALLSIGN:", 0, 0, 0, 1, COLOR_RED);
    callsign_prompt_frame.children.emplace_back(std::ref(callsign_prompt_title));

    TUI::Text_input callsign_prompt(1, 48, 1, 1, COLOR_BRIGHT_WHITE);
    callsign_prompt_frame.children.emplace_back(std::ref(callsign_prompt));

    // ----------
    // Main menu
    // ----------

    int menu_selected_item = 0;

    std::vector<std::string> menu_items = {"start", "info", "deck editor", "settings", "quit"};

    TUI::UI_Object menu_navigation_hint(0, 3);
    TUI::Text menu_navigation_hint_up("W /\\", 0, 0, -1, 20, COLOR_BRIGHT_BLACK);
    TUI::Text menu_navigation_hint_down("S \\/", 0, 0, 1, 20, COLOR_BRIGHT_BLACK);
    menu_navigation_hint.children = {std::ref(menu_navigation_hint_down), std::ref(menu_navigation_hint_up)};
    
    TUI::Sprite menu_logo = load_sprite("../sprites/logo").value();
    menu_logo.x = 7;
    menu_logo.y = 4;

    while (run_loop) {
        unsigned input = tui.get_input();
        tui.clear();
        switch (cur_screen)
        {
        case INTRO:
            intro_timer.process();

            intro_rect.draw();
            intro_text1.draw();
            intro_text2.draw();
            intro_rolling1.draw();
            intro_rolling2.draw();
            break;
        
        case CALLSIGN_PROMPT:

            if (input == ' ') {
                input = 0; // allow no spaces in callsign
            }
            callsign_prompt_frame.draw(input);
            if (input == KEY_ENTR && callsign_prompt.text.size() > 1) {
                storage.put_vector("callsign", std::vector<char>(callsign_prompt.text.begin(), callsign_prompt.text.end()));
                cur_screen = MAIN_MENU;
            }
            break;
        case MAIN_MENU:
            {
                switch (input)
                {
                case 'w': case 'W':
                    if (menu_selected_item != 0) {
                        menu_selected_item--;
                    }
                    break;
                case 's': case 'S':
                    if (menu_selected_item != menu_items.size()-1) {
                        menu_selected_item++;
                    }
                    break;
                case KEY_ENTR:
                    switch (menu_selected_item)
                    {
                    case 4:
                        run_loop = false;
                        break;
                    }
                    break;
                }
                menu_logo.draw();

                TUI::Text menu_item_text;
                menu_item_text.children = {std::ref(menu_navigation_hint)};
                menu_item_text.x = 3;
                for (int i = 0; i < menu_items.size(); i++) {
                    menu_item_text.y = 21 + i*2;
                    menu_item_text.text = " " + menu_items[i];
                    while (menu_item_text.text.size() < 28) {
                        menu_item_text.text.append(" ");
                    }
                    if (i == menu_selected_item) {
                        menu_item_text.foreground = COLOR_BRIGHT_WHITE;
                        menu_item_text.background = COLOR_RED;
                        menu_navigation_hint.visible = true;
                    }
                    else {
                        menu_item_text.foreground = COLOR_BRIGHT_BLACK;
                        menu_item_text.background = COLOR_BLACK;
                        menu_navigation_hint.visible = false;
                    }
                    menu_item_text.draw();
                }
            }   
            break;

        case GAME:
            // int turn = gm.get_turn();
            // commander.update_state(gm.get_game_state(turn));

            // // process all events
            // std::optional<Commander::Event> ev = gm.get_event(turn);
            // while (ev.has_value()) {
            //     commander.process_event(ev.value());
            //     gm.pop_event(turn);
            //     ev = gm.get_event(turn);
            // }

            // unsigned input = TUI::get().get_input();
            // commander.active_id = turn;
            // commander.draw(input);

            // std::optional<Commander::Order> ord = commander.get_order();
            // if (ord.has_value()) {
            //     int order_code = gm.exec_order(turn, ord.value());
            //     commander.process_order_feedback(order_code);
            // }
            break;
        case DECK_ED:

            break;
        }
        tui.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}