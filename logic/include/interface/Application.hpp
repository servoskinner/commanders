#include "TUI.hpp"
#include "Event_timer.hpp"
#include "Focus.hpp"

#include <string>

class Application {
    public:
    Application& get();
    void run_graphics();
    void run_gameplay();

    private:
    Focus::Focus_space global_fspace;

    Focus focus_intro = {global_fspace};
    Focus focus_callsign_prompt = {global_fspace};
    Focus focus_main_menu = {global_fspace};
    Focus focus_game = {global_fspace};

    Event_timer animation_timer;

    int main_menu_selection;

    void draw_callsign_prompt(unsigned input);
    void draw_intro();
    void draw_main_menu();

    Application(Application& other) = delete;
    Application& operator= (Application& other) = delete;
};
