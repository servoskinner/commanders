#include <iostream>
#include <string>
#include <vector>

#include "game_logic.hpp"
#include "player_controller.hpp"

#define TILE_WIDTH_CHARS	6
#define TILE_HEIGHT_CHARS	6

ncurses_control::ncurses_control()
{

}

ncurses_control::ncurses_control()
{

}

void ncurses_control::render_UI()
{
   // GRID _________________________________________
    int width, height;

    height = master->get_grid_height();
    width = master->get_grid_width();
};

Order CLI_control::get_action() 
{
   
}

void CLI_control::handle_controller_event(int errorCode) // display some kind of message (not overlaid)
{

}

void CLI_control::apply_updates() // update buffer (don't re-render it)
{

}

void CLI_control::highlight_tile_bold(std::string &buffer, int g_width, int x, int y)
{

}

void CLI_control::highlight_tile_subtle(std::string &buffer, int g_width, int x, int y)
{

}

void CLI_control::highlight_tile_funky(std::string &buffer, int g_width, int x, int y)
{

}

/*

CONTROLS TODO
tab - toggle hand
shift - see additional game data
wasd - move cursor 

rclick deployed cards - see descriptions

scroll card descriptions

*/