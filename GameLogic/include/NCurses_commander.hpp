#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"

#include <ncurses.h>
#include <iostream>

class NCurses_commander : public Commander
{
    public:

    Order get_order() override;
    void process_event(const Event& event) override;
    void apply_updates();

    NCurses_commander();
    ~NCurses_commander();

    private:
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_funky(std::string &buffer, int g_width, int x, int y);
};