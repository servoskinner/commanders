#include "Proxy_commander.hpp"

Commander::Event Proxy_commander::get_event()
{
    Event event = {};
    if(event_queue.size() != 0)
    {
        event = event_queue.front();
        event_queue.pop();
    }
    return event;
}

Commander::Event Proxy_commander::get_order()
{
    Order order = {};
    if(order_queue.size() != 0)
    {
        order = order_queue.front();
        order_queue.pop();
    }
    return order;
}