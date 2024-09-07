#include "Socket_wrappers.hpp"
#include "Serialization.hpp"

#include <iostream>
#include <string>

int main()
{
    u_short client_port;

    std::cout << "Enter client port: " << std::flush;
    std::cin >> client_port;

    UDP_wrapper discovery_socket(client_port);
    TCP_client client_socket;

    std::string incoming_message; 
    std::vector<Socket_info> discovered_servers = {};

    while(true)
    {
        std::cout << "~" << std::endl;
        bool repeat = false;
        Socket_inbound_message discovery_ans = discovery_socket.get_message();
        if (discovery_ans.msg.size() > 0) {
            repeat = true;
            std::cout << "Got discovery reply" << std::endl;
        }
        if (discovery_ans.msg.size() >= 1 && discovery_ans.msg[0] == ICTRL_DISCOVER_REPLY) {
            std::string info_string = {discovery_ans.msg.begin()+1, discovery_ans.msg.end()};
            std::cout << discovery_ans.sender.port << " Discovered: " << info_string << std::endl;
            discovered_servers.push_back(discovery_ans.sender);
        }
        if (discovery_ans.msg.size() >= 2 && discovery_ans.msg[0] == ICTRL_ACK) {
            std::cout << "Acknowledged " << (u_short)(discovery_ans.sender.port+1) << std::endl;

            if (discovery_ans.msg[1] == ICTRL_CONNECTION_REQUEST) {
                Socket_info request_socket = {(u_short)(discovery_ans.sender.port+1), "127.0.0.1"};
                if (!client_socket.connect_to(request_socket)) {
                    std::cout << "failed to connect" << std::endl;
                }
            }
        }
        if (discovery_ans.msg.size() >= 2 && discovery_ans.msg[0] == ICTRL_NACK) {
            std::cout << "Negative" << std::endl;
        }

        std::vector<char> msg = client_socket.get_message().msg;
        if (msg.size() != 0) 
        {
            repeat = true;
            incoming_message = std::string(msg.begin(), msg.end());
            std::cout << "received message: " << incoming_message << std::endl;
        }
        if (repeat) {
            continue;
        }
        // Every queue empty, go on to read user input
        std::cout << "Pick an option: [D]iscover [C]onnect [S]end [R]efresh" << std::endl;
        std::cout << "Available_servers:___________________________________" << std::endl;
        
        for (int i = 0; i < discovered_servers.size(); i++) {
            std::cout << "[" << i << "] " << discovered_servers[i].addrstr() << ":" << discovered_servers[i].port << std::endl;
        }
        std::cout << std::endl;

        char command;
        std::cin >> command;

        switch (command)
        {
            case 'D':
            case 'd':
            {
                u_short server_port;
                std::cout << "Enter server port: " << std::flush;
                std::cin >> server_port;
                
                if (discovery_socket.send_to({server_port, BROADCAST_IP}, {ICTRL_DISCOVER})) {
                    std::cout << "Sent successfully" << std::endl;
                }
                break;
            }
            case 'C':
            case 'c':
            {
                int id;
                std::cout << "ID of server to connect to: " << std::flush;
                std::cin >> id;

                if (id < 0 || id >= discovered_servers.size()) {
                    std::cout << "Out of range" << std::endl;
                    break;
                }
                int magic_number;
                std::cout << "Enter magic number: " << std::flush;
                std::cin >> magic_number;

                std::vector<char> message = serialize(magic_number);
                message.insert(message.begin(), ICTRL_CONNECTION_REQUEST);

                if (discovery_socket.send_to(discovered_servers[id], message)) {
                    std::cout << "Sent successfully" << std::endl;
                }
                break;
            }
            case 'S':
            case 's':
            {
                if (client_socket.get_connection().has_value()) {
                    std::string message;
                    std::cout << ">" << std::endl;
                    std::cin >> message;

                    if (client_socket.send_msg({message.begin(), message.end()})) {
                        std::cout << "Sent successfully" << std::endl;
                    }
                }
                else {
                    std::cout << "Error: not connected to any server" << std::endl;
                }
            }
        }
    }
    return 0;
}