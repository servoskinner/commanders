#pragma once

#include <algorithm>
#include <memory>

template <typename Type>
bool vectorPop(std::vector<Type>& vec, const Type& key) //Remove element with target value from a vector
{
    //find first pointer that points to object that is equal to key.
    auto iter = std::remove(vec.begin(), vec.end(), key);

    if(iter == vec.end()) return false;

    vec.erase(iter, vec.end());
    return true;
}

template <typename Type>
bool vectorPopIndex(std::vector<Type>& vec, const int& index) //Remove element with target index from a vector
{
    if(index >= vec.size() || index < 0) return false;

    auto iter = vec.begin();
    std::advance(iter, index);
    vec.erase(iter, vec.end());
    
    return true;
}

