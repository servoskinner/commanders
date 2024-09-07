#include "Storage_manager.hpp"
#include "Serialization.hpp"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    Storage_manager& sm = Storage_manager::get_default();
    bool is_running = true;

    while(is_running) {
        std::cout << "(R)ead (W)rite (D)elete (Q)uit" << std::endl;
        std::string in_string;
        std::cin >> in_string;
        char input = in_string[0];

        switch (input)
        {
        case 'R': case 'r':
            {
                std::cout << "Enter key: " << std::flush;
                std::string key;
                std::cin >> key;

                std::optional<Serialized> value = sm.get_item(key);
                if (value.has_value()) {
                    std::cout << std::string(value->begin(), value->end()) << std::endl;
                }
                else {
                    std::cout << "no such entry" << std::endl;
                }

            break;
            }
        case 'W': case 'w':
            {
                std::cout << "Enter key: " << std::flush;
                std::string key;
                std::cin >> key;
                std::cout << "Enter value: " << std::flush;
                std::string value;
                std::cin >> value;
            
                if (sm.put_item(key, Serialized{value.begin(), value.end()})) {
                    std::cout << "written successfully" << std::endl;
                }
                else {
                    std::cout << "failed to write" << std::endl;
                }
            break;
            }
        case 'D': case 'd':
            {
                std::cout << "Enter key: " << std::flush;
                std::string key;
                std::cin >> key;
                if (sm.delete_item(key)) {
                    std::cout << "deleted successfully" << std::endl;
                }
                else {
                    std::cout << "failed to delete" << std::endl;
                }
            break;
            }
        case 'Q': case 'q':
            is_running = false;
            break;
        default:
            std::cout << "Unrecognized command" << std::endl;
            break;
        }
    }
}