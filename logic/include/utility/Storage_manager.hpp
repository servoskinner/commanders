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
    
    std::optional<Serialized> get_serialized(std::string locator);
    bool put_serialized(std::string locator, std::vector<char> value);
    bool del(std::string locator);

    template <typename Type>
    inline std::optional<Type> get(std::string locator)
    {
        std::optional<Serialized> serialized = get_serialized(locator);
        if (!serialized.has_value()) {
            return {};
        }
        return {deserialize<Type>(serialized.value())};
    }

    template <typename Type>
    inline std::optional<std::vector<Type>> get_vector(std::string locator)
    {
        std::optional<Serialized> serialized = get_serialized(locator);
        if (!serialized.has_value()) {
            return {};
        }
        return {deserialize_vector<Type>(serialized.value())};
    }

    template <typename Keytype, typename Valtype>
    inline std::optional<std::unordered_map<Keytype, Valtype>> get_map(std::string locator)
    {
        std::optional<Serialized> serialized = get_serialized(locator);
        if (!serialized.has_value()) {
            return {};
        }
        return {deserialize_map<Keytype, Valtype>(serialized.value())};
    }

    template <typename Type>
    inline bool put(std::string locator, Type value)
    {
        Serialized serialized = serialize(value);
        return put_serialized(locator, serialized);
    }

    template <typename Type>
    inline bool put_vector(std::string locator, std::vector<Type> value)
    {
        Serialized serialized = serialize_vector(value);
        return put_serialized(locator, serialized);
    }

    template <typename Keytype, typename Valtype>
    inline bool get_map(std::string locator, std::unordered_map<Keytype, Valtype> value)
    {
        Serialized serialized = serialize_map(value);
        return put_serialized(locator, serialized);
    }
};