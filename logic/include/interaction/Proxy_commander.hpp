#pragma once

#include "Commander.hpp"
#include <queue>

class Proxy_commander : Commander
{
    public:
    void queue_order(Order order);
    void process_order_feedback(Event event) override;

    Order get_order() override;
    Event get_event();

    int events_size() { return event_queue.size();}
    int orders_size() { return order_queue.size();}

    private:
    std::queue<Order> order_queue;
    std::queue<Event> event_queue;
};