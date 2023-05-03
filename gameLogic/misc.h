#pragma once

#include <algorithm>
#include <memory>

template <typename Type>
bool vectorPop(std::vector<Type>& vec, const Type& key) //Simplified removal of element from a list.
{
    //find first pointer that points to object that is equal to key.
    auto iter = std::remove(vec.begin(), vec.end(), key);

    if(iter == vec.end()) return false;

    vec.erase(iter, vec.end());
    return true;
}
