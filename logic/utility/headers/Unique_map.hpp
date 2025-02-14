#pragma once

#include "Unique.hpp"

#include <type_traits>
#include <unordered_map>
#include <iterator>

template <typename Type>
class Unique_map{
    static_assert(std::is_base_of<Unique, Type>::value, "This container accepts only Unique or derivatives.");

public:
    using map_t = std::unordered_map<uint32_t, Type>;
    using map_iter_t = typename map_t::iterator;
    using map_const_iter_t = typename map_t::const_iterator;

    class iterator;
    class const_iterator;

    inline void clear() { map.clear(); }
    inline void emplace(Type&& value) { map.emplace(value.get_uid(), std::move(value)); };
    inline void swap(Unique_map<Type>& other) { map.swap(other.map); }
    inline iterator find(uint32_t key) { return iterator(map.find(key)); }
    inline Type& operator[](uint32_t key) { 
        auto access_iter = map.find(key);
        if (access_iter == map.end()) {
            throw std::out_of_range("Unique_map::operator[]: Element does not exist");
        }
        return access_iter->second;
    }

    inline void erase(uint32_t key) { map.erase(key); }
    inline void erase(iterator iter) { map.erase(iter.iter); }
    inline void erase(const_iterator iter) { map.erase(iter.iter); }

    inline iterator begin() { return iterator(map.begin()); }
    inline const_iterator begin() const { return const_iterator(map.begin()); }
    inline const_iterator cbegin() const noexcept { return const_iterator(map.cbegin()); }

    inline iterator end() { return iterator(map.end()); }
    inline const_iterator end() const { return const_iterator(map.end()); }
    inline const_iterator cend() const noexcept { return const_iterator(map.end()); }

    class iterator {
        private:
        map_iter_t iter;
        
        public:
        iterator(map_iter_t map_iter) : iter(map_iter) {}

        Type& operator->() { return iter->second; }
        Type& operator*() { return iter->second; }

        iterator& operator++() { 
            ++iter;
            return *this;
        }

        bool operator==(const iterator& other) const { return iter == other.iter; }
        bool operator!=(const iterator& other) const { return iter != other.iter; }
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
    };

    class const_iterator {
        private:
        map_const_iter_t iter;

        public:
        const_iterator(map_const_iter_t map_const_iter) : iter(map_const_iter) {}

        const Type& operator->() { return iter->second; }
        const Type& operator*() { return iter->second; }

        const_iterator& operator++() { 
            ++iter;
            return *this;
        }

        bool operator==(const const_iterator& other) const { return iter == other.iter; }
        bool operator!=(const const_iterator& other) const { return iter == other.iter; }
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
    };

private:
    map_t map = {};
};

template <typename Type>
bool Unique_map<Type>::iterator::operator==(const Unique_map<Type>::const_iterator& other) const { return iter == other.iter; }
template <typename Type>
bool Unique_map<Type>::iterator::operator!=(const Unique_map<Type>::const_iterator& other) const { return iter != other.iter; }
template <typename Type>
bool Unique_map<Type>::const_iterator::operator==(const Unique_map<Type>::iterator& other) const { return iter == other.iter; }
template <typename Type>
bool Unique_map<Type>::const_iterator::operator!=(const Unique_map<Type>::iterator& other) const { return iter == other.iter; }