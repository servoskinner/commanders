#pragma once

#include "Socket_wrapper.hpp"
#include "Commander.hpp"

#include <queue>
#include <vector>
#include <optional>
#include <utility>

#define CLIENT_PORT 8989
#define SERVER_UPKEEP_COUNTDOWN 4
#define SERVER_UPKEEP_DELAY_MS 100

class Client
{
    public:
    Client(unsigned short port = CLIENT_PORT) : client_socket(port) {}

    struct Server_list_entry 
    {
        Socket_wrapper::Socket_info socket;
        int upkeep;
        Server_info info;

        Server_list_entry() = default;
    };

    virtual void process_msgs(int limit = -1);
    void bcast_discovery_msg(unsigned short server_port = 9898);
    const std::vector<Server_list_entry> get_discovered_connections() { return discovered_connections; }
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
    std::vector<Server_list_entry> discovered_connections;

    std::queue<Commander::Event> events;
    std::vector<std::vector<std::optional<Commander::Card_info>>> field;
    std::vector<Commander::Player_info> players;

    int grid_width, grid_height;
};