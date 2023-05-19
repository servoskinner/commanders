#pragma once

#include <algorithm>
#include <vector>

template <typename Type>
bool vectorPop(std::vector<Type>& vec, const Type& key) //Remove element with target value from a vector
{
    //find first pointer that points to object that is equal to key.
    for(auto iter = vec.begin(); iter != vec.end(); iter++)
        if(*iter == key)
        {
            vec.erase(iter);
            return true;
        }

    return false;
}

template <typename Type>
bool vectorPopIndex(std::vector<Type>& vec, const int& index) //Remove element with target index from a vector
{
    if(index >= vec.size() || index < 0) return false;

    auto iter = vec.begin() + index;
    vec.erase(iter);

    return true;
}

template <typename Type>
Type constrain(const Type& arg, const Type& lowbound, const Type& upbound)
{
    if(arg > upbound)
        return upbound;
    if(arg < lowbound)
        return lowbound;
        
    return arg;
}

