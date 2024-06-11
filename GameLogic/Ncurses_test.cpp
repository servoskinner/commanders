#include "NCurses_commander.hpp"

int main()
{
    NCurses_commander commander;
    while (true)
    {
        commander.apply_updates();
    }
}