#include "Network.hpp"
#include "UDP_wrapper.hpp"

UDP_peer::UDP_peer(u_short port)
{
    socket_fdesc = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fdesc < 0)
    {
        throw std::runtime_error("Socket_wrapper_UDP: Failed to create socket");
    }
    sockaddr_in own_addr = {}; 

    own_addr.sin_family = AF_INET;
    own_addr.sin_addr.s_addr = INADDR_ANY;
    own_addr.sin_port = htons(port);
    own_port = port;

    // Enable non-blocking mode
    int flags = fcntl(socket_fdesc, F_GETFL, 0); 
    if (fcntl(socket_fdesc, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        close(socket_fdesc);
        throw std::runtime_error("Socket_wrapper_UDP: Failed to enable non-blocking mode");
    }
    if(bind(socket_fdesc, (sockaddr*)&own_addr, sizeof(own_addr)) < 0)
    {
        close(socket_fdesc);
        throw std::runtime_error("Socket_wrapper_UDP: Failed to bind socket");
    }
    int flag_value = 1;
    if(setsockopt(socket_fdesc, SOL_SOCKET, SO_BROADCAST, &flag_value, sizeof(flag_value)) < 0)
    {
        close(socket_fdesc);
        throw std::runtime_error("Socket_wrapper_UDP: Failed to enable broadcast");
    }
} 

const bool UDP_peer::send_to(const Socket_info& dest, const std::vector<char>& msg)
{
    char buffer[SOCKET_BUFFER_SIZE];
    std::copy(msg.begin(), msg.end(), buffer);
    sockaddr_in receiver_addr = {};

    receiver_addr.sin_port = htons(dest.port);
    receiver_addr.sin_addr.s_addr = dest.address;
    receiver_addr.sin_family = AF_INET;

    int bytes_sent = sendto(socket_fdesc, buffer, msg.size(), 0, \
                            (sockaddr*)&receiver_addr, sizeof(receiver_addr));

    return bytes_sent == msg.size();
}

const Socket_inbound_message UDP_peer::get_message()
{
    sockaddr_in inbound_addr;
    socklen_t inbound_addr_len = sizeof(inbound_addr);

    char buffer[SOCKET_BUFFER_SIZE];
    int bytes_received = recvfrom(socket_fdesc, buffer, sizeof(buffer), \
                                  0, (sockaddr*)&inbound_addr, &inbound_addr_len);
    std::vector<char> msg = {};
    // check if failure to receive the message was due to 
    if (bytes_received >= 0)
    {
        msg = std::vector<char>(buffer, buffer + bytes_received);
    }
    else if (!(errno == EWOULDBLOCK || errno == EAGAIN))
    {
        std::clog << "WARNING: Socket_wrapper (port " << own_port << ") failed to receive message: errno #" << errno << std::endl;
    }
    
    Socket_inbound_message inbound_message = {{ntohs(inbound_addr.sin_port), inbound_addr.sin_addr.s_addr}, msg};
    return inbound_message;
}