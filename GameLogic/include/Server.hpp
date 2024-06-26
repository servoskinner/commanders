#pragma once

#include <optional>
#include <utility>
#include <functional>
#include <queue>

#include "Socket_wrapper.hpp"
#include "Game_master.hpp"
#include "Proxy_commander.hpp"
#include <ctime>

#define SERVER_PORT 9898
#define CLIENT_UPKEEP_COUNTDOWN 4
#define CLIENT_UPKEEP_DELAY_MS 100

class Server
{
    public:
    Server(unsigned short port = 9898) : serv_socket(port), prev_time(std::clock()) {};
    void process_messages();
    std::vector<Socket_wrapper::Socket_info> get_peers();
    void force_disconnect(int peer_id);
    std::string description;

    private:
    int player_limit = 2;

    int prev_time;
    int upkeep_broadcast_cooldown;

    struct Client_info
    {
        Socket_wrapper::Socket_info sock_info;
        std::optional<commander_ref> commander;
        std::vector<std::vector<int>> deck_image;
        int upkeep;
    };
    std::vector<Client_info> clients;
    std::vector<Proxy_commander> commanders;

    Socket_wrapper serv_socket;
    std::optional<Game_master> master;

    std::vector<char> pack_commander_data(int index);
    void broadcast_upkeep_requests();
};