#include "Proxy_commander.hpp"

std::optional<Commander::Event> Proxy_commander::get_event()
{
    if (event_queue.empty()) {
        return {};
    }
    else {
        Event ev = event_queue.back();
        event_queue.pop();
        return ev;
    }
}

