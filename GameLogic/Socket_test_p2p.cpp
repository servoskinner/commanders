#include "Socket_wrapper.hpp"
#include <iostream>
#include <string>

int main()
{
    u_short user_port, peer_port;

    std::cout << "Enter user port: " << std::flush;
    std::cin >> user_port;

    Socket_wrapper socket_self(user_port);

    std::cout << "Enter peer port(s): " << std::flush;
    while(true)
    {
        std::cin >> peer_port;
        if(peer_port == 0)
        {
            break;
        }
        socket_self.destinations.push_back({peer_port, "127.0.0.1"});
    }

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
            socket_self.send_all(std::vector<char>(user_input.begin(), user_input.end()));
        }
        // read incoming messages
        while(true)
        {
            std::vector<char> msg = socket_self.receive().msg;
            if (msg.size() == 0)
            {
                break;
            }
            incoming_message = std::string(msg.begin(), msg.end());
            std::cout << incoming_message << std::endl;
        }
    }
    return 0;
}