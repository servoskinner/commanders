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

struct Socket_info
{
    u_short port;
    in_addr_t address;

    Socket_info(u_short n_port, const char *addrstr = "127.0.0.1") : port(n_port), address(inet_addr(addrstr)) {}
    Socket_info(u_short n_port, std::string addrstr = "127.0.0.1") : port(n_port), address(inet_addr(addrstr.c_str())) {}
    Socket_info(u_short n_port, in_addr_t addr) : port(n_port), address(addr) {}
};

struct Socket_inbound_message
{
    Socket_info sender;
    std::vector<char> msg;
};

class Socket_wrapper
{
    public:
    Socket_wrapper(u_short port);
    ~Socket_wrapper();

    const Socket_inbound_message receive();
    const bool send(const std::vector<char>& msg, int dest_id = 0);
    const bool send_all(const std::vector<char>& msg);
    inline const bool enable_broadcast();

    const inline u_short get_port();

    std::vector<Socket_info> destinations;

    private:
    u_short own_port;
    int socket_fdesc;

    sockaddr_in sender_addr;
};