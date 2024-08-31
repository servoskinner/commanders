#pragma once

#include <optional>
#include <utility>
#include <functional>
#include <queue>

#include "Socket_wrappers.hpp"
#include "Game_master.hpp"
#include "Proxy_commander.hpp"
#include "Misc_functions.hpp"

#define SERVER_PORT 9898
#define CLIENT_UPKEEP_COUNTDOWN 4
#define CLIENT_UPKEEP_DELAY_MS 100

class Server
{
    public:
    Server(unsigned short port = SERVER_PORT) : discovery_socket(port) {};

    void process_messages();
    void process_timers();

    std::vector<Socket_info> get_peers();

    void force_disconnect(Socket_info peer);
    
    std::string description;
    Server_info get_info();

    private:
    struct Client_slot
    {
        Socket_info sock_info;
        std::string name;
        std::optional<std::reference_wrapper<Proxy_commander>> commander;
        std::vector<std::vector<int>> deck_image;
        int upkeep;

        const bool operator==(const Client_slot& other) { return sock_info == other.sock_info;}
        const bool operator==(const Socket_info& other) {return sock_info == other;}
    };

    std::vector<std::optional<Client_slot>> client_slots;
    std::vector<Proxy_commander> commanders;

    UDP_wrapper discovery_socket;
    std::optional<Game_master> master;

    std::vector<char> pack_commander_data(int index);
    void manage_upkeep();
};