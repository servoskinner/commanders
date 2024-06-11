#include "NCurses_commander.hpp"

int main()
{
    NCurses_commander commander;
    commander.grid_height = 6;
    commander.grid_width = 8;

    while (true)
    {
        commander.apply_updates();
    }
}