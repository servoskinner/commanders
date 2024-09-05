#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include "Serialization.hpp"

#define DEFAULT_STORAGE_FILE "userdata"

class Storage_manager
{
    private:
    Storage_manager& operator=(Storage_manager&) = delete;
    Storage_manager(Storage_manager&) = delete;
    int n_entries;
    std::fstream file;

    struct Storage_entry
    {   
        // uint16 locator_size
        // int data_size
        std::string locator;
        std::vector<char> data;
    };

    bool put_entry(unsigned at_pos, Storage_entry& entry);
    std::optional<Serialized> read_entry(unsigned at_pos);
    std::optional<unsigned int> locate_entry(std::string locator);
    std::optional<unsigned> locate_entry_indexed(int index);
    
    public:
    ~Storage_manager() { file.close();}
    Storage_manager(std::string storage_file = DEFAULT_STORAGE_FILE);
    static Storage_manager& get_default();
    
    std::optional<Serialized> get_item(std::string locator);
    bool put_item(std::string locator, std::vector<char> value);
    
    bool delete_entry(std::string locator) { return false;};
};