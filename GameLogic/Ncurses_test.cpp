#include "NCurses_commander.hpp"

int main()
{
    NCurses_commander commander;
    commander.grid_height = 6;
    commander.grid_width = 8;

    bool is_running = true;
    while (is_running)
    {
        commander.apply_updates();
        is_running = commander.is_on();
    }
}