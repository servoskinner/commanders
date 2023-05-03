#include <vector>
#include <iostream>
#include "misc.h"

int main()
{
std::vector<int> vec = {1, 2, 4, 8, 16};
vectorPopIndex(vec, 2);

for(int i : vec)
{
    std::cout << i << " ";
}
std::cout << std::endl;

return 0;
}
