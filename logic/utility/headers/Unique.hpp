#pragma once

#include <vector>
#include <mutex>
#include <stdexcept>

#define UNIQUE_ENTITY_LIMIT 4096

/// @brief A unique 32-bit unsigned ID for comparing objects.
class Unique 
{
    private:
    unsigned int id;

    static std::vector<bool> available_ids;
    static std::mutex id_mutex;

    /// @brief Marks given id as "in use" or "not in use"
    static void claim_id(int id, bool val);

    // Non-copyable
    Unique& operator= (const Unique& other) = delete;
    Unique(const Unique& other) = delete;
    
    public:
    const static bool is_taken(int id);

    Unique();
    ~Unique();

    bool operator==(const Unique& other) const 
    { return other.id == id;}
    bool operator!=(const Unique& other) const 
    { return other.id != id;}

    bool operator==(const unsigned int& other) const 
    { return other == id;}
    bool operator!=(const unsigned int& other) const 
    { return other != id;}

    operator unsigned int() const { return id; }
};