#pragma once

#include "Commander.hpp"

#include <queue>

class Proxy_commander : public Commander
{
    public:

    Order get_order() override
    {
        Order ord = Order();
        if (orders.size() > 0)
        {
            ord = orders.front();
            orders.pop();
        }
        return ord;
    }
    void push_order(Order ord) { orders.push(ord);}

    void process_event(const Event& event) override { events.push(event);}
    Event receive_event()
    {
        Event ev = Event();
        if (events.size() > 0)
        {
            ev = events.front();
            events.pop();
        }
        return ev;
    };
    Proxy_commander() = default;

    private:
    
    std::queue<Order> orders; // Inbound orders
    std::queue<Event> events; // Outbound events
};