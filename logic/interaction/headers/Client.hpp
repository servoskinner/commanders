#pragma once

#include "Misc_functions.hpp"
#include "TCP_server.hpp"
#include "UDP_peer.hpp"
#include "Commander.hpp"
#include "Game_master.hpp"

#include <queue>
#include <vector>
#include <optional>
#include <utility>

#define CLIENT_PORT 8989

#define SERVER_UPKEEP_COUNTDOWN 4
#define SERVER_UPKEEP_DELAY_MS 100

#define CONNECTED_UPKEEP_COUNTDOWN 4
#define CONNECTED_UPKEEP_DELAY_MS 100

class Client
{
    public:
    Client(unsigned short port = CLIENT_PORT) : client_socket(port) {}

    struct Server_list_entry 
    {
        Socket_info socket;
        int upkeep;
        Server_info info;

        Server_list_entry() = default;
    };

    virtual void process_msgs(int limit = -1);
    virtual void process_timers();
    
    void bcast_discovery_msg(unsigned short server_port = 9898);
    const std::vector<Server_list_entry> get_discovered_connections() { return discovered_servers; }
    void request_connection(Socket_info server);

    std::vector<Game_master::Card_info> get_active_cards();
    std::vector<Game_master::Player_info> get_players();
    std::vector<std::vector<std::optional<Game_master::Card_info>>> get_field() { return field;}

    const int events_size() { return events.size(); }
    const Game_master::Event events_back() { return events.back(); }
    bool events_pop_back();
    
    private:
    void handle_control_message(Socket_inbound_message msg);
    void manage_upkeep();

    UDP_peer client_socket;
    std::optional<Socket_info> connection;
    int connection_upkeep;
    std::vector<Server_list_entry> discovered_servers;

    std::queue<Game_master::Event> events;
    std::vector<std::vector<std::optional<Game_master::Card_info>>> field;
    std::vector<Game_master::Player_info> players;

    int grid_width, grid_height;
};