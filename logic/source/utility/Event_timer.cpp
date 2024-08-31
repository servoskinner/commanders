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
    for (int ev_id = 0;ev_id < events.size(); ev_id++)
    {
        events[ev_id].countdown -= delta_time;
        while(events[ev_id].countdown <= 0.0)
        {
            // Lock mutex if provided
            std::optional<std::lock_guard<std::mutex>> opt_lock;
            if(events[ev_id].mutex.has_value()) {
                opt_lock.emplace(events[ev_id].mutex->get());
            }

            if (events[ev_id].n_repeat > 0) {
                events[ev_id].n_repeat--;
            }
            else if (events[ev_id].n_repeat == 0) {
                events.erase(events.begin() + ev_id);
                ev_id--;
                break;
            }
            

            events[ev_id].event();


            if (skip_stalled) {
                events[ev_id].countdown = events[ev_id].t_seconds;
            }
            else {
                events[ev_id].countdown += events[ev_id].t_seconds;
            }
        }
    }
    prev_time = cur_time;
}