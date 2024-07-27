#include "Socket_wrappers.hpp"
#include <iostream>
#include <string>

int main()
{
    u_short server_port;
    int magic_number;

    std::cout << "Enter server port: " << std::flush;
    std::cin >> server_port;
    std::cout << "Enter magic number: " << std::flush; 
    std::cin >> magic_number;

    UDP_wrapper discovery_socket(server_port);
    TCP_server service_socket(server_port+1);

    std::string incoming_message;

    while(true)
    {
        Socket_inbound_message discovery_req = discovery_socket.receive();
        if (discovery_req.msg.size() >= 1 && discovery_req.msg[0] == ICTRL_DISCOVER) {
            discovery_socket.send_to(discovery_req.sender, {ICTRL_DISCOVER_REPLY, 'f', 'r', 'o', 'g', '\0'});
        }
        if (discovery_req.msg.size() >= 5 && discovery_req.msg[0] == ICTRL_CONNECTION_REQUEST) {
            if(*(int*)(discovery_req.msg.data()+1) == magic_number) {
                service_socket.allow_ip(discovery_req.sender.address);
                discovery_socket.send_to(discovery_req.sender, {ICTRL_ACK, ICTRL_CONNECTION_REQUEST});
            }
            else {
                discovery_socket.send_to(discovery_req.sender, {ICTRL_NACK, ICTRL_CONNECTION_REQUEST});
            }
        }

        

        std::vector<char> msg = service_socket.get_message().msg;
        if (msg.size() == 0) {
            continue;
        }

        incoming_message = std::string(msg.begin(), msg.end());
        std::cout << "received message" << std::endl;

        for (int i = 0; i < service_socket.get_peers().size(); i++) {
            if (service_socket.send_to(i, {incoming_message.begin(), incoming_message.end()})) {
                std::cout << "forwarded successfully to" << i << std::endl;
            }
        }
    }
    return 0;
}