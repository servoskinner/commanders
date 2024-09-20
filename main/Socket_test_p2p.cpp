#include "Network.hpp"
#include "UDP_peer.hpp"

#include <iostream>
#include <string>

int main()
{
    u_short user_port, peer_port;

    std::cout << "Enter user port: " << std::flush;
    std::cin >> user_port;

    UDP_peer socket_self(user_port);

    std::cout << "Enter peer port: " << std::flush;
    std::cin >> peer_port;

    std::string username;
    std::cout << "enter username: " << std::flush;
    std::cin >> username;

    std::string user_input;
    std::string incoming_message;
    while(true)
    {
        std::cout << ">" << std::flush;
        std::getline(std::cin, user_input);
        // loop exit
        if(user_input == "!q")
        {
            break;
        }
        // send nonzero messages
        if(user_input.size() > 0)
        {
            user_input = "<" + username + ">: " + user_input;
            socket_self.send_to({peer_port, BROADCAST_IP},std::vector<char>(user_input.begin(), user_input.end()));
        }
        // read incoming messages
        while(true)
        {
            Socket_inbound_message inbound = socket_self.get_message();
            if (inbound.msg.size() == 0)
            {
                break;
            }
            incoming_message = std::string(inbound.msg.begin(), inbound.msg.end());
            std::cout << "[" << inbound.sender.port << "]" << incoming_message << std::endl;
        }
    }
    return 0;
}