#pragma once

#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>

#include "Serialization.hpp"

#define DEFAULT_STORAGE_FILE "userdata"

/// @brief Manages long-term variable storage by writing
///        and reading key-value pairs from a file.
class Archivist
{
    private:
    std::string filename;
    Archivist& operator=(Archivist&) = delete;
    Archivist(Archivist&) = delete;
    int n_entries;
    std::fstream file;

    /// @brief Single locator-data pair from a file.
    struct Storage_entry
    {   
        // uint16 locator_size
        // int data_size
        std::string locator;
        std::vector<char> data;
    };
    /// @brief Writes given entry over file contents, starting from given position.
    /// @warning Does NOT relocate contents if new entry overlaps with the next one.
    /// @return Whether no files occurred when interacting with file.
    bool write_entry_at(unsigned at_pos, Storage_entry& entry);

    /// @brief Seeks entry that comes after entry starting at prev_pos.
    /// @return Position of next entry's start or nothing if EOF is reached.
    std::optional<unsigned> next_entry(unsigned prev_pos);

    /// @brief Reads the contents of entry that starts at given position.
    /// @param at_pos Position of entry start (not entry's data!)
    /// @return Serialized contents of the entry or nothing if an error occurred.
    std::optional<Serialized> read_entry_at(unsigned at_pos);

    /// @brief Locates an entry by its Locator.
    /// @return Position of entry or nothing if no such entry exists.
    std::optional<unsigned int> locate_entry(std::string locator);

    /// @brief Locate n-th entry from file start.
    /// @return Position of entry or nothing if EOF is reached.
    std::optional<unsigned> locate_entry_indexed(int index);
    
    public:
    ~Archivist();
    Archivist(std::string storage_file = DEFAULT_STORAGE_FILE);

    /// @brief Gets the global Archivist instance to store data used by multiple modules.
    static Archivist& get_default();
    
    /// @brief Retreives serialized data marked with given Locator.
    std::optional<Serialized> get_serialized(std::string locator);

    /// @brief Creates or overwrites an entry with given Locator.
    /// @return
    bool put_serialized(std::string locator, std::vector<char> value);

    /// @brief Deletes data marked with given Locator.
    /// @return 
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

    inline std::optional<std::string> get_string(std::string locator)
    {
        std::optional<Serialized> serialized = get_serialized(locator);
        if (!serialized.has_value()) {
            return {};
        }
        return {deserialize_string(serialized.value())};
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

    inline bool put_string(std::string locator, std::string value)
    {
        Serialized serialized = serialize_string(value);
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