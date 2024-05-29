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

class Socket_wrapper
{
    public:
    Socket_wrapper(u_short port);
    ~Socket_wrapper();

    const std::vector<char> receive();
    const bool send(const std::vector<char>& msg);

    const inline bool is_bound();
    void bind_destination(u_short port, const std::string& address = "127.0.0.1");
    inline void unbind();

    private:
    u_short port;
    int socket_fdesc;
    bool bound;

    sockaddr_in sender_addr, receiver_addr;
};