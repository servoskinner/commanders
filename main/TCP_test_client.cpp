#include "Socket_wrappers.hpp"
#include <iostream>
#include <string>

int main()
{
    u_short server_port;
    TCP_client socket_self;

    socket_self.handshake_timeout_ms = 1000;

    std::cout << "Enter server port: " << std::flush;
    std::cin >> server_port;

    std::string username;
    std::cout << "enter username: " << std::flush;
    std::cin >> username;

    std::string user_input;
    std::string incoming_message;

    std::cout << "Connecting..." << std::endl;
    if(socket_self.connect_to({server_port, "127.0.0.1"}, 3))
    {
        std::cout << "Success" << std::endl;
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
            if(socket_self.send_msg(std::vector<char>(user_input.begin(), user_input.end()))) {
                std::cout << "message sent" << std::endl;
            }
        }
        // read incoming messages
        while(true)
        {
            std::vector<char> msg = socket_self.get_message().msg;
            if (msg.size() == 0)
            {
                break;
            }
            incoming_message = std::string(msg.begin(), msg.end());
            std::cout << incoming_message << std::endl;
        }
    }
    }
    else {
        std::cout << "failed" << std::endl;
    }
    return 0;
}