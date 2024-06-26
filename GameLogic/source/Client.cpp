#include <iostream>
#include <string>
#include <vector>

#include "Client.hpp"
#include "Description_generator.hpp"
#include "Misc_functions.hpp"

void Client::bcast_discovery_msg(unsigned short server_port)
{

}

void Client::process_msgs(int limit)
{
    int msgs_processed = 0;
    while(msgs_processed < limit || limit == -1)
    {
        Socket_wrapper::Socket_inbound_message inbound = client_socket.receive();
        if (inbound.msg.size() < 2) {
            break;
        }

        switch (inbound.msg[0])
        {
        case MSG_GAMEPLAY:
                // cut the message type tag
                events.push(Commander::Event({inbound.msg.begin()+1, inbound.msg.end()}));
            break;
        case MSG_CONTROL:
                switch (inbound.msg[1])
                {
                case ICTRL_ACKNOWLEDGE:
                    if (inbound.msg.size() >= 3 && inbound.msg[3] == ICTRL_CONNECT_REQ) {
                        
                    }
                    break;
                case ICTRL_DISCOVER_REPLY:
                    if (inbound.msg.size() >= DISCOVER_REPLY_SIZE) {
                        Server_list_entry new_entry = {inbound.sender, \
                                                       SERVER_UPKEEP_COUNTDOWN, \
                                                       {{inbound.msg.begin()+2, inbound.msg.begin()+DISCOVER_REPLY_SIZE}}};
                        discovered_connections.push_back(new_entry);
                    }
                    break;
                case ICTRL_SERVER_FORCE_DISCONNECT:

                    break;
                }
            break;
        case MSG_CHAT:

            break;
        }
    }
}

void Client::bcast_discovery_msg(unsigned short server_port)
{
    Socket_wrapper::Socket_info broadcast = {server_port, BROADCAST_IP};
    client_socket.enable_broadcast();

    client_socket.destinations.push_back(broadcast);
    std::vector<char> message = {MSG_CONTROL, ICTRL_DISCOVER_BCAST};
}