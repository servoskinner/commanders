#pragma once

#include <vector>
#include <mutex>
#include <stdexcept>

#define UNIQUE_ENTITY_LIMIT 8192

class Unique 
{
    private:
    unsigned int id;
    static std::vector<char> available_ids;
    static std::mutex id_mutex;

    static void claim_id(int id, bool val);
    static void resize_id_list();

    Unique& operator= (const Unique& other) = delete;
    Unique(const Unique& other) = delete;
    
    public:
    const static bool is_taken(int id);

    Unique();
    ~Unique();

    const bool operator==(const Unique& other) { return other.id == id;}
    const bool operator!=(const Unique& other) { return other.id != id;}

    const bool operator==(const int& other) { return other == id;}
    const bool operator!=(const int& other) { return other != id;}

    const unsigned int get_id() { return id;}
};