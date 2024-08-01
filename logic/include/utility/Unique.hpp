#pragma once

#include <vector>
#include <mutex>
#include <stdexcept>

#define UNIQUE_ENTITY_LIMIT 8192

class Unique 
{
    private:
    int id;
    static std::vector<char> available_ids;
    static std::mutex id_mutex;

    static void set_id(int id, bool val);
    static bool check_id(int id);
    static void resize_id_list();
    
    public:
    Unique();
    ~Unique();

    bool operator==(const Unique& other) { return other.id == id;}
    bool operator!=(const Unique& other) { return other.id != id;}

    bool operator==(const int& other) { return other == id;}
    bool operator!=(const int& other) { return other != id;}

    int get_id() { return id;}
};