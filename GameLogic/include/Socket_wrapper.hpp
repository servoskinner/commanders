#pragma once

#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

#define SOCKET_BUFFER_SIZE 1024
#define BROADCAST_IP "255.255.255.255"
#define DISCOVER_REPLY_SIZE (2+64+1+8)

enum inet_message_tags
{
    MSG_CONTROL,
    MSG_GAMEPLAY,
    MSG_CHAT
}; 
enum inet_control_tags
{
    ICTRL_NACKNOWLEDGE,
    ICTRL_ACKNOWLEDGE,            // code of prev op
    ICTRL_UPKEEP
};

struct Server_info
{
    std::string description;
    char connected_players;
    char flags[8];

    Server_info(std::vector<char> packed);
    Server_info() = default;
    std::vector<char> packed();
};

template <typename Type>
std::vector<char> socket_c_pack(const Type& object)
{
    char packed[sizeof(Type)];
    std::memcpy(packed, this, sizeof(Type));

    return std::vector<char>((char*)packed, (char*)packed+sizeof(Type));
}

template <typename Type>
Type socket_c_unpack(std::vector<char> packed)
{
    Type new_obj = {};
    if(packed.size() < sizeof(Type))
    {
        throw std::runtime_error("Player_info(): char vector is too short to be unpacked");
    }
    std::memcpy(&new_obj, packed.data(), sizeof(Player_info));
}

class Socket_wrapper
{
    public:
    Socket_wrapper(u_short port);
    ~Socket_wrapper();

    static std::vector<char> int_to_chars(int source);

    struct Socket_info
    {
        u_short port = 0;
        in_addr_t address = 0;

        Socket_info(u_short n_port, const char *addrstr = "127.0.0.1") : port(n_port), address(inet_addr(addrstr)) {}
        Socket_info(u_short n_port, std::string addrstr = "127.0.0.1") : port(n_port), address(inet_addr(addrstr.c_str())) {}
        Socket_info(u_short n_port, in_addr_t addr) : port(n_port), address(addr) {}
        Socket_info() : port(0), address(0) {}

        const bool operator==(const Socket_info& other) {return port == other.port && address == other.address; }
        const std::string addrstr();
    };
    struct Socket_inbound_message
    {
        Socket_info sender = {};
        std::vector<char> msg = {};
    };

    const Socket_inbound_message receive();
    const bool send_id(const std::vector<char>& msg, int dest_id = 0);
    const bool send(const Socket_info& to, const std::vector<char>& msg);
    const bool send_all(const std::vector<char>& msg);
    inline const bool enable_broadcast();

    const inline u_short get_port();

    std::vector<Socket_info> destinations;

    private:
    u_short own_port;
    int socket_fdesc;

    sockaddr_in sender_addr;
};
