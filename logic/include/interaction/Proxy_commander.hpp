#pragma once

#include "Commander.hpp"
#include <queue>

class Proxy_commander : Commander
{
    public:
    inline bool queue_order(Order order) {};
    inline void process_event(Event event) override { event_queue.push(event);}
    inline void process_order_feedback(int code) override;

    inline std::optional<Order> get_order() override { return queued_order;};
    std::optional<Event> get_event();

    int events_size() { return event_queue.size();}

    private:
    std::optional<int> order_feedback;
    std::optional<Order> queued_order;

    std::queue<Event> event_queue;
};