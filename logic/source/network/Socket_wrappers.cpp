#include "Socket_wrappers.hpp"

const std::string Socket_info::addrstr()
{
    char bytes[3*4+4] = {};
    inet_ntop(AF_INET, &address, bytes, 3*4+3);
    return std::string(bytes);
}
