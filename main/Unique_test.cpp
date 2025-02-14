#include <iostream>
#include "Unique.hpp"

int main()
{
    {
    Unique u1, u2;
    std::cout << u1 << " " << u2 << std::endl;
    }

    Unique u3, u4, u5;

    std::cout << u3 << " " << u4 << " " << u5 << std::endl;
    u4.~Unique();
    Unique u6;

    std::cout << u3 << " " << u5 << " " << u6 << std::endl;
    return 0;
}