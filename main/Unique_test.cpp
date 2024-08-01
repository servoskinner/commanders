#include <iostream>
#include "Unique.hpp"

int main()
{
    {
    Unique u1, u2;
    std::cout << u1.get_id() << " " << u2.get_id() << std::endl;
    }

    Unique u3, u4, u5;

    std::cout << u3.get_id() << " " << u4.get_id() << " " << u5.get_id() << std::endl;
    u4.~Unique();
    Unique u6;

    std::cout << u3.get_id() << " " << u5.get_id() << " " << u6.get_id() << std::endl;
    return 0;
}