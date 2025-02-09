#include "TUI.hpp"
#include "Animations.hpp"
#include "Timer.hpp"
#include "Sprite_storage.hpp"
#include "Misc_functions.hpp"
#include "Deck_editor.hpp"

#include "NCurses_commander.hpp"
#include "Game_master.hpp"

#include <string>
#include <vector>
#include <thread>
#include <functional>

int main()
{
    // -----------------
    // Global variables
    // -----------------

    Archivist& storage = Archivist::get_default();

    enum game_screen {
        GAME_INTRO,
        GAME_CALLSIGN_PROMPT,
        GAME_MAIN_MENU,
        GAME_PREPARATION,
        GAME_CONTROL,
        GAME_DECK_ED_MAIN,
        GAME_DECK_ED_EDITOR,
        GAME_SETTINGS
    };

    game_screen cur_screen = GAME_INTRO;
    bool run_loop = true;
    std::string callsign;

    // ------
    // Intro
    // ------

    Timer intro_timer;
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
     72 A0 2F DD INITIATING STARTUP SEQUENCE ............ DONE \
     PARTICLE BEAM WARMUP PROCEDURE ............... DONE \
     2A 96 97 DF GENETIC SIGNATURE CHECK PASSED - INITIATING CONTROL AUTHORITY TRANSFER ........... DONE";
    intro_rolling1.visible = false;

    Rolling_text intro_rolling2(0.0001);
    intro_rolling2.text.x = 59;
    intro_rolling2.text.y = 25;
    intro_rolling2.text.width = 23;
    intro_rolling2.text.foreground = COLOR_RED;
    intro_rolling2.origin_text = "FF 9A 4C CE E1 96 69 CB C8 55 6F 1A B1 EB 12 87 DD 6C 15 47 48 86 0A f6 97 8D 0A 54 DD 26 10 29 LAUNCH CONFIRM";
    intro_rolling2.visible = false;

    intro_timer.events.push_back({[&intro_text1](){ intro_text1.visible = true;}, 0.15, 1});
    intro_timer.events.push_back({[&intro_text2](){ intro_text2.visible = true;}, 0.3, 1});
    intro_timer.events.push_back({[&intro_rolling1](){ intro_rolling1.visible = true;}, 0.5, 1});
    intro_timer.events.push_back({[&intro_rolling2](){ intro_rolling2.visible = true;}, 1.8, 1});

    std::function<void(void)> end_intro = [&cur_screen, &storage, &callsign]() {
        std::optional<std::vector<char>> entry = storage.get_vector<char>("callsign");
        if (entry.has_value()) {
            callsign = {entry.value().begin(), entry.value().end()};
            cur_screen = GAME_MAIN_MENU;
        }
        else {
            cur_screen = GAME_CALLSIGN_PROMPT;
        }
    };
    
    intro_timer.events.push_back({end_intro, 3.0, 1});

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
    enum main_menu_item {
        MAIN_MENU_START,
        MAIN_MENU_INFO,
        MAIN_MENU_DECK_ED,
        MAIN_MENU_SETTINGS,
        MAIN_MENU_QUIT
    };

    TUI::UI_Object menu_navigation_hint(0, 3);
    TUI::Text menu_navigation_hint_up("W /\\", 0, 0, -1, 20, COLOR_BRIGHT_BLACK);
    TUI::Text menu_navigation_hint_down("S \\/", 0, 0, 1, 20, COLOR_BRIGHT_BLACK);
    menu_navigation_hint.children = {std::ref(menu_navigation_hint_down), std::ref(menu_navigation_hint_up)};
    
    TUI::Sprite menu_logo = load_sprite("../sprites/logo").value();
    menu_logo.x = 4;
    menu_logo.y = 4;

    // ------
    // Game
    // ------

    // ----------------
    // Deck editor
    // ----------------

    TUI::Rect deck_ed_deck_sprite(11, 11, 17, 0);
    TUI::Rect deck_ed_deck_selector(11, 11, 17, 0, COLOR_BLACK, COLOR_BRIGHT_WHITE);
    TUI::Text deck_ed_deck_caption("");

    Deck_editor editor;

    while (run_loop) {
        unsigned input = tui.get_input();
        tui.clear();
        switch (cur_screen)
        {
        case GAME_INTRO:
            intro_timer.process();

            intro_rect.draw();
            intro_text1.draw();
            intro_text2.draw();
            intro_rolling1.draw();
            intro_rolling2.draw();
            break;
        
        case GAME_CALLSIGN_PROMPT:

            if (input == ' ') {
                input = 0; // allow no spaces in callsign
            }
            callsign_prompt_frame.draw(input);
            if (input == KEY_SUBMIT && callsign_prompt.text.size() > 1) {
                storage.put_string("callsign", callsign_prompt.text);
                callsign = callsign_prompt.text;
                cur_screen = GAME_MAIN_MENU;
            }
            break;
        case GAME_MAIN_MENU:
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
                case KEY_SUBMIT:
                    switch (menu_selected_item)
                    {
                    case MAIN_MENU_DECK_ED:
                        cur_screen = GAME_DECK_ED_MAIN;
                        editor.on_load();
                        break;
                    case MAIN_MENU_QUIT:
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
                    menu_item_text.y = 21 + i*3;
                    menu_item_text.text = " " + menu_items[i];
                    while (menu_item_text.text.size() < 28) {
                        menu_item_text.text.append(" ");
                    }
                    if (i == menu_selected_item) {
                        // highlight selected item
                        menu_item_text.text = " " + menu_item_text.text;
                        menu_item_text.foreground = COLOR_BRIGHT_WHITE;
                        menu_item_text.background = COLOR_RED;
                        menu_navigation_hint.visible = true;
                        // set nav hints to visible/invisible if UI item is first or last
                        if (menu_selected_item == menu_items.size()-1) {
                            menu_navigation_hint_down.visible = false;
                        }
                        else {
                            menu_navigation_hint_down.visible = true;
                        }
                        if (menu_selected_item == 0) {
                            menu_navigation_hint_up.visible = false;
                        }
                        else {
                            menu_navigation_hint_up.visible = true;
                        }
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

        case GAME_CONTROL:
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
        case GAME_DECK_ED_MAIN:
            editor.draw(input);
            break;
        }
        tui.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}
