#pragma once

#include <algorithm>
#include <vector>
#include <functional>

template <typename Type, typename Keytype>
bool pop_element(std::vector<Type>& vec, const Keytype& key, \
                 const std::function<bool(const Type&, const Keytype&)> compare = \
                 [](const Type& left, const Keytype& right){ return left == right; }) 
                 // Remove element that matches comparison criterion from a vector (== by default).
                 // Returns TRUE if element was found and destroyed and FALSE otherwise

                 // CAUTION: mind the order of Type and Keytype. don't repeat my mistakes
                 // (this function is sharty killing vantablack brimstone, 'thon users rejoice)
{
    // Find first iter that points to object that yields true when compared against key..
    for(auto iter = vec.begin(); iter != vec.end(); iter++)
        if(compare(*iter, key))
        {
            vec.erase(iter);
            return true;
        }

    return false;
}

template <typename Type>
bool pop_index(std::vector<Type>& vec, const int& index) //Remove element with target index from a vector
{
    if(index >= vec.size() || index < 0) return false;

    auto iter = vec.begin() + index;
    vec.erase(iter);

    return true;
}

