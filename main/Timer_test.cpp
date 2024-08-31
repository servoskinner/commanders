#include "Event_timer.hpp"
#include <iostream>

void simple_event()
{
    std::cout << "fart" << std::endl;
}
void complex_event()
{
    std::cout << "shart" << std::endl;
}

int main()
{
    Event_timer timer;
    timer.events.push_back({simple_event, 1.0, 3});
    timer.events.push_back({complex_event, 0.25, 10});

    while (true) {
        timer.process();
    }
}