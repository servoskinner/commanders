#include "Socket_wrappers.hpp"

const std::string Socket_info::addrstr()
{
    char bytes[3*4+4] = {};
    inet_ntop(AF_INET, &address, bytes, 3*4+3);
    return std::string(bytes);
}

Server_info::Server_info(std::vector<char> packed)
{
    if(packed.size() < sizeof(char)*(1+8))
    {
        throw std::runtime_error("Server_info(): char vector is too short to be unpacked");
    }

    connected_players = packed[0];
    std::memcpy(flags, packed.data()+sizeof(char), sizeof(char)*8);
    description = {packed.begin()+1+8, packed.end()};
}

std::vector<char> Server_info::packed()
{
    std::vector<char> pack = std::vector<char>(1+8+description.size()+1);

    pack[0] = connected_players;
    std::copy(flags, flags+8, pack.begin()+1);
    std::copy(description.begin(), description.end(), pack.begin()+1+8);
    *pack.rbegin() = 0;

    return pack;
}