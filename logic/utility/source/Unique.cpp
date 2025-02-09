#include "Unique.hpp"
#include <iostream>

std::vector<bool> Unique::available_ids = {};
std::mutex Unique::id_mutex;

Unique::Unique()
{
    id = -1;
    for(int i = 0; i < UNIQUE_ENTITY_LIMIT; i++) {
        if (!Unique::is_taken(i)) {
            Unique::claim_id(i, true);
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
    Unique::claim_id(id, false);
}

const bool Unique::is_taken(int id)
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

void Unique::claim_id(int id, bool val)
{
    std::lock_guard<std::mutex> lock(id_mutex);

    if (id >= UNIQUE_ENTITY_LIMIT) {
        throw std::out_of_range("set_id(): Unique entity ID out of range");
    }
    if (val) {
        if (id >= available_ids.size()) {
            available_ids.resize(id + 1);
        }
        available_ids[id] = val;
    }
    else if (id < available_ids.size()) {
        available_ids[id] = val;
    }
}