#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>

#include "Serialization.hpp"

#define DEFAULT_STORAGE_FILE "userdata"

class Storage_manager
{
    private:
    std::string filename;
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

    bool write_entry_at(unsigned at_pos, Storage_entry& entry);
    std::optional<unsigned> next_entry(unsigned prev_pos);
    std::optional<Serialized> read_entry_at(unsigned at_pos);
    std::optional<unsigned int> locate_entry(std::string locator);
    std::optional<unsigned> locate_entry_indexed(int index);
    
    public:
    ~Storage_manager();
    Storage_manager(std::string storage_file = DEFAULT_STORAGE_FILE);
    static Storage_manager& get_default();
    
    std::optional<Serialized> get_item(std::string locator);
    bool put_item(std::string locator, std::vector<char> value);
    bool delete_item(std::string locator);
};