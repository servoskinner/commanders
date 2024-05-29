#pragma once

#include <vector>

#include "networking.hpp"
#include "game_logic.hpp"
#include "player_controller.hpp"
#include "roster.hpp"

#define DEFAULT_PORT 7878
#define MAX_CONNECTIONS 2

class Server
{
    public:
    Server(u_short port = DEFAULT_PORT, u_short max_connections);
    void process_messages();
    int get_client_count();

    std::string name;
    enum inbound_message_type {CONTROL, GAMEPLAY, CHAT};

    private:
    void register_player(std::string name, std::vector<char> deck);
    void send_discovery_message(Socket_info requester);

    std::vector<Server_interf_control> controllers;
    Game_master master;
    Socket_wrapper serv_socket;
    Roster roster;
};

