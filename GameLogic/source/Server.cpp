#include "Server.hpp"

void Server::process_messages()
{
    // todo add message limit 
    Socket_inbound_message inbound;
    while((inbound = serv_socket.receive()).msg.size() > 1) {

        std::optional<Client_slot&> sender;
        auto sender_iter = std::find_if(client_slots.begin(), client_slots.end(), \
                                        [&inbound](std::optional<Client_slot>& c_info) \
                                        {return c_info.has_value() && c_info->sock_info == inbound.sender;});
        if (sender_iter != client_slots.end()) {
            sender.emplace(sender_iter->value());
        }
    
        switch (inbound.msg[0])
        {
        case MSG_CONTROL:
            switch (inbound.msg[1])
            {
                case ICTRL_UPKEEP:
                {
                    if (sender.has_value())
                    {
                        sender->upkeep = CLIENT_UPKEEP_COUNTDOWN;
                    }
                }
                break;

                case ICTRL_DISCOVER:
                {
                    std::vector<char> discover_msg = {MSG_CONTROL, ICTRL_DISCOVER};
                    std::vector<char> info_packed = get_info().packed();
                    std::copy(info_packed.begin(), info_packed.end(), std::back_inserter(discover_msg));
                    
                    serv_socket.send(inbound.sender, discover_msg);
                }
                break;
            }
            break;
        
        case MSG_GAMEPLAY:
            {
                if(sender.has_value() && sender->commander.has_value())
                {
                    std::vector<char> order_packed = inbound.msg;
                    order_packed.erase(order_packed.begin());
                    sender->commander->get().queue_order({order_packed});
                }
            }
            break;

        case MSG_CHAT:
            if(inbound.msg.size() > 2 && sender.has_value() && sender->get().commander.has_value()) {
                int sender_id = find_index(client_slots, inbound.sender);

                std::vector<char> forward_msg = inbound.msg;
                forward_msg[1] = sender_id;

                if(inbound.msg[1] == -1) { // broadcast
                    for (const std::optional<Client_slot>& slot : client_slots) {
                        if (slot.has_value()) {
                            serv_socket.send(slot->sock_info, forward_msg);
                        }
                    }
                }
                else if (inbound.msg[1] < commanders.size()) { // whisper
                    
                }
                // TODO add confirmation
            }
            break;
        }
    }
}

void Server::process_timers()
{
    int cur_time = std::clock();
    int delta_time = cur_time - prev_tick_time;
    prev_tick_time = cur_time;

    if (upkeep_broadcast_cooldown <= delta_time * 1000 / CLOCKS_PER_SEC) {
        upkeep_broadcast_cooldown = CLIENT_UPKEEP_DELAY_MS;
        manage_upkeep();
    }
    else {
        upkeep_broadcast_cooldown -= delta_time; 
    }
}

void Server::manage_upkeep()
{
    for (std::optional<Client_slot>& slot : client_slots)
    {
        if (slot.has_value())
        {
            slot->upkeep--;
            if (slot->upkeep <= 0)
            {

            }
        }
    }
}

Server_info Server::get_info()
{
    Server_info info;
    info.connected_players = client_slots.size();
    info.description = description;
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