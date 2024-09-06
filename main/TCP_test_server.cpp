#include "Socket_wrappers.hpp"
#include <iostream>
#include <string>

int main()
{
    u_short server_port;

    std::cout << "Enter server port: " << std::flush;
    std::cin >> server_port;
    TCP_server socket_self(server_port);
    socket_self.accept_all = true;
    socket_self.do_handshake = true;

    std::string incoming_message;
    while(true)
    {
        std::optional<Socket_info> new_event = socket_self.get_connection_event();
        while (new_event.has_value()) {
            std::cout << new_event->addrstr() + ":" + std::to_string(new_event->port) + " connected;" << std::endl;
            new_event = socket_self.get_connection_event();
        }
        std::vector<char> msg = socket_self.get_message().msg;
        if (msg.size() == 0) {
            continue;
        }
        incoming_message = std::string(msg.begin(), msg.end());
        std::cout << "received message" << std::endl;

        for (int i = 0; i < socket_self.get_peers().size(); i++) {
            if (socket_self.send_to(i, {incoming_message.begin(), incoming_message.end()})) {
                std::cout << "forwarded successfully to" << i << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}