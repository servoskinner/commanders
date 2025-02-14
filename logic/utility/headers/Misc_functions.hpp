#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <string>
#include <sstream>

/// @brief Attempts to find an element in vector and remove it.
/// @param compare Criterion the searched-for element must match.
/// @return Whether the element was found and popped.
template <typename Type, typename Key_type>
inline bool pop_element(std::vector<Type>& vec, Key_type& key, \
                 const std::function<bool(Type&, Key_type&)> compare = \
                 [](Type& left, Key_type& right){ return left == right; }) 
                 // Remove element that matches comparison criterion from a vector (== by default).
                 // Returns TRUE if element was found and destroyed and FALSE otherwise

                 // CAUTION: mind the order of Type and Keytype
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
/// @brief  Finds an element in array and returns its index.
/// @param compare Criterion that the element must match. X == key by default.
/// @return the index of found element or -1 if no element was found.
template <typename Type, typename Key_type>
inline int find_index(std::vector<Type> vec, Key_type& key, \
               const std::function<bool(Type&, Key_type&)> compare = \
               [](Type& left, Key_type& right){ return left == right; }) 
               // Remove element that matches comparison criterion from a vector (== by default).
               // Returns TRUE if element was found and destroyed and FALSE otherwise

               // CAUTION: mind the order of Type and Keytype
{
    int index = 0;
    for(auto iter = vec.begin(); iter != vec.end(); iter++, index++) {
        if(compare(*iter, key)) {
            return index;
        }
    }
    return -1;
}

/// @brief attempts to erase the vector element at given index
/// @returns whether deleting was successful
template <typename Type>
inline bool pop_index(std::vector<Type>& vec, const int& index) //Remove element with target index from a vector
{
    if(index >= vec.size() || index < 0) return false;

    auto iter = vec.begin() + index;
    vec.erase(iter);

    return true;
}

/// @brief Split text into lines to fit inside given line width constraint.
std::vector<std::string> wrap_text(const std::string& text, int line_width);

/// @return Whether a character is a visible symbol
constexpr inline bool is_printable(unsigned ch)
{
    return (31 < ch && ch < 127);
}

/// @brief Maps 2d array indices into 1d array index.
constexpr inline int flatten_index(std::pair<int, int> indices, int width)
{
    return indices.first * width + indices.second;
}

/// @brief Maps 1d array index into 2d array pair of indices.
constexpr inline std::pair<int, int> unflatten_index(int index, int width)
{
    return {index / width, index % width};
}