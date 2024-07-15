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
                case ICTRL_UPKEEP:
                    // bounce message back
                    client_socket.send(inbound.sender, inbound.msg);
                    break;
                case ICTRL_ACKNOWLEDGE:
                    if (inbound.msg.size() >= 3 && inbound.msg[3] == ICTRL_CONNECT_REQ) 
                    {
                        connection = inbound.sender;
                        connection_upkeep = CONNECTED_UPKEEP_COUNTDOWN;
                    }
                    break;
                case ICTRL_DISCOVER_REPLY:
                    if (inbound.msg.size() >= DISCOVER_REPLY_SIZE) {
                        // Check if this server is still remembered
                        auto entry_it = std::find_if(discovered_servers.begin(), discovered_servers.end(), \
                                                    [&inbound](Server_list_entry& entry){ return entry.socket = inbound.sender;});

                        if (entry_it == discovered_servers.end()) { // New server discovered
                            Server_list_entry new_entry = {inbound.sender, \
                                                        SERVER_UPKEEP_COUNTDOWN, \
                                                        {{inbound.msg.begin()+2, inbound.msg.begin()+DISCOVER_REPLY_SIZE}}};
                            discovered_servers.push_back(new_entry);
                        }
                        else { // Refresh existing
                            entry_it->upkeep = SERVER_UPKEEP_COUNTDOWN;
                        }
                    }
                    break;
                case ICTRL_SERVER_FORCE_DISCONNECT:
                    // check that connection was severed by same server
                    if (inbound.sender == *connection) {
                        connection.reset();
                    }
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