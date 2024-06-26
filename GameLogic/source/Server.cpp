#include "Server.hpp"

void Server::process_messages()
{
    int cur_time = std::clock();
    int delta_time = cur_time - prev_time;
    prev_time = cur_time;

    Socket_wrapper::Socket_inbound_message next_msg;
    while((next_msg = serv_socket.receive()).msg.size() > 1) {

        std::optional<Client_info&> sender;
        auto sender_iter = std::find_if(clients.begin(), clients.end(), [&next_msg](Client_info& c_info){return c_info.sock_info == next_msg.sender;});
        if (sender_iter != clients.end()) {
            sender.emplace(*sender_iter);
        }
    
        switch (next_msg.msg[0])
        {
        case MSG_CONTROL:
            switch (next_msg.msg[1])
            {
                case ICTRL_UPKEEP:

                break;

                case ICTRL_DISCOVER_BCAST:

                break;

                case ICTRL_CONNECT_REQ:

                break;
            }
            break;
        
        case MSG_GAMEPLAY:

            break;

        case MSG_CHAT:
            if(next_msg.msg.size() > 2 && sender.has_value() && sender->get().commander.has_value()) {
                std::function<bool(Client_info&, Socket_wrapper::Socket_info&)> compare_addr = [](Client_info& cl, Socket_wrapper::Socket_info& sock){ return cl.sock_info == sock;};
                
                int sender_id = find_index(clients, next_msg.sender, compare_addr);
                std::vector<char> forward_msg = next_msg.msg;
                forward_msg[1] = sender_id;

                if(next_msg.msg[1] == -1) { // broadcast
                    for (const Client_info& client : clients) {
                        serv_socket.send(client.sock_info, forward_msg);
                    }
                }
                else if (next_msg.msg[1] < commanders.size()) { // whisper
                    
                }
                // TODO add confirmatio
            }
            break;
        }
    }

    if(upkeep_broadcast_cooldown <= delta_time * 1000 / CLOCKS_PER_SEC) {
        upkeep_broadcast_cooldown = CLIENT_UPKEEP_DELAY_MS;
    }
    else {
        upkeep_broadcast_cooldown -= delta_time; 
    }
}