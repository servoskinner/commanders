#pragma once

#include <vector>
#include <cstring>
#include <unordered_map>
#include <utility>
#include <cstdlib>


typedef std::vector<char> Serialized;

/**
 * @brief Packs a fixed-size structure into a byte vector to be sent as a message.
 */
template <typename Type>
Serialized serialize(const Type& object)
{
    std::vector<char> serialized(sizeof(Type));
    std::memcpy(serialized.data(), &object, sizeof(Type));
    return serialized;
}

inline std::vector<char> serialize_string(std::string str) {
    return std::vector<char>(str.begin(), str.end());
}

template <typename Type>
Serialized serialize_vector(std::vector<Type> vector)
{
    Serialized serialized(vector.size()*sizeof(Type));
    
    std::memcpy(serialized.data(), \
                (char*)(vector.data()), \
                vector.size()*sizeof(Type));
    return serialized;
}

template <typename Keytype, typename Valtype>
Serialized serialize_map(std::unordered_map<Keytype, Valtype> map)
{
    Serialized serialized;
    for (std::pair<Keytype, Valtype> pair : map) {
        Serialized serialized_key = serialize(pair.first);
        Serialized serialized_value = serialize(pair.second);
        // append to result
        std::copy(serialized_key.begin(), serialized_key.end(), std::back_inserter(serialized)); 
        std::copy(serialized_value.begin(), serialized_value.end(), std::back_inserter(serialized)); 
    }
    return serialized;
}

/**
 * @brief Creates a fixed-size structure out of a byte vector received over network.
 */
template <typename Type>
Type deserialize(Serialized serialized)
{
    Type new_obj;
    if(serialized.size() != sizeof(Type)) {
        throw std::runtime_error("deserialize_struct(): byte vector size is not equal to type size");
    }
    std::memcpy(&new_obj, serialized.data(), sizeof(Type));
    return new_obj;
}

inline std::string deserialize_string(Serialized serialized) {
    return std::string(serialized.begin(), serialized.end());
}

template <typename Type>
std::vector<Type> deserialize_vector(Serialized serialized)
{
    if (serialized.size() % sizeof(Type) != 0) {
        throw std::runtime_error("deserialize_vector(): byte vector not divisible by type size");
    }
    unsigned vector_size = serialized.size() / sizeof(Type);
    
    std::vector<Type> vector;
    vector.resize(vector_size);

    std::memcpy((char*)(vector.data()), serialized.data(), sizeof(Type)*vector_size);
    return vector;
}

template <typename Keytype, typename Valtype>
std::unordered_map<Keytype, Valtype> deserialize_map(Serialized serialized)
{
    if (serialized.size() % (sizeof(Keytype) + sizeof(Valtype))) {
        throw std::runtime_error("deserialize_map(): byte vector size not divisible by key+value size");
    }
    unsigned map_size = serialized.size() / (sizeof(Keytype) + sizeof(Valtype));
    std::unordered_map<Keytype, Valtype> map;

    for (unsigned int i = 0; i < map_size; i++) {
        Serialized serialized_key(serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)), \
                                  serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype));
        Keytype key = deserialize<Keytype>(serialized_key);
        Serialized serialized_val(serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype), \
                                  serialized.begin() + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype) \
                                  + sizeof(Valtype));
        Valtype val = deserialize<Valtype>(serialized_val);
        map.insert({key, val});
    }
    return map;
}