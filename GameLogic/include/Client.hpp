#pragma once

#include "Socket_wrapper.hpp"
#include "Commander.hpp"

#include <queue>
#include <vector>
#include <optional>
#include <utility>

#define CLIENT_PORT 8989
#define SV_LIST_TICKS_TO_FORGET 3
#define TICK_RATE 100

class Client
{
    public:
    Client();

    virtual void process_msgs(int quantity = -1);
    void bcast_discovery_msg(int server_port = 9898);
    const std::vector<std::pair<Socket_wrapper::Socket_info, int>> get_discovered_connections() { return discovered_connections; }
    void request_connection(Socket_wrapper::Socket_info server);

    std::vector<Commander::Card_info> get_active_cards();
    std::vector<Commander::Player_info> get_players();
    std::vector<std::vector<std::optional<Commander::Card_info>>> get_field() { return field;}

    const int events_size() { return events.size(); }
    const Commander::Event events_back() { return events.back(); }
    bool events_pop_back();
    
    private:
    void handle_control_message(Socket_wrapper::Socket_inbound_message msg);

    Socket_wrapper client_socket;
    std::optional<Socket_wrapper::Socket_info> connection;
    std::vector<std::pair<Socket_wrapper::Socket_info, int>> discovered_connections;

    std::queue<Commander::Event> events;
    std::vector<std::vector<std::optional<Commander::Card_info>>> field;
    std::vector<Commander::Player_info> players;

    int grid_width, grid_height;
};