#include "Storage_manager.hpp"
#include "Serialization.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    Storage_manager& sm = Storage_manager::get_default();

    sm.put_item("poop", serialize_struct(1));
    std::string name = "johnny";
    sm.put_item("name", std::vector<char>(name.begin(), name.end()));

    std::optional<Serialized> item1 = sm.get_item("poop");
    int value1 = -1;
    if (item1.has_value()) {
        value1 = deserialize_struct<int>(item1.value());
    }
    std::optional<Serialized> item2 = sm.get_item("name");
    std::string value2 = "none";
    if (item2.has_value()) {
        value2 = {item2.value().begin(), item2.value().end()};
    }

    std::cout << value1 << std::endl;
    std::cout << value2 << std::endl;
}