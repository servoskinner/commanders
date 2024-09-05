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
Serialized serialize_struct(const Type& object)
{
    std::vector<char> serialized(sizeof(Type));
    std::memcpy(serialized.data(), &object, sizeof(Type));

    return serialized;
}

template <typename Type>
Serialized serialize_vector(std::vector<Type> vector)
{
    // build prefix
    std::vector<char> serialized = {serialize_struct<unsigned int>(vector.size())};
    serialized.resize(sizeof(unsigned int) + vector.size()*sizeof(Type));

    std::memcpy((char*)(vector.data()) + sizeof(unsigned int), \
                (char*)(vector.data()) + sizeof(unsigned int) + vector.size()*sizeof(Type), \
                vector.size()*sizeof(Type));

    return serialized;
}

template <typename Keytype, typename Valtype>
Serialized serialize_map(std::unordered_map<Keytype, Valtype> map)
{
    // build prefix
    Serialized serialized = {serialize_struct<unsigned int>(map.size())};

    for (std::pair<Keytype, Valtype> pair : map) {
        Serialized serialized_pair = {serialize_struct(pair.first), serialize_struct(pair.second)};
        std::copy(serialized_pair.begin(), serialized_pair.end(), std::back_inserter(serialized)); // append to result
    }
    return serialized;
}

/**
 * @brief Creates a fixed-size structure out of a byte vector received over network.
 */
template <typename Type>
Type deserialize_struct(Serialized serialized)
{
    Type new_obj;
    if(serialized.size() < sizeof(Type))
    {
        throw std::runtime_error("deserialize_struct(): char vector is too short to be unpacked");
    }
    std::memcpy(&new_obj, serialized.data(), sizeof(Type));
    return new_obj;
}

template <typename Type>
std::vector<Type> deserialize_vector(Serialized serialized)
{
    if (serialized.size() < sizeof(unsigned int)) {
        throw std::runtime_error("deserialize_map(): byte vector too short to infer size");
    }
    // decode prefix
    unsigned int vector_size = {serialized.begin(), serialized.begin()+sizeof(unsigned int)};

    if (serialized.size() - sizeof(unsigned int) < sizeof(Type)*vector_size) {
        throw std::runtime_error("deserialize_map(): byte vector too short for specified size");
    }
    std::vector<Type> vector;
    vector.resize(vector_size);

    std::memcpy((char*)(vector.data()), serialized.data(), sizeof(Type)*vector_size);
    return vector;
}

template <typename Keytype, typename Valtype>
std::unordered_map<Keytype, Valtype> deserialize_map(Serialized serialized)
{
    if (serialized.size() < sizeof(unsigned int)) {
        throw std::runtime_error("deserialize_map(): byte vector too short to infer size");
    }
    // decode prefix
    unsigned int map_size = {serialized.begin(), serialized.begin()+sizeof(unsigned int)};

    if (serialized.size() - sizeof(unsigned int) < (sizeof(Keytype) + sizeof(Valtype))*map_size) {
        throw std::runtime_error("deserialize_map(): byte vector too short for specified size");
    }
    std::unordered_map<Keytype, Valtype> map;
    for (unsigned int i = 0; i < map_size; i++) {
        Serialized serialized_key = {serialized.begin() + sizeof(unsigned int) + i*(sizeof(Keytype) + sizeof(Valtype)), \
                                            serialized.begin() + sizeof(unsigned int) + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype)};
        Keytype key = deserialize_struct<Keytype>(serialized_key);
        Serialized serialized_val = {serialized.begin() + sizeof(unsigned int) + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype),
                                            serialized.begin() + sizeof(unsigned int) + i*(sizeof(Keytype) + sizeof(Valtype)) + sizeof(Keytype) \
                                            + sizeof(Valtype)};
        Valtype val = deserialize_struct<Valtype>(serialized_val);
        map.insert({key, val});
    }
    return map;
}