#include <iostream>

#include "Unique.hpp"
#include "Unique_map.hpp"

class unistring : public Unique {
    public:
    std::string str;
    unistring(std::string&& str) : str(str) {}
};

int main() {
    Unique_map<unistring> unimap;

    unimap.emplace(unistring("toad"));
    unimap.emplace(unistring("frog"));
    unimap.emplace(unistring("le"));
    unimap.emplace(unistring("gushka"));
    
    unimap.erase(1);
    unimap.erase(2);

    for(const auto& u : unimap) {
        std::cout << u.str << std::endl;
    }

    unistring loose = unistring("jaba");
    unimap.emplace(std::move(loose));

    for(const auto& u : unimap) {
        std::cout << u.get_uid() << std::endl;
    }
    std::cout << unimap[1].str << std::endl;

    return 0;
}