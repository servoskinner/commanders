#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <ncurses.h>
#include <queue>

#include "game_logic.hpp"

class Player_controller;
class CLI_control;
class Server_interf_control;

typedef std::reference_wrapper<Player_controller> pctrl_ref;

class Player_controller // Abstract player interface class (slave of GameMaster)
{
    public:
    int id;

    std::string name;
    
    inline int getId() { return id;}

    std::vector<Card_info> active_cards;
    std::vector<Card_info> hand;
    std::vector<Player_info> players;

    virtual std::vector<int> get_action() = 0;
    virtual void apply_updates() = 0;
    virtual void process_event(std::vector<int> event) = 0;
    //virtual std::vector<int> chooseTile() = 0;
    //virtual std::vector<int> chooseContract() = 0;
    //virtual std::vector<int> choosePlayer() = 0;
};

class CLI_control : public Player_controller
{
    public:
    master_ref master;

    std::vector<int> get_action() override;
    void process_event(std::vector<int> event) override;
    void apply_updates() override;

    CLI_control() = default;

    private:
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_funky(std::string &buffer, int g_width, int x, int y);
};

class ncurses_control : public Player_controller
{
    public:

    std::vector<int> get_action() override;
    void process_event(std::vector<int> event) override;
    void apply_updates() override;

    ncurses_control();
    ~ncurses_control();

    private:
    void render_UI();

    void highlight_tile_bold(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_subtle(std::string &buffer, int g_width, int x, int y);
    void highlight_tile_funky(std::string &buffer, int g_width, int x, int y);
};

class Server_interf_control : public Player_controller
{
    public:
    // Server-side interface 
    void pass_inbound_message(std::vector<char> msg);
    std::vector<char> get_outbound_message();
    // Master-side interface
    void process_event(std::vector<int> event) override;
    std::vector<int> get_action() override;

    void apply_updates() override;

    Server_interf_control() = default;

    private:
    std::queue<std::vector<char>> inbound_msgs, outbound_msgs;
};