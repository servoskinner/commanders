#pragma once

#include "Misc_functions.hpp"
#include "Network.hpp"


/// @brief Class used to send and receive byte arrays over network using UDP.
class UDP_peer
{
    public:
    UDP_peer(u_short port);
    ~UDP_peer() { close(socket_fdesc);}
 
    const Socket_inbound_message get_message();
    const bool send_to(const Socket_info& dest, const std::vector<char>& msg);

    const inline u_short get_port() { return own_port;};

    protected:
    u_short own_port = -1;
    int socket_fdesc = -1;
};