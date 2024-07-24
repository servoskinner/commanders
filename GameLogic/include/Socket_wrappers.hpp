#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <optional>
#include <set>

#define SOCKET_BUFFER_SIZE 1024
#define BROADCAST_IP "255.255.255.255"
#define DISCOVER_REPLY_SIZE (2+64+1+8)

#define TCP_RECEIVER_THREAD_DELAY_MS 10
#define TCP_CLIENT_MAX_QUEUE 256
#define TCP_SERVER_MAX_QUEUE  256
#define TCP_SERVER_MAX_REQ 64
#define TCP_SERVER_MAX_CLIENTS 1023

enum inet_message_tags
{
    MSG_CONTROL,
    MSG_GAMEPLAY,
    MSG_CHAT
}; 
enum inet_control_tags
{
    ICTRL_NACKNOWLEDGE,
    ICTRL_ACKNOWLEDGE,  // code of prev op
    ICTRL_UPKEEP,
    ICTRL_DISCOVER,
    ICTRL_DISCOVER_REPLY,
    ICTRL_SERVER_FORCE_DISCONNECT
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
std::vector<char> pack_struct(const Type& object)
{
    char packed[sizeof(Type)];
    std::memcpy(packed, &object, sizeof(Type));

    return std::vector<char>((char*)packed, (char*)packed+sizeof(Type));
}

template <typename Type>
Type unpack_struct(std::vector<char> packed)
{
    Type new_obj;
    if(packed.size() < sizeof(Type))
    {
        throw std::runtime_error("Player_info(): char vector is too short to be unpacked");
    }
    std::memcpy(&new_obj, packed.data(), sizeof(Type));
    return new_obj;
}
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

class UDP_wrapper
{
    public:
    UDP_wrapper(u_short port);
    ~UDP_wrapper() { close(socket_fdesc);}

    const Socket_inbound_message receive();
    const bool send_to(const Socket_info& dest, const std::vector<char>& msg);

    const inline u_short get_port() { return own_port;};

    private:
    u_short own_port = -1;
    int socket_fdesc = -1;
};

class TCP_client
{
    public:
    TCP_client();
    ~TCP_client();

    Socket_inbound_message receive();
    const bool send_msg(const std::vector<char>& msg);
    bool connect_to(const Socket_info& destination);
    bool disconnect();

    private:
    void receive_messages();

    u_short own_port = -1;
    int socket_fdesc = -1;

    std::thread receiver_thread;
    std::atomic<bool> connected;
    std::queue<Socket_inbound_message> message_queue;
    std::optional<Socket_info> connected_to = {};
    std::mutex mutex;
};

class TCP_server
{
    public:
    std::set<in_addr_t> allowed_ips;
    bool accept_all = false;

    TCP_server(u_short port);
    ~TCP_server();

    void poll_events();
    inline const std::vector<Socket_info> get_peers() { return peer_info;}
    bool send_to(int id, std::vector<char> message);
    Socket_inbound_message get_message();
    bool disconnect(int id);
    

    private:
    u_short own_port;
    int socket_fdesc;

    std::vector<pollfd> polled; // first one is always own socket
    // pollfd structs must be stored continuously
    std::vector<Socket_info> peer_info;
    std::queue<Socket_inbound_message> inbound;

    std::thread receiver_thread;
    std::atomic<bool> is_running;
    std::mutex inbound_mutex, peer_list_mutex, socket_mutex;

    void handle_request();
    void handle_client(int id);
};



