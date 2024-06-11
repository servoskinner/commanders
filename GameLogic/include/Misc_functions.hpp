#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <string>

template <typename Type, typename Key_type>
bool pop_element(std::vector<Type>& vec, Key_type& key, \
                 const std::function<bool(Type&, Key_type&)> compare = \
                 [](Type& left, Key_type& right){ return left == right; }) 
                 // Remove element that matches comparison criterion from a vector (== by default).
                 // Returns TRUE if element was found and destroyed and FALSE otherwise

                 // CAUTION: mind the order of Type and Keytype. don't repeat my mistakes
{
    for(auto iter = vec.begin(); iter != vec.end(); iter++)
    {
        if(compare(*iter, key))
        {
            vec.erase(iter);
            return true;
        }
    }
    return false;
}

template <typename Type, typename Key_type>
bool contains_elem(std::vector<Type>& vec, Key_type& key, \
                 const std::function<bool(Type&, Key_type&)> compare = \
                 [](Type& left, Key_type& right){ return left == right; }) 
                 // Remove element that matches comparison criterion from a vector (== by default).
                 // Returns TRUE if element was found and destroyed and FALSE otherwise

                 // CAUTION: mind the order of Type and Keytype. don't repeat my mistakes
{
    for(auto iter = vec.begin(); iter != vec.end(); iter++)
    {
        if(compare(*iter, key))
        {
            return true;
        }
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

std::vector<std::string> wrap_text(const std::string& text, int line_width); // Split text into lines to fit inside given line width.