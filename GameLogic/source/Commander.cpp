#include "Commander.hpp"

const std::vector<char> Commander::Player_info::packed()
{
    char packed[sizeof(Player_info)];
    std::memcpy(packed, this, sizeof(Player_info));

    return std::vector<char>((char*)packed, (char*)packed+sizeof(Player_info));
}

Commander::Player_info::Player_info(const std::vector<char>& packed)
{
    if(packed.size() < sizeof(Player_info))
    {
        throw std::runtime_error("Player_info(): char vector is too short to be unpacked");
    }
    std::memcpy(this, packed.data(), sizeof(Player_info));
}

const std::vector<char> Commander::Card_info::packed()
{
    char packed[sizeof(Card_info)];
    std::memcpy(packed, this, sizeof(Card_info));

    return std::vector<char>((char*)packed, (char*)packed+sizeof(Card_info));
}

Commander::Card_info::Card_info(const std::vector<char>& packed)
{
    if(packed.size() < sizeof(Card_info))
    {
        throw std::runtime_error("Player_info(): char vector is too short to be unpacked");
    }
    std::memcpy(this, packed.data(), sizeof(Card_info));
}

const std::vector<char> Commander::Commander_message::packed()
{
    std::vector<char> packed(data.size()*sizeof(int)+sizeof(char));
    packed[0] = type;

    std::memcpy(&packed[1], data.data(), sizeof(int)*data.size());
    return packed;
}

Commander::Commander_message::Commander_message(const std::vector<char>& packed)
{
    if((packed.size()-1)*sizeof(char) % sizeof(int) != 0)
    {
        throw std::runtime_error("Order(): packed message does not add up to full ints");
    }
    data = std::vector<int>((packed.size()-1)*sizeof(char) / sizeof(int));
    type = packed[0];

    std::memcpy(data.data(), &packed.data()[1],(packed.size()-1)*sizeof(int));
}