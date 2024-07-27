#include "Event_timer.hpp"



float Event_timer::get_cur_time_s() {
    return (float)clock()/(float)CLOCKS_PER_SEC;
}

void Event_timer::process()
{
    float cur_time = get_cur_time_s();

    delta_time = cur_time - prev_time;

    if (delta_time < 0.0) { // overflow
        delta_time = (float)(__LONG_MAX__/CLOCKS_PER_SEC) - prev_time + cur_time;
    }
    for (Timed_event& event : events)
    {
        event.countdown -= delta_time;
        while(event.countdown <= 0.0)
        {
            // Lock mutex if provided
            std::optional<std::lock_guard<std::mutex>> opt_lock;
            if(event.mutex.has_value()) {
                opt_lock.emplace(event.mutex->get());
            }
            event.event();

            if (skip_stalled) {
                event.countdown = event.t_seconds;
            }
            else {
                event.countdown += event.t_seconds;
            }
        }
    }
    prev_time = cur_time;
}