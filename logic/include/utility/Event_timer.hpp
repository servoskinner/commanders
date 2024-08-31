#pragma once

#include <ctime>
#include <vector>
#include <functional>
#include <optional>
#include <mutex>

class Event_timer
{
    private:
    static float get_cur_time_s();
    float prev_time, delta_time;

    public:
    Event_timer() : events(), delta_time(0), \
                    prev_time(get_cur_time_s()), skip_stalled(true) {}

    struct Timed_event
    {
        std::function<void(void)> event = {};
        std::optional<std::reference_wrapper<std::mutex>> mutex;
        float t_seconds = 0.0;
        float countdown = 0.0;
        int n_repeat = -1;

        Timed_event(std::function<void(void)> event, float t_seconds, int n_repeat = -1) : event(event), t_seconds(t_seconds), \
                                                                                           countdown(t_seconds), mutex(),
                                                                                           n_repeat(n_repeat) {}
    };

    void process();
    std::vector<Timed_event> events;
    bool skip_stalled;
};