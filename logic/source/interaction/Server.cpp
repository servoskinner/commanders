#include "Server.hpp"

void Server::process_timers()
{

}

void Server::manage_upkeep() 
{
    for (std::optional<Client_slot>& slot : client_slots) {
        if (slot.has_value()) 
        {
            slot->upkeep--;
            if (slot->upkeep <= 0)
            {
                force_disconnect(slot.value().sock_info);
            }
        }
    }
}

Server_info Server::get_info()
{
    Server_info info;
    info.connected_players = client_slots.size();
    if (description.size() <= 128) {
        std::copy(description.begin(), description.end(), info.description);
    }
    else {
        std::copy(description.begin(), description.begin()+128, info.description);
    }
    return info;
}

std::vector<Socket_info> Server::get_peers()
{
    std::vector<Socket_info> peers = {};

    for(const std::optional<Client_slot>& slot : client_slots)
    {
        if (slot.has_value())
        {
            peers.push_back(slot->sock_info);
        }
    }
    return peers;
}