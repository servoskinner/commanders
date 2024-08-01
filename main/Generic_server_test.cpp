#include "Socket_wrappers.hpp"
#include <iostream>
#include <string>
#include <thread>

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

    std::cout << service_socket.get_port() << std::endl;

    std::string incoming_message;

    while(true)
    {
        // Process queues in parallel
        Socket_inbound_message discovery_req = discovery_socket.get_message();
        if (discovery_req.msg.size() != 0) {
            std::cout << "got discovery message" << std::endl;
        }
        if (discovery_req.msg.size() >= 1 && discovery_req.msg[0] == ICTRL_DISCOVER) {
            if (discovery_socket.send_to(discovery_req.sender, {ICTRL_DISCOVER_REPLY, 'f', 'r', 'o', 'g', '\0'})) {
                std::cout << discovery_req.sender.port << std::endl;
            }
        }
        if (discovery_req.msg.size() >= 5 && discovery_req.msg[0] == ICTRL_CONNECTION_REQUEST) {
            if(*(int*)(discovery_req.msg.data()+1) == magic_number) {
                service_socket.allow_ip(discovery_req.sender.address); // Whitelist new ip
                discovery_socket.send_to(discovery_req.sender, {ICTRL_ACK, ICTRL_CONNECTION_REQUEST});
                std::cout << "allowed ip " << discovery_req.sender.addrstr() << std::endl;
            }
            else { // Deny request
                discovery_socket.send_to(discovery_req.sender, {ICTRL_NACK, ICTRL_CONNECTION_REQUEST});
            }
        }

        Socket_info new_connection = service_socket.get_connection_event();
        if (new_connection.address != 0) {
            service_socket.disallow_ip(new_connection.address); // Require to request connection again
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
        std::this_thread::sleep_for(10);
    }
    return 0;
}