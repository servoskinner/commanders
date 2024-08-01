#include "Unique.hpp"

std::vector<char> Unique::available_ids = {};
std::mutex Unique::id_mutex;

Unique::Unique()
{
    id = -1;
    for(int i = 0; i < UNIQUE_ENTITY_LIMIT; i++) {
        if (!Unique::check_id(i)) {
            Unique::set_id(i, true);
            id = i;
            break;
        }
    }
    if (id == -1) {
        throw std::runtime_error("Unique(): Unique entity limit exceeded");
    }
}

Unique::~Unique()
{
    Unique::set_id(id, false);
    Unique::resize_id_list();
}

bool Unique::check_id(int id)
{
    std::lock_guard<std::mutex> lock(id_mutex);

    if (id >= UNIQUE_ENTITY_LIMIT) {
        throw std::out_of_range("check_id(): Unique entity ID out of range");
    }
    int byte = id/8;
    int pos  = id%8;

    if (byte >= Unique::available_ids.size()) {
        return false;
    }
    return (Unique::available_ids[byte] & (1 << pos)) != 0;
}

void Unique::set_id(int id, bool val)
{
    std::lock_guard<std::mutex> lock(id_mutex);

    if (id >= UNIQUE_ENTITY_LIMIT) {
        throw std::out_of_range("set_id(): Unique entity ID out of range");
    }
    int byte = id/8;
    int pos  = id%8;

    // Extend vector to
    while (byte >= Unique::available_ids.size()) {
        Unique::available_ids.push_back(0b00000000);
    }
    if (val) { // set bit
        Unique::available_ids[byte] |= 1 << pos;
    }
    else { // clear bit
        Unique::available_ids[byte] &= ~(1 << pos);
    }
}

void Unique::resize_id_list()
{
    std::lock_guard<std::mutex> lock(id_mutex);
    
    while(Unique::available_ids.size() > 0 && Unique::available_ids.back() == 0b00000000) {
        Unique::available_ids.pop_back();
    }
}