#include "networking.hpp"

Socket_wrapper::Socket_wrapper(u_short n_port)
{
    socket_fdesc = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fdesc < 0)
    {
        throw std::runtime_error("Socket_wrapper: Failed to create socket");
    }
    receiver_addr = {};
    sender_addr = {}; 

    sender_addr.sin_family = AF_INET;
    sender_addr.sin_addr.s_addr = INADDR_ANY;
    sender_addr.sin_port = htons(n_port);
    port = n_port;

    receiver_addr.sin_family = AF_INET;
    // prevent blocking when receiving message
    int flags = fcntl(socket_fdesc, F_GETFL, 0); // get flags
    fcntl(socket_fdesc, F_SETFL, flags | O_NONBLOCK); // set updated flags

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

const inline bool Socket_wrapper::is_bound()
{
    return bound;
}

void Socket_wrapper::bind_destination(u_short d_port, const std::string& d_address)
{
    receiver_addr.sin_port = htons(d_port);
    receiver_addr.sin_addr.s_addr = inet_addr(d_address.c_str());
    bound = true;
}

void Socket_wrapper::unbind()
{
    receiver_addr = {};
    bound = false;
}

const bool Socket_wrapper::send(const std::vector<char>& msg)
{
    if (!bound)
    {
        return false;
    }
    char buffer[SOCKET_BUFFER_SIZE];
    std::copy(msg.begin(), msg.end(), buffer);

    int bytes_sent = sendto(socket_fdesc, buffer, msg.size(), 0, \
                            (sockaddr*)&receiver_addr, sizeof(receiver_addr));
    return bytes_sent == msg.size();
}

const std::vector<char> Socket_wrapper::receive()
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
        std::clog << "WARNING: Socket_wrapper (port " << port << ") failed to receive message: errno #" << errno << std::endl;
    } 
    return msg;
}