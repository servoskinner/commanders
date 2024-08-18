#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"

#include <iostream>

class CLI_commander : public Commander
{
    public:

    Order get_order() override;
    void process_order_feedback(int code);
    void process_event(Event event) {}
    void apply_updates();

    CLI_commander();

    private:
    Description_generator& desc_gen;
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_funky(std::string &buffer, int g_width, int x, int y);
};