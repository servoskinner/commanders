#include "Socket_wrapper.hpp"

Socket_wrapper::Socket_wrapper(u_short port)
{
    socket_fdesc = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fdesc < 0)
    {
        throw std::runtime_error("Socket_wrapper: Failed to create socket");
    }
    destinations = {};
    sender_addr = {}; 

    sender_addr.sin_family = AF_INET;
    sender_addr.sin_addr.s_addr = INADDR_ANY;
    sender_addr.sin_port = htons(port);
    own_port = port;

    // prevent blocking when receiving message
    int flags = fcntl(socket_fdesc, F_GETFL, 0); // get flags
    fcntl(socket_fdesc, F_SETFL, flags | O_NONBLOCK ); // set updated flags

    bool bind_successful = bind(socket_fdesc, (sockaddr*)&sender_addr, sizeof(sender_addr)) >= 0;
    if(!bind_successful)
    {
        throw std::runtime_error("Socket_wrapper: Failed to bind socket");
    }
} 

Socket_wrapper::~Socket_wrapper()
{
    close(socket_fdesc);
}

inline const u_short Socket_wrapper::get_port()
{
    return own_port;
}

inline const bool Socket_wrapper::enable_broadcast()
{
    int flag_value = 1;
    return setsockopt(socket_fdesc, SOL_SOCKET, SO_BROADCAST, &flag_value, sizeof(flag_value)) >= 0;
}

const bool Socket_wrapper::send(const std::vector<char>& msg, int dest_id)
{
    if(destinations.size() >= dest_id)
    {
        throw std::runtime_error("Socket_wrapper::send(): Destination index out of range");
    }
    char buffer[SOCKET_BUFFER_SIZE];
    std::copy(msg.begin(), msg.end(), buffer);
    sockaddr_in receiver_addr = {};

    receiver_addr.sin_port = htons(destinations[dest_id].port);
    receiver_addr.sin_addr.s_addr = destinations[dest_id].address;
    receiver_addr.sin_family = AF_INET;

    int bytes_sent = sendto(socket_fdesc, buffer, msg.size(), 0, \
                            (sockaddr*)&receiver_addr, sizeof(receiver_addr));

    return bytes_sent == msg.size();
}

const bool Socket_wrapper::send_all(const std::vector<char>& msg)
{
    char buffer[SOCKET_BUFFER_SIZE];
    std::copy(msg.begin(), msg.end(), buffer);
    sockaddr_in receiver_addr = {};
    receiver_addr.sin_family = AF_INET;

    bool success = true;
    for(Socket_info dest : destinations)
    {
        receiver_addr.sin_port = htons(dest.port);
        receiver_addr.sin_addr.s_addr = dest.address;

        int bytes_sent = sendto(socket_fdesc, buffer, msg.size(), 0, \
                                (sockaddr*)&receiver_addr, sizeof(receiver_addr));

        success = success && bytes_sent == msg.size();
    }
    return success;
}

const Socket_wrapper::Socket_inbound_message Socket_wrapper::receive()
{
    sockaddr_in inbound_addr;
    socklen_t inbound_addr_len = sizeof(inbound_addr);

    char buffer[SOCKET_BUFFER_SIZE];
    int bytes_received = recvfrom(socket_fdesc, buffer, SOCKET_BUFFER_SIZE, \
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
    
    Socket_inbound_message inbound_message = {{inbound_addr.sin_port, inbound_addr.sin_addr.s_addr}, msg};
    return inbound_message;
}

const std::string Socket_wrapper::Socket_info::addrstr()
{
    char bytes[3*4+4] = {};
    inet_ntop(AF_INET, &address, bytes, 3*4+3);
    return std::string(bytes);
}