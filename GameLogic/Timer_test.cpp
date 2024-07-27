#include "Event_timer.hpp"
#include <iostream>

int times = 10;

void simple_event()
{
    std::cout << "fart" << std::endl;
    times--;
}

int main()
{
    Event_timer timer;
    timer.events.push_back({simple_event, 1.0});

    while(times > 0)
    {
        timer.process();
    }
}