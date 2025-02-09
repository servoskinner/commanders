#include "Commander.hpp"

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