#pragma once

#include <vector>
#include <mutex>
#include <stdexcept>

#define UNIQUE_ENTITY_LIMIT 8192

/// @brief A unique unsigned int tag.
class Unique 
{
    private:
    unsigned int id;
    static std::vector<char> available_ids;
    static std::mutex id_mutex;

    /// @brief Marks given id as "in use" or "not in use"
    static void claim_id(int id, bool val);
    /// @brief Trims zero bytes at the end of free ids list.
    static void resize_id_list();

    Unique& operator= (const Unique& other) = delete;
    Unique(const Unique& other) = delete;
    
    public:
    const static bool is_taken(int id);

    Unique();
    ~Unique();

    const bool operator==(const Unique& other) { return other.id == id;}
    const bool operator!=(const Unique& other) { return other.id != id;}

    const bool operator==(const unsigned int& other) { return other == id;}
    const bool operator!=(const unsigned int& other) { return other != id;}

    const unsigned int get_id() { return id;}
};